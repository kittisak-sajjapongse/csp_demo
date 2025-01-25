#!/usr/bin/env bash
#
# kill_csp_processes.sh
#
# 1) Find all processes matching "csp_" (except the grep command itself)
# 2) Send SIGKILL (-9) to each of those PIDs

# Grab the PIDs of processes whose names contain "csp_"
pids=$(ps aux | grep csp_ | grep -v grep | awk '{print $2}')

# If no matching PIDs were found, exit
if [ -z "$pids" ]; then
  echo "No csp_ processes found."
  exit 0
fi

# Otherwise, kill them all
echo "Killing the following csp_ processes: $pids"
kill -9 $pids

