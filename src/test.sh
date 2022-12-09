#!/bin/sh

predictor=$1
echo "test $predictor"
for file in ../traces/* ; do
    echo "dataset: $file"
    ./predictor --$predictor $file
done