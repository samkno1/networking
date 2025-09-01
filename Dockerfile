FROM ubuntu:22.04

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
	bash \
	build-essential \
	ca-certificates \
	gcc \
	git \
	wget \
	xz-utils \
	&& update-ca-certificates \
	&& rm -rf /var/lib/apt/lists/*

RUN wget -q https://toolchains.bootlin.com/downloads/releases/toolchains/aarch64/tarballs/aarch64--glibc--stable-2025.08-1.tar.xz \
	--no-check-certificate \
	&& tar -xJf aarch64--glibc--stable-2025.08-1.tar.xz -C /opt \
	&& rm aarch64--glibc--stable-2025.08-1.tar.xz

ENV PATH="/opt/aarch64--glibc--stable-2025.08-1/bin:${PATH}"

WORKDIR /opt

RUN mkdir aarch64

RUN git clone https://github.com/samkno1/networking.git
