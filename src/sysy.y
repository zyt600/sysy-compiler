%code requires {
  #include <memory>
  #include <string>
  #include "ast.h"
}

%{

#include <iostream>
#include <memory>
#include <string>
#include "ast.h"

// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);

using namespace std;

%}

// 定义 parser 函数和错误处理函数的附加参数
// 我们需要返回一个字符串作为 AST, 所以我们把附加参数定义成字符串的智能指针
// 解析完成后, 我们要手动修改这个参数, 把它设置成解析得到的字符串
%parse-param { std::unique_ptr<BaseAST> &ast }

// yylval 的定义, 我们把它定义成了一个联合体 (union)
// 因为 token 的值有的是字符串指针, 有的是整数
// 之前我们在 lexer 中用到的 str_val 和 int_val 就是在这里被定义的
// 至于为什么要用字符串指针而不直接用 string 或者 unique_ptr<string>?
// 请自行 STFW 在 union 里写一个带析构函数的类会出现什么情况
%union {
  std::string *str_val;
  int int_val;
  BaseAST *ast_val;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token INT RETURN GE LE EQ NE AND OR CONST IF ELSE
%token <str_val> IDENT 
%token <int_val> INT_CONST

// 非终结符的类型定义
%type <str_val> BType
%type <ast_val> FuncDef FuncType Block Stmt Exp UnaryExp UnaryOp PrimaryExp MulExp AddExp LOrExp LAndExp EqExp RelExp MutiBlockItem BlockItem ConstDecl ConstDef ConstInitVal ConstExp Decl LVal MultiConstDef VarDecl VarDef InitVal MultiVarDef NonIfStmt MatchedStmt UnmatchedStmt
%type <int_val> Number

%%

// 开始符, CompUnit ::= FuncDef, 大括号后声明了解析完成后 parser 要做的事情
// 之前我们定义了 FuncDef 会返回一个 str_val, 也就是字符串指针
// 而 parser 一旦解析完 CompUnit, 就说明所有的 token 都被解析了, 即解析结束了
// 此时我们应该把 FuncDef 返回的结果收集起来, 作为 AST 传给调用 parser 的函数
// $1 指代规则里第一个符号的返回值, 也就是 FuncDef 的返回值
CompUnit
  : FuncDef {
    auto comp_unit = make_unique<CompUnitAST>();
    comp_unit->func_def = unique_ptr<BaseAST>($1);
    ast = move(comp_unit);
  }
  ;

// FuncDef ::= FuncType IDENT '(' ')' Block;
// 我们这里可以直接写 '(' 和 ')', 因为之前在 lexer 里已经处理了单个字符的情况
// 解析完成后, 把这些符号的结果收集起来, 然后拼成一个新的字符串, 作为结果返回
// $$ 表示非终结符的返回值, 我们可以通过给这个符号赋值的方法来返回结果
// 你可能会问, FuncType, IDENT 之类的结果已经是字符串指针了
// 为什么还要用 unique_ptr 接住它们, 然后再解引用, 把它们拼成另一个字符串指针呢
// 因为所有的字符串指针都是我们 new 出来的, new 出来的内存一定要 delete
// 否则会发生内存泄漏, 而 unique_ptr 这种智能指针可以自动帮我们 delete
// 虽然此处你看不出用 unique_ptr 和手动 delete 的区别, 但当我们定义了 AST 之后
// 这种写法会省下很多内存管理的负担
FuncDef
  : FuncType IDENT '(' ')' Block {
    auto ast = new FuncDefAST();
    ast->func_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    ast->block = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  ;

// 同上, 不再解释
FuncType
  : INT {
    auto ast = new FuncTypeAST();
    ast->func_type = "i32";
    $$ = ast;
  }
  ;

Block
  : '{' MutiBlockItem '}' {
    auto ast = new BlockAST();
    ast->multi_block_items = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

MutiBlockItem
  : BlockItem MutiBlockItem {
    MutiBlockItemAST* ast = (MutiBlockItemAST*)$2;
    ast->blockitems.push_front(unique_ptr<BaseAST>($1));
    $$ = (BaseAST*)ast;
  }
  | {
    $$ = (BaseAST*)new MutiBlockItemAST();
  }
  ;

BlockItem
  : Stmt {
    auto ast = new BlockItemAST(BlockItemAST::Kind::Stmt);
    ast->stmt = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | Decl {
    auto ast = new BlockItemAST(BlockItemAST::Kind::Decl);
    ast->decl = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

Stmt
  : NonIfStmt {
    auto ast = new StmtAST(StmtAST::Kind::NONIF);
    ast->non_if_stmt = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | MatchedStmt{
    auto ast = new StmtAST(StmtAST::Kind::MATCHED);
    ast->matched_stmt = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | UnmatchedStmt{
    auto ast = new StmtAST(StmtAST::Kind::UNMATCHED);
    ast->unmatched_stmt = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

NonIfStmt
  : RETURN Exp ';' {
    auto ast = new NonIfStmtAST(NonIfStmtAST::Kind::RET_EXP);
    ast->exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | LVal '=' Exp ';' {
    auto ast = new NonIfStmtAST(NonIfStmtAST::Kind::LVALeqEXP);
    ast->l_val = unique_ptr<BaseAST>($1);
    ast->exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | RETURN ';' {
    auto ast = new NonIfStmtAST(NonIfStmtAST::Kind::RET);
    $$ = ast;
  }
  | ';' {
    auto ast = new NonIfStmtAST(NonIfStmtAST::Kind::EMPTY);
    $$ = ast;
  }
  | Exp ';' {
    auto ast = new NonIfStmtAST(NonIfStmtAST::Kind::EXP);
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | Block {
    auto ast = new NonIfStmtAST(NonIfStmtAST::Kind::BLOCK);
    ast->block = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

MatchedStmt
  : IF '(' Exp ')' MatchedStmt ELSE MatchedStmt {
    auto ast = new MatchedStmtAST(MatchedStmtAST::Kind::IF_ELSE);
    ast->exp = unique_ptr<BaseAST>($3);
    ast->matched_stmt1 = unique_ptr<BaseAST>($5);
    ast->matched_stmt2 = unique_ptr<BaseAST>($7);
    $$ = ast;
  }
  | NonIfStmt {
    auto ast = new MatchedStmtAST(MatchedStmtAST::Kind::NON_IF);
    ast->non_if_stmt = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

UnmatchedStmt
  : IF '(' Exp ')' Stmt {
    auto ast = new UnmatchedStmtAST(UnmatchedStmtAST::Kind::IF);
    ast->exp = unique_ptr<BaseAST>($3);
    ast->stmt = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  | IF '(' Exp ')' MatchedStmt ELSE UnmatchedStmt {
    auto ast = new UnmatchedStmtAST(UnmatchedStmtAST::Kind::IF_ELSE);
    ast->exp = unique_ptr<BaseAST>($3);
    ast->matched_stmt = unique_ptr<BaseAST>($5);
    ast->unmatched_stmt = unique_ptr<BaseAST>($7);
    $$ = ast;
  }
  ;


Number
  : INT_CONST {
    $$ = $1;
  }
  ;

Exp
  : LOrExp {
    auto ast = new ExpAST();
    ast->l_or_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

UnaryExp
  : PrimaryExp {
    auto ast = new UnaryExpAST(UnaryExpAST::Kind::PrimaryExp);
    ast->primary_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | UnaryOp UnaryExp {
    auto ast = new UnaryExpAST(UnaryExpAST::Kind::UnaryOp_UnaryExp);
    ast->unary_op= unique_ptr<BaseAST>($1);
    ast->unary_exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

UnaryOp
  : '+' {
    auto ast = new UnaryOpAST();
    ast->op = "+";
    $$ = ast;
  }
  | '-' {
    auto ast = new UnaryOpAST();
    ast->op = "-";
    $$ = ast;
  }
  | '!' {
    auto ast = new UnaryOpAST();
    ast->op = "!";
    $$ = ast;
  }
  ;

PrimaryExp
  : Number {
    auto ast = new PrimaryExpAST(PrimaryExpAST::Kind::Number);
    ast->num = $1;
    $$ = ast;
  }
  | '(' Exp ')' {
    auto ast = new PrimaryExpAST(PrimaryExpAST::Kind::Exp);
    ast->exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | LVal {
    auto ast = new PrimaryExpAST(PrimaryExpAST::Kind::LVal);
    ast->l_val = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

AddExp
  : MulExp {
    auto ast = new AddExpAST(AddExpAST::Kind::MulExp);
    ast->mul_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | AddExp '+' MulExp {
    auto ast = new AddExpAST(AddExpAST::Kind::AddExp_Add_MulExp);
    ast->add_exp = unique_ptr<BaseAST>($1);
    ast->mul_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | AddExp '-' MulExp {
    auto ast = new AddExpAST(AddExpAST::Kind::AddExp_Sub_MulExp);
    ast->add_exp = unique_ptr<BaseAST>($1);
    ast->mul_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

MulExp
  : UnaryExp {
    auto ast = new MulExpAST(MulExpAST::Kind::UnaryExp);
    ast->unary_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | MulExp '*' UnaryExp {
    auto ast = new MulExpAST(MulExpAST::Kind::MulExp_Mul_UnaryExp);
    ast->mul_exp = unique_ptr<BaseAST>($1);
    ast->unary_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | MulExp '/' UnaryExp {
    auto ast = new MulExpAST(MulExpAST::Kind::MulExp_Div_UnaryExp);
    ast->mul_exp = unique_ptr<BaseAST>($1);
    ast->unary_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | MulExp '%' UnaryExp {
    auto ast = new MulExpAST(MulExpAST::Kind::MulExp_Mod_UnaryExp);
    ast->mul_exp = unique_ptr<BaseAST>($1);
    ast->unary_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

LOrExp
  : LAndExp {
    auto ast = new LOrExpAST(LOrExpAST::Kind::LAndExp);
    ast->l_and_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | LOrExp OR LAndExp {
    auto ast = new LOrExpAST(LOrExpAST::Kind::LOrExp_LAndExp);
    ast->l_or_exp = unique_ptr<BaseAST>($1);
    ast->l_and_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

LAndExp
  : EqExp {
    auto ast = new LAndExpAST(LAndExpAST::Kind::EqExp);
    ast->eq_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | LAndExp AND EqExp {
    auto ast = new LAndExpAST(LAndExpAST::Kind::LAndExp_EqExp);
    ast->l_and_exp = unique_ptr<BaseAST>($1);
    ast->eq_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

EqExp
  : RelExp {
    auto ast = new EqExpAST(EqExpAST::Kind::RelExp);
    ast->rel_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | EqExp EQ RelExp {
    auto ast = new EqExpAST(EqExpAST::Kind::EqExp_EqEq_RelExp);
    ast->eq_exp = unique_ptr<BaseAST>($1);
    ast->rel_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | EqExp NE RelExp {
    auto ast = new EqExpAST(EqExpAST::Kind::EqExp_NotEq_RelExp);
    ast->eq_exp = unique_ptr<BaseAST>($1);
    ast->rel_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

RelExp
  : AddExp {
    auto ast = new RelExpAST(RelExpAST::Kind::AddExp);
    ast->add_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | RelExp '<' AddExp {
    auto ast = new RelExpAST(RelExpAST::Kind::RelExp_LT_AddExp);
    ast->rel_exp = unique_ptr<BaseAST>($1);
    ast->add_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | RelExp '>' AddExp {
    auto ast = new RelExpAST(RelExpAST::Kind::RelExp_GT_AddExp);
    ast->rel_exp = unique_ptr<BaseAST>($1);
    ast->add_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | RelExp LE AddExp {
    auto ast = new RelExpAST(RelExpAST::Kind::RelExp_LE_AddExp);
    ast->rel_exp = unique_ptr<BaseAST>($1);
    ast->add_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | RelExp GE AddExp {
    auto ast = new RelExpAST(RelExpAST::Kind::RelExp_GE_AddExp);
    ast->rel_exp = unique_ptr<BaseAST>($1);
    ast->add_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

Decl
  : ConstDecl {
    auto ast = new DeclAST(DeclAST::Kind::ConstDecl);
    ast->const_decl = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | VarDecl{
    auto ast = new DeclAST(DeclAST::Kind::VarDecl);
    ast->var_decl = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

VarDecl
  : BType VarDef MultiVarDef';' {
    auto ast = new VarDeclAST();
    if(*$1=="int"){
      nextEleType=SymbolTableElementType::INT;
    }
    else{
      cout<<"error: unknown BType"<<endl;
    }
    ast->b_type=*unique_ptr<string>($1);
    MultiVarDefAST* temp = (MultiVarDefAST*)$3;
    temp->var_defs.push_front(unique_ptr<BaseAST>($2));
    ast->var_defs=unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

MultiVarDef
  : ',' VarDef MultiVarDef {
    MultiVarDefAST* ast = (MultiVarDefAST*)$3;
    ast-> var_defs.push_front(unique_ptr<BaseAST>($2));
    $$ = (BaseAST*)ast;
  }
  | {
    $$ = new MultiVarDefAST();
  }
  ;

VarDef
  : IDENT {
    auto ast = new VarDefAST(VarDefAST::Kind::IDENT);
    ast->type = nextEleType;
    ast->ident = *unique_ptr<string>($1);
    $$ = ast;
  }
  | IDENT '=' InitVal {
    auto ast = new VarDefAST(VarDefAST::Kind::IDENT_InitVal);
    ast->ident = *unique_ptr<string>($1);
    ast->init_val = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

InitVal
  : Exp {
    auto ast = new InitValAST();
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

ConstDecl
  : CONST BType ConstDef MultiConstDef ';' {
    auto ast = new ConstDeclAST();
    if(*$2=="int"){
      nextEleType=SymbolTableElementType::INT;
    }
    else{
      cout<<"error: unknown BType"<<endl;
    }

    ast->b_type=*unique_ptr<string>($2);
    MultiConstDefAST* temp = (MultiConstDefAST*)$4;
    temp->const_defs.push_front(unique_ptr<BaseAST>($3));
    ast->const_defs=unique_ptr<BaseAST>((BaseAST*)temp);
    $$ = ast;
  }
  ;

MultiConstDef
  : ',' ConstDef MultiConstDef {
    MultiConstDefAST* ast = (MultiConstDefAST*)$3;
    ast-> const_defs.push_front(unique_ptr<BaseAST>($2));
    $$ = (BaseAST*)ast;
  }
  | {
    $$ = new MultiConstDefAST();
  }
  ;

BType
  : INT {
    $$ = new string("int");
  }
  ;

ConstDef
  : IDENT '=' ConstInitVal {
    auto ast = new ConstDefAST();
    ast->type = nextEleType;
    ast->ident = *unique_ptr<string>($1);
    ast->const_init_val = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

ConstInitVal
  : ConstExp {
    auto ast = new ConstInitValAST();
    ast->const_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

ConstExp
  : Exp {
    auto ast = new ConstExpAST();
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

LVal
  : IDENT {
    auto ast = new LValAST();
    ast->ident = *unique_ptr<string>($1);
    $$ = ast;
  }
  ;
%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}
