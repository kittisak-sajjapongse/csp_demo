#!/usr/bin/env bash

set -e

# Path to the configuration file
CONF_FILE="vbus.conf"
PID_FILE="daemon.pid"

# Clear the PID file if it exists
> "$PID_FILE"

# Get the IP address of the 'eth1' interface
CURRENT_IP=$(ip -4 addr show eth1 | grep -oP '(?<=inet\s)\d+(\.\d+){3}')

# Check if CURRENT_IP is empty (e.g., eth1 doesn't exist)
if [[ -z "$CURRENT_IP" ]]; then
    echo "Error: Unable to find IP address for eth1."
    exit 1
fi

# Loop through each line of the configuration file
while IFS=: read -r IP_ADDR SRC_INTERFACE; do
    # Skip if the IP address matches the current host
    if [[ "$IP_ADDR" == "$CURRENT_IP" ]]; then
        echo "Running socketcand for IP IF" $IP_ADDR $SRC_INTERFACE
        sudo socketcand -i "$SRC_INTERFACE" -l eth1 -v > /dev/null 2>&1 &
        echo "$!" >> "$PID_FILE"

        while IFS=: read -r IP_ADDR DST_INTERFACE; do
            if [[ "$IP_ADDR" == "$CURRENT_IP" ]]; then
                continue
            fi
            # Run the second command (2b) for the given IP address
            echo "        Running socketcandcl for IP IF: $IP_ADDR $SRC_INTERFACE $DST_INTERFACE"
            sudo socketcandcl -s "$IP_ADDR" -i "$SRC_INTERFACE","$DST_INTERFACE" > /dev/null 2>&1 &
            echo "$!" >> "$PID_FILE"
        done < "$CONF_FILE"
    fi
done < "$CONF_FILE"