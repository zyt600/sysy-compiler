#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include "ast.h"
#include <fstream>
#include "koopa.h"
#include <string.h>
#include "IR2RISCV.h"
// RVT是raw value type的缩写嗷
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

void read_file_contents(FILE *file) { // 读取文件内容，并恢复文件指针
    std::fseek(file, 0, SEEK_END); // 移动到文件末尾
    long length = std::ftell(file); // 获取文件大小
    std::fseek(file, 0, SEEK_SET); // 回到文件开始
    std::string content(length, '\0'); // 创建足够大的字符串
    std::fread(&content[0], 1, length, file); // 读取文件内容到字符串
    rewind(file);
    cout<<content<<endl;
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
  read_file_contents(yyin);
  assert(yyin);

  // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
  unique_ptr<BaseAST> ast;
  auto ret = yyparse(ast);
  if(ret) {
    cout << "yyparse error" << endl;
  }
  assert(!ret);

  // 输出解析得到的 AST, 其实就是个字符串
  string s=ast->DumpKoopa();
  // string s="fun @main(): i32 {\n%entry: \n%1 = mul 0, 3\n%2 = add 55, 5\n%3 = mul %1 , 7\n%4 = add %3, %1\nret %1\n}\n";

  if(strcmp(mode, "-koopa") == 0) {
    cout<<"--- koopa mode ---\n";
    cout<<s;
    file_write(s, output);
    return 0;
  }
  else if(strcmp(mode, "-riscv") == 0){
    cout<<"--- riscv mode ---\n";
    cout<<s<<endl;
    koopa_program_t program;
    koopa_error_code_t ret = koopa_parse_from_string(s.c_str(), &program);
    assert(ret == KOOPA_EC_SUCCESS);

    koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
    koopa_raw_program_t raw = koopa_build_raw_program(builder, program);

    string rsicV_code;
    rsicV_code.reserve(1000);
    rsicV_code = "  .text\n";
    Visit(raw, rsicV_code);
    cout<<endl<<rsicV_code<<endl;
    file_write(rsicV_code, output);
  }
  else{
    cout<<"--- error mode ---\n";
    return 0;
  }
  
  cout << endl;
  return 0;
}
