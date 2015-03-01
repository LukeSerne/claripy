import itertools
bitvec_counter = itertools.count()

import logging
l = logging.getLogger('claripy.claripy')

class Claripy(object):
    def __init__(self, name, model_backends, solver_backends, parallel=None):
        self.name = name
        self.solver_backends = solver_backends
        self.model_backends = model_backends
        self.unique_names = True
        self.parallel = parallel if parallel else False

        self.true = self.BoolVal(True)
        self.false = self.BoolVal(False)

    #
    # Backend management
    #
    def backend_of_type(self, b_type):
        for b in self.model_backends + self.solver_backends:
            if type(b) is b_type:
                return b
        return None

    #
    # Solvers
    #
    def solver(self):
        '''
        Returns a new solver.
        '''
        raise NotImplementedError()

    #
    # Operations
    #

    def BitVec(self, name, size, explicit_name=None):
        explicit_name = explicit_name if explicit_name is not None else False
        if self.unique_names and not explicit_name:
            name = "%s_%d_%d" % (name, bitvec_counter.next(), size)
        return BV(self, 'BitVec', (name, size), variables={ name }, symbolic=True, simplified=Base.FULL_SIMPLIFY, length=size)
    BV = BitVec

    def BitVecVal(self, value, size):
        return BVI(self, BVV(value, size), variables=set(), symbolic=False, simplified=Base.FULL_SIMPLIFY, length=size)
    BVV = BitVecVal

    def FP(self, name, sort, explicit_name=None):
        if self.unique_names and not explicit_name:
            name = "FP_%s_%d_%d" % (name, fp_counter.next(), size)
        return FP(self, 'FP', (name, sort), variables={name}, symbolic=True, simplified=Base.FULL_SIMPLIFY)

    def FPV(self, *args):
        return FPI(self, FPV(*args), variables=set(), symbolic=False, simplified=Base.FULL_SIMPLIFY)

    # Bitwise ops
    # TODO: some of these types don't make sense (maybe)
    def LShR(self, *args): return type(args[0])(self, 'LShR', args).reduced
    def SignExt(self, *args): return type(args[0])(self, 'SignExt', args).reduced
    def ZeroExt(self, *args): return type(args[0])(self, 'ZeroExt', args).reduced
    def Extract(self, *args): return type(args[0])(self, 'Extract', args).reduced
    def Concat(self, *args):
        if len(args) == 1:
            return args[0]
        else:
            return type(args[0])(self, 'Concat', args).reduced
    def RotateLeft(self, *args): return type(args[0])(self, 'RotateLeft', args).reduced
    def RotateRight(self, *args): return type(args[0])(self, 'RotateRight', args).reduced
    def Reverse(self, o): return type(args[0])(self, 'Reverse', (o,), collapsible=False).reduced

    #
    # Strided interval
    #
    def StridedInterval(self, name=None, bits=0, lower_bound=None, upper_bound=None, stride=None, to_conv=None):
        si = BackendVSA.CreateStridedInterval(name=name,
                                            bits=bits,
                                            lower_bound=lower_bound,
                                            upper_bound=upper_bound,
                                            stride=stride,
                                            to_conv=to_conv)
        return I(self, si, variables={ si.name }, symbolic=False)
    SI = StridedInterval

    def TopStridedInterval(self, bits, name=None, signed=False, uninitialized=False):
        si = BackendVSA.CreateTopStridedInterval(bits=bits, name=name, signed=signed, uninitialized=uninitialized)
        return I(self, si, variables={ si.name }, symbolic=False)
    TSI = TopStridedInterval

    # Value Set
    def ValueSet(self, **kwargs):
        vs = ValueSet(**kwargs)
        return I(self, vs, variables={ vs.name }, symbolic=False)
    VS = ValueSet

    # a-loc
    def AbstractLocation(self, *args, **kwargs): #pylint:disable=no-self-use
        aloc = AbstractLocation(*args, **kwargs)
        return aloc

    #
    # Boolean ops
    #
    def BoolVal(self, *args):
        return Bool(self, 'I', (args[0],), variables=set(), symbolic=False)
        #return self._do_op('BoolVal', args, variables=set(), symbolic=False, raw=True)

    def And(self, *args): return type(args[0])(self, 'And', args).reduced
    def Not(self, *args): return type(args[0])(self, 'Not', args).reduced
    def Or(self, *args): return type(args[0])(self, 'Or', args).reduced
    def ULT(self, *args): return type(args[0])(self, 'ULT', args).reduced
    def ULE(self, *args): return type(args[0])(self, 'ULE', args).reduced
    def UGE(self, *args): return type(args[0])(self, 'UGE', args).reduced
    def UGT(self, *args): return type(args[0])(self, 'UGT', args).reduced

    #
    # Other ops
    #
    def If(self, *args):
        if len(args) != 3: raise ClaripyOperationError("invalid number of args passed to If")
        if type(args[1]) != type(args[2]): raise ClaripyOperationError("differently-typed args passed to If")
        return type(args[1])(self, 'If', args).reduced

    #def size(self, *args): return self._do_op('size', args)

    def ite_dict(self, i, d, default):
        return self.ite_cases([ (i == c, v) for c,v in d.items() ], default)

    def ite_cases(self, cases, default):
        sofar = default
        for c,v in reversed(cases):
            sofar = self.If(c, v, sofar)
        return sofar

    def simplify(self, e):
        for b in self.model_backends:
            try: return b.simplify(e)
            except BackendError: pass

        l.debug("Simplifying via solver backend")

        for b in self.solver_backends:
            try: return b.simplify(e)
            except BackendError: pass

        l.debug("Unable to simplify expression")
        return e

    def is_true(self, e):
        for b in self.model_backends:
            try: return b.is_true(b.convert(e))
            except BackendError: pass

        l.debug("Unable to tell the truth-value of this expression")
        return False

    def is_false(self, e):
        for b in self.model_backends:
            try: return b.is_false(b.convert(e))
            except BackendError: pass

        l.debug("Unable to tell the truth-value of this expression")
        return False

    def is_identical(self, *args):
        '''
        Attempts to check if the underlying models of the expression are identical,
        even if the hashes match.

        This process is somewhat conservative: False does not necessarily mean that
        it's not identical; just that it can't (easily) be determined to be identical.
        '''
        if not all([isinstance(a, A) for a in args]):
            return False

        if len(set(hash(a) for a in args)) == 1:
            return True

        first = args[0]
        identical = None
        for o in args:
            for b in self.model_backends:
                try:
                    i = b.identical(first, o)
                    if identical is None:
                        identical = True
                    identical &= i is True
                except BackendError:
                    pass

        return identical is True

    def constraint_to_si(self, expr): #pylint:disable=unused-argument
        '''
        Convert a constraint to SI if possible
        :param expr:
        :return:
        '''
        ret = True, [ ]

        for b in self.model_backends:
            if type(b) is BackendVSA:
                ret = b.constraint_to_si(expr)

        return ret

    def model_object(self, e, result=None):
        for b in self.model_backends:
            try: return b.convert(e, result=result)
            except BackendError: pass
        raise BackendError('no model backend can convert expression')

from .ast import Base, BV, BVI, FP, FPI, Bool
from .backend import BackendError
from .bv import BVV
from .fp import FPV
from .vsa import ValueSet, AbstractLocation
from .backends import BackendVSA
from .errors import ClaripyOperationError
