#!/usr/bin/env bash

EXECUTABLE="../../builddir/neural_net2_example"

run_type() {
    # Start 6 nodes:
    $EXECUTABLE -i 7000 -m 1 -t 1 -r &
    sleep 1
    $EXECUTABLE -i 8000 -m 1 -t 1 -r &
    sleep 1
    $EXECUTABLE -i 9000 -m 1 -t 1 -r &
    sleep 1
    $EXECUTABLE -i 10000 -m 1 -t 1 -r &
    sleep 1
    $EXECUTABLE -i 10000 -m 1 -t 1 &
    sleep 1
    $EXECUTABLE -i 10000 -m 1 -t 8 &
}

run_type2() {
    # Start 6 nodes:
    $EXECUTABLE -i 10000 -m 1 -t 1 &
    sleep 1
    $EXECUTABLE -i 10000 -m 1 -t 2 &
    sleep 1
    $EXECUTABLE -i 10000 -m 1 -t 8 &
    sleep 1
    $EXECUTABLE -i 10000 -m 1 -t 1 -r &
    sleep 1
    $EXECUTABLE -i 10000 -m 1 -t 2 -r &
    sleep 1
    $EXECUTABLE -i 10000 -m 1 -t 8 -r &
}

# Start the server:
$EXECUTABLE --server &
sleep 1

run_type
sleep 1
echo ""
