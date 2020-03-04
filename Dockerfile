FROM ubuntu:xenial

RUN apt-get update && apt-get install -y \
 build-essential \
 cmake \
 libboost-filesystem-dev \
 libboost-program-options-dev \
 libboost-regex-dev \
 libboost-system-dev \
 libboost-test-dev \
 libc6 \
 libgcc1 \
 libglib2.0-dev \
 libicu-dev \
 pkg-config \
 wget \
 zlibc \
 && rm -rf /var/lib/apt/lists/*

################################################################################
# Download and build GFSM libraries
# (required for Norma, and not available through repositories)

WORKDIR /tmp/gfsm
RUN wget http://kaskade.dwds.de/~moocow/mirror/projects/gfsm/gfsm-0.0.19-1.tar.gz
RUN tar xzf gfsm-0.0.19-1.tar.gz
WORKDIR /tmp/gfsm/gfsm-0.0.19-1
RUN sh ./configure --prefix=/usr && make && make install

WORKDIR /tmp/gfsm
RUN wget http://kaskade.dwds.de/~moocow/mirror/projects/gfsm/gfsmxl-0.0.17.tar.gz
RUN tar xzf gfsmxl-0.0.17.tar.gz
WORKDIR /tmp/gfsm/gfsmxl-0.0.17
RUN sh ./configure --prefix=/usr && make && make install

RUN rm -rf /tmp/gfsm

################################################################################

################################################################################
# Build Norma

WORKDIR /tmp/norma
COPY . .
WORKDIR /tmp/norma/build_docker
RUN cmake /tmp/norma \
 -DSTRING_IMPL=ICU \
 -DCMAKE_BUILD_TYPE=Release \
 -DCMAKE_INSTALL_PREFIX=/usr \
 && make \
 && make test \
 && make install

RUN rm -rf /tmp/norma

################################################################################

WORKDIR /home
ENV LANG=en_US.UTF-8

ENTRYPOINT ["/usr/bin/normalize"]
CMD ["/usr/bin/normalize", "--help"]
