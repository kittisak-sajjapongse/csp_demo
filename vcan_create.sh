#!/usr/bin/env bash

create_vcan_interfaces() {
    local num_vcan=$1
    for ((i = 0; i < NUM_VCAN; i++)); do
        local interface_name="vcan$i"
        sudo ip link add dev "$interface_name" type vcan
        sudo ip link set up "$interface_name"
        echo "Virtual CAN interface $i ($interface_name) ... created"
    done
}

create_can_gateway_rules() {
    local num_vcan=$1

    for ((src_if = 0; src_if < num_vcan; src_if++)); do
        for ((dst_if = 0; dst_if < num_vcan; dst_if++)); do
            if [ "$src_if" -ne "$dst_if" ]; then
                sudo cangw -A -s "vcan$src_if" -d "vcan$dst_if" -e
                echo "Rule $src_if -> $dst_if ... created"
            fi
        done
    done
}

set -e
# Check if NUM_VCAN is provided
if [ -z "$1" ]; then
    echo "Usage: $0 NUM_VCAN"
    exit 1
fi

# Validate that NUM_VCAN is numeric
if ! [[ "$1" =~ ^[0-9]+$ ]]; then
    echo "Error: NUM_VCAN must be a numeric value."
    exit 1
fi

# Validate that NUM_VCAN is not greater than 10
if [ "$1" -gt 10 ]; then
    echo "Error: NUM_VCAN must not be greater than 10."
    exit 1
fi

# Number of virtual CAN interfaces to create
NUM_VCAN=$1

# Install and load vcan kernel module - the command below need a restart to pick up the vcan module
# References: 
# [1] - https://www.reddit.com/r/aws/comments/17m3lue/ubuntu_2204_upgraded_to_kernel_6201015aws_missing/
# [2] - https://stackoverflow.com/questions/21022749/how-to-create-virtual-can-port-on-linux-c
sudo apt install linux-modules-extra-$(uname -r) can-utils net-tools
sudo modprobe vcan

# Create VCAN IFs
create_vcan_interfaces $NUM_VCAN

# Load CAN gateway kernel module and attach all VCAN IFs to the same bus
# References:
# [1] - https://stackoverflow.com/questions/54296852/how-to-connect-two-vcan-ports-in-linux
sudo modprobe can-gw
create_can_gateway_rules $NUM_VCAN
