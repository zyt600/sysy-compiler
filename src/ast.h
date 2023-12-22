#pragma once
#include <memory>
#include <string>
#include <iostream>

// 所有 AST 的基类
class BaseAST {
 public:
  virtual ~BaseAST() = default;
  virtual void Dump() const = 0;
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
    return "fun @" + ident + "(): " +func_type->DumpKoopa()+ " {\n%entry: \n" + block->DumpKoopa()+"}";
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
    return stmt->DumpKoopa();
  }
};

class StmtAST : public BaseAST {
 public:
  int num;

  void Dump() const override {
    std::cout << "StmtAST { ";
    std::cout << num;
    std::cout << " }";
  }

  std::string DumpKoopa() const override {
    return "ret " + std::to_string(num) + "\n";
  }
};

