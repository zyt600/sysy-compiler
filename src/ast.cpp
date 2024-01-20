#include "ast.h"
#include <iostream>
#include <cassert>
using namespace std;

string ExpAST::DumpKoopa(){
    if(DEBUG) cout<<"ExpAST called"<<endl;
    string ret = l_or_exp->DumpKoopa();
    storeNum = l_or_exp->storeNum;
    return ret;
}

string UnaryExpAST::DumpKoopa(){
    if(DEBUG) cout<<"UnaryExpAST called"<<endl;
    switch (kind) {
        case Kind::PrimaryExp:{
            string preIR_Code = primary_exp->DumpKoopa();
            storeNum = primary_exp->storeNum;
            return preIR_Code;
        }
        case Kind::UnaryOp_UnaryExp:{
            string op=unary_op->DumpKoopa();
            string preIR_Code = unary_exp->DumpKoopa();
            string lastStoreNum = unary_exp->storeNum;

            if(op=="!"){
                storeNum = GetNext();
                string code_this_line = "eq " + lastStoreNum + ", 0\n";
                return preIR_Code + storeNum + " = " + code_this_line;
            }else if(op=="-"){
                storeNum = GetNext();
                string code_this_line = "sub 0, " + lastStoreNum + "\n";
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

string PrimaryExpAST::DumpKoopa(){
    if(DEBUG) cout<<"PrimaryExpAST called"<<endl;
    switch (kind) {
        case Kind::Exp:{
            string ret = exp->DumpKoopa();
            storeNum = exp->storeNum;
            return ret;
        }
        case Kind::Number:{
            // TODO: 这里存疑，koopa怎么写的我也不确定；不过实在不行可以写成数字加0
            // return GetNext() + " = " + to_string(num);
            storeNum = GetNext();
            return storeNum + " = add 0, " + to_string(num) + "\n";
        }
        case Kind::LVal:{
            LValAST* l_val_temp = dynamic_cast<LValAST*>(l_val.get());
            string code =l_val->DumpKoopa();
            
            storeNum = GetNext();
            string temp_id = symbolTableNow->find(l_val_temp->ident);
            string code_this_line = storeNum + " = load " + temp_id + "\n";
            return code + code_this_line;
        }
        default:
            return "PrimaryExpAST error\n";
    }
}

string MulExpAST::DumpKoopa(){
    if(DEBUG) cout<<"MulExpAST called"<<endl;
    switch (kind) {
        case Kind::UnaryExp:{
            string preIR_Code = unary_exp->DumpKoopa();
            storeNum = unary_exp->storeNum;
            return preIR_Code;
        }
        case Kind::MulExp_Mul_UnaryExp:{
            string preIR_Code = mul_exp->DumpKoopa();
            string preIR_Code2 = unary_exp->DumpKoopa();
            string storeNum1=mul_exp->storeNum;
            string storeNum2=unary_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = mul " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::MulExp_Div_UnaryExp:{
            string preIR_Code = mul_exp->DumpKoopa();
            string preIR_Code2 = unary_exp->DumpKoopa();
            string storeNum1=mul_exp->storeNum;
            string storeNum2=unary_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = div " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::MulExp_Mod_UnaryExp:{
            string preIR_Code = mul_exp->DumpKoopa();
            string preIR_Code2 = unary_exp->DumpKoopa();
            string storeNum1=mul_exp->storeNum;
            string storeNum2=unary_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = mod " + storeNum1 + ", " + storeNum2 + "\n";
        }
        default:
            return "MulExpAST error\n";
    }
}

string AddExpAST::DumpKoopa(){
    if(DEBUG) cout<<"AddExpAST called"<<endl;
    switch (kind) {
        case Kind::MulExp:{
            string preIR_Code = mul_exp->DumpKoopa();
            storeNum = mul_exp->storeNum;
            return preIR_Code;
        }
        case Kind::AddExp_Add_MulExp:{
            string preIR_Code = add_exp->DumpKoopa();
            string preIR_Code2 = mul_exp->DumpKoopa();
            string storeNum1=add_exp->storeNum;
            string storeNum2=mul_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = add " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::AddExp_Sub_MulExp:{
            string preIR_Code = add_exp->DumpKoopa();
            string preIR_Code2 = mul_exp->DumpKoopa();
            string storeNum1=add_exp->storeNum;
            string storeNum2=mul_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = sub " + storeNum1 + ", " + storeNum2 + "\n";
        }
        default:
            return "AddExpAST error\n";
    }
}

string StmtAST::DumpKoopa(){
    if(DEBUG) cout<<"StmtAST called"<<endl;
    switch (kind)
    {
    case Kind::NONIF:{
        return non_if_stmt->DumpKoopa();
    }
    case Kind::MATCHED:{
        return matched_stmt->DumpKoopa();
    }
    case Kind::UNMATCHED:{
        return unmatched_stmt->DumpKoopa();
    }
    default:
        return "StmtAST error\n";
    }
}

string MatchedStmtAST::DumpKoopa() {
    if(DEBUG) cout<<"MatchedStmt called"<<endl;
    switch (kind)
    {
    case Kind::IF_ELSE:{  // if expr -> MatchedStmtAST else MatchedStmtAST
        string preIR_Code = exp->DumpKoopa();
        string expStoreNum=exp->storeNum;
        string entrySuccess="%entry" + to_string(GlobalCounter::GetInstance().GetNext());
        string entryFail="%entry" + to_string(GlobalCounter::GetInstance().GetNext());
        string entryNext="%entry" + to_string(GlobalCounter::GetInstance().GetNext()); // 结束了if lese的下一条指令
        string code_this_line = "br " + expStoreNum + ", " 
               + entrySuccess + ", " + entryFail + "\n";
        string code_this_line2 = entrySuccess + ":\n" 
               + matched_stmt1->DumpKoopa() + "jump "+ entryNext + "\n";
        string code_this_line3 = entryFail + ":\n" 
               + matched_stmt2->DumpKoopa() + "jump "+ entryNext + "\n";
        // TODO: 这里的entryFail + "\n"其实是有点问题的，没考虑到如果if后面没有语句的情况
        return preIR_Code + code_this_line + code_this_line2
               + code_this_line3 + entryNext + ":\n";
    }
    case Kind::NON_IF:{
        return non_if_stmt->DumpKoopa();
    }
    default:
        return "MatchedStmt error\n";
    }
}

string UnmatchedStmtAST::DumpKoopa() {
    if(DEBUG) cout<<"UnmatchedStmt called"<<endl;
    switch (kind)
    {
    case Kind::IF:{  // if expr ->stmt 否则跳过
        string preIR_Code = exp->DumpKoopa();
        string expStoreNum=exp->storeNum;
        string entrySuccess="%entry" + to_string(GlobalCounter::GetInstance().GetNext());
        string entryFail="%entry" + to_string(GlobalCounter::GetInstance().GetNext());

        string code_this_line = "br " + expStoreNum + ", " + entrySuccess + ", " + entryFail + "\n";
        string code_this_line2 = entrySuccess + ":\n" + stmt->DumpKoopa() + "jump "+ entryFail + "\n";
        // TODO: 这里的entryFail + "\n"其实是有点问题的，没考虑到如果if后面没有语句的情况
        return preIR_Code + code_this_line + code_this_line2 + entryFail + ":\n";
    }
    case Kind::IF_ELSE:{
        string preIR_Code = exp->DumpKoopa();
        string expStoreNum=exp->storeNum;
        string entrySuccess="%entry" + to_string(GlobalCounter::GetInstance().GetNext());
        string entryFail="%entry" + to_string(GlobalCounter::GetInstance().GetNext());
        string entryNext="%entry" + to_string(GlobalCounter::GetInstance().GetNext()); // 结束了if lese的下一条指令
        string code_this_line = "br " + expStoreNum + ", " 
               + entrySuccess + ", " + entryFail + "\n";
        string code_this_line2 = entrySuccess + ":\n" + 
               matched_stmt->DumpKoopa() + "jump "+ entryNext + "\n";
        string code_this_line3 = entryFail + ":\n" + 
               unmatched_stmt->DumpKoopa() + "jump "+ entryNext + "\n";
        return preIR_Code + code_this_line + code_this_line2 
               + code_this_line3 + entryNext + ":\n";
    }
    default:
        return "UnmatchedStmt error\n";
    }
}

string NonIfStmtAST::DumpKoopa(){
    if(DEBUG) cout<<"NonIfStmtAST called"<<endl;
    switch (kind)
    {
    case Kind::RET_EXP:{
        string preIR_Code = exp->DumpKoopa();
        storeNum = exp->storeNum;
        return preIR_Code + "ret " + storeNum + "\n";
    }
    case Kind::LVALeqEXP:{
        string preIR_Code = exp->DumpKoopa();
        LValAST* l_val_temp = dynamic_cast<LValAST*>(l_val.get());
        // symbolTableNow->change(l_val_temp->ident, exp->storeNum);
        string code_this_line = "store " + exp->storeNum + ", " + symbolTableNow->find(l_val_temp->ident) + "\n";
        return preIR_Code + code_this_line;
    }
    case Kind::EMPTY:{
        return "";
    }
    case Kind::EXP:{
        string preIR_Code = exp->DumpKoopa();
        // storeNum = exp->storeNum;
        return preIR_Code;
    }
    case Kind::RET:{
        return "ret 0\n";
    }
    case Kind::BLOCK:{
        newSymbolTable();
        string preIR_Code = block->DumpKoopa();
        deleteSymbolTable();
        return preIR_Code;
    }
    case Kind::WHILE:{
        string entryCond="%entryCond"+ to_string(GlobalCounter::GetInstance().GetNext());
        string entryBody="%entryBody"+ to_string(GlobalCounter::GetInstance().GetNext());
        string entryNext="%entryNext"+ to_string(GlobalCounter::GetInstance().GetNext());
        string exp_code = exp->DumpKoopa();
        string stmt_code = stmt->DumpKoopa();
        return "jump " + entryCond + "\n" + entryCond + ":\n" + exp_code + "br " + exp->storeNum + ", "+entryBody + ", " + entryNext 
               + "\n" + entryBody + ":\n"  + stmt_code + "jump " + entryCond + "\n" + entryNext + ":\n";
    }
    }
}

string LOrExpAST::DumpKoopa(){
    if(DEBUG) cout<<"LOrExpAST called"<<endl;
    switch (kind)
    {
        case Kind::LAndExp:{
            string preIR_Code = l_and_exp->DumpKoopa();
            storeNum = l_and_exp->storeNum;
            return preIR_Code;
        }
        case Kind::LOrExp_LAndExp:{
            string preIR_Code = l_or_exp->DumpKoopa();
            string preIR_Code2 = l_and_exp->DumpKoopa();
            string storeNum1=l_or_exp->storeNum;
            string storeNum2=l_and_exp->storeNum;
            string v1,v2;
            v1=GetNext();
            v2=GetNext();
            string code_this_line = v1 + " = ne " + storeNum1 + ", 0 \n";
            string code_this_line2 = v2 + " = ne " + storeNum2 + ", 0 \n";
            storeNum = GetNext();
            string code_this_line3=storeNum + " = or " + v1 + ", " + v2 + "\n";
            return preIR_Code + preIR_Code2 + code_this_line + code_this_line2 + code_this_line3;
        }
        default:
            return "LOrExpAST error\n";
    }
}

string LAndExpAST::DumpKoopa(){
    if(DEBUG) cout<<"LAndExpAST called"<<endl;
    switch(kind){
        case Kind::EqExp:{
            string preIR_Code = eq_exp->DumpKoopa();
            storeNum = eq_exp->storeNum;
            return preIR_Code;
        }
        case Kind::LAndExp_EqExp:{
            string preIR_Code = l_and_exp->DumpKoopa();
            string preIR_Code2 = eq_exp->DumpKoopa();
            string storeNum1=l_and_exp->storeNum;
            string storeNum2=eq_exp->storeNum;
            string v1,v2;
            v1=GetNext();
            v2=GetNext();
            string code_this_line = v1 + " = ne " + storeNum1 + ", 0 \n";
            string code_this_line2 = v2 + " = ne " + storeNum2 + ", 0 \n";
            storeNum = GetNext();
            string code_this_line3=storeNum + " = and " + v1 + ", " + v2 + "\n";
            return preIR_Code + preIR_Code2 + code_this_line + code_this_line2 + code_this_line3;
        }
    }
}

string EqExpAST::DumpKoopa(){
    if(DEBUG) cout<<"EqExpAST called"<<endl;
    switch(kind){
        case Kind::RelExp:{
            string preIR_Code = rel_exp->DumpKoopa();
            storeNum = rel_exp->storeNum;
            return preIR_Code;
        }
        case Kind::EqExp_EqEq_RelExp:{
            string preIR_Code = eq_exp->DumpKoopa();
            string preIR_Code2 = rel_exp->DumpKoopa();
            string storeNum1=eq_exp->storeNum;
            string storeNum2=rel_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = eq " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::EqExp_NotEq_RelExp:{
            string preIR_Code = eq_exp->DumpKoopa();
            string preIR_Code2 = rel_exp->DumpKoopa();
            string storeNum1=eq_exp->storeNum;
            string storeNum2=rel_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = ne " + storeNum1 + ", " + storeNum2 + "\n";
        }
    }
}

string RelExpAST::DumpKoopa(){
    if(DEBUG) cout<<"RelExpAST called"<<endl;
    switch(kind){
        case Kind::AddExp:{
            string preIR_Code = add_exp->DumpKoopa();
            storeNum = add_exp->storeNum;
            return preIR_Code;
        }
        case Kind::RelExp_LT_AddExp:{
            string preIR_Code = rel_exp->DumpKoopa();
            string preIR_Code2 = add_exp->DumpKoopa();
            string storeNum1=rel_exp->storeNum;
            string storeNum2=add_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = lt " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::RelExp_GT_AddExp:{
            string preIR_Code = rel_exp->DumpKoopa();
            string preIR_Code2 = add_exp->DumpKoopa();
            string storeNum1=rel_exp->storeNum;
            string storeNum2=add_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = gt " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::RelExp_LE_AddExp:{
            string preIR_Code = rel_exp->DumpKoopa();
            string preIR_Code2 = add_exp->DumpKoopa();
            string storeNum1=rel_exp->storeNum;
            string storeNum2=add_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = le " + storeNum1 + ", " + storeNum2 + "\n";
        }
        case Kind::RelExp_GE_AddExp:{
            string preIR_Code = rel_exp->DumpKoopa();
            string preIR_Code2 = add_exp->DumpKoopa();
            string storeNum1=rel_exp->storeNum;
            string storeNum2=add_exp->storeNum;
            storeNum = GetNext();
            return preIR_Code + preIR_Code2 + storeNum + " = ge " + storeNum1 + ", " + storeNum2 + "\n";
        }
    }
}


string DeclAST::DumpKoopa(){
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


string VarDefAST::DumpKoopa(){
    if(DEBUG) cout<<"VarDefAST called"<<endl;
    switch (kind)
    {
    case Kind::IDENT:{
        string identName = "@" + ident + to_string(GlobalCounter::GetInstance().GetNext());
        string code_this_line = identName + " = alloc i32\n";
        symbolTableNow->insert(ident, identName);
        return code_this_line;
    }
    case Kind::IDENT_InitVal:{
        string code = init_val->DumpKoopa();
        string identName = "@" + ident + to_string(GlobalCounter::GetInstance().GetNext());
        string code_this_line = identName + " = alloc i32\n";
        string code_this_line2 = "store " + init_val->storeNum + ", "+ identName + "\n";
        symbolTableNow->insert(ident, identName);
        return code + code_this_line + code_this_line2;
    }
    default:
        return "VarDefAST error\n";
    }
}