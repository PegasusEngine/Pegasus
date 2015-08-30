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
%pure-parser
%lex-param   { void* scanner }
%parse-param { void* scanner }
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

    #include "Pegasus/BlockScript/CompilerState.h"
    #ifndef YY_EXTRA_TYPE
    #define YY_EXTRA_TYPE Pegasus::BlockScript::CompilerState*
    #endif

    #include "Pegasus/BlockScript/BlockScriptBuilder.h"
    #include "Pegasus/BlockScript/IBlockScriptCompilerListener.h"
    #include "Pegasus/BlockScript/BlockScriptAst.h"
    #include "Pegasus/BlockScript/StackFrameInfo.h"
    #include "Pegasus/BlockScript/SymbolTable.h"
    #include "Pegasus/BlockScript/TypeDesc.h"
    #include "Pegasus/BlockScript/IFileIncluder.h"
    #include "Pegasus/BlockScript/bs.parser.hpp"
    #include "Pegasus/BlockScript/bs.lexer.hpp"
    #include "Pegasus/Memory/MemoryManager.h"
    
    #include "Pegasus/Core/Io.h"

    #define BS_GlobalBuilder BS_get_extra(scanner)->mBuilder
    #define BS_BUILD(r, exp) if ((r = BS_GlobalBuilder->exp) == nullptr) YYERROR ;
    #define BS_CHECKLIST(l) if (l == nullptr) {BS_parseerror("Empty list element, syntax error."); YYERROR;}


    using namespace Pegasus::Io;
    using namespace Pegasus::BlockScript;
    using namespace Pegasus::BlockScript::Ast;

    #include <stdio.h>

    // Pegasus hooks
    extern bool BS_HasNext(void* scanner);

    void BS_ErrorDispatcher(BlockScriptBuilder* builder, const char* message) 
    {
        builder->IncErrorCount();
        if (builder->GetEventListener() != nullptr)
        {
            yyscan_t scanner = builder->GetScanner();
            builder->GetEventListener()->OnCompilationError(builder->GetCurrentLine(), message, BS_get_text(scanner));
        }
    }

    #define BS_parseerror(errorstr) BS_ErrorDispatcher(BS_GlobalBuilder, errorstr)
    #define BS_error(scanner, errorstr)  BS_ErrorDispatcher(BS_GlobalBuilder, errorstr)
    
    //***************************************************//
    //              Let the insanity begin               //
    //***************************************************//
%}

// expect 108 reduce/shift warnings due to grammar ambiguity
%expect 130

%union {
    int    token;
    int    integerValue;
    float  floatValue;
    char*  identifierText;
    Pegasus::BlockScript::StackFrameInfo*      vFrameInfo;
    Pegasus::BlockScript::TypeDesc*            vTypeDesc;
    Pegasus::BlockScript::EnumNode*            vEnumNode;
    #define BS_PROCESS(N) Pegasus::BlockScript::Ast::N* v##N;
    #include "Pegasus/BlockScript/Ast.inl"
    #undef BS_PROCESS
}

%token <floatValue> I_FLOAT
%token <integerValue> I_INT
%token <identifierText> IDENTIFIER
%token <identifierText> TYPE_IDENTIFIER
%token <identifierText> I_STRING
%token <token> K_IF
%token <token> K_ELSE_IF
%token <token> K_ELSE
%token <token> K_SEMICOLON
%token <token> K_L_PAREN
%token <token> K_R_PAREN
%token <token> K_L_BRAC
%token <token> K_R_BRAC
%token <token> K_L_LACE
%token <token> K_R_LACE
%token <token> K_COMMA
%token <token> K_COL
%token <token> K_RETURN
%token <token> K_WHILE
%token <token> K_STRUCT
%token <token> K_ENUM
%token <token> K_STATIC_ARRAY
%token <token> K_SIZE_OF
%token <token> K_EXTERN
%token <token> O_PLUS 
%token <token> O_MINUS
%token <token> O_MUL
%token <token> O_DIV  
%token <token> O_MOD
%token <token> O_EQ
%token <token> O_NEQ
%token <token> O_GT
%token <token> O_LT
%token <token> O_GTE
%token <token> O_LTE
%token <token> O_LAND
%token <token> O_LOR
%token <token> O_SET
%token <token> O_DOT
%token <token> O_ACCESS
%token <token> O_METHOD_CALL
%token <token> O_IMPLICIT_CAST
%token <token> O_EXPLICIT_CAST
%type <vFrameInfo> if_begin_scope
%type <vFrameInfo> else_keyword
%type <vFrameInfo> struct_keyword 
%type <vProgram> program
%type <vExp>  exp 
%type <vExp>  immediate 
%type <vExp>  ident
%type <vExpList> exp_list
%type <vStmtList> stmt_list
%type <vStmtIfElse>   stmt_else_if_tail
%type <vStmtIfElse>   stmt_else_tail
%type <vStmtList> fun_stmt_list
%type <vStmtFunDec> fun_declaration
%type <vStmt> stmt
%type <vArgList> arg_list
%type <vArgList> struct_def_list
%type <vArgDec>  arg_dec
%type <vTypeDesc> fun_type
%type <vTypeDesc> type_desc
%type <vEnumNode> enum_list

%left O_LAND O_LOR
%left O_SET
%left O_EQ O_NEQ
%left O_GT O_LT O_GTE O_LTE
%left O_MOD
%left O_PLUS O_MINUS
%left O_MUL
%left O_DIV
%left O_METHOD_CALL
%left ACCESS_PREC
%left NEG CAST
%left O_DOT


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
        | K_EXTERN ident O_SET exp K_SEMICOLON  { 
            Exp * exp = nullptr;
            BS_BUILD(exp, BuildSetBinop($2, $4, true /*isExtern*/)); 
            if (exp != nullptr)
            {
                BS_BUILD($$, BuildStmtExp(exp));
            }
        }
        | K_RETURN exp K_SEMICOLON { BS_BUILD($$, BuildStmtReturn($2)); }
        | fun_declaration fun_stmt_list  {BS_BUILD($$, BindFunImplementation($1, $2));}
        | while_keyword K_L_PAREN exp K_R_PAREN K_L_BRAC stmt_list K_R_BRAC { BS_BUILD($$, BuildStmtWhile($3, $6));}
        | struct_keyword IDENTIFIER K_L_BRAC struct_def_list K_R_BRAC K_SEMICOLON { BS_BUILD($$, BuildStmtStructDef($2, $4)); }
        | K_ENUM IDENTIFIER K_L_BRAC enum_list K_R_BRAC K_SEMICOLON 
        { 
            if (BS_GlobalBuilder->GetSymbolTable()->GetTypeByName($2) == nullptr)
            {
                const Pegasus::BlockScript::TypeDesc* enumType = BS_GlobalBuilder->GetSymbolTable()->CreateEnumType(
                    $2,
                    $4 //the definition!
                );
        
                BS_BUILD($$, BuildStmtEnumTypeDef(enumType));
            }
            else
            {
                BS_parseerror("Syntax error. Enum type re-declaration. A type with this name already exists");
                YYERROR; 
            }
        }
        | K_IF K_L_PAREN exp if_begin_scope stmt_list K_R_BRAC stmt_else_if_tail stmt_else_tail 
                 { 
                    if ($8 != nullptr && $7 != nullptr)
                    {
                        Pegasus::BlockScript::Ast::StmtIfElse* tail = $7;
                        while (tail->GetTail() != nullptr) tail = tail->GetTail();
                        tail->SetTail($8); //else is the last statement
                    }
                    if ($8 != nullptr && $7 == nullptr)
                    {
                        BS_BUILD($$, BuildStmtIfElse($3, $5, $8, $4));
                    }
                    else
                    {
                        BS_BUILD($$, BuildStmtIfElse($3, $5, $7, $4));
                    }

                    BS_GlobalBuilder->PopFrame();
                 }
        ;

struct_keyword  : K_STRUCT { BS_BUILD($$, StartNewFrame()); }
                ;

if_begin_scope : K_R_PAREN K_L_BRAC { BS_BUILD($$, StartNewFrame()); }
               ;

fun_type : type_desc { $$ = $1; if ($$ == nullptr) { BS_parseerror("Syntax error. Invalid function type.");YYERROR; }; if (!BS_GlobalBuilder->StartNewFunction($$)) {BS_parseerror("cannot declare function within a function"); YYERROR;} }
         ;

while_keyword : K_WHILE { BS_GlobalBuilder->StartNewFrame(); }
              ; 

stmt_else_if_tail :  stmt_else_if_tail else_if_keyword K_L_PAREN exp if_begin_scope stmt_list K_R_BRAC
                    {
                        $$ = $1;
                        BS_CHECKLIST($1);
                        Pegasus::BlockScript::Ast::StmtIfElse* tail = $1;
                        while (tail->GetTail() != nullptr)
                        {
                            tail = tail->GetTail();
                        }
                        
                        tail->SetTail(
                            BS_GlobalBuilder->BuildStmtIfElse($4, $6, nullptr, $5)
                        ); 
                    }
                  |  else_if_keyword K_L_PAREN exp if_begin_scope stmt_list K_R_BRAC
                   {
                        $$ = BS_GlobalBuilder->BuildStmtIfElse($3, $5, nullptr, $4);
                   }
                  | /*empty*/ { $$ = nullptr; }
                  ;

else_if_keyword : K_ELSE_IF { //pop previous frame
                              BS_GlobalBuilder->PopFrame();  
                            }
                ;

fun_declaration : fun_type IDENTIFIER K_L_PAREN arg_list K_R_PAREN  {BS_BUILD($$, BuildStmtFunDec($4, $1, $2));}
                ;

stmt_else_tail : else_keyword K_L_BRAC stmt_list K_R_BRAC { BS_BUILD($$, BuildStmtIfElse(nullptr, $3, nullptr, $1)); }
               | /* empty */  { $$ = nullptr; } 
               ;

else_keyword  : K_ELSE { BS_GlobalBuilder->PopFrame(); BS_BUILD($$, StartNewFrame()); }
              ;

enum_list : enum_list K_COMMA IDENTIFIER {
                Pegasus::BlockScript::EnumNode* enumNode = BS_GlobalBuilder->GetSymbolTable()->NewEnumNode(); 
                enumNode->mIdd = $3;
                Pegasus::BlockScript::EnumNode* tailList = $1;
                while (tailList->mNext != nullptr) { tailList = tailList->mNext; }
                tailList->mNext = enumNode;
                enumNode->mGuid = tailList->mGuid + 1; 
                $$ = $1;
          }
          | IDENTIFIER {
                Pegasus::BlockScript::EnumNode* enumNode = BS_GlobalBuilder->GetSymbolTable()->NewEnumNode(); 
                enumNode->mIdd = $1;
                $$ = enumNode;
          }
          ;

fun_stmt_list : K_L_BRAC stmt_list K_R_BRAC { $$ = $2; }
              | K_SEMICOLON { $$ = nullptr; }
              ;

immediate : I_INT   { BS_BUILD($$, BuildImmInt($1)); }
          | I_FLOAT { BS_BUILD($$, BuildImmFloat($1)); }
          |  K_L_BRAC I_FLOAT K_COMMA I_FLOAT K_COMMA I_FLOAT K_COMMA I_FLOAT K_R_BRAC { BS_BUILD($$, BuildImmFloat4($2,$4,$6,$8)); }
          |  K_L_BRAC I_FLOAT K_COMMA I_FLOAT K_COMMA I_FLOAT K_R_BRAC { BS_BUILD($$, BuildImmFloat3($2,$4,$6)); }
          |  K_L_BRAC I_FLOAT K_COMMA I_FLOAT K_R_BRAC { BS_BUILD($$, BuildImmFloat2($2,$4)); }
          |  K_SIZE_OF K_L_PAREN type_desc K_R_PAREN 
          {
            //figure out size at compile time!
            BS_BUILD($$, BuildImmInt($3->GetByteSize()));
          }
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
        | IDENTIFIER      K_L_PAREN exp_list K_R_PAREN { BS_BUILD($$, BuildFunCall($3, $1)); }
        | TYPE_IDENTIFIER K_L_PAREN exp_list K_R_PAREN { BS_BUILD($$, BuildFunCall($3, $1)); }
        | K_STATIC_ARRAY O_LT type_desc  O_GT { BS_BUILD($$, BuildStaticArrayDec($3)); }
        | I_STRING { BS_BUILD($$, BuildStrImm($1)); }
        | immediate { $$ = $1; }
        | exp O_METHOD_CALL IDENTIFIER K_L_PAREN exp_list K_R_PAREN { BS_BUILD($$, BuildMethodCall($1, $3, $5)); }
        | exp O_METHOD_CALL TYPE_IDENTIFIER K_L_PAREN exp_list K_R_PAREN { BS_BUILD($$, BuildMethodCall($1, $3, $5)); }
        | exp O_SET exp    { BS_BUILD($$, BuildBinop($1, $2, $3)); }
        | exp O_PLUS exp   { BS_BUILD($$, BuildBinop($1, $2, $3)); }
        | exp O_MINUS exp  { BS_BUILD($$, BuildBinop($1, $2, $3)); }
        | exp O_MUL exp    { BS_BUILD($$, BuildBinop($1, $2, $3)); }
        | exp O_DIV exp    { BS_BUILD($$, BuildBinop($1, $2, $3)); }
        | exp O_MOD exp    { BS_BUILD($$, BuildBinop($1, $2, $3)); }
        | exp O_EQ   exp     { BS_BUILD($$, BuildBinop($1, $2, $3)); }
        | exp O_NEQ   exp     { BS_BUILD($$, BuildBinop($1, $2, $3)); }
        | exp O_LAND exp     { BS_BUILD($$, BuildBinop($1, $2, $3)); }
        | exp O_LOR  exp     { BS_BUILD($$, BuildBinop($1, $2, $3)); }
        | exp O_LT   exp     { BS_BUILD($$, BuildBinop($1, $2, $3)); }
        | exp O_GT   exp     { BS_BUILD($$, BuildBinop($1, $2, $3)); }
        | exp O_LTE exp    { BS_BUILD($$, BuildBinop($1, $2, $3)); }
        | exp O_GTE exp    { BS_BUILD($$, BuildBinop($1, $2, $3)); }
        | exp O_DOT ident    { BS_BUILD($$, BuildBinop($1, $2, $3)); }
        | exp K_L_LACE exp K_R_LACE %prec ACCESS_PREC { BS_BUILD($$, BuildBinop($1, O_ACCESS, $3)); }
        | O_MINUS exp %prec NEG   { BS_BUILD($$, BuildUnop($1, $2)); }
        | K_L_PAREN type_desc K_R_PAREN exp %prec CAST   { BS_BUILD($$, BuildExplicitCast($4, $2)); }
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

type_desc : type_desc K_L_LACE I_INT K_R_LACE { 
				Pegasus::BlockScript::TypeDesc* resultType = nullptr;
				if ($1->GetModifier() != Pegasus::BlockScript::TypeDesc::M_ARRAY)
				{
					$1->ComputeSize();
					resultType = BS_GlobalBuilder->GetSymbolTable()->CreateArrayType(
					    $1->GetName(), //name
					    $1,  // child type
					    $3   //array count
					);
				}        
				else
				{
					Pegasus::BlockScript::TypeDesc* target = $1;
					while (target->GetChild()->GetModifier() == Pegasus::BlockScript::TypeDesc::M_ARRAY)
					{
						target = target->GetChild();
					}
					Pegasus::BlockScript::TypeDesc* tmp = target->GetChild();
					resultType = BS_GlobalBuilder->GetSymbolTable()->CreateArrayType(
						$1->GetName(),
						tmp,
						$3
					);
					target->SetChild(resultType);
                    resultType = $1;
					$1->ComputeSize();
				}
                
                if (resultType != nullptr)
                {
                    $$ = resultType;
                }
                else
                {
                    BS_parseerror("Semantics error, unknown type specified.");
                    YYERROR;
                }
              }
            | TYPE_IDENTIFIER {                
                TypeDesc* typeDesc = BS_GlobalBuilder->GetTypeByName($1);
                if (typeDesc != nullptr)
                {
					typeDesc->ComputeSize();
                    $$ = typeDesc;
                }
                else
                {
                    BS_parseerror("Semantics error, unknown type specified.");
                    YYERROR;
                }
            }
            ;

struct_def_list : struct_def_list arg_dec K_SEMICOLON{
                $$ = $1;
                BS_CHECKLIST($1);
                ArgList* newList = BS_GlobalBuilder->CreateArgList(); 
                newList->SetArgDec($2);
                ArgList* tailTraversal = $1;
                while (tailTraversal->GetTail() != nullptr) { tailTraversal = tailTraversal->GetTail(); }
                tailTraversal->SetTail(newList);
         }
         | arg_dec K_SEMICOLON { $$ = BS_GlobalBuilder->CreateArgList(); $$->SetArgDec($1); }
         | /* empty */ { $$ = BS_GlobalBuilder->CreateArgList(); }
         ;

arg_dec : IDENTIFIER K_COL type_desc { BS_BUILD($$, BuildArgDec($1, $3)); }
        ;

%%         

//***************************************************//
//              Let the insanity end                 //
//***************************************************//

extern void BS_restart(FILE* f);


void Bison_BlockScriptParse(const FileBuffer* fileBuffer, BlockScriptBuilder* builder, IFileIncluder* fileIncluder) 
{          
    CompilerState compilerState(builder->GetAllocator());
    compilerState.mBuilder = builder;
    compilerState.mFileBuffer = fileBuffer;
    compilerState.GetPreprocessor().SetFileIncluder(fileIncluder);

    yyscan_t scanner;
    BS_lex_init_extra(&compilerState, &scanner);
    builder->SetScanner(scanner);

    do 
    {
	    BS_parse(scanner);
    } while (BS_HasNext(scanner) && BS_GlobalBuilder->GetErrorCount() == 0);

    builder->SetScanner(nullptr);
    BS_restart(nullptr, scanner);
    BS_lex_destroy(scanner);

}

