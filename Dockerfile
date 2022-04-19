# Use build.sh to build this in most circumstances
# If building directly, this is meant to be built with docker buildkit enabled:
#    export DOCKER_BUILDKIT=1
# Note that build arguments to this Dockerfile are not comprehensive w.r.t. all
# CMake options. Edit the CMake stage code directly to add or remove cmake
# options as desired.


# Set this to base if no dev stuff is desired
# Set to base_dev_extras to also install optional packages
ARG BASE_STAGE=base_dev

# Set this to config_verbose if desired
ARG CONFIG_STAGE=config


##################################################
#                   Base Stage                   #
##################################################


FROM ubuntu:20.04 as base
LABEL stage=base
SHELL [ "/bin/bash", "-ecux" ]

# Optional build args
ARG CXX="g++"

# Prep apt
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update
RUN apt-get install -yq \
	python3 \
	gnupg2 \
	wget \
	git

# Install pip and prep python
RUN apt-get install -yq python3-pip python3.8-venv
ENV VIRTUAL_ENV=/venv
RUN python3 -m venv "${VIRTUAL_ENV}"
ENV PATH="${VIRTUAL_ENV}/bin:${PATH}"
RUN pip3 install --upgrade pip

# Install required dependencies

# Native dependencies
RUN apt-get install -yq "${CXX}" make

# TODO: until a new z3 release
RUN git clone --depth 1 https://github.com/Z3Prover/z3 /z3-src \
    && cd /z3-src/src/api/python \
	&& python setup.py sdist \
	&& cd dist \
	&& tar -xvf * \
	&& rm *.gz \
	&& cd * \
	&& pip install . \
	&& cd / \
	&& rm -rf /z3-src


FROM base as base_dev
LABEL stage=base_dev
ENV DEV_MODE=1

# Improved backtraces
RUN apt-get install -yq libdw-dev

# Python dependencies
RUN pip3 install \
	'setuptools>=39.6.0' \
	requests \
	wheel \
	cmake \
	tqdm
	# TODO: Once new z3 comes out 'z3-solver>=4.8.15.0' \

FROM base_dev as base_dev_extras
LABEL stage=base_dev_extras

RUN pip3 install clang-format \
RUN apt-get install -yq \
    \
    `# Documentation` \
    graphviz \
    doxygen \
    \
    `# Static Analysis` \
    clang-tidy \
    cppcheck \
    iwyu \
    `# Dynamic Analysis` \
    valgrind


##################################################
#                  Config Stage                  #
##################################################


FROM "${BASE_STAGE}" as config
LABEL stage=config

# Optional build arguments
ARG CTEST_OUTPUT_ON_FAILURE=1

# Constants
ENV CLARIPY="/claripy/" \
	CTEST_OUTPUT_ON_FAILURE="${CTEST_OUTPUT_ON_FAILURE}"
ENV BUILD="${CLARIPY}/native/build/"
ENV CXX="${CXX}"

# Get source
RUN mkdir "${CLARIPY}"
COPY . "${CLARIPY}"
WORKDIR "${CLARIPY}"

# Verbose config stage
FROM config as config_verbose
LABEL stage=config_verbose
ENV VERBOSE=1


##################################################
#              setup.py pip stages               #
##################################################


FROM "${CONFIG_STAGE}" as install
LABEL stage=install
# TODO: remove once new z3 comes out
RUN pip3 install \
	'setuptools>=39.6.0' \
	requests \
	wheel \
	cmake \
	tqdm
RUN pip3 install --no-build-isolation --verbose .
# TODO: once new z3 comes out. RUN pip3 install --verbose .


##################################################
#              setup.py dev stages               #
##################################################


# Let's test things individually
# If a setp fails, this makes debugging easier
# All stages which derive from sdist do for only for speed

FROM "${CONFIG_STAGE}" as clean
LABEL stage=clean
RUN if [[ "${DEV_MODE}" -ne 1 ]]; then \
		echo "To run the python setup.py dev stages you must use the base_dev base stage" \
		exit 1; \
	fi
RUN python setup.py clean

FROM clean as sdist
LABEL stage=sdist
RUN python setup.py sdist

FROM sdist as build
LABEL stage=build
RUN python setup.py build

FROM build as bdist_wheel
LABEL stage=bdist_wheel
RUN python setup.py bdist_wheel

FROM sdist as build_tests
LABEL stage=build_tests
RUN python setup.py native --debug --tests

FROM sdist as docs
LABEL stage=docs
RUN apt-get install -yq graphviz doxygen
RUN python setup.py native --docs


##################################################
#                   Test Stage                   #
##################################################


FROM build_tests as test
LABEL stage=test
RUN cd "${BUILD}" && ctest .
