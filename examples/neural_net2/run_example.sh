#!/usr/bin/env bash

EXECUTABLE="../../builddir/neural_net2_example"

run_type() {
    # Start 6 nodes:
    $EXECUTABLE -i 10000 -m 1 -t 1 &
    sleep 1
    $EXECUTABLE -i 10000 -m 1 -t 8 &
    sleep 1
    $EXECUTABLE -i 10000 -m 1 -t 13 &
    sleep 1
    $EXECUTABLE -i 10000 -m 1 -t 14 &
    sleep 1
    $EXECUTABLE -i 10000 -m 1 -t 14 &
    sleep 1
    $EXECUTABLE -i 10000 -m 1 -t 14 &
}

run_type1() {
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

# Start the server:
$EXECUTABLE --server &
sleep 1

run_type
sleep 1
echo ""
