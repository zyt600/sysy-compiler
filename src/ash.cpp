#include "ast.h"
#include <iostream>

std::string ExpAST::DumpKoopa(){
    std::string ret = add_exp->DumpKoopa();
    storeNum = dynamic_cast<AddExpAST*> (add_exp.get())->storeNum;
    return ret;
}

std::string UnaryExpAST::DumpKoopa(){
    switch (kind) {
        case Kind::PrimaryExp:{
        std::string ret = primary_exp->DumpKoopa();
        storeNum = dynamic_cast<PrimaryExpAST*> (primary_exp.get())->storeNum;
        return ret;
        }
        case Kind::UnaryOp_UnaryExp:{
        std::string op=unary_op->DumpKoopa();
        std::string preIR_Code = unary_exp->DumpKoopa();

        std::string lastStoreNum = dynamic_cast<UnaryExpAST*> (unary_exp.get())->storeNum;
        storeNum = GetNext();

        if(op=="!"){
            std::string code_this_line = "eq " + lastStoreNum + ", 0\n";
            return preIR_Code + storeNum + " = " + code_this_line;
        }else if(op=="-"){
            std::string code_this_line = "sub 0, " + lastStoreNum + "\n";
            return preIR_Code + storeNum + " = " + code_this_line;
        }else if(op=="+"){
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
        std::string ret = unary_exp->DumpKoopa();
        storeNum = dynamic_cast<UnaryExpAST*> (unary_exp.get())->storeNum;
        return ret;
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
        std::string ret = mul_exp->DumpKoopa();
        storeNum = dynamic_cast<MulExpAST*> (mul_exp.get())->storeNum;
        return ret;
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




