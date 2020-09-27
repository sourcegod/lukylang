#! /bin/bash
### Script for running luky tests
### Author: Coolbrother
### Date: sun, 27/09/2020

scriptDir=$(dirname $0)
testDir="$scriptDir/../tests"
progName="$scriptDir/../build/release/luky"
# echo "voici scriptDir: $scriptDir, et testDir: $testDir"
for fname in $testDir/*.luk; do
  name=$(basename $fname)
  if [ $name = "17_03_native_readln.luk" ]; then
    continue
  fi
  # echo $name
  $progName $fname
done
echo
echo "End test";
exit 0
