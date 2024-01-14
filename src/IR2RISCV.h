#pragma once
#include <iostream>
#include "koopa.h"
#include <cassert>
#define DEBUG 1
using namespace std;

void Visit(koopa_raw_program_t raw, string &rsicV_code);
void Visit(koopa_raw_function_t func, string &rsicV_code);
void Visit(koopa_raw_basic_block_t bb, string &rsicV_code);
void Visit(koopa_raw_value_t value, string &rsicV_code);



