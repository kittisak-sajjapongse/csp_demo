#!/usr/bin/env bash

set -e
cd ~
git clone https://github.com/linux-can/socketcand.git
cd socketcand
sudo apt-get install -y autoconf build-essential net-tools | tee apt-get.log
autoupdate
./autogen.sh | tee autogen.log
./configure | tee configure.log
make | tee make.log
sudo make install | tee make_install.log