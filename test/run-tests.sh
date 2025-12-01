#!/usr/bin/bash
set -e  # exit on first failure

PROGRAM=./reservation

# Helper to print test info
run_test() 
{
    description="$1"
    shift
    echo "TEST: $description"
    echo "COMMAND: $PROGRAM $@"
    
    # Will fail if the program exits with non-zero status
    if $PROGRAM "$@"; then
        echo
        echo "RESULT: PASS ✓"
    else
        echo
        echo "RESULT: FAIL ✗"
    fi
    echo
}

echo "===========STARTING TESTS==========="
# 1) No arguments
echo "===================================="
run_test "TEST 1: NO ARGUMENTS" 
echo "===================================="

# 2) One argument
echo "===================================="
run_test "TEST 2: ONE ARGUMENT" 100
echo "===================================="

# 3) Three arguments
echo "===================================="
run_test "TEST 3: THREE ARGUMENTS" 100 1000 50
echo "===================================="

# 4) Huge numbers (customers > seats)
echo "===================================="
run_test "TEST 4: LARGE ARGUMENT VALUES" 300 150
echo "===================================="

# 5) Small numbers (customers < seats)
echo "===================================="
run_test "TEST 5: SMALL ARGUMENT VALUES" 2 200
echo "===================================="

# 6) First argument: 50 customers Second argument: 500 seed
echo "===================================="
run_test "TEST 6: CUSTOMERS: 50, SEED: 500" 50 500
echo "===================================="
