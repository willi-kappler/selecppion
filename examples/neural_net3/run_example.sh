#!/usr/bin/env bash

EXECUTABLE="../../builddir/neural_net3_example"

run_type() {
    # Start 6 nodes:
    $EXECUTABLE --prefix "1_1_r_" -i 10000 -m 1 -t 1 -r &
    sleep 1
    $EXECUTABLE --prefix "2_1_r_" -i 10000 -m 1 -t 1 -r &
    sleep 1
    $EXECUTABLE --prefix "3_1_" -i 10000 -m 1 -t 1 &
    sleep 1
    $EXECUTABLE --prefix "4_8_" -i 10000 -m 1 -t 8 &
    sleep 1
    $EXECUTABLE --prefix "5_13_" -i 10000 -m 1 -t 13 &
    sleep 1
    $EXECUTABLE --prefix "6_14_" -i 10000 -m 1 -t 14 &
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
