#include "SymbolTable.h"

SymbolTableElementType nextEleType; // 下一个要插入符号表的元素的类型
SymbolTable* symbolTableNow=new SymbolTable(); // 当前符号表

void deleteSymbolTable(){
  SymbolTable* temp = symbolTableNow->father;
  delete symbolTableNow;
  symbolTableNow = temp;
}

SymbolTable* newSymbolTable(){
  SymbolTable* newTable = new SymbolTable();
  newTable->father = symbolTableNow;
  symbolTableNow->son = newTable;
  symbolTableNow = newTable;  
  return newTable;
}
