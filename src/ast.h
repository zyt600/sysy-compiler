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
  std::unique_ptr<BaseAST> l_or_exp;
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

class LOrExpAST : public BaseAST {
 public:
  enum class Kind {
    LAndExp = 0,
    LOrExp_LAndExp
  };

  Kind kind;
  std::unique_ptr<BaseAST> l_and_exp, l_or_exp;
  std::string storeNum; // 标识这个表达式的值存在哪个临时变量里

  LOrExpAST() = default;
  LOrExpAST(Kind k) : kind(k) {}
  
  std::string DumpKoopa() override ;
};

class LAndExpAST : public BaseAST {
 public:
  enum class Kind {
    EqExp = 0,
    LAndExp_EqExp
  };

  Kind kind;
  std::unique_ptr<BaseAST> eq_exp, l_and_exp;
  std::string storeNum; // 标识这个表达式的值存在哪个临时变量里

  LAndExpAST() = default;
  LAndExpAST(Kind k) : kind(k) {}
  
  std::string DumpKoopa() override ;
};

class EqExpAST : public BaseAST {
 public:
  enum class Kind {
    RelExp = 0,
    EqExp_EqEq_RelExp,
    EqExp_NotEq_RelExp
  };

  Kind kind;
  std::unique_ptr<BaseAST> rel_exp, eq_exp;
  std::string storeNum; // 标识这个表达式的值存在哪个临时变量里

  EqExpAST() = default;
  EqExpAST(Kind k) : kind(k) {}
  
  std::string DumpKoopa() override ;
};

class RelExpAST : public BaseAST {
 public:
  enum class Kind {
    AddExp = 0,
    RelExp_LT_AddExp,
    RelExp_GT_AddExp,
    RelExp_LE_AddExp,
    RelExp_GE_AddExp
  };

  Kind kind;
  std::unique_ptr<BaseAST> add_exp, rel_exp;
  std::string storeNum; // 标识这个表达式的值存在哪个临时变量里

  RelExpAST() = default;
  RelExpAST(Kind k) : kind(k) {}
  
  std::string DumpKoopa() override ;
};


