#pragma once
#include <memory>
#include <string>
#include <iostream>
#include "GlobalCounter.h"
#include <vector>
#include <deque>
using namespace std;

// 所有 AST 的基类
class BaseAST {
 public:
  virtual ~BaseAST() = default;
  virtual void Dump() const {};
  virtual string DumpKoopa() = 0;
};

class CompUnitAST : public BaseAST {
 public:
  unique_ptr<BaseAST> func_def;

  string DumpKoopa() override {
    return func_def->DumpKoopa();
  }
};

class FuncDefAST : public BaseAST {
 public:
  unique_ptr<BaseAST> func_type;
  string ident;
  unique_ptr<BaseAST> block;

  string DumpKoopa() override {
    return "fun @" + ident + "(): " +func_type->DumpKoopa()+ " {\n" + block->DumpKoopa()+"}\n";
  }
};

class FuncTypeAST : public BaseAST {
 public:
  string func_type;

  string DumpKoopa() override {
    return func_type;
  }
};

class BlockAST : public BaseAST {
 public:
  unique_ptr<BaseAST> multi_block_items;

  string DumpKoopa() override {
    string code="%entry: \n";
    code += multi_block_items->DumpKoopa();
    return code;
  }
};

class BlockItemAST : public BaseAST {
 public:
  enum class Kind {
    Stmt = 0,
    Decl
  };
  unique_ptr<BaseAST> stmt;
  unique_ptr<BaseAST> decl;
  Kind kind;

  BlockItemAST() = default;
  BlockItemAST(Kind k) : kind(k) {}

  string DumpKoopa() override {
    if(kind == Kind::Stmt){
      return stmt->DumpKoopa();
    }
    else{
      return decl->DumpKoopa();
    }
  }
};

class MutiBlockItemAST : public BaseAST {
 public:
  deque<unique_ptr<BaseAST> > blockitems;
  string DumpKoopa() override {
    string code;
    for(auto &item : blockitems){
      code += item->DumpKoopa();
    }
    return code;
  }
};

class StmtAST : public BaseAST {
 public:
  unique_ptr<BaseAST> exp;
  string storeNum; // 标识这个表达式的值存在哪个临时变量里

  string DumpKoopa() override ;
};

class ExpAST : public BaseAST {
 public:
  unique_ptr<BaseAST> l_or_exp;
  string storeNum; // 标识这个表达式的值存在哪个临时变量里

  string DumpKoopa() override;
};

class UnaryExpAST : public BaseAST {
 public:
  enum class Kind {
    PrimaryExp = 0,
    UnaryOp_UnaryExp
  };
  Kind kind;
  unique_ptr<BaseAST> primary_exp;
  unique_ptr<BaseAST> unary_op, unary_exp;
  string storeNum; // 标识这个表达式的值存在哪个临时变量里

  UnaryExpAST() = default;
  UnaryExpAST(Kind k) : kind(k) {}

  string DumpKoopa() override ;
};

class PrimaryExpAST : public BaseAST {
 public:
  enum class Kind {
    Exp = 0,
    Number,
    LVal 
  };
  Kind kind;
  unique_ptr<BaseAST> exp, l_val;
  string storeNum; // 标识这个表达式的值存在哪个临时变量里
  int num;

  PrimaryExpAST() = default;
  PrimaryExpAST(Kind k) : kind(k) {}

  string DumpKoopa() override ;
};

class UnaryOpAST : public BaseAST {
 public:
  string op;

  string DumpKoopa() override {
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
  unique_ptr<BaseAST> unary_exp, mul_exp;
  string storeNum; // 标识这个表达式的值存在哪个临时变量里

  MulExpAST() = default;
  MulExpAST(Kind k) : kind(k) {}
  
  string DumpKoopa() override ;
};

class AddExpAST : public BaseAST {
 public:
  enum class Kind {
    MulExp = 0,
    AddExp_Add_MulExp,
    AddExp_Sub_MulExp 
  };

  Kind kind;
  unique_ptr<BaseAST> mul_exp, add_exp;
  string storeNum; // 标识这个表达式的值存在哪个临时变量里

  AddExpAST() = default;
  AddExpAST(Kind k) : kind(k) {}
  
  string DumpKoopa() override ;
};

class LOrExpAST : public BaseAST {
 public:
  enum class Kind {
    LAndExp = 0,
    LOrExp_LAndExp
  };

  Kind kind;
  unique_ptr<BaseAST> l_and_exp, l_or_exp;
  string storeNum; // 标识这个表达式的值存在哪个临时变量里

  LOrExpAST() = default;
  LOrExpAST(Kind k) : kind(k) {}
  
  string DumpKoopa() override ;
};

class LAndExpAST : public BaseAST {
 public:
  enum class Kind {
    EqExp = 0,
    LAndExp_EqExp
  };

  Kind kind;
  unique_ptr<BaseAST> eq_exp, l_and_exp;
  string storeNum; // 标识这个表达式的值存在哪个临时变量里

  LAndExpAST() = default;
  LAndExpAST(Kind k) : kind(k) {}
  
  string DumpKoopa() override ;
};

class EqExpAST : public BaseAST {
 public:
  enum class Kind {
    RelExp = 0,
    EqExp_EqEq_RelExp,
    EqExp_NotEq_RelExp
  };

  Kind kind;
  unique_ptr<BaseAST> rel_exp, eq_exp;
  string storeNum; // 标识这个表达式的值存在哪个临时变量里

  EqExpAST() = default;
  EqExpAST(Kind k) : kind(k) {}
  
  string DumpKoopa() override ;
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
  unique_ptr<BaseAST> add_exp, rel_exp;
  string storeNum; // 标识这个表达式的值存在哪个临时变量里

  RelExpAST() = default;
  RelExpAST(Kind k) : kind(k) {}
  
  string DumpKoopa() override ;
};

class DeclAST : public BaseAST {
 public:
  unique_ptr<BaseAST> const_decl;
  
  string DumpKoopa() override {
    
  }
};

class ConstDeclAST : public BaseAST {
 public:
  string b_type;
  unique_ptr<BaseAST> const_defs;

  string DumpKoopa() override {
    
  }
};

class ConstDefAST : public BaseAST {
 public:
  string ident;
  unique_ptr<BaseAST> const_init_val;

  string DumpKoopa() override {
    
  }
};

class MultiConstDefAST : public BaseAST {
 public:
  deque<unique_ptr<BaseAST> > const_defs;

  string DumpKoopa() override {
    
  }
};


class ConstInitValAST : public BaseAST {
 public:
  unique_ptr<BaseAST> const_exp;

  string DumpKoopa() override {
    
  }
};

class ConstExpAST : public BaseAST {
 public:
  unique_ptr<BaseAST> exp;

  string DumpKoopa() override {
    
  }
};





