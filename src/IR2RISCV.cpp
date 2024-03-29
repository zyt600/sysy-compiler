// IR to risc-V

#include <iostream>
#include "koopa.h"
#include "koopaUtility.h"
#include <cassert>
#include <unordered_map>
#include "IR2RISCV.h"
using namespace std;

IRSymbolTable irSymbolTable;

void VisitIR(koopa_raw_program_t raw, string &rsicV_code){
    if(DEBUG) cout<<"Visit raw program"<<endl;
    for (size_t i = 0; i < raw.funcs.len; ++i) {
      assert(raw.funcs.kind == KOOPA_RSIK_FUNCTION);
      koopa_raw_function_t func = (koopa_raw_function_t) raw.funcs.buffer[i];
      VisitIR(func,rsicV_code);
    }
}

void VisitIR(koopa_raw_function_t func, string &rsicV_code){
    string func_name = (func->name)+1;
    if(DEBUG) cout<<"Visit raw function: "<<func_name<<endl;
    rsicV_code += "  .globl "+func_name+"\n"+func_name+":\n";
    rsicV_code += "addi sp, sp, -2000\n";
    for (size_t j = 0; j < func->bbs.len; ++j) {
        assert(func->bbs.kind == KOOPA_RSIK_BASIC_BLOCK);
        koopa_raw_basic_block_t bb = (koopa_raw_basic_block_t) func->bbs.buffer[j];
        VisitIR(bb,rsicV_code);
    }
}

void VisitIR(koopa_raw_basic_block_t bb, string &rsicV_code){
    if(DEBUG) cout<<"Visit raw basic block"<<endl;
    for(size_t k=0; k<bb->insts.len; ++k){
        assert(bb->insts.kind == KOOPA_RSIK_VALUE);
        koopa_raw_value_t value = (koopa_raw_value_t) bb->insts.buffer[k];
        VisitIR(value,rsicV_code);
        // rsicV_code += retS;
    }
}

void VisitIR(koopa_raw_value_t value, string &rsicV_code){
    if(DEBUG) {
        cout<<"Visit raw value ";
        if(value->name)
            cout<<"with name: "<<value->name<<endl;
        else
            cout<<"without name"<<endl;
    }
    switch (value->kind.tag)
    {
    case KOOPA_RVT_RETURN:{ // Function return.
        if(DEBUG) cout<<"KOOPA_RVT_RETURN"<<endl;
        koopa_raw_return_t ret = value->kind.data.ret;
        koopa_raw_value_t ret_value = ret.value;
        // koopa_raw_value_t 就是 koopa_raw_value_data 的指针
        if(ret_value==NULL){
            rsicV_code+="  ret\n";
            break;
        }
        string code_this_time = loadRegister(ret_value, "a0");
        rsicV_code += code_this_time;
        rsicV_code+="  ret\n";
        break;
    }
    case KOOPA_RVT_BINARY:{
        if(DEBUG) cout<<"KOOPA_RVT_BINARY"<<endl;
        koopa_raw_binary_t binary = value->kind.data.binary;
        string code_this_time = VisitIR(binary, rsicV_code);
        rsicV_code += code_this_time;
        rsicV_code+="  sw a1, "+to_string(irSymbolTable.find(value))+"(sp)\n";
        break;
    }
    case KOOPA_RVT_INTEGER:{
        if(DEBUG) cout<<"KOOPA_RVT_INTEGER"<<endl;
        koopa_raw_integer_t integer = value->kind.data.integer;
        int32_t int_val = integer.value;
        rsicV_code+="  li a0, "+to_string(int_val)+"\n";
        break;
    }
    case KOOPA_RVT_ALLOC:{
        if(DEBUG) cout<<"KOOPA_RVT_ALLOC"<<endl;
        koopa_raw_block_arg_ref_t alloc = value->kind.data.block_arg_ref;
        cout<<"index: "<<alloc.index<<endl;
        cout<<"alloc store at: "<<irSymbolTable.find(value)<<endl;
        break;
    }
    case KOOPA_RVT_LOAD:{
        // 什么也不做？
        if(DEBUG) cout<<"KOOPA_RVT_LOAD"<<endl;
        koopa_raw_load_t load = value->kind.data.load;
        koopa_raw_value_t src = load.src;
        cout<<"src tag: "<<raw_value_tag(src->kind.tag)<<endl;
        cout<<"load store at: "<<irSymbolTable.find(value)<<endl;
        break;
    }
    case KOOPA_RVT_STORE:{
        if(DEBUG) cout<<"KOOPA_RVT_STORE"<<endl;
        koopa_raw_store_t store = value->kind.data.store;
        cout<<"store value: "<<raw_value_tag(store.value->kind.tag)<<endl;
        cout<<"store dst: "<<raw_value_tag(store.dest->kind.tag)<<endl;
        // store.dest的tag99%是ALLOC

        if(store.value->kind.tag==KOOPA_RVT_LOAD){
            koopa_raw_load_t load = store.value->kind.data.load;
            koopa_raw_value_t src = load.src;
            irSymbolTable.change(store.dest, irSymbolTable.find(src));
            cout<<"store value change to: "<<irSymbolTable.find(store.dest)<<endl;
        }
        else{
            irSymbolTable.change(store.dest, irSymbolTable.find(store.value));
        }
        break;
    }
    case KOOPA_RVT_BRANCH:{
        if(DEBUG) cout<<"KOOPA_RVT_BRANCH"<<endl;
        koopa_raw_branch_t branch = value->kind.data.branch;
        // TODO
        koopa_raw_value_t cond = branch.cond;

        // koopa_raw_basic_block_t true_bb = branch.true_bb;
        // koopa_raw_basic_block_t false_bb = branch.false_bb;
        // string code_this_time = loadRegister(cond, "a1");
        // rsicV_code += code_this_time;
        // rsicV_code+="  beqz a1, "+to_string(irSymbolTable.find(false_bb))+"\n";
        // rsicV_code+="  j "+to_string(irSymbolTable.find(true_bb))+"\n";
        break;
    }
    case KOOPA_RVT_JUMP:{

    }
    default:
        cout<<"koopa_raw_value_t error:value->kind.tag: "
        <<raw_value_kind(value->kind.tag)<<endl;
        break;
    }
}

// 计算二元表达式，把算出来的结果放在a1中，返回riscv代码
string VisitIR(koopa_raw_binary_t binary, string &rsicV_code){
    if(DEBUG) cout<<"Visit raw binary"<<endl;
    string code_this_time;
    koopa_raw_value_t lhs = binary.lhs, rhs=binary.rhs;
    /*
    if(DEBUG) 
    {
    cout<<"  KOOPA_RVT_BINARY"<<endl;
    string lhs_name, rhs_name;
    if(binary.lhs->name)
        lhs_name = binary.lhs->name;
    else
        lhs_name = "null";

    if(binary.rhs->name)
        rhs_name = binary.rhs->name;
    else
        rhs_name = "null";
    koopa_raw_slice_t used_by=value->used_by;
    koopa_raw_type_t ty=value->ty;


    cout<<"    binary.op: "<<raw_binary_op(binary.op)
    <<", binary.lhs: { kind:"<<raw_value_tag(binary.lhs->kind.tag)
    <<", type:"<<raw_type_tag(binary.lhs->ty->tag)
    <<", name: "<<lhs_name
    <<", used_by.len: "<<binary.lhs->used_by.len<<", used_by.kind: "
    <<raw_slice_item_kind(binary.lhs->used_by.kind)<<"},      ";

    cout<<"binary.rhs: { kind:"
    <<raw_value_tag(binary.rhs->kind.tag)
    <<", type:"<<raw_type_tag(binary.rhs->ty->tag)
    <<", name: "<<rhs_name<<", used_by.len: "<<binary.rhs->used_by.len<<", used_by.kind: "
    <<raw_slice_item_kind(binary.rhs->used_by.kind)<<"}"<<endl;
    }
    */

    code_this_time+=loadRegister(lhs, "a1");
    code_this_time+=loadRegister(rhs, "a2");
    
    switch (binary.op)
    {
    case KOOPA_RBO_NOT_EQ:
        code_this_time+="sub a1, a1, a2\n";
        code_this_time+="snez a1, a1\n";
        break;
    case KOOPA_RBO_EQ:
        code_this_time+="sub a1, a1, a2\n";
        code_this_time+="seqz a1, a1\n";
        break;
    case KOOPA_RBO_GT:
        code_this_time+="sgt a1, a1, a2\n";
        break;
    case KOOPA_RBO_LT:
        code_this_time+="slt a1, a1, a2\n";
        break;
    case KOOPA_RBO_LE:
        code_this_time+="slt a3, a1, a2\n"; // 如果a1小于a2，a3为1，否则为0
        code_this_time+="sub a4, a1, a2\n"; // 如果a1等于a2，a4为0，否则为非0
        code_this_time+="seqz a4, a4\n"; // 如果a1等于a2，a4为1，否则为0
        code_this_time+="add a1, a3, a4\n"; // 如果a1小于等于a2，a1为1，否则为0
        break;
    case KOOPA_RBO_GE:
        code_this_time+="sgt a3, a1, a2\n"; // 如果a1大于a2，a3为1，否则为0
        code_this_time+="sub a4, a1, a2\n"; // 如果a1等于a2，a4为0，否则为非0
        code_this_time+="seqz a4, a4\n"; // 如果a1等于a2，a4为1，否则为0
        code_this_time+="add a1, a3, a4\n"; // 如果a1大于等于a2，a1为1，否则为0
        break;
    case KOOPA_RBO_ADD:
        code_this_time+="add a1, a1, a2\n";
        break;
    case KOOPA_RBO_SUB:
        code_this_time+="sub a1, a1, a2\n";
        break;
    case KOOPA_RBO_MUL:
        code_this_time+="mul a1, a1, a2\n";
        break;
    case KOOPA_RBO_DIV:
        code_this_time+="div a1, a1, a2\n";
        break;
    case KOOPA_RBO_MOD:
        code_this_time+="rem a1, a1, a2\n";
        break;
    case KOOPA_RBO_AND:
        code_this_time+="and a1, a1, a2\n";
        break;
    case KOOPA_RBO_OR:
        code_this_time+="or a1, a1, a2\n";
        break;
    case KOOPA_RBO_XOR:
        code_this_time+="xor a1, a1, a2\n";
        break;
    case KOOPA_RBO_SHL:
        code_this_time+="sll a1, a1, a2\n";
        break;
    case KOOPA_RBO_SHR:
        code_this_time+="srl a1, a1, a2\n";
        break;
    case KOOPA_RBO_SAR:
        code_this_time+="sra a1, a1, a2\n";
        break;
    default:
        cout<<"binary.op error"<<binary.op<<endl;
        break;
    }
    return code_this_time;
}

string loadRegister(koopa_raw_value_t value, string target){
    switch (value->kind.tag)
    {
    case KOOPA_RVT_INTEGER:{
        int32_t int_val = value->kind.data.integer.value;
        return "li " + target + ", "+to_string(int_val)+"\n";
    }
    case KOOPA_RVT_LOAD:{
        return "lw " + target + ", "+to_string(irSymbolTable.find(value->kind.data.load.src))+"(sp)\n";
    }
    case KOOPA_RVT_BINARY:{
        return "lw " + target + ", "+to_string(irSymbolTable.find(value))+"(sp)\n";
    }
    default:
        cout<<"loadRegister error"<<endl;
        return "loadRegister error";
    }
}
        