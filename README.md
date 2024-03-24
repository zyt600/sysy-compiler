[Chinese](README_zh.md)

This repository implements a compiler for the SysY language, compiling SysY language code into RISC-V assembly code. The SysY language is a compact version of the C language.

Reference Document: https://pku-minic.github.io/online-doc/#/

## Usage

```sh
cd sysy-make-template
cmake -DCMAKE_BUILD_TYPE=Debug -B build
cmake --build build
```

CMake will generate an executable named `compiler` in the `build` directory.

## 1. Compiler Overview

The compiler fundamentally provides the following functionalities:
1. Parse SysY into an abstract syntax tree (AST).
2. Parse the AST to generate Intermediate Representation (IR).
3. Parse IR to generate target code.


## 2. Compiler Design

### 2.1 Main Modules

The compiler consists of 6 main modules:
- `ast` is the abstract syntax tree,
- `GlobalCounter` is a singleton global counter,
- `IR2RISCV` converts intermediate code to target code,
- `koopaUtility` contains some auxiliary functions for debugging,
- `SymbolTable` is the symbol table,
- `processIR` and `processRISCV` are additional processing after generating intermediate and target code.

### 2.2 Main Data Structures

The most core data structure of this compiler is the design of the abstract syntax tree (see source code for details). Moreover, a data structure `SymbolTable` was designed to accomplish the recursive query of the symbol table.

```c++
// Base class for all AST nodes
class BaseAST {
 public:
  string storeNum; // Indicates which temporary variable holds the value of this expression

  virtual ~BaseAST() = default;
  virtual string DumpKoopa() {
    ...
  };
};

class CompUnitAST : public BaseAST {
 public:
  unique_ptr<BaseAST> func_def;
  ...
}

// Other AST classes follow a similar structure, using unique_ptr<BaseAST> to uniformly store pointers to child nodes and dynamic_cast for type conversion when necessary.
```

For `if...else...` statements, due to ambiguity issues, the following syntax was designed:

```lex
Stmt
  : NonIfStmt {...}
  | MatchedStmt {...}
  | UnmatchedStmt {...}

NonIfStmt
  : RETURN Exp ';' {...}
  ...

MatchedStmt
  : IF '(' Exp ')' MatchedStmt ELSE MatchedStmt {...}
  ...

UnmatchedStmt
  : IF '(' Exp ')' Stmt {...}
  ...
```

Additionally, for convenience in code writing and to avoid repetition, certain codes were encapsulated into functions.

### 2.3 Main Design Considerations and Algorithm Choices

#### 2.3.1 Symbol Table Design
Initially, the symbol table was designed as a singleton like the global counter but was later revised. The first version was a single, globally unified symbol table singleton. The second version is as follows:

```c++
enum class SymbolTableElementType {
  INT = 0
};

extern SymbolTable* symbolTableNow;  // Current symbol table

class SymbolTable{
  public:
    SymbolTable* father;
    unordered_map<string,string> symbolTable;  // Symbol table mapping symbol names to temporary variable names

    bool insert(string name, string element){
        // Insert name and element into the current symbol table
    }
    bool change(string name, string element){...}
    string find(string name){
        // If found in the current table, return. Otherwise, recursively search the father table.
    }
}

// Functions for managing symbol tables
```

The global `symbolTableNow` pointer indicates the current symbol table. Entering a new scope creates a new symbol table linked back to the previous one, simulating nested scopes.

#### 2.3.2 Register Allocation Strategy
The strategy for register allocation is to use the stack. Additionally, a `loadRegister` function was implemented for convenience:

```c++
// Generates assembly code to load a value into a register
string loadRegister(koopa_raw_value_t value, string target){
    ...
}
```

## 3. Summary
The most important realization was that compiling is not just translating C++ into assembly. It's about converting one structured thing into another structured thing. The compiler is a bridge between the high-level language and the machine language, and the key is to design a good intermediate representation.