#!/bin/bash

pushd ../build
ninja
popd

function tst() {
    SS=$1 ../build/out 2>data-$1-small.txt
}

tst 10
tst 600
tst 15000
tst 60000
