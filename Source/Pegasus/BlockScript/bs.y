/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

// \file   bs.y
// \author Kleber Garcia
// \date   August 27th 2014
// \brief  Parser generator file (input to bison tool). 
//         This file contains a grammar description of the BlockScript scripting language.
//         This grammar gets parsed and becomes a cpp file containing the necesary functions

%{
    /****************************************************************************************/
    /*                                                                                      */
    /*                                       Pegasus                                        */
    /*                                                                                      */
    /****************************************************************************************/
    //! \file   bs.parser.cpp
    //! \author Kleber Garcia
    //! \brief  Generated file from bison tool.

    //Disable pegasus warning for generated code
    // warning 4065: missing default clause on switch
    //     reason: the generated code has a lot of warnings generated regarding this.
    #pragma warning(disable : 4065)
    #include "Pegasus/BlockScript/BlockScriptBuilder.h"
    #include "Pegasus/BlockScript/BlockScriptAst.h"
    #include "Pegasus/BlockScript/IddStrPool.h"
    #include "Pegasus/BlockScript/bs.parser.hpp"
    #include "Pegasus/Memory/MemoryManager.h"
    #include "Pegasus/Core/Io.h"

    #define BS_BUILD(r, exp) if ((r = BS_GlobalBuilder->exp) == nullptr) YYERROR ;
    #define BS_CHECKLIST(l) if (l == nullptr) {BS_error("Empty list element, syntax error."); YYERROR;}

    using namespace Pegasus::Io;
    using namespace Pegasus::BlockScript;
    using namespace Pegasus::BlockScript::Ast;

    #include <stdio.h>

    // Pegasus hooks
    extern int BS_lex();
    extern char* BS_text;
    extern int  BS_line;
    extern bool BS_HasNext();
    extern int BS_bufferPosition;

    BlockScriptBuilder* BS_GlobalBuilder    = nullptr;
    const FileBuffer*   BS_GlobalFileBuffer = nullptr;
    IddStrPool*         BS_GlobalIddStrPool = nullptr;

    void BS_error(const char *s) { BS_GlobalBuilder->IncErrorCount(); printf("%d : ERROR: %s near token '%s'\n", BS_line, s, BS_text); }
    
    //***************************************************//
    //              Let the insanity begin               //
    //***************************************************//
%}

// expect 50 reduce/shift warnings due to grammar ambiguity
%expect 50 

%union {
    int    token;
    int    integerValue;
    float  floatValue;
    char*  identifierText;
    #define BS_PROCESS(N) Pegasus::BlockScript::Ast::N* v##N;
    #include "Pegasus/BlockScript/Ast.inl"
    #undef BS_PROCESS
}

%token <floatValue> I_FLOAT
%token <integerValue> I_INT
%token <identifierText> IDENTIFIER
%token <token> K_IF
%token <token> K_ELSE_IF
%token <token> K_ELSE
%token <token> K_SEMICOLON
%token <token> K_L_PAREN
%token <token> K_R_PAREN
%token <token> K_L_BRAC
%token <token> K_R_BRAC
%token <token> K_COMMA
%token <token> K_COL
%token <token> K_RETURN
%token <token> K_WHILE
%token <token> O_MUL
%token <token> O_PLUS 
%token <token> O_MINUS
%token <token> O_DIV  
%token <token> O_ACCESS 
%token <token> O_EQ
%token <token> O_SET
%token <token> O_TREE
%type <vProgram> program
%type <vExp>  exp 
%type <vExp>  numeric
%type <vIdd>  ident
%type <vExpList> exp_list
%type <vStmtList> stmt_list
%type <vElseIfTail> stmt_else_if_tail
%type <vElseTail>   stmt_else_tail
%type <vStmtList> fun_stmt_list
%type <vStmt> stmt
%type <vArgList> arg_list
%type <vArgDec>  arg_dec

%left O_PLUS
%left O_MUL
%left O_SET

%start program

%%
        
program : stmt_list  { BS_BUILD($$, CreateProgram()); $$->SetStmtList($1); }
        ;

stmt_list : stmt_list stmt { 
                    $$ = $1;
                    BS_CHECKLIST($1);
                    StmtList* newTail = BS_GlobalBuilder->CreateStmtList();
                    newTail->SetStmt($2);
                    StmtList* tailTraversal = $1;
                    while(tailTraversal->GetTail() != nullptr) { tailTraversal = tailTraversal->GetTail(); }
                    tailTraversal->SetTail(newTail);
            }
          | stmt        { $$ = BS_GlobalBuilder->CreateStmtList(); $$->SetStmt($1); }
          | /* empty */ { $$ = BS_GlobalBuilder->CreateStmtList(); }
          ;

stmt    : exp K_SEMICOLON { BS_BUILD($$, BuildStmtExp($1)); }
        | ident O_TREE K_L_PAREN exp_list K_R_PAREN K_SEMICOLON { BS_BUILD($$, BuildStmtTreeModifier($4, $1)); }
        | K_RETURN exp K_SEMICOLON { BS_BUILD($$, BuildStmtReturn($2)); }
        | IDENTIFIER IDENTIFIER K_L_PAREN arg_list K_R_PAREN fun_stmt_list  {BS_BUILD($$, BuildStmtFunDec($4, $6, $1, $2));}
        | while_keyword K_L_PAREN exp K_R_PAREN K_L_BRAC stmt_list K_R_BRAC { BS_BUILD($$, BuildStmtWhile($3, $6));}
        | if_keyword K_L_PAREN exp K_R_PAREN K_L_BRAC stmt_list K_R_BRAC stmt_else_if_tail stmt_else_tail 
                 { 
                    if ($9 != nullptr && $8 != nullptr)
                    {
                        Pegasus::BlockScript::Ast::ElseTail* tail = $8;
                        while (tail->GetTail() != nullptr) tail = tail->GetTail();
                        tail->SetTail($9); //else is the last statement
                    }
                    if ($9 != nullptr && $8 == nullptr)
                    {
                        BS_BUILD($$, BuildStmtIfElse($3, $6, $9));
                    }
                    else
                    {
                        BS_BUILD($$, BuildStmtIfElse($3, $6, $8));
                    }
                 }
        ;

while_keyword : K_WHILE { BS_GlobalBuilder->StartNewFrame(); }
              ; 

if_keyword : K_IF { BS_GlobalBuilder->StartNewFrame(); }
           ; 

stmt_else_if_tail :  stmt_else_if_tail else_if_keyword K_L_PAREN exp K_R_PAREN K_L_BRAC stmt_list K_R_BRAC
                    {
                        $$ = $1;
                        BS_CHECKLIST($1);
                        Pegasus::BlockScript::Ast::ElseTail* tail = $1;
                        while (tail->GetTail() != nullptr)
                        {
                            tail = tail->GetTail();
                        }
                        
                        tail->SetTail(
                            BS_GlobalBuilder->BuildStmtElseIfTail($4, $7, nullptr)
                        ); 
                    }
                  |  else_if_keyword K_L_PAREN exp K_R_PAREN K_L_BRAC stmt_list K_R_BRAC
                   {
                        $$ = BS_GlobalBuilder->BuildStmtElseIfTail($3, $6, nullptr);
                   }
                  | /*empty*/ { $$ = nullptr; }
                  ;

else_if_keyword : K_ELSE_IF { BS_GlobalBuilder->StartNewFrame(); }
                ;

stmt_else_tail : else_keyword K_L_BRAC stmt_list K_R_BRAC { BS_BUILD($$, BuildStmtElseTail($3)); }
               | /* empty */  { $$ = nullptr; } 
               ;

else_keyword  : K_ELSE { BS_GlobalBuilder->StartNewFrame(); }
              ;

fun_stmt_list : K_L_BRAC stmt_list K_R_BRAC { $$ = $2; }
              | K_SEMICOLON { $$ = nullptr; }
              ;

numeric : I_INT   { BS_BUILD($$, BuildImmInt($1)); }
        | I_FLOAT { BS_BUILD($$, BuildImmFloat($1)); }
        ;

exp_list : exp_list K_COMMA exp {
                $$ = $1;
                BS_CHECKLIST($1);
                ExpList* newList = BS_GlobalBuilder->CreateExpList(); 
                newList->SetExp($3);
                ExpList* tailTraversal = $1;
                while (tailTraversal->GetTail() != nullptr) { tailTraversal = tailTraversal->GetTail(); }
                tailTraversal->SetTail(newList);
         }
         | exp { $$ = BS_GlobalBuilder->CreateExpList(); $$->SetExp($1); }
         |/* empty */ { $$ = BS_GlobalBuilder->CreateExpList(); }
         ; 

ident   : IDENTIFIER { BS_BUILD($$, BuildIdd($1)); }
        ;

exp     : ident  { $$ = $1; }
        | IDENTIFIER K_L_PAREN exp_list K_R_PAREN { BS_BUILD($$, BuildFunCall($3, $1)); }
        | numeric { $$ = $1; }
        | exp O_MUL exp  { BS_BUILD($$, BuildBinop($1, $2, $3)); }
        | exp O_PLUS exp { BS_BUILD($$, BuildBinop($1, $2, $3)); }
        | exp O_SET exp  { BS_BUILD($$, BuildBinop($1, $2, $3)); }
        | K_L_PAREN exp K_R_PAREN { $$ = $2; }
        ;

arg_list : arg_list K_COMMA arg_dec {
                $$ = $1;
                BS_CHECKLIST($1);
                ArgList* newList = BS_GlobalBuilder->CreateArgList(); 
                newList->SetArgDec($3);
                ArgList* tailTraversal = $1;
                while (tailTraversal->GetTail() != nullptr) { tailTraversal = tailTraversal->GetTail(); }
                tailTraversal->SetTail(newList);
         }
         | arg_dec { $$ = BS_GlobalBuilder->CreateArgList(); $$->SetArgDec($1); }
         | /* empty */ { $$ = BS_GlobalBuilder->CreateArgList(); }
         ;


arg_dec : IDENTIFIER K_COL IDENTIFIER { BS_BUILD($$, BuildArgDec($1, $3)); }
        ;

%%         

//***************************************************//
//              Let the insanity end                 //
//***************************************************//



void Bison_BlockScriptParse(const FileBuffer* fileBuffer, BlockScriptBuilder* builder, IddStrPool* iddStrPool) 
{          
    BS_line = 1;
    BS_bufferPosition = 0;
    BS_GlobalBuilder = builder;
    BS_GlobalIddStrPool = iddStrPool;
    BS_GlobalFileBuffer = fileBuffer;

    do 
    {
	    BS_parse();
    } while (BS_HasNext());
    
}

