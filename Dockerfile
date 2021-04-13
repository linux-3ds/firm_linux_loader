FROM debian:buster-slim
RUN apt-get update && apt-get install -y gcc-arm-none-eabi binutils-arm-none-eabi build-essential git python3-pip
WORKDIR /firm_linux_loader
RUN pip3 install -U git+https://github.com/TuxSH/firmtool.git
ENV CC=arm-none-eabi-gcc
