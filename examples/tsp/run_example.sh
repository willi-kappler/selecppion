#!/usr/bin/env bash

EXECUTABLE="../../build/linux/x86_64/release/se_example_tsp"

run_type() {
    echo "Run type $1."

    # Start 4 nodes:
    $EXECUTABLE -i 20000 -m 1 -t $1 &
    sleep 1
    $EXECUTABLE -i 30000 -m 1 -r -t $1 &
    sleep 1
    $EXECUTABLE -i 40000 -m 1 -r -t $1 &
    sleep 1
    $EXECUTABLE -i 50000 -m 1 -r -t $1 &
}

run_all_types() {
    echo "Run all types."

    $EXECUTABLE -i 20000 -m 1 -t 1 &
    sleep 1
    $EXECUTABLE -i 20000 -m 1 -t 2 &
    sleep 1
    $EXECUTABLE -i 20000 -m 1 -t 3 &
    sleep 1
    $EXECUTABLE -i 20000 -m 1 -t 4 &
    sleep 1
    $EXECUTABLE -i 20000 -m 1 -t 5 &
    sleep 1
    $EXECUTABLE -i 2000 -m 10 -t 6 &
    sleep 1
    $EXECUTABLE -i 20000 -m 1 -t 7 &
    sleep 1
    $EXECUTABLE -i 20000 -m 1 -t 8 &
    sleep 1
    $EXECUTABLE -i 20000 -m 1 -t 9 &
    sleep 1
    $EXECUTABLE -i 20000 -m 1 -t 10 &
    sleep 1
}

# Start the server:
$EXECUTABLE --server &
sleep 1

run_type 1
# run_all_types
