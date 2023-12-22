#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include "ast.h"
#include <fstream>
#include "koopa.h"
#include <string.h>

using namespace std;

// 声明 lexer 的输入, 以及 parser 函数
// 为什么不引用 sysy.tab.hpp 呢? 因为首先里面没有 yyin 的定义
// 其次, 因为这个文件不是我们自己写的, 而是被 Bison 生成出来的
// 你的代码编辑器/IDE 很可能找不到这个文件, 然后会给你报错 (虽然编译不会出错)
// 看起来会很烦人, 于是干脆采用这种看起来 dirty 但实际很有效的手段
extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);

void file_write(string s, const char* output){
  ofstream file(output);
  if (file.is_open()) {
        file << s;  // 将字符串 str 写入到文件中
        file.close(); // 关闭文件
  }
}

int main(int argc, const char *argv[]) {
  // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
  // compiler 模式 输入文件 -o 输出文件
  assert(argc == 5);
  auto mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];

  // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
  yyin = fopen(input, "r");
  assert(yyin);

  // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
  unique_ptr<BaseAST> ast;
  auto ret = yyparse(ast);
  if(ret) {
    cout << "yyparse error" << endl;
  }
  assert(!ret);

  // 输出解析得到的 AST, 其实就是个字符串
  // cout << *ast << endl;
  string s=ast->DumpKoopa();

  if(strcmp(mode, "-koopa") == 0) {
    cout<<"--- koopa mode ---\n";
    cout<<s;
    file_write(s, output);
    return 0;
  }
  else if(strcmp(mode, "-riscv") == 0){
    cout<<"--- riscv mode ---\n";
    koopa_program_t program;
    koopa_error_code_t ret = koopa_parse_from_string(s.c_str(), &program);
    assert(ret == KOOPA_EC_SUCCESS);  

    koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
    koopa_raw_program_t raw = koopa_build_raw_program(builder, program);

    string rsicV_code;
    rsicV_code = "  .text\n";
    for (size_t i = 0; i < raw.funcs.len; ++i) {
      assert(raw.funcs.kind == KOOPA_RSIK_FUNCTION);
      koopa_raw_function_t func = (koopa_raw_function_t) raw.funcs.buffer[i];
      string func_name = (func->name)+1;
      rsicV_code += "  .globl "+func_name+"\n"+func_name+":\n";
      
      for (size_t j = 0; j < func->bbs.len; ++j) {
        assert(func->bbs.kind == KOOPA_RSIK_BASIC_BLOCK);
        koopa_raw_basic_block_t bb = (koopa_raw_basic_block_t) func->bbs.buffer[j];
        for(size_t k=0; k<bb->insts.len; ++k){
          assert(bb->insts.kind == KOOPA_RSIK_VALUE);
          koopa_raw_value_t value = (koopa_raw_value_t) bb->insts.buffer[k];

          // 示例程序中, 你得到的 value 一定是一条 return 指令
          assert(value->kind.tag == KOOPA_RVT_RETURN);
          // 于是我们可以按照处理 return 指令的方式处理这个 value
          // return 指令中, value 代表返回值
          koopa_raw_value_t ret_value = value->kind.data.ret.value;
          // 示例程序中, ret_value 一定是一个 integer
          assert(ret_value->kind.tag == KOOPA_RVT_INTEGER);
          // 于是我们可以按照处理 integer 的方式处理 ret_value
          // integer 中, value 代表整数的数值
          int32_t int_val = ret_value->kind.data.integer.value;
          // 示例程序中, 这个数值一定是 0
          // assert(int_val == 0);
          rsicV_code+="  li a0, "+to_string(int_val)+"\n  ret\n";
        }
      }
    }
    cout<<rsicV_code;
    file_write(rsicV_code, output);
  }
  else{
    cout<<"--- error mode ---\n";
    return 0;
  }
  
  cout << endl;
  return 0;
}
