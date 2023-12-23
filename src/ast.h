#pragma once
#include <memory>
#include <string>
#include <iostream>
#include "GlobalCounter.h"


// 所有 AST 的基类
class BaseAST {
 public:
  virtual ~BaseAST() = default;
  virtual void Dump() const {};
  virtual std::string DumpKoopa() = 0;
};

class CompUnitAST : public BaseAST {
 public:
  // 用智能指针管理对象
  std::unique_ptr<BaseAST> func_def;

  std::string DumpKoopa() override {
    return func_def->DumpKoopa();
  }
};

class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;

  std::string DumpKoopa() override {
    return "fun @" + ident + "(): " +func_type->DumpKoopa()+ " {\n" + block->DumpKoopa()+"}\n";
  }
};

class FuncTypeAST : public BaseAST {
 public:
  std::string func_type;

  std::string DumpKoopa() override {
    return func_type;
  }
};

class BlockAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> stmt;

  std::string DumpKoopa() override {
    return "%entry: \n" + stmt->DumpKoopa();
  }
};

class StmtAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> exp;
  std::string storeNum; // 标识这个表达式的值存在哪个临时变量里

  std::string DumpKoopa() override ;
};

class ExpAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> add_exp;
  std::string storeNum; // 标识这个表达式的值存在哪个临时变量里

  std::string DumpKoopa() override;
};

class UnaryExpAST : public BaseAST {
 public:
  enum class Kind {
    PrimaryExp = 0,
    UnaryOp_UnaryExp
  };
  Kind kind;
  std::unique_ptr<BaseAST> primary_exp;
  std::unique_ptr<BaseAST> unary_op, unary_exp;
  std::string storeNum; // 标识这个表达式的值存在哪个临时变量里

  UnaryExpAST() = default;
  UnaryExpAST(Kind k) : kind(k) {}

  std::string DumpKoopa() override ;
};

class PrimaryExpAST : public BaseAST {
 public:
  enum class Kind {
    Exp = 0,
    Number
  };
  Kind kind;
  std::unique_ptr<BaseAST> exp;
  std::string storeNum; // 标识这个表达式的值存在哪个临时变量里
  int num;

  PrimaryExpAST() = default;
  PrimaryExpAST(Kind k) : kind(k) {}

  std::string DumpKoopa() override ;
};

class UnaryOpAST : public BaseAST {
 public:
  std::string op;

  std::string DumpKoopa() override {
    return op;
  }
};

class MulExpAST : public BaseAST {
 public:
  enum class Kind {
    UnaryExp = 0,
    MulExp_Mul_UnaryExp,
    MulExp_Div_UnaryExp,
    MulExp_Mod_UnaryExp
  };

  Kind kind;
  std::unique_ptr<BaseAST> unary_exp, mul_exp;
  std::string storeNum; // 标识这个表达式的值存在哪个临时变量里

  MulExpAST() = default;
  MulExpAST(Kind k) : kind(k) {}
  
  std::string DumpKoopa() override ;
};

class AddExpAST : public BaseAST {
 public:
  enum class Kind {
    MulExp = 0,
    AddExp_Add_MulExp,
    AddExp_Sub_MulExp 
  };

  Kind kind;
  std::unique_ptr<BaseAST> mul_exp, add_exp;
  std::string storeNum; // 标识这个表达式的值存在哪个临时变量里

  AddExpAST() = default;
  AddExpAST(Kind k) : kind(k) {}
  
  std::string DumpKoopa() override ;
};

