#! /bin/bash
### Script for managing luky application.
### Author: Coolbrother
### Date: Tue, 30/09/2020

# globals variables
appName="$0"
rootDir=$(dirname $0)
testDir="$rootDir/tests"
lukApp="$rootDir/build/release/luky"
lukDebugApp="$rootDir/build/debug/luky_debug"
sconsFile="$rootDir/SConstruct"
excludeFile="17_03_native_readln.luk"
# echo "Here is rootDir: $rootDir, et testDir: $testDir"

# some files to test
declare -a someFiles=("04_01_hello.luk" "04_02_calc.luk"
    "06_01_condition_1.luk" 
    "08_var.luk" "10_01_loop_while.luk"
    "11_01_func_1.luk" "11_05_fact.luk" "11_08_fib_3.luk"
    "11_10_lambda_1.luk"
    "12_03_class_getter.luk" "12_04_class_this.luk" "12_10_class_init.luk"
    "13_04_superclass_super.luk" 
    "17_02_native_println.luk" )

# echo -e "somefiles: ${someFiles[@]}\n"
help="$appName usage: 
    -h, help: print this help
    -b, build: building app
    -B, builddbg: building debug version
    run: running app.
    rundbg: debug app
    -c, com: run from commandline
    -C, comdbg: run debug from commandline
    -f: run from file
    -F: run debug from file
    -t, test: running some tests
    -T, testall: running all tests
    "

# check whether lukyApp exists
function CheckFile() {
    if [ ! -e "$1" ]; then 
        echo "$appName Error: $1 does not exists, please build it before running."
        exit 1
    fi
}

# Printing Help
if [[ "$1" = "-h" || "$1" = "help" ]]; then
    echo "$help"
    exit 0

# build normal version app
elif [[ "$1" = "-b" || "$1" = "build" ]]; then
    CheckFile $sconsFile
    scons

# build debug version app
elif [[ "$1" = "-B" || "$1" = "builddbg" ]]; then
    CheckFile $sconsFile
    scons debug

# run normal version REPL
elif [[ -z "$1" || "$1" = "run" ]]; then
    CheckFile $lukApp
    rlwrap $lukApp

# run debug version REPL
elif [[ "$1" = "rundbg" ]]; then
    CheckFile $lukDebugApp
    rlwrap $lukDebugApp

# run normal version from commandline
elif [[ "$1" = "-c" || "$1" = "com" ]]; then
    CheckFile $lukApp
    $lukApp -c "$2"

# run debug version from commandline
elif [[ "$1" = "-C" || "$1" = "comdbg" ]]; then
    CheckFile $lukDebugApp
    $lukDebugApp -c "$2"

# run normal version with file
elif [ "$1" = "-f" ]; then
    CheckFile $lukApp
    $lukApp "$2"

# run debug version with file
elif [ "$1" = "-F" ]; then
    CheckFile $lukDebugApp
    $lukDebugApp "$2"
# running some tests
elif [[ "$1" = "-t" || "$1" = "test" ]]; then
    CheckFile $lukApp
    echo -e "Running some tests\n"
    for fname in ${someFiles[@]}; do
        name=$(basename $fname)
        echo "File name: $name"
        $lukApp "$testDir/$fname"
    done
    echo -e "\nEnd test";

# running all tests
elif [[ "$1" = "-T" || "$1" = "testall" ]]; then
    CheckFile $lukApp
    echo -e "Running all tests\n"
    for fname in $testDir/*.luk; do
        name=$(basename $fname)
        echo "File name: $name"
        if [ $name = $excludeFile ]; then
            echo -e "Pass...\n"
            continue
        fi
        $lukApp $fname
    done
    echo -e "\nEnd test";

else
    echo "$help"
    exit 1

fi

exit 0
