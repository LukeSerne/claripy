#!/usr/bin/env python

import collections
import logging
l = logging.getLogger("claripy.expression")

from .storable import Storable

class A(object):
    '''
    An A(ST) tracks a tree of operations on arguments.
    '''

    __slots__ = [ 'op', 'args', '_length', '_hash', '__weakref__' ]

    def __init__(self, op, args):
        self.op = op
        self.args = args
        self._hash = None
        self._length = None

    @staticmethod
    def arg_size(backends, o):
        if isinstance(o, E):
            return o.size()
        elif isinstance(o, A):
            return o.size(backends=backends)
        else:
            for b in backends:
                try: return b.call('size', (o,))
                except BackendError: pass
            return -1

    def calculate_length(self, backends):
        if self.op in length_none_operations:
            return -1

        if self.op in length_same_operations:
            if self.op == 'If':
                lengths = set(self.arg_size(backends, a) for a in self.args[1:])
            else:
                lengths = set(self.arg_size(backends, a) for a in self.args)

            if len(lengths) != 1:
                raise ClaripySizeError("invalid length combination for operation %s", self.op)
            return lengths.__iter__().next()

        if self.op in length_change_operations:
            if self.op in ( "SignExt", "ZeroExt" ):
                length = self.arg_size(backends, self.args[1])
                if length == -1: raise ClaripyTypeError("extending an object without a length")
                return length + self.args[0]
            if self.op == 'Concat':
                lengths = [ self.arg_size(backends, a) for a in self.args ]
                if len(lengths) != len(self.args) or -1 in lengths:
                    raise ClaripyTypeError("concatenating an object without a length")
                return sum(lengths)
            if self.op == 'Extract':
                return self.args[0]-self.args[1]+1

        if self.op == 'BoolVal':
            return -1

        if self.op in length_new_operations:
            return self.args[1]

        raise ClaripyOperationError("unknown operation %s" % self.op)

    def size(self, backends=()):
        if self._length is None:
            self._length = self.calculate_length(backends)
        return self._length

    def resolve(self, b, save=None, result=None):
        if result is not None and self in result.resolve_cache[b]:
            return result.resolve_cache[b][self]

        args = [ ]
        for a in self.args:
            if isinstance(a, E):
                args.append(b.convert_expr(a, result=result))
            elif isinstance(a, A):
                args.append(a.resolve(b, save=save, result=result))
            else:
                args.append(b.convert(a, result=result))

        l.debug("trying evaluation with %s", b)
        r = b.call(self.op, args, result=result)
        if result is not None:
            result.resolve_cache[b][self] = r
        return r

    def deepercopy(self):
        args = [a.deepercopy() if isinstance(a, E) else a for a in self.args]
        return A(self.op, args)

    def __repr__(self):
        if '__' in self.op:
            return "%s.%s%s" % (self.args[0], self.op, self.args[1:])
        else:
            return "%s%s" % (self.op, self.args)

    def __hash__(self):
        if self._hash is None:
            self._hash = hash((self.op,) + tuple(self.args))
        return self._hash

    def __getstate__(self):
        return self.op, self.args
    def __setstate__(self, state):
        self._hash = None
        self.op, self.args = state

class E(Storable):
    '''
    A class to wrap expressions.
    '''
    __slots__ = [ 'variables', 'symbolic', '_uuid', '_actual_model', '_stored', 'objects', '_simplified', '__weakref__', '_pending_operations' ]

    def __init__(self, claripy, variables=None, symbolic=None, uuid=None, objects=None, model=None, stored=False, simplified=False):
        Storable.__init__(self, claripy, uuid=uuid)
        have_uuid = uuid is not None
        have_data = not (variables is None or symbolic is None or model is None)
        self.objects = { }
        self._simplified = simplified
        self._pending_operations = [ ]

        if have_uuid and not have_data:
            self._load()
        elif have_data:
            self.variables = variables
            self.symbolic = symbolic

            self._uuid = uuid
            self._actual_model = model
            self._stored = stored

            if objects is not None:
                self.objects.update(objects)
        else:
            raise ValueError("invalid arguments passed to E()")

    @property
    def _model(self):
        if len(self._pending_operations) != 0:
            e = self
            p = self._pending_operations
            self._pending_operations = [ ]

            for o in p:
                e = self._claripy._do_op(o, (e.deepercopy(),))

            self.variables = e.variables
            self.symbolic = e.symbolic
            self._actual_model = e._actual_model
            self.objects = e.objects

        return self._actual_model

    #
    # Some debug stuff:
    #

    @staticmethod
    def _part_count(a):
        return sum([ E._part_count(aa) for aa in a.args ]) if isinstance(a, A) else E._part_count(a._model) if isinstance(a, E) else 1

    @staticmethod
    def _depth(a):
        return max([ E._depth(aa)+1 for aa in a.args ]) if isinstance(a, A) else E._depth(a._model) if isinstance(a, E) else 1

    @staticmethod
    def _hash_counts(a, d=None):
        if d is None: d = collections.defaultdict(int)
        d[(a.__class__, hash(a))] += 1

        if isinstance(a, A):
            for aa in a.args:
                E._hash_counts(aa, d=d)
        elif isinstance(a, E):
            E._hash_counts(a._model, d=d)
        return d

    def __hash__(self):
        return hash(self._model)

    def _load(self):
        e = self._claripy.datalayer.load_expression(self._uuid)
        self.variables = e.variables
        self.symbolic = e.symbolic

        self._uuid = e._uuid
        self._actual_model = e._actual_model
        self._stored = e._stored

    def __nonzero__(self):
        raise ClaripyExpressionError('testing Expressions for truthiness does not do what you want, as these expressions can be symbolic')

    def __repr__(self):
        start = "E"
        if self.symbolic:
            start += "S"

        start += "("
        end = ")"

        for o in self._pending_operations:
            start += o + "("
            end += ")"

        return start + str(self._actual_model) + end

    def split(self, split_on):
        if not isinstance(self._model, A):
            return [ self ]

        if self._model.op in split_on:
            l.debug("Trying to split: %r", self._model)
            if all(isinstance(a, E) for a in self._model.args):
                return self._model.args[:]
            else:
                raise ClaripyExpressionError('the abstraction of this expression was not done with "%s" in split_on' % self._model.op)
        else:
            l.debug("no split for you")
            return [ self ]

    #
    # Storing and loading of expressions
    #

    def store(self):
        self._claripy.datalayer.store_expression(self)

    def __getstate__(self):
        if self._uuid is not None:
            l.debug("uuid pickle on %s", self)
            return self._uuid
        l.debug("full pickle on %s", self)
        return self._uuid, self._model, self.variables, self.symbolic, self._simplified

    def __setstate__(self, s):
        if type(s) is str:
            self.__init__(get_claripy(), uuid=s)
            return

        uuid, model, variables, symbolic, simplified = s
        self.__init__(get_claripy(), variables=variables, symbolic=symbolic, model=model, uuid=uuid, simplified=simplified)

    def copy(self):
        c = E(claripy=self._claripy, variables=self.variables, symbolic=self.symbolic, uuid=self._uuid, objects=self.objects, model=self._actual_model, stored=self._stored, simplified=self._simplified)
        c._pending_operations.extend(self._pending_operations)
        return c

    def deepercopy(self):
        if isinstance(self._actual_model, A):
            model = self._actual_model.deepercopy()
        else:
            model = self._actual_model
        c = E(claripy=self._claripy, variables=self.variables, symbolic=self.symbolic,
              uuid=self._uuid, objects=self.objects, model=model, stored=self._stored,
              simplified=self._simplified)
        c._pending_operations.extend(self._pending_operations)
        return c

    #
    # BV operations
    #

    def __len__(self):
        if type(self._model) is A:
            return self._model.size(backends=self._claripy.model_backends)
        else:
            for b in self._claripy.model_backends:
                try: return b.call_expr('size', (self,))
                except BackendError: pass
            raise ClaripyExpressionError("unable to determine size of expression")
    size = __len__

    @property
    def length(self):
        return self.size()

    def __iter__(self):
        raise ClaripyExpressionError("Please don't iterate over Expressions!")

    def chop(self, bits=1):
        s = len(self)
        if s % bits != 0:
            raise ValueError("expression length (%d) should be a multiple of 'bits' (%d)" % (len(self), bits))
        elif s == bits:
            return [ self ]
        else:
            return list(reversed([ self[(n+1)*bits - 1:n*bits] for n in range(0, s / bits) ]))

    def reversed(self, lazy=True):
        '''
        Reverses the expression.
        '''
        return self._claripy.Reverse(self, lazy=lazy)
    reverse = reversed

    def __getitem__(self, rng):
        if type(rng) is slice:
            return self._claripy.Extract(int(rng.start), int(rng.stop), self)
        else:
            return self._claripy.Extract(int(rng), int(rng), self)

    def zero_extend(self, n):
        return self._claripy.ZeroExt(n, self)

    def sign_extend(self, n):
        return self._claripy.SignExt(n, self)

#
# Wrap stuff
#

def e_operator(cls, op_name):
    def wrapper(self, *args):
        return self._claripy._do_op(op_name, (self,) + tuple(args))
    wrapper.__name__ = op_name
    setattr(cls, op_name, wrapper)

def make_methods():
    for name in expression_operations:
        e_operator(E, name)

from .errors import ClaripyExpressionError, BackendError, ClaripySizeError, ClaripyOperationError, ClaripyTypeError
from .operations import expression_operations, length_none_operations, length_change_operations, length_same_operations, length_new_operations
make_methods()
from . import get_claripy
