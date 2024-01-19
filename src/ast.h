#pragma once
#include <memory>
#include <string>
#include <iostream>
#include "GlobalCounter.h"
#include "SymbolTable.h"
#include "debug.h"
#include <vector>
#include <deque>
using namespace std;



// 所有 AST 的基类
class BaseAST {
 public:
  string storeNum; // 标识这个表达式的值存在哪个临时变量里

  virtual ~BaseAST() = default;
  virtual void Dump() const {};
  virtual string DumpKoopa() {
    if(DEBUG) cout<<"BaseAST called"<<endl;
    return "";
  };
};

class CompUnitAST : public BaseAST {
 public:
  unique_ptr<BaseAST> func_def;

  string DumpKoopa() override {
    if(DEBUG) cout<<"CompUnitAST called"<<endl;
    return func_def->DumpKoopa();
  }
};

class FuncDefAST : public BaseAST {
 public:
  unique_ptr<BaseAST> func_type;
  string ident;
  unique_ptr<BaseAST> block;

  string DumpKoopa() override {
    if(DEBUG) cout<<"FuncDefAST called"<<endl;
    return "fun @" + ident + "(): " +func_type->DumpKoopa()+ " {\n%entry:\n" + block->DumpKoopa()+"}\n";
  }
};

class FuncTypeAST : public BaseAST {
 public:
  string func_type;

  string DumpKoopa() override {
    if(DEBUG) cout<<"FuncTypeAST called"<<endl;
    return func_type;
  }
};

class BlockAST : public BaseAST {
 public:
  unique_ptr<BaseAST> multi_block_items;

  string DumpKoopa() override {
    if(DEBUG) cout<<"BlockAST called"<<endl;
    string code = multi_block_items->DumpKoopa();
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
    if(DEBUG) cout<<"BlockItemAST called"<<endl;
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
    if(DEBUG) cout<<"MutiBlockItemAST called"<<endl;
    string code;
    for(auto &item : blockitems){
      code += item->DumpKoopa();
    }
    return code;
  }
};

class StmtAST : public BaseAST {
 public:
  enum class Kind {
    MATCHED = 0,
    UNMATCHED,
    NONIF
  };
  Kind kind;
  unique_ptr<BaseAST> non_if_stmt, matched_stmt, unmatched_stmt;

  string DumpKoopa() override ;
  StmtAST() = default;
  StmtAST(Kind k) : kind(k) {}
};

class NonIfStmtAST : public BaseAST {
 public:
  enum class Kind {
    RET_EXP = 0,
    LVALeqEXP, // left_value = expression
    EMPTY, // 单纯只有一个分号
    EXP, // 单纯只有一个表达式，会被求值 (即存在副作用), 但所求的值会被丢弃
    RET, // 单纯只有一个 return;
    BLOCK
  };
  Kind kind;
  unique_ptr<BaseAST> exp, l_val, block;

  string DumpKoopa() override ;
  NonIfStmtAST() = default;
  NonIfStmtAST(Kind k) : kind(k) {}
};

class MatchedStmtAST : public BaseAST {
 public:
  enum class Kind {
    IF_ELSE = 0,
    NON_IF
  };
  Kind kind;
  unique_ptr<BaseAST> exp, matched_stmt1, matched_stmt2, non_if_stmt;

  string DumpKoopa() override ;
  MatchedStmtAST() = default;
  MatchedStmtAST(Kind k) : kind(k) {}
};

class UnmatchedStmtAST : public BaseAST {
 public:
  enum class Kind {
    IF = 0,
    IF_ELSE
  };
  Kind kind;
  unique_ptr<BaseAST> exp, matched_stmt, unmatched_stmt, stmt;

  string DumpKoopa() override ;
  UnmatchedStmtAST() = default;
  UnmatchedStmtAST(Kind k) : kind(k) {}
};

class ExpAST : public BaseAST {
 public:
  unique_ptr<BaseAST> l_or_exp;

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
  int num;

  PrimaryExpAST() = default;
  PrimaryExpAST(Kind k) : kind(k) {}

  string DumpKoopa() override ;
};

class LValAST : public BaseAST {
 public:
  string ident;
 
  string DumpKoopa() override {
    if(DEBUG) cout<<"LValAST called"<<endl;
    return "";
  };
};

class UnaryOpAST : public BaseAST {
 public:
  string op;

  string DumpKoopa() override {
    if(DEBUG) cout<<"UnaryOpAST called"<<endl;
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

  RelExpAST() = default;
  RelExpAST(Kind k) : kind(k) {}
  
  string DumpKoopa() override ;
};

class DeclAST : public BaseAST {
 public:
  enum class Kind {
    ConstDecl = 0,
    VarDecl
  };
  unique_ptr<BaseAST> const_decl, var_decl;
  Kind kind;

  DeclAST() = default;
  DeclAST(Kind k) : kind(k) {}
  string DumpKoopa() override ;
};

class ConstDeclAST : public BaseAST {
 public:
  string b_type;
  unique_ptr<BaseAST> const_defs; // const_defs的类型是MultiConstDefAST

  string DumpKoopa() override {
    if(DEBUG) cout<<"ConstDeclAST called"<<endl;
    return const_defs->DumpKoopa();
  }
};

class ConstDefAST : public BaseAST {
 public:
  string ident;
  unique_ptr<BaseAST> const_init_val;
  SymbolTableElementType type;

  string DumpKoopa() override {
    if(DEBUG) cout<<"ConstDefAST called"<<endl;
    string code = const_init_val->DumpKoopa();
    symbolTableNow->insert(ident,const_init_val->storeNum);
    return code;
  }
};

class MultiConstDefAST : public BaseAST {
 public:
  deque<unique_ptr<BaseAST> > const_defs;

  string DumpKoopa() override {
    if(DEBUG) cout<<"MultiConstDefAST called"<<endl;
    string code;
    for(auto &item : const_defs){
      code += item->DumpKoopa();
    }
    return code;
  }
};

class ConstInitValAST : public BaseAST {
 public:
  unique_ptr<BaseAST> const_exp;

  string DumpKoopa() override {
    if(DEBUG) cout<<"ConstInitValAST called"<<endl;
    string code =const_exp->DumpKoopa();
    storeNum = const_exp->storeNum;
    return code;
  }
};

class ConstExpAST : public BaseAST {
 public:
  unique_ptr<BaseAST> exp;

  string DumpKoopa() override {
    if(DEBUG) cout<<"ConstExpAST called"<<endl;
    string code = exp->DumpKoopa();
    storeNum = exp->storeNum;
    return code;
  }
};

class VarDeclAST : public BaseAST {
 public:
  string b_type;
  unique_ptr<BaseAST> var_defs; // var_defs的类型是MultiVarDefAST

  string DumpKoopa() override {
    if(DEBUG) cout<<"VarDeclAST called"<<endl;
    return var_defs->DumpKoopa();
  }
};

class VarDefAST : public BaseAST {
 public:
  enum class Kind {
    IDENT = 0,
    IDENT_InitVal
  };
  string ident;
  unique_ptr<BaseAST> init_val;
  SymbolTableElementType type;
  Kind kind;
  
  VarDefAST() = default;
  VarDefAST(Kind k) : kind(k) {}

  string DumpKoopa() override;
};

class MultiVarDefAST : public BaseAST {
 public:
  deque<unique_ptr<BaseAST> > var_defs;

  string DumpKoopa() override {
    if(DEBUG) cout<<"MultiVarDefAST called"<<endl;
    string code;
    for(auto &item : var_defs){
      code += item->DumpKoopa();
    }
    return code;
  }
};

class InitValAST : public BaseAST {
 public:
  unique_ptr<BaseAST> exp;

  string DumpKoopa() override {
    if(DEBUG) cout<<"InitValAST called"<<endl;
    string code = exp->DumpKoopa();
    storeNum = exp->storeNum;
    return code;
  }
};









