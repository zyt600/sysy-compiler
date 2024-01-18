#include "ast.h"
#include <iostream>
#include <cassert>

std::string ExpAST::DumpKoopa(){
    if(DEBUG) std::cout<<"ExpAST called"<<std::endl;
    std::string ret = l_or_exp->DumpKoopa();
    storeNum = l_or_exp->storeNum;
    return ret;
}

std::string UnaryExpAST::DumpKoopa(){
    if(DEBUG) std::cout<<"UnaryExpAST called"<<std::endl;
    switch (kind) {
        case Kind::PrimaryExp:{
            std::string preIR_Code = primary_exp->DumpKoopa();
            storeNum = primary_exp->storeNum;
            return preIR_Code;
        }
        case Kind::UnaryOp_UnaryExp:{
            std::string op=unary_op->DumpKoopa();
            std::string preIR_Code = unary_exp->DumpKoopa();
            std::string lastStoreNum = unary_exp->storeNum;

            if(op=="!"){
                storeNum = GetNext();
                std::string code_this_line = "eq " + lastStoreNum + ", 0\n";
                return preIR_Code + storeNum + " = " + code_this_line;
            }else if(op=="-"){
                storeNum = GetNext();
                std::string code_this_line = "sub 0, " + lastStoreNum + "\n";
                return preIR_Code + storeNum + " = " + code_this_line;
            }else if(op=="+"){
                storeNum = lastStoreNum;
                return preIR_Code;
            }
            else{
                return "UnaryExpAST error\n";
            }
        }
    }
}

std::string PrimaryExpAST::DumpKoopa(){
    if(DEBUG) std::cout<<"PrimaryExpAST called"<<std::endl;
    switch (kind) {
        case Kind::Exp:{
            std::string ret = exp->DumpKoopa();
            storeNum = exp->storeNum;
            return ret;
        }
        case Kind::Number:{
            // TODO: 这里存疑，koopa怎么写的我也不确定；不过实在不行可以写成数字加0
            // return GetNext() + " = " + std::to_string(num);
            storeNum = GetNext();
            return storeNum + " = add 0, " + std::to_string(num) + "\n";
        }
        case Kind::LVal:{
            LValAST* l_val_temp = dynamic_cast<LValAST*>(l_val.get());
            storeNum = symbolTableNow->find(l_val_temp->ident);
            string code =l_val->DumpKoopa();
            return code;
        }
        default:
            return "PrimaryExpAST error\n";
    }
}

std::string MulExpAST::DumpKoopa(){
    if(DEBUG) std::cout<<"MulExpAST called"<<std::endl;
    switch (kind) {
        case Kind::UnaryExp:{
            std::string preIR_Code = unary_exp->DumpKoopa();
            storeNum = unary_exp->storeNum;
            return preIR_Code;
        }
        case Kind::MulExp_Mul_UnaryExp:{
            std::string preIR_Code = mul_exp->DumpKoopa();
            std::string preIR_Code2 = unary_exp->DumpKoopa();
            std::string storeNum1=mul_exp->storeNum;
            std::string storeNum2=unary_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = mul " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::MulExp_Div_UnaryExp:{
            std::string preIR_Code = mul_exp->DumpKoopa();
            std::string preIR_Code2 = unary_exp->DumpKoopa();
            std::string storeNum1=mul_exp->storeNum;
            std::string storeNum2=unary_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = div " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::MulExp_Mod_UnaryExp:{
            std::string preIR_Code = mul_exp->DumpKoopa();
            std::string preIR_Code2 = unary_exp->DumpKoopa();
            std::string storeNum1=mul_exp->storeNum;
            std::string storeNum2=unary_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = mod " + storeNum1 + ", " + storeNum2 + "\n";
        }
        default:
            return "MulExpAST error\n";
    }
}

std::string AddExpAST::DumpKoopa(){
    if(DEBUG) std::cout<<"AddExpAST called"<<std::endl;
    switch (kind) {
        case Kind::MulExp:{
            std::string preIR_Code = mul_exp->DumpKoopa();
            storeNum = mul_exp->storeNum;
            return preIR_Code;
        }
        case Kind::AddExp_Add_MulExp:{
            std::string preIR_Code = add_exp->DumpKoopa();
            std::string preIR_Code2 = mul_exp->DumpKoopa();
            std::string storeNum1=add_exp->storeNum;
            std::string storeNum2=mul_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = add " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::AddExp_Sub_MulExp:{
            std::string preIR_Code = add_exp->DumpKoopa();
            std::string preIR_Code2 = mul_exp->DumpKoopa();
            std::string storeNum1=add_exp->storeNum;
            std::string storeNum2=mul_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = sub " + storeNum1 + ", " + storeNum2 + "\n";
        }
        default:
            return "AddExpAST error\n";
    }
}

std::string StmtAST::DumpKoopa(){
    if(DEBUG) std::cout<<"StmtAST called"<<std::endl;
    switch (kind)
    {
    case Kind::RET_EXP:{
        std::string preIR_Code = exp->DumpKoopa();
        storeNum = exp->storeNum;
        return preIR_Code + "ret " + storeNum + "\n";
    }
    case Kind::LVALeqEXP:{
        std::string preIR_Code = exp->DumpKoopa();
        LValAST* l_val_temp = dynamic_cast<LValAST*>(l_val.get());
        symbolTableNow->change(l_val_temp->ident, exp->storeNum);
        return preIR_Code;
    }
    default:
        return "StmtAST error\n";
    }
    
}

std::string LOrExpAST::DumpKoopa(){
    if(DEBUG) std::cout<<"LOrExpAST called"<<std::endl;
    switch (kind)
    {
        case Kind::LAndExp:{
            std::string preIR_Code = l_and_exp->DumpKoopa();
            storeNum = l_and_exp->storeNum;
            return preIR_Code;
        }
        case Kind::LOrExp_LAndExp:{
            std::string preIR_Code = l_or_exp->DumpKoopa();
            std::string preIR_Code2 = l_and_exp->DumpKoopa();
            std::string storeNum1=l_or_exp->storeNum;
            std::string storeNum2=l_and_exp->storeNum;
            std::string v1,v2;
            v1=GetNext();
            v2=GetNext();
            std::string code_this_line = v1 + " = ge " + storeNum1 + ", 1 \n";
            std::string code_this_line2 = v2 + " = ge " + storeNum2 + ", 1 \n";
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + code_this_line + code_this_line2 + storeNum + " = or " + v1 + ", " + v2 + "\n";
        }
    }
}

std::string LAndExpAST::DumpKoopa(){
    if(DEBUG) std::cout<<"LAndExpAST called"<<std::endl;
    switch(kind){
        case Kind::EqExp:{
            std::string preIR_Code = eq_exp->DumpKoopa();
            storeNum = eq_exp->storeNum;
            return preIR_Code;
        }
        case Kind::LAndExp_EqExp:{
            std::string preIR_Code = l_and_exp->DumpKoopa();
            std::string preIR_Code2 = eq_exp->DumpKoopa();
            std::string storeNum1=l_and_exp->storeNum;
            std::string storeNum2=eq_exp->storeNum;
            std::string v1,v2;
            v1=GetNext();
            v2=GetNext();
            std::string code_this_line = v1 + " = ge " + storeNum1 + ", 1 \n";
            std::string code_this_line2 = v2 + " = ge " + storeNum2 + ", 1 \n";
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + code_this_line + code_this_line2 + storeNum + " = and " + v1 + ", " + v2 + "\n";
        }
    }
}

std::string EqExpAST::DumpKoopa(){
    if(DEBUG) std::cout<<"EqExpAST called"<<std::endl;
    switch(kind){
        case Kind::RelExp:{
            std::string preIR_Code = rel_exp->DumpKoopa();
            storeNum = rel_exp->storeNum;
            return preIR_Code;
        }
        case Kind::EqExp_EqEq_RelExp:{
            std::string preIR_Code = eq_exp->DumpKoopa();
            std::string preIR_Code2 = rel_exp->DumpKoopa();
            std::string storeNum1=eq_exp->storeNum;
            std::string storeNum2=rel_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = eq " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::EqExp_NotEq_RelExp:{
            std::string preIR_Code = eq_exp->DumpKoopa();
            std::string preIR_Code2 = rel_exp->DumpKoopa();
            std::string storeNum1=eq_exp->storeNum;
            std::string storeNum2=rel_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = ne " + storeNum1 + ", " + storeNum2 + "\n";
        }
    }
}

std::string RelExpAST::DumpKoopa(){
    if(DEBUG) std::cout<<"RelExpAST called"<<std::endl;
    switch(kind){
        case Kind::AddExp:{
            std::string preIR_Code = add_exp->DumpKoopa();
            storeNum = add_exp->storeNum;
            return preIR_Code;
        }
        case Kind::RelExp_LT_AddExp:{
            std::string preIR_Code = rel_exp->DumpKoopa();
            std::string preIR_Code2 = add_exp->DumpKoopa();
            std::string storeNum1=rel_exp->storeNum;
            std::string storeNum2=add_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = lt " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::RelExp_GT_AddExp:{
            std::string preIR_Code = rel_exp->DumpKoopa();
            std::string preIR_Code2 = add_exp->DumpKoopa();
            std::string storeNum1=rel_exp->storeNum;
            std::string storeNum2=add_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = gt " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::RelExp_LE_AddExp:{
            std::string preIR_Code = rel_exp->DumpKoopa();
            std::string preIR_Code2 = add_exp->DumpKoopa();
            std::string storeNum1=rel_exp->storeNum;
            std::string storeNum2=add_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = le " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::RelExp_GE_AddExp:{
            std::string preIR_Code = rel_exp->DumpKoopa();
            std::string preIR_Code2 = add_exp->DumpKoopa();
            std::string storeNum1=rel_exp->storeNum;
            std::string storeNum2=add_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = ge " + storeNum1 + ", " + storeNum2 + "\n";
        }
    }
}


std::string DeclAST::DumpKoopa(){
    if(DEBUG) cout<<"DeclAST called"<<endl;
    switch (kind)
    {
    case Kind::ConstDecl:
        return const_decl->DumpKoopa();
    case Kind::VarDecl:
        return var_decl->DumpKoopa();
    default:
        return "DeclAST error\n";
    }
  }


std::string VarDefAST::DumpKoopa(){
    if(DEBUG) cout<<"VarDefAST called"<<endl;
    switch (kind)
    {
    case Kind::IDENT:{
        symbolTableNow->insert(ident,"not init");
        return "";
    }
    case Kind::IDENT_InitVal:{
        string code = init_val->DumpKoopa();
        symbolTableNow->insert(ident,init_val->storeNum);
        return code;
    }
    default:
        return "VarDefAST error\n";
    }
}