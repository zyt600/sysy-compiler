#pragma once
#include <memory>
#include <string>
#include <iostream>
#include "GlobalCounter.h"

// 获取全局用到了第几个临时数字变量，转成字符串
std::string GetNext();
std::string GetCurrent();

// 所有 AST 的基类
class BaseAST {
 public:
  virtual ~BaseAST() = default;
  virtual void Dump() const {};
  virtual std::string DumpKoopa() const = 0;
};

// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST {
 public:
  // 用智能指针管理对象
  std::unique_ptr<BaseAST> func_def;

  void Dump() const override {
    std::cout << "CompUnitAST { ";
    func_def->Dump();
    std::cout << " }";
  }

  std::string DumpKoopa() const override {
    return func_def->DumpKoopa();
  }
};

// FuncDef 也是 BaseAST
class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;

  void Dump() const override {
    std::cout << "FuncDefAST { ";
    func_type->Dump();
    std::cout << ", " << ident << ", ";
    block->Dump();
    std::cout << " }";
  }

  std::string DumpKoopa() const override {
    return "fun @" + ident + "(): " +func_type->DumpKoopa()+ " {\n" + block->DumpKoopa()+"}\n";
  }
};

class FuncTypeAST : public BaseAST {
 public:
  std::string func_type;

  void Dump() const override {
    std::cout << "FuncTypeAST { " << func_type << " }";
  }

  std::string DumpKoopa() const override {
    return func_type;
  }
};

class BlockAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> stmt;

  void Dump() const override {
    std::cout << "BlockAST { ";
    stmt->Dump();
    std::cout << " }";
  }

  std::string DumpKoopa() const override {
    return "%entry: \n" + stmt->DumpKoopa();
  }
};

class StmtAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> exp;

  std::string DumpKoopa() const override {
    // TODO: 要改，缺解析表达式过程
    std::string ret=exp->DumpKoopa();

    return ret + "ret " + GetCurrent() + "\n";
  }
};

// 不用改，对的
class ExpAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> unary_exp;

  std::string DumpKoopa() const override {
    return unary_exp->DumpKoopa();
  }
};

// 我觉得写好了
class UnaryExpAST : public BaseAST {
 public:
  enum class Kind {
    PrimaryExp = 0,
    UnaryOp_UnaryExp
  };
  Kind kind;
  std::unique_ptr<BaseAST> primary_exp;
  std::unique_ptr<BaseAST> unary_op, unary_exp;

  UnaryExpAST() = default;
  UnaryExpAST(Kind k) : kind(k) {}

  std::string DumpKoopa() const override {
    switch (kind) {
      case Kind::PrimaryExp:
        return primary_exp->DumpKoopa();
      case Kind::UnaryOp_UnaryExp:
        std::string op=unary_op->DumpKoopa();
        if(op=="!"){
          std::string preCode = unary_exp->DumpKoopa();
          std::string codeNow = "eq " + GetCurrent() + ", 0\n";
          return preCode + GetNext() + " = " + codeNow;
        }else if(op=="-"){
          std::string preCode = unary_exp->DumpKoopa();
          std::string codeNow = "sub 0, " + GetCurrent() + "\n";
          return preCode + GetNext() + " = " + codeNow;
        }else if(op=="+"){
          return unary_exp->DumpKoopa();
        }
        else{
          return "error";
        }
    }
  }
};

// 我觉得写好了
class PrimaryExpAST : public UnaryExpAST {
 public:
  enum class Kind {
    Exp = 0,
    Number
  };
  Kind kind;
  std::unique_ptr<BaseAST> exp;
  int num;

  PrimaryExpAST() = default;
  PrimaryExpAST(Kind k) : kind(k) {}

  std::string DumpKoopa() const override {
    switch (kind) {
      case Kind::Exp:
        return exp->DumpKoopa();
      case Kind::Number:
        // TODO: 这里存疑，koopa怎么写的我也不确定；不过实在不行可以写成数字加0
        // return GetNext() + " = " + std::to_string(num);
        return GetNext() + " = add 0, " + std::to_string(num) + "\n";
    }
  }
};

// 我觉得写好了
class UnaryOpAST : public BaseAST {
 public:
  std::string op;

  std::string DumpKoopa() const override {
    return op;
  }
};

