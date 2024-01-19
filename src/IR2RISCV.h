// IR to risc-V

#pragma once
#include <iostream>
#include "koopa.h"
#include <cassert>
#include "debug.h"
using namespace std;

void VisitIR(koopa_raw_program_t raw, string &rsicV_code);
void VisitIR(koopa_raw_function_t func, string &rsicV_code);
void VisitIR(koopa_raw_basic_block_t bb, string &rsicV_code);
void VisitIR(koopa_raw_value_t value, string &rsicV_code);
string VisitIR(koopa_raw_binary_t binary, string &rsicV_code);

class IRSymbolTable{
public:
    unordered_map<koopa_raw_value_t,int> symbolTable;
    int cnt = 0;

    int find(koopa_raw_value_t value){
        if(symbolTable.find(value) != symbolTable.end()){
            return symbolTable[value];
        }else{
            cnt++;
            symbolTable[value] = cnt*4;
            return symbolTable[value];
        }
    }

    bool exist(koopa_raw_value_t value){
        if(symbolTable.find(value) != symbolTable.end()){
            return true;
        }else{
            return false;
        }
    }
};

extern IRSymbolTable irSymbolTable;