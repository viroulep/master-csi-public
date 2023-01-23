#!/bin/bash

set -xe

apt-get update
apt-get install -yq --no-install-recommends \
  apt-transport-https vim emacs gcc g++ binutils ca-certificates \
  gdb cmake lsb-core software-properties-common gpg-agent zlib1g-dev \
  git file wget curl ninja-build unzip sudo python3 python3-pip make xz-utils \

apt-get autoremove -yq
apt-get clean -yqq

pip3 install lit filecheck
