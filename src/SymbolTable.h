#pragma once
#include <unordered_map>
#include <iostream>
#include <variant>
#include "debug.h"
#include "GlobalCounter.h"
using namespace std;

extern int symbolTableLevel; // 当前符号表的层数

enum class SymbolTableElementType {
  INT = 0
};
extern SymbolTableElementType nextEleType; // 下一个要插入符号表的元素的类型

class SymbolTable{
  public:
    SymbolTable* father=nullptr,*son=nullptr;
    unordered_map<string,string> symbolTable;  // 符号表，映射 符号名称——存符号的临时变量名

    // 插入符号表。值得注意的是，这里查找重复符号仅查找本表，而不是递归查找
    bool insert(string name, string element){
      if(DEBUG){cout<<"SymbolTable insert "<<name<<" at "<<element<<endl;}
      if(symbolTable.find(name) != symbolTable.end()){
        cout<<"Error: "<<name<<" already exists"<<endl;
        return false;
      }
      symbolTable[name] = element;
      return true;
    }

    bool change(string name, string element){
      if(DEBUG){cout<<"SymbolTable change "<<name<<" to "<<element<<endl;}
      if(symbolTable.find(name) != symbolTable.end()){
        symbolTable[name] = element;
        return true;
      }else{
        if(father == nullptr){
          cout<<"Error: "<<name<<" not found"<<endl;
          return false;
        }else{
          return father->change(name,element);
        }
      }
    }

    // 递归查找符号表，看看某个符号是否被定义过
    string find(string name){
      if(symbolTable.find(name) != symbolTable.end()){
        return symbolTable[name];
      }else if(father != nullptr){
        return father->find(name);
      }
      cout<<"Error: "<<name<<" not found"<<endl;
      return "Error";
    }
};

extern SymbolTable* symbolTableNow;

SymbolTable* newSymbolTable();

void deleteSymbolTable();

