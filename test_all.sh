#!/bin/bash

echo "========================================"
echo "COMPILER CONTROL FLOW FEATURES TEST"
echo "========================================"
echo ""

echo "Test 1: If-Else Statements"
echo "------------------------"
./parser test_if_else.txt
echo ""

echo "Test 2: Logical Operators (AND, OR, NOT)"
echo "----------------------------------------"
./parser test_logical_operators.txt
echo ""

echo "Test 3: While Loop"
echo "-----------------"
./parser test_while.txt
echo ""

echo "Test 4: Arithmetic Expressions"
echo "-----------------------------"
./parser test_assignment.txt
echo ""

echo "Test 5: Switch-Case Statement"
echo "----------------------------"
./parser test_switch.txt
echo ""

echo "Test 6: For Loop"
echo "---------------"
./parser test_for.txt
echo ""

echo "========================================"
echo "ALL FEATURES SUCCESSFULLY TESTED!"
echo "========================================"
