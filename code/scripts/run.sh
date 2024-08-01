# i need to run the make build_and_run 100 times. the makefile is on my parent directory

#!/bin/bash
set -e
for i in {1..100}
do
    echo "Running iteration $i"
    make build_and_run_random
done