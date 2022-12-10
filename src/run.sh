#!/bin/sh

for file in ../traces/*.bz2 ; do
    echo "dataset: $file"
    bunzip2 -kc $file | ./predictor --$1
done