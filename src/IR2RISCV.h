// IR to risc-V

#pragma once
#include <iostream>
#include "koopa.h"
#include <cassert>
#include "debug.h"
#include "koopaUtility.h"
using namespace std;

void VisitIR(koopa_raw_program_t raw, string &rsicV_code);
void VisitIR(koopa_raw_function_t func, string &rsicV_code);
void VisitIR(koopa_raw_basic_block_t bb, string &rsicV_code);
void VisitIR(koopa_raw_value_t value, string &rsicV_code);
string VisitIR(koopa_raw_binary_t binary, string &rsicV_code);
string loadRegister(koopa_raw_value_t value, string target);

class IRSymbolTable{
public:
    unordered_map<koopa_raw_value_t,int> symbolTable;  
    // 不是所有东西都是koopa_raw_value_t类型的，但方便起见，都这么存
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

    void change(koopa_raw_value_t value, int new_value){
        // if(symbolTable.find(value) == symbolTable.end()){
        //     cout<<"IRSymbolTable change error"<<endl;
        //     return;
        // }
        symbolTable[value] = new_value;
    }
};

extern IRSymbolTable irSymbolTable;