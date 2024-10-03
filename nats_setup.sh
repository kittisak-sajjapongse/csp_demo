#!/usr/bin/env bash

sudo apt-get install -y cmake openssl libssl-dev libprotobuf-dev libprotobuf-c-dev libprotobuf-c1
cd ~
git clone https://github.com/nats-io/nats.c.git
mkdir -p ~/nats.c/build
cd ~/nats.c/build
cmake ..
make

# This installs libnats.so and others into /usr/local/lib
sudo make install