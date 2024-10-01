#!/bin/env bash

set -e

# Path to the PID file
PID_FILE="daemon.pid"

# Check if the PID file exists
if [[ ! -f "$PID_FILE" ]]; then
    echo "Error: PID file not found."
    exit 1
fi

# Loop through each PID in the file and attempt to kill the process
while IFS= read -r PID; do
    # Extract just the PID (ignoring any label like "socketcand: PID")
    PID_NUMBER=$(echo "$PID" | awk '{print $NF}')

    # Check if the process exists before killing
    if kill -0 "$PID_NUMBER" 2>/dev/null; then
        echo "Killing process with PID: $PID_NUMBER"
        kill "$PID_NUMBER"
    else
        echo "Process with PID $PID_NUMBER not found or already terminated."
    fi
done < "$PID_FILE"

# Optionally, clear the PID file after killing all processes
rm "$PID_FILE"
echo "All processes in $PID_FILE have been terminated."