// IR to risc-V

#include <iostream>
#include "koopa.h"
#include "koopaUtility.h"
#include <cassert>
#include "IR2RISCV.h"
using namespace std;

void Visit(koopa_raw_program_t raw, string &rsicV_code){
    if(DEBUG) cout<<"Visit raw program"<<endl;
    for (size_t i = 0; i < raw.funcs.len; ++i) {
      assert(raw.funcs.kind == KOOPA_RSIK_FUNCTION);
      koopa_raw_function_t func = (koopa_raw_function_t) raw.funcs.buffer[i];
      Visit(func,rsicV_code);
    }
}

void Visit(koopa_raw_function_t func, string &rsicV_code){
    string func_name = (func->name)+1;
    if(DEBUG) cout<<"Visit raw function: "<<func_name<<endl;
    rsicV_code += "  .globl "+func_name+"\n"+func_name+":\n";
    for (size_t j = 0; j < func->bbs.len; ++j) {
        assert(func->bbs.kind == KOOPA_RSIK_BASIC_BLOCK);
        koopa_raw_basic_block_t bb = (koopa_raw_basic_block_t) func->bbs.buffer[j];
        Visit(bb,rsicV_code);
    }
}

void Visit(koopa_raw_basic_block_t bb, string &rsicV_code){
    if(DEBUG) cout<<"Visit raw basic block"<<endl;
    for(size_t k=0; k<bb->insts.len; ++k){
        assert(bb->insts.kind == KOOPA_RSIK_VALUE);
        koopa_raw_value_t value = (koopa_raw_value_t) bb->insts.buffer[k];
        Visit(value,rsicV_code);
    }
}

void Visit(koopa_raw_value_t value, string &rsicV_code){
    if(DEBUG) cout<<"Visit raw value"<<endl;
    switch (value->kind.tag)
    {
    case KOOPA_RVT_RETURN:{
        if(DEBUG) cout<<"  KOOPA_RVT_RETURN"<<endl;
        /* 示例程序中, 你得到的 value 一定是一条 return 指令
         assert(value->kind.tag == KOOPA_RVT_RETURN);
         于是我们可以按照处理 return 指令的方式处理这个 value
         return 指令中, value 代表返回值 */
        koopa_raw_value_t ret_value = value->kind.data.ret.value;
        // 示例程序中, ret_value 一定是一个 integer
        if(DEBUG) cout<<"    ret_value->kind.tag: "<<raw_value_tag(ret_value->kind.tag)<<endl;
        // assert(ret_value->kind.tag == KOOPA_RVT_INTEGER);
        // 于是我们可以按照处理 integer 的方式处理 ret_value
        // integer 中, value 代表整数的数值
        int32_t int_val = ret_value->kind.data.integer.value;
        // 示例程序中, 这个数值一定是 0
        // assert(int_val == 0);
        rsicV_code+="  li a0, "+to_string(int_val)+"\n  ret\n";
        break;
    }
    case KOOPA_RVT_BINARY:{
        if(DEBUG) cout<<"  KOOPA_RVT_BINARY"<<endl;
        koopa_raw_binary_t binary = value->kind.data.binary;
        string lhs_name, rhs_name;
        if(binary.lhs->name)
            lhs_name = binary.lhs->name;
        else
            lhs_name = "null";

        if(binary.rhs->name)
            rhs_name = binary.rhs->name;
        else
            rhs_name = "null";
        if(DEBUG) cout<<"    binary.op: "<<raw_binary_op(binary.op)<<", binary.lhs: { kind:"<<raw_value_tag(binary.lhs->kind.tag)<<", type:"<<raw_type_tag(binary.lhs->ty->tag)<<", name: "<<lhs_name<<"}, ";
        if(DEBUG) cout<<"binary.rhs: { kind:"<<raw_value_tag(binary.rhs->kind.tag)<<", type:"<<raw_type_tag(binary.lhs->ty->tag)<<", name: "<<rhs_name<<"}"<<endl;

        switch (binary.op)
        {
        case KOOPA_RBO_NOT_EQ:
            break;
        
        default:
            break;
        }


        break;
    }
    default:
        cout<<"value->kind.tag"<<value->kind.tag<<endl;
        break;
    }
}


