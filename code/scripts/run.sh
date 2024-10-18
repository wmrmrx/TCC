#!/bin/bash
set -e
while true;
do
    echo "Running iteration $i"
    make build_and_run_random
done