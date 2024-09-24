#!/usr/bin/env bash

sudo apt-get install -y build-essentials cmake ninja-build libsocketcan2 libsocketcan-dev
git clone https://github.com/libcsp/libcsp.git
cd libcsp

sudo apt-get install -y python-dev-is-python3 #python-config required for building Python binding
sudo apt-get install -y pkg-config
sudo apt install -y libzmq3-dev
wget https://waf.io/waf-2.1.2
chmod 755 waf-2.1.2
mv waf-2.1.2 waf
# Look at  vi examples/buildall.py for options with waf
./waf configure --prefix=install --enable-shlib --enable-python3-bindings --enable-examples --enable-can-socketcan --enable-if-zmqhub --with-driver-usart=linux --with-os=posix --enable-promisc

# This will install at location /usr/local/lib/python3.12/dist-packages/libcsp_py3.cpython-312-x86_64-linux-gnu.so
sudo ./waf build install

# === CMake ===
# cmake -G Ninja -B builddir
# cmake --build builddir
