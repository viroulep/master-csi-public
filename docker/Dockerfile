FROM ubuntu:jammy

ENV DEBIAN_FRONTEND=noninteractive
RUN useradd -ms /bin/bash tp
RUN mkdir /scripts
COPY install-essentials.sh /scripts
COPY build-install-llvm.sh /scripts
WORKDIR /scripts
RUN /scripts/install-essentials.sh
RUN /scripts/build-install-llvm.sh

WORKDIR /home/tp

RUN adduser tp sudo
# Don't do this at home
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

USER tp
