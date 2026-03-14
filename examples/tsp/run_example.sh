#!/usr/bin/env bash

EXECUTABLE="../../build/linux/x86_64/release/se_example_tsp"

# Start the server:
$EXECUTABLE --server &
sleep 2

# Start 4 nodes:
$EXECUTABLE &
sleep 1

$EXECUTABLE &
sleep 1

$EXECUTABLE &
sleep 1

$EXECUTABLE &
