#!/usr/bin/env bash

EXECUTABLE="../../build/linux/x86_64/release/se_example_tsp"

# Start the server:
$EXECUTABLE --server &
sleep 2

# Start 4 nodes:
$EXECUTABLE -i 10000 &
sleep 1

$EXECUTABLE -i 10000 -r &
sleep 1

$EXECUTABLE -i 10000 -r &
sleep 1

$EXECUTABLE -i 10000 -r &
