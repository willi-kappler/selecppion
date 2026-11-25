#!/usr/bin/env bash

EXECUTABLE="../../builddir/neural_net2_example"

run_1() {
    # Start 6 nodes:
    $EXECUTABLE --prefix "1_1_b_" -i 10000 -m 1 -t 1 -b 5 &
    sleep 1
    $EXECUTABLE --prefix "2_1_b_" -i 10000 -m 1 -t 1 -b 10 &
    sleep 1
    $EXECUTABLE --prefix "3_1_b_" -i 10000 -m 1 -t 1 -b 15 &
    sleep 1
    $EXECUTABLE --prefix "4_1_b_" -i 10000 -m 1 -t 1 -b 20 &
    sleep 1
    $EXECUTABLE --prefix "5_1_b_" -i 10000 -m 1 -t 1 -b 25 &
    sleep 1
    $EXECUTABLE --prefix "6_8_" -i 10000 -m 1 -t 8 &
}

run_2() {
    # Start 6 nodes:
    $EXECUTABLE --prefix "1_01_r_" -i 10000 -m 1 -t 1 -r &
    sleep 1
    $EXECUTABLE --prefix "2_01_" -i 10000 -m 1 -t 1 &
    sleep 1
    $EXECUTABLE --prefix "3_08_" -i 10000 -m 1 -t 8 &
    sleep 1
    $EXECUTABLE --prefix "4_13_" --poffset 5 -i 10000 -m 1 -t 13 &
    sleep 1
    $EXECUTABLE --prefix "5_13_" --poffset 10 -i 10000 -m 1 -t 13 &
    sleep 1
    $EXECUTABLE --prefix "6_13_" --poffset 15 -i 10000 -m 1 -t 13 &
}

run_3() {
    # Start 6 nodes:
    $EXECUTABLE --prefix "1_13_" --poffset 5 -i 10000 -m 1 -t 13 &
    sleep 1
    $EXECUTABLE --prefix "2_13_" --poffset 10 -i 10000 -m 1 -t 13 &
    sleep 1
    $EXECUTABLE --prefix "3_13_" --poffset 15 -i 10000 -m 1 -t 13 &
    sleep 1
    $EXECUTABLE --prefix "4_14_" --poffset 5 -i 10000 -m 1 -t 14 &
    sleep 1
    $EXECUTABLE --prefix "5_14_" --poffset 10 -i 10000 -m 1 -t 14 &
    sleep 1
    $EXECUTABLE --prefix "6_14_" --poffset 15 -i 10000 -m 1 -t 14 &
}

run_4() {
    # Start 6 nodes:
    $EXECUTABLE --prefix "1_1_r_" -i 10000 -m 1 -t 1 -r &
    sleep 1
    $EXECUTABLE --prefix "2_1_r_" -i 10000 -m 1 -t 1 -r &
    sleep 1
    $EXECUTABLE --prefix "3_1_r_" -i 10000 -m 1 -t 1 -r &
    sleep 1
    $EXECUTABLE --prefix "4_1_r_" -i 10000 -m 1 -t 1 -r &
    sleep 1
    $EXECUTABLE --prefix "5_1_r_" -i 10000 -m 1 -t 1 -r &
    sleep 1
    $EXECUTABLE --prefix "6_1_" -i 10000 -m 1 -t 1 &
}

# Start the server:
$EXECUTABLE --server &
sleep 1

run_1
sleep 1
echo ""
