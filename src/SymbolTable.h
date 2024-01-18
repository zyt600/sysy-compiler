#pragma once
#include <unordered_map>
#include <iostream>
#include <variant>
#include "debug.h"
using namespace std;

enum class SymbolTableElementType {
  INT = 0
};
extern SymbolTableElementType nextEleType; // 下一个要插入符号表的元素的类型

class SymbolTable{
  public:
    SymbolTable* father=nullptr,*son=nullptr;
    unordered_map<string,string> symbolTable;  // 符号表，映射 符号名称——存符号的临时变量名
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
      if(symbolTable.find(name) == symbolTable.end()){
        cout<<"Error: "<<name<<" not found"<<endl;
        return false;
      }
      symbolTable[name] = element;
      return true;
    }

    string find(string name){
      if(symbolTable.find(name) != symbolTable.end()){
        return symbolTable[name];
      }else if(father != nullptr){
        return father->find(name);
      }
      cout<<"Error: "<<name<<" not found"<<endl;
      return "";
    }
};

extern SymbolTable* symbolTableNow;