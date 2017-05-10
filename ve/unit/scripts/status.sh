#!/bin/sh

testname=$1

no_tests=`grep 'Running 0 tests' run.log | wc -l`

if test $no_tests -ne 0; then
  echo "FAIL: $testname - no tests run"
else
  echo "PASS: $testname - no tests run"
fi
