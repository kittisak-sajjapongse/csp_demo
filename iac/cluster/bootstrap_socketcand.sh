#!/usr/bin/env bash

set -e

git clone https://github.com/linux-can/socketcand.git
cd socketcand
sudo apt-get install -y autoconf build-essential
autoupdate
./autogen.sh
./configure
make
sudo make install

sudo socketcand -i vcan0 -l eth1 -v
sudo socketcandcl -s 10.10.0.2 -i vcan0,vcan0