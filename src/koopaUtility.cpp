#include <iostream>
#include "koopa.h"
#include "koopaUtility.h"
#include <vector>
using namespace std;

int tabLen=2;

// 输出koopa_raw_value_tag_t对应的字符串
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

// koopa_raw_value_kind_t结构包含koopa_raw_value_tag_t，所以raw_value_tag和raw_value_kind一样
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

string raw_type_tag(int tag){ // koopa_raw_type_tag_t 就是koopa_raw_type_kind_t
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

string raw_slice_item_kind(int kind){ // koopa_raw_slice_item_kind_t
    switch (kind)
    {
    case KOOPA_RSIK_UNKNOWN:
        return "UNKNOWN";
    case KOOPA_RSIK_TYPE:
        return "TYPE";
    case KOOPA_RSIK_FUNCTION:
        return "FUNCTION";
    case KOOPA_RSIK_BASIC_BLOCK:
        return "BASIC_BLOCK";
    case KOOPA_RSIK_VALUE:
        return "VALUE";
    default:
        printf("raw_slice_item_kind error\n");
        return "raw_slice_item_kind error";
    }
}

bool startsWith(const string& str, const string& prefix) {
    if (str.length() < prefix.length()) {
        return false; // str 比 prefix 短，不可能以 prefix 开头
    }
    return str.compare(0, prefix.length(), prefix) == 0;
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (first == string::npos)
        return ""; // 字符串全是空白字符

    size_t last = str.find_last_not_of(" \t\r\f\v"); // 找到最后一个不是空白字符的位置（排除\n)
    return str.substr(first, (last - first + 1));
}

string processIR(string s){
    vector<string> v;
    istringstream iss(s);
    string tmp;
    while(getline(iss, tmp)){
        v.push_back(trim(tmp));
    }

    // 删除连着的jump和ret
    for(auto i=v.begin(); i!=v.end()-1; ){
        if(startsWith(*i, "ret") ){
            if(i+1!=v.end() && startsWith(*(i+1), "jump")){
                i = v.erase(i+1);
                continue;
            }
        }
        i++;
    }
    
    // 如果没有return，在最后加上
    bool hasEntry = false, hasBr = false;
    for(auto i=v.begin(); i!=v.end()-1/* 这里暂时假设了没有嵌套花括号 */; i++){
        if(startsWith(*i, "ret ")||startsWith(*i, "br ")||startsWith(*i, "jump ")){
            if(i+1!=v.end() && !startsWith(*(i+1), "%entry") && !startsWith(*(i+1), "}")){
                i = v.insert(i+1, "%entry"+to_string(GlobalCounter::GetInstance().GetNext())+":");
                i++;
                hasEntry=true;
            }
        }

        if(startsWith(*i, "%entry")){
            if(hasEntry&&!hasBr){ //这个基本块entry没有条件转移出口
                i = v.insert(i, "ret");
                i++;
            }
            hasEntry = true;
            hasBr = false;
        }
    }


    // 把字符串拼接回一起
    string ret;
    int level=0;
    for(auto i=v.begin(); i!=v.end(); i++){
        if((*i).find("}") != string::npos){
            level--;
        }
        ret += string(level*tabLen, ' ');
        if(!startsWith(*i, "%entry")&& i+1!=v.end() &&!startsWith(*i, "fun")){
            ret += string(tabLen, ' ');
        }
        ret += *i + "\n";
        if((*i).find("{") != string::npos){
            level++;
        }
    }
    return ret;
}

void file_append(string s, const char* output){
  ofstream file(output, ios::app);
  if (file.is_open()) {
        file << s <<endl;  // 将字符串 str 写入到文件中
        file.close(); // 关闭文件
  }else{
    cout<<"Error: cannot open file "<<output<<endl;
  }
}

string processRISCV(string s){
    vector<string> v;
    istringstream iss(s);
    string tmp;
    while(getline(iss, tmp)){
        v.push_back(trim(tmp));
    }
    for(auto i=v.begin(); i!=v.end(); i++){
        if(startsWith(*i, "ret")){
            v.insert(i, "addi sp, sp, 2000");
            i++;
        }
    }

    // 拼接代码，返回
    string ret;
    for(auto i=v.begin(); i!=v.end(); i++){
        if(startsWith(*i, ".")){
            ret += "  ";
        }
        ret += *i + "\n";
    }
    return ret;
}
