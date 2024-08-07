#!/usr/bin/env bash

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

set -e

remove_vcan_interface() {
    local num_vcan=$1
    for ((i = 0; i < NUM_VCAN; i++)); do
        sudo ip link delete "vcan$i"
        echo "Virtual CAN interface $i (vcan$i) ... deleted"
    done
}

remove_vcan_interface NUM_VCAN
