# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2017, libcork authors
# All rights reserved.
#
# Please see the COPYING file in this distribution for license details.
# ----------------------------------------------------------------------

# Generates some test cases for our u128 types.

from __future__ import print_function

import operator
import random
import sys

test_count = 25000
random.seed()

def random_shift_count():
    # We want to test values [0, 128)
    return int(random.random() * 128)

def random_128():
    result = 0
    for i in range(4):
        result = result << 32
        result = result + int(random.random() * 2**32)
    return result

def dec_128(value):
    return format(value, "40d")

def hex_128(value):
    return ("UINT64_C(0x" + format(value >> 64, "016x") + "), " +
            "UINT64_C(0x" + format(value & 0xffffffffffffffff, "016x") + ")")


def output_one_cmp_test_case(op, op_str, lhs, rhs):
    result = op(lhs, rhs)
    result_str = "    " if result else "not "
    result_c_str = "true" if result else "false"
    print()
    print("/*        ", dec_128(lhs), sep="")
    print(" * ", result_str, op_str, " ", dec_128(rhs), sep="")
    print(" */")
    print("{", sep="")
    print("    ", hex_128(lhs), ",", sep="")
    print("    ", hex_128(rhs), ",", sep="")
    print("    ", result_c_str, sep="")
    print("},", sep="")

def create_one_cmp_test_case(op, op_str):
    lhs = random_128()
    rhs = random_128()
    output_one_cmp_test_case(op, op_str, lhs, lhs)
    output_one_cmp_test_case(op, op_str, lhs, rhs)
    output_one_cmp_test_case(op, op_str, rhs, rhs)


def create_one_shl_test_case():
    lhs = random_128()
    rhs = random_shift_count()
    result = (lhs << rhs) % 2**128
    print()
    print("/*    ", dec_128(lhs), sep="")
    print(" * << ", rhs, sep="")
    print(" *  = ", dec_128(result), sep="")
    print(" */")
    print("{", sep="")
    print("    ", hex_128(lhs), ",", sep="")
    print("    ", rhs, ",", sep="")
    print("    ", hex_128(result), sep="")
    print("},", sep="")


def create_one_shr_test_case():
    lhs = random_128()
    rhs = random_shift_count()
    result = (lhs >> rhs) % 2**128
    print()
    print("/*    ", dec_128(lhs), sep="")
    print(" * >> ", rhs, sep="")
    print(" *  = ", dec_128(result), sep="")
    print(" */")
    print("{", sep="")
    print("    ", hex_128(lhs), ",", sep="")
    print("    ", rhs, ",", sep="")
    print("    ", hex_128(result), sep="")
    print("},", sep="")


def create_one_add_test_case():
    lhs = random_128()
    rhs = random_128()
    result = (lhs + rhs) % 2**128
    print()
    print("/*    ", dec_128(lhs), sep="")
    print(" *  + ", dec_128(rhs), sep="")
    print(" *  = ", dec_128(result), sep="")
    print(" */")
    print("{", sep="")
    print("    ", hex_128(lhs), ",", sep="")
    print("    ", hex_128(rhs), ",", sep="")
    print("    ", hex_128(result), sep="")
    print("},", sep="")


def create_one_sub_test_case():
    lhs = random_128()
    rhs = random_128()
    if lhs < rhs:
        lhs, rhs = rhs, lhs
    result = (lhs - rhs) % 2**128
    print()
    print("/*    ", dec_128(lhs), sep="")
    print(" *  - ", dec_128(rhs), sep="")
    print(" *  = ", dec_128(result), sep="")
    print(" */")
    print("{", sep="")
    print("    ", hex_128(lhs), ",", sep="")
    print("    ", hex_128(rhs), ",", sep="")
    print("    ", hex_128(result), sep="")
    print("},", sep="")


def create_one_mul_test_case():
    lhs = random_128()
    rhs = random_128()
    result = (lhs * rhs) % 2**128
    print()
    print("/*    ", dec_128(lhs), sep="")
    print(" *  * ", dec_128(rhs), sep="")
    print(" *  = ", dec_128(result), sep="")
    print(" */")
    print("{", sep="")
    print("    ", hex_128(lhs), ",", sep="")
    print("    ", hex_128(rhs), ",", sep="")
    print("    ", hex_128(result), sep="")
    print("},", sep="")


def create_one_div_test_case():
    lhs = random_128()
    rhs = random_128()
    if rhs == 0:
        rhs = 1
    if lhs < rhs:
        lhs, rhs = rhs, lhs
    result = (lhs // rhs) % 2**128
    print()
    print("/*    ", dec_128(lhs), sep="")
    print(" *  / ", dec_128(rhs), sep="")
    print(" *  = ", dec_128(result), sep="")
    print(" */")
    print("{", sep="")
    print("    ", hex_128(lhs), ",", sep="")
    print("    ", hex_128(rhs), ",", sep="")
    print("    ", hex_128(result), sep="")
    print("},", sep="")


def create_one_mod_test_case():
    lhs = random_128()
    rhs = random_128()
    if rhs == 0:
        rhs = 1
    if lhs < rhs:
        lhs, rhs = rhs, lhs
    result = (lhs % rhs) % 2**128
    print()
    print("/*    ", dec_128(lhs), sep="")
    print(" *  % ", dec_128(rhs), sep="")
    print(" *  = ", dec_128(result), sep="")
    print(" */")
    print("{", sep="")
    print("    ", hex_128(lhs), ",", sep="")
    print("    ", hex_128(rhs), ",", sep="")
    print("    ", hex_128(result), sep="")
    print("},", sep="")


if len(sys.argv) == 1:
    print("Usage: create-u128-test-cases.py [operator]")
    sys.exit(1)

if len(sys.argv) > 2:
    sys.stdout = open(sys.argv[2], 'w')

print("/* This file is autogenerated.  DO NOT EDIT! */")

for i in range(test_count):
    if sys.argv[1] == "eq":
        create_one_cmp_test_case(operator.eq, "==")
    elif sys.argv[1] == "ne":
        create_one_cmp_test_case(operator.ne, "!=")
    elif sys.argv[1] == "lt":
        create_one_cmp_test_case(operator.lt, "< ")
    elif sys.argv[1] == "le":
        create_one_cmp_test_case(operator.le, "<=")
    elif sys.argv[1] == "gt":
        create_one_cmp_test_case(operator.gt, "> ")
    elif sys.argv[1] == "ge":
        create_one_cmp_test_case(operator.ge, ">=")
    elif sys.argv[1] == "shl":
        create_one_shl_test_case()
    elif sys.argv[1] == "shr":
        create_one_shr_test_case()
    elif sys.argv[1] == "add":
        create_one_add_test_case()
    elif sys.argv[1] == "sub":
        create_one_sub_test_case()
    elif sys.argv[1] == "mul":
        create_one_mul_test_case()
    elif sys.argv[1] == "div":
        create_one_div_test_case()
    elif sys.argv[1] == "mod":
        create_one_mod_test_case()
