ARG VARIANT=ubuntu-22.04
FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-${VARIANT}

LABEL maintainer="Bernd Doser <bernd.doser@braintwister.eu>"

RUN apt-get update \
 && export DEBIAN_FRONTEND=noninteractive \
 && apt-get -y install --no-install-recommends \
       apt-transport-https \
       build-essential \
       ca-certificates \
       curl \
       dirmngr \
       ffmpeg \
       git \
       libsm6 \
       libxext6 \
       python3 \
       python3-dev \
       python3-pip \
       software-properties-common \
       sudo \
       vim \
       wget \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/*

RUN python3 -m pip install --upgrade pip

RUN pip install \
      argparse \
      black \
      cmake \
      conan \
      matplotlib \
      networkx \
      numba \
      numpy \
      opencv-python \
      pandas \
      pylint \
      pytest \
      pyyaml \
      scikit-learn \
      scipy
