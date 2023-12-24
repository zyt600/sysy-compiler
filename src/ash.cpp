#include "ast.h"
#include <iostream>
#include <cassert>

std::string ExpAST::DumpKoopa(){
    std::string ret = l_or_exp->DumpKoopa();
    LOrExpAST* lOrExp = dynamic_cast<LOrExpAST*> (l_or_exp.get());
    if(lOrExp){
        storeNum = lOrExp->storeNum;
    }else{
        printf("ExpAST dynamic_cast error\n");
        assert(0);
    }
    return ret;
}

std::string UnaryExpAST::DumpKoopa(){
    switch (kind) {
        case Kind::PrimaryExp:{
            std::string preIR_Code = primary_exp->DumpKoopa();
            storeNum = dynamic_cast<PrimaryExpAST*> (primary_exp.get())->storeNum;
            return preIR_Code;
        }
        case Kind::UnaryOp_UnaryExp:{
            std::string op=unary_op->DumpKoopa();
            std::string preIR_Code = unary_exp->DumpKoopa();
            std::string lastStoreNum = dynamic_cast<UnaryExpAST*> (unary_exp.get())->storeNum;

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
                return "error";
            }
        }
    }
}

std::string PrimaryExpAST::DumpKoopa(){
    switch (kind) {
        case Kind::Exp:{
            std::string ret = exp->DumpKoopa();
            storeNum = dynamic_cast<ExpAST*> (exp.get())->storeNum;
            return ret;
        }
        case Kind::Number:{
            // TODO: 这里存疑，koopa怎么写的我也不确定；不过实在不行可以写成数字加0
            // return GetNext() + " = " + std::to_string(num);
            storeNum = GetNext();
            return storeNum + " = add 0, " + std::to_string(num) + "\n";
        }
    }
}

std::string MulExpAST::DumpKoopa(){
    switch (kind) {
        case Kind::UnaryExp:{
            std::string preIR_Code = unary_exp->DumpKoopa();
            storeNum = dynamic_cast<UnaryExpAST*> (unary_exp.get())->storeNum;
            return preIR_Code;
        }
        case Kind::MulExp_Mul_UnaryExp:{
            std::string preIR_Code = mul_exp->DumpKoopa();
            std::string preIR_Code2 = unary_exp->DumpKoopa();
            std::string storeNum1=dynamic_cast<MulExpAST*> (mul_exp.get())->storeNum;
            std::string storeNum2=dynamic_cast<UnaryExpAST*> (unary_exp.get())->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = mul " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::MulExp_Div_UnaryExp:{
            std::string preIR_Code = mul_exp->DumpKoopa();
            std::string preIR_Code2 = unary_exp->DumpKoopa();
            std::string storeNum1=dynamic_cast<MulExpAST*> (mul_exp.get())->storeNum;
            std::string storeNum2=dynamic_cast<UnaryExpAST*> (unary_exp.get())->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = div " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::MulExp_Mod_UnaryExp:{
            std::string preIR_Code = mul_exp->DumpKoopa();
            std::string preIR_Code2 = unary_exp->DumpKoopa();
            std::string storeNum1=dynamic_cast<MulExpAST*> (mul_exp.get())->storeNum;
            std::string storeNum2=dynamic_cast<UnaryExpAST*> (unary_exp.get())->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = mod " + storeNum1 + ", " + storeNum2 + "\n";
        }
        default:
            return "error";
    }
}

std::string AddExpAST::DumpKoopa(){
    switch (kind) {
        case Kind::MulExp:{
            std::string preIR_Code = mul_exp->DumpKoopa();
            storeNum = dynamic_cast<MulExpAST*> (mul_exp.get())->storeNum;
            return preIR_Code;
        }
        case Kind::AddExp_Add_MulExp:{
            std::string preIR_Code = add_exp->DumpKoopa();
            std::string preIR_Code2 = mul_exp->DumpKoopa();
            std::string storeNum1=dynamic_cast<AddExpAST*> (add_exp.get())->storeNum;
            std::string storeNum2=dynamic_cast<MulExpAST*> (mul_exp.get())->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = add " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::AddExp_Sub_MulExp:{
            std::string preIR_Code = add_exp->DumpKoopa();
            std::string preIR_Code2 = mul_exp->DumpKoopa();
            std::string storeNum1=dynamic_cast<AddExpAST*> (add_exp.get())->storeNum;
            std::string storeNum2=dynamic_cast<MulExpAST*> (mul_exp.get())->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = sub " + storeNum1 + ", " + storeNum2 + "\n";
        }
        default:
            return "error";
    }
}

std::string StmtAST::DumpKoopa(){
    std::string preIR_Code = exp->DumpKoopa();
    storeNum = dynamic_cast<ExpAST*> (exp.get())->storeNum;
    return preIR_Code + "ret " + storeNum + "\n";
}

std::string LOrExpAST::DumpKoopa(){
    switch (kind)
    {
        case Kind::LAndExp:{
            std::string preIR_Code = l_and_exp->DumpKoopa();
            storeNum = dynamic_cast<LAndExpAST*> (l_and_exp.get())->storeNum;
            return preIR_Code;
        }
        case Kind::LOrExp_LAndExp:{
            std::string preIR_Code = l_or_exp->DumpKoopa();
            std::string preIR_Code2 = l_and_exp->DumpKoopa();
            std::string storeNum1=dynamic_cast<LOrExpAST*> (l_or_exp.get())->storeNum;
            std::string storeNum2=dynamic_cast<LAndExpAST*> (l_and_exp.get())->storeNum;
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
    switch(kind){
        case Kind::EqExp:{
            std::string preIR_Code = eq_exp->DumpKoopa();
            storeNum = dynamic_cast<EqExpAST*> (eq_exp.get())->storeNum;
            return preIR_Code;
        }
        case Kind::LAndExp_EqExp:{
            std::string preIR_Code = l_and_exp->DumpKoopa();
            std::string preIR_Code2 = eq_exp->DumpKoopa();
            std::string storeNum1=dynamic_cast<LAndExpAST*> (l_and_exp.get())->storeNum;
            std::string storeNum2=dynamic_cast<EqExpAST*> (eq_exp.get())->storeNum;
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
    switch(kind){
        case Kind::RelExp:{
            std::string preIR_Code = rel_exp->DumpKoopa();
            storeNum = dynamic_cast<RelExpAST*> (rel_exp.get())->storeNum;
            return preIR_Code;
        }
        case Kind::EqExp_EqEq_RelExp:{
            std::string preIR_Code = eq_exp->DumpKoopa();
            std::string preIR_Code2 = rel_exp->DumpKoopa();
            std::string storeNum1=dynamic_cast<EqExpAST*> (eq_exp.get())->storeNum;
            std::string storeNum2=dynamic_cast<RelExpAST*> (rel_exp.get())->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = eq " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::EqExp_NotEq_RelExp:{
            std::string preIR_Code = eq_exp->DumpKoopa();
            std::string preIR_Code2 = rel_exp->DumpKoopa();
            std::string storeNum1=dynamic_cast<EqExpAST*> (eq_exp.get())->storeNum;
            std::string storeNum2=dynamic_cast<RelExpAST*> (rel_exp.get())->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = ne " + storeNum1 + ", " + storeNum2 + "\n";
        }
    }
}

std::string RelExpAST::DumpKoopa(){
    switch(kind){
        case Kind::AddExp:{
            std::string preIR_Code = add_exp->DumpKoopa();
            storeNum = dynamic_cast<AddExpAST*> (add_exp.get())->storeNum;
            return preIR_Code;
        }
        case Kind::RelExp_LT_AddExp:{
            std::string preIR_Code = rel_exp->DumpKoopa();
            std::string preIR_Code2 = add_exp->DumpKoopa();
            std::string storeNum1=dynamic_cast<RelExpAST*> (rel_exp.get())->storeNum;
            std::string storeNum2=dynamic_cast<AddExpAST*> (add_exp.get())->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = lt " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::RelExp_GT_AddExp:{
            std::string preIR_Code = rel_exp->DumpKoopa();
            std::string preIR_Code2 = add_exp->DumpKoopa();
            std::string storeNum1=dynamic_cast<RelExpAST*> (rel_exp.get())->storeNum;
            std::string storeNum2=dynamic_cast<AddExpAST*> (add_exp.get())->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = gt " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::RelExp_LE_AddExp:{
            std::string preIR_Code = rel_exp->DumpKoopa();
            std::string preIR_Code2 = add_exp->DumpKoopa();
            std::string storeNum1=dynamic_cast<RelExpAST*> (rel_exp.get())->storeNum;
            std::string storeNum2=dynamic_cast<AddExpAST*> (add_exp.get())->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = le " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::RelExp_GE_AddExp:{
            std::string preIR_Code = rel_exp->DumpKoopa();
            std::string preIR_Code2 = add_exp->DumpKoopa();
            std::string storeNum1=dynamic_cast<RelExpAST*> (rel_exp.get())->storeNum;
            std::string storeNum2=dynamic_cast<AddExpAST*> (add_exp.get())->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = ge " + storeNum1 + ", " + storeNum2 + "\n";
        }
    }
}


