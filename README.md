本仓库为北京大学编译原理秋季课程大作业，实现了一个SysY语言的编译器，将SysY语言编译为RISC-V汇编代码。

参考文档：https://pku-minic.github.io/online-doc/#/

## 使用方法

```sh
cd sysy-make-template
cmake -DCMAKE_BUILD_TYPE=Debug -B build
cmake --build build
```

CMake 将在 `build` 目录下生成名为 `compiler` 的可执行文件.



## 一、编译器概述

### 1.1 基本功能

本编译器基本具备如下功能：
1. 解析SysY到抽象语法树。
2. 解析抽象语法树生成IR。
3. 解析IR生成目标代码。

### 1.2 主要特点

本编译器的主要特点是**与lab教程较为贴合，几乎完全顺着走**、**在解析IR、目标代码完成后，还额外经过了一层processIR、processRISCV函数处理**。
<!-- AA是……BB是……CC是……（各特点不要超过一句话） -->

## 二、编译器设计

### 2.1 主要模块组成

<!-- （不超过100字） -->
编译器由6个主要模块组成：
ast是抽象语法树，GlobalCounter是单例模式的全局计数器，IR2RISCV是将中间代码转换为目标代码的模块，koopaUtility包含了一些辅助调试的函数，SymbolTable是符号表，processIR、processRISCV是在生成中间代码、目标代码后的额外加工。

### 2.2 主要数据结构

<!-- （这部分无特别限制，这里给了一个小的参考书写建议，根据需要可以适当贴少量代码。这部分纯文字不要超过500字，贴上代码不要超过1000字） -->
本编译器最核心的数据结构是抽象语法树设计（详见源码）。
<!-- 另外，在实际代码编写过程中，设计了数据结构SymbolTable用来完成符号表的递归查询 -->

<!-- 如果将一个SysY程序视作一棵树，那么一个`class CompUnitAST`的实例就是这棵树的根，根据这一情况设计了数据结构BaseAST…… -->

```c++
// 所有 AST 的基类
class BaseAST {
 public:
  string storeNum; // 标识这个表达式的值存在哪个临时变量里

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

class FuncDefAST : public BaseAST {
 public:
  unique_ptr<BaseAST> func_type;
  string ident;
  unique_ptr<BaseAST> block;
  ...
}

class FuncTypeAST : public BaseAST {
 public:
  string func_type;
  ...
}

class BlockAST : public BaseAST {
 public:
  unique_ptr<BaseAST> multi_block_items;
    ...
}

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
  ...
}

class MutiBlockItemAST : public BaseAST{...}

...

// 总的来说，使用unique_ptr<BaseAST>统一储存子节点的指针，必要时使用dynamic_cast转换类型，在父节点DumpKoopa产生IR代码时，调用子节点的DumpKoopa函数
```

在`if...else...`语句方面，由于涉及到二义性问题，所以我设计了如下语法

```lex
Stmt
  : NonIfStmt {...}
  | MatchedStmt {...}
  | UnmatchedStmt {...}

NonIfStmt
  : RETURN Exp ';' {...}
  | LVal '=' Exp ';' {...}
  | RETURN ';' {...}
  ...

MatchedStmt
  : IF '(' Exp ')' MatchedStmt ELSE MatchedStmt {...}
  | NonIfStmt {...}

UnmatchedStmt
  : IF '(' Exp ')' Stmt {...}
  | IF '(' Exp ')' MatchedStmt ELSE UnmatchedStmt {...}
```

除此之外，为了代码编写的便利性，避免写诸如
next_Symbol = "%" + std::to_string(GlobalCounter::GetInstance().GetNext())的重复代码，我将这类代码封装成了函数

<!-- 前端和后端部分都各有一个`GlobalInfo`的数据结构。设计`global_info`的目的是…… -->



### 2.3 主要设计考虑及算法选择

#### 2.3.1 符号表的设计考虑
<!-- 谈谈你是如何处理符号表，特别是变量作用域的处理。不超过200字。 -->
本来也想设计成全局计数器一样的单例模式，但后来发现不合适。第一版设计是全局统一的、只有一个的符号表单例。第二版设计思路如下

```c++
enum class SymbolTableElementType {
  INT = 0
};

extern SymbolTable* symbolTableNow;  // 标识当前符号表

class SymbolTable{
  public:
    SymbolTable* father;
    unordered_map<string,string> symbolTable;  // 符号表，映射 符号名称——存符号的临时变量名

    bool insert(string name, string element){
        把name和element插入到当前符号表
    }
    bool change(string name, string element){...}
    string find(string name){
        如果：当前符号表找到了，返回
        否则，递归查找father符号表
    }
}

void deleteSymbolTable(){
  SymbolTable* temp = symbolTableNow->father;
  delete symbolTableNow;
  symbolTableNow = temp;
  symbolTableLevel--;
}

SymbolTable* newSymbolTable(){
  SymbolTable* newTable = new SymbolTable();
  newTable->father = symbolTableNow;
  symbolTableNow->son = newTable;
  symbolTableNow = newTable;  
  symbolTableLevel++;
  return newTable;
}
```

全局的symbolTableNow指针指向当前符号表，每次进入新的作用域，就新建一个符号表，father指向上一个符号表，然后symbolTableNow指向新的符号表，像是一个链表，实现作用域的嵌套了。



#### 2.3.2 寄存器分配策略
<!-- 谈谈你是如何完成寄存器分配的，没分配（扔栈上也给个说法）也谈谈。不超过200字。 -->
寄存器分配策略是放在栈上。此外，出于方便考虑，我还实现了loadRegister

```c++
// 输入value和希望存入的寄存器名称，返回将value存入寄存器的汇编代码
string loadRegister(koopa_raw_value_t value, string target){
    ...
}
```



#### 2.3.3 采用的优化策略
<!-- 谈谈你做了哪些优化，不超过200字，没有的话就不用写。 -->

#### 2.3.4 其它补充设计考虑
<!-- 谈谈你做了哪些设计考虑，不超过200字，没有的话就不用写。 -->
我把所有加载立即数 i 的操作都写成了`%1 = add 0, i`，比较统一方便

我并没有计算当前程序的最大栈深度，而是直接把栈减去一个很大的数，这样就不用考虑栈溢出的问题了；还有一个计数器，记录现在栈的分配深度，也设计了一个“符号表”，记录符号与栈中偏移的映射

## 三、编译器实现

### 3.1 各阶段编码细节

<!-- 介绍各阶段实现的时候自己认为有价值的信息，本部分内容**不做特别要求和限制**。可以考虑按如下所示的各阶段组织。 -->

#### Lv1. main函数和Lv2. 初试目标代码生成

<!-- 这部分可以写一些你对编译器的第一印象，或者是你对编译器的第一次尝试，或者是你对编译器的第一次成功，或者是你对编译器的第一次失败，或者是你对编译器的第一次…… -->
```txt
在这部分入门了lex词法分析、语法分析
```

#### Lv3. 表达式

<!-- 有感触就说点啥，例如优先级 -->

```txt
用语法分析安排表达式的优先级，比起以前C++的解析符号方法更容易
```

#### Lv4. 常量和变量
<!-- 总会有点想说的，因为多了变量。 -->

```txt
遇到过困难：没意识到IR中变换作用域后，之前的赋值会失效，需要alloc
我发现不区分常量和变量也能写过
```

#### Lv5. 语句块和作用域
<!-- 作用域嵌套就没有让你分享的地方么？ -->
```txt
在这个level，我发现需要修改我符号表的结构，改为链表结构
```

#### Lv6. if语句

```txt
二义性的处理需要修改语法分析，这是我第一次处理二义性
```

#### Lv7. while语句

```lex
WHILE '(' Exp ')' Stmt会遇到问题（此前解析if else没问题，但加入while后出问题，定位原因是语法分析出错），改为WHILE '(' Exp ')' MatchedStmt后解决了问题
```
<!-- #### Lv8. 函数和全局变量
```
传参和返回值的处理有想分享的地方么？
```
#### Lv9. 数组
```
数组参数？多维数组？不过……如果没做到这个阶段就不用写了。
``` -->


## 四、实习总结

<!-- 请至少谈1点，多谈谈更好。有机会获得奶茶或咖啡一杯。可以考虑按下面的几点讨论。 -->

### 4.1 收获和体会

最重要的是意识到，编译不只是把c++翻译成汇编，而是把一种结构化的东西转换成另一种结构化的东西

<!-- ### 4.2 学习过程中的难点，以及对实习过程和内容的建议

lab文档有些地方不够详细，比如koopa的文档有点少，而且struct命名方式（至少让我）不够能清晰的意识到这是什么东西（比如koopa_raw_value_t就是koopa_raw_value_data*指针，名字不一样，搜索起来看代码十分费解，而且形如koopa_raw_branch_t的代码又突然不是指针了），koopa_raw_value_data的name什么时候有，什么时候没有，是做什么的，我一概是试出来，而且截止我多个level拿到分，我也没真正明白为什么要这么设计（不用这个字段也能写，但name又经常是null，很迷惑）。

### 4.3 对老师讲解内容与方式的建议

我有时候听不懂课，感觉课程有些内容不够小白友好，不是从“我们有什么问题，需要解决什么，所以想到了什么”讲的，回放很重要
 -->
