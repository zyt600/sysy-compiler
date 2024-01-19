#pragma once
#include <iostream>
#include "koopa.h"
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

string raw_value_tag(int tag);
string raw_value_kind(int tag);
string raw_binary_op(int op);
string raw_type_tag(int tag);
string processIR(string s);
void file_append(string s, const char* output="/root/compiler/mydebug/testIRcode.txt");