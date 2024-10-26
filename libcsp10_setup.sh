#!/usr/bin/env bash
set -e
sudo apt-get update
sudo apt-get install -y build-essential
cd ~; mkdir python3.4; cd python3.4; wget https://www.python.org/ftp/python/3.4.7/Python-3.4.7.tgz;
tar -zxvf Python-3.4.7.tgz; cd Python-3.4.7; ./configure --prefix=/opt/python3.4.7; make -j2; sudo make install
sudo ln -s /opt/python3.4.7/bin/python3 /usr/bin/python

sudo apt-get install -y pkg-config python3-dev libsocketcan2 libsocketcan-dev libzmq3-dev
cd ~;git clone https://github.com/libcsp/libcsp.git; cd libcsp
git checkout libcsp-1
# Fix compile error
sed -i "s/if (csp_assert_fail_action)/if (csp_assert_fail_action != NULL)/" include/csp/csp_debug.h
set +e
./waf configure # This prevents segfault from the following command. Dont't know why the following command causes segfault when it's invoked at first on libcsp-1
set -e
./waf configure  --prefix=/usr --enable-shlib --enable-python3-bindings --enable-examples --enable-can-socketcan --enable-if-zmqhub --with-driver-usart=linux --enable-promisc
./waf build install
