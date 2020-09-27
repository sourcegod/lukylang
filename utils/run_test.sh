#! /bin/bash
### Script for running luky tests
### Author: Coolbrother
### Date: sun, 27/09/2020

scriptDir=$(dirname $0)
testDir="$scriptDir/../tests"
progName="$scriptDir/../build/release/luky"
# echo "voici scriptDir: $scriptDir, et testDir: $testDir"
for fname in $testDir/*.luk; do
  $progName $fname
done

exit 0
