#include <iostream>
#include "koopa.h"
#include "koopaUtility.h"
using namespace std;


string raw_value_tag(int tag){
    switch (tag)
    {
    case KOOPA_RVT_INTEGER:
        return "INTEGER";
    case KOOPA_RVT_ZERO_INIT:
        return "ZERO_INIT";
    case KOOPA_RVT_UNDEF:
        return "UNDEF";
    case KOOPA_RVT_AGGREGATE:
        return "AGGREGATE";
    case KOOPA_RVT_FUNC_ARG_REF:
        return "FUNC_ARG_REF";
    case KOOPA_RVT_BLOCK_ARG_REF:
        return "BLOCK_ARG_REF";
    case KOOPA_RVT_ALLOC:
        return "ALLOC";
    case KOOPA_RVT_GLOBAL_ALLOC:
        return "GLOBAL_ALLOC";
    case KOOPA_RVT_LOAD:
        return "LOAD";
    case KOOPA_RVT_STORE:
        return "STORE";
    case KOOPA_RVT_GET_PTR:
        return "GET_PTR";
    case KOOPA_RVT_GET_ELEM_PTR:
        return "GET_ELEM_PTR";
    case KOOPA_RVT_BINARY:
        return "BINARY";
    case KOOPA_RVT_BRANCH:
        return "BRANCH";
    case KOOPA_RVT_JUMP:
        return "JUMP";
    case KOOPA_RVT_CALL:
        return "CALL";
    case KOOPA_RVT_RETURN:
        return "RETURN";
    default:
        printf("raw_value_tag error\n");
        return "raw_value_tag error";
    }
}

string raw_value_kind(int tag){
    switch (tag)
    {
    case KOOPA_RVT_INTEGER:
        return "INTEGER";
    case KOOPA_RVT_ZERO_INIT:
        return "ZERO_INIT";
    case KOOPA_RVT_UNDEF:
        return "UNDEF";
    case KOOPA_RVT_AGGREGATE:
        return "AGGREGATE";
    case KOOPA_RVT_FUNC_ARG_REF:
        return "FUNC_ARG_REF";
    case KOOPA_RVT_BLOCK_ARG_REF:
        return "BLOCK_ARG_REF";
    case KOOPA_RVT_ALLOC:
        return "ALLOC";
    case KOOPA_RVT_GLOBAL_ALLOC:
        return "GLOBAL_ALLOC";
    case KOOPA_RVT_LOAD:
        return "LOAD";
    case KOOPA_RVT_STORE:
        return "STORE";
    case KOOPA_RVT_GET_PTR:
        return "GET_PTR";
    case KOOPA_RVT_GET_ELEM_PTR:
        return "GET_ELEM_PTR";
    case KOOPA_RVT_BINARY:
        return "BINARY";
    case KOOPA_RVT_BRANCH:
        return "BRANCH";
    case KOOPA_RVT_JUMP:
        return "JUMP";
    case KOOPA_RVT_CALL:
        return "CALL";
    case KOOPA_RVT_RETURN:
        return "RETURN";
    default:
        printf("raw_value_kind error\n");
        return "raw_value_kind error";
    }
}

string raw_binary_op(int op){
    switch (op)
    {
    case KOOPA_RBO_NOT_EQ:
        return "!=";
    case KOOPA_RBO_EQ:
        return "==";
    case KOOPA_RBO_GT:
        return ">";
    case KOOPA_RBO_LT:
        return "<";
    case KOOPA_RBO_GE:
        return ">=";
    case KOOPA_RBO_LE:
        return "<=";
    case KOOPA_RBO_ADD:
        return "+";
    case KOOPA_RBO_SUB:
        return "-";
    case KOOPA_RBO_MUL:
        return "*";
    case KOOPA_RBO_DIV:
        return "/";
    case KOOPA_RBO_MOD:
        return "%";
    case KOOPA_RBO_AND:
        return "&";
    case KOOPA_RBO_OR:
        return "|";
    case KOOPA_RBO_XOR:
        return "^";
    case KOOPA_RBO_SHL:
        return "<<";
    case KOOPA_RBO_SHR:
        return ">>";
    case KOOPA_RBO_SAR:
        return ">>>";
    default:
        printf("raw_binary_op error\n");
        return "raw_binary_op error";
    }
}

string raw_type_tag(int tag){
    switch (tag)
    {
    case KOOPA_RTT_INT32:
        return "INT32";
    case KOOPA_RTT_UNIT:
        return "UNIT";
    case KOOPA_RTT_ARRAY:
        return "ARRAY";
    case KOOPA_RTT_POINTER:
        return "POINTER";
    case KOOPA_RTT_FUNCTION:
        return "FUNCTION";
    default:
        printf("raw_type_tag error\n");
        return "raw_type_tag error";
    }
}





