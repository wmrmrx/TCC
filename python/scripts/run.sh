#!/bin/bash
set -e
for i in {1..100}
do
    echo "Running iteration $i"
    make build_and_run_random
done