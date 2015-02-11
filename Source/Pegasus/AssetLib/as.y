/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

// \file   as.y
// \author Kleber Garcia
// \date   February 8th 2015
// \brief  Parser generator file (input to bison tool). 
//         This file contains a grammar description of the AssetScript scripting language.
//         This grammar gets parsed and becomes a cpp file containing the necesary functions
%pure_parser
%lex-param   { void* scanner }
%parse-param { void* scanner }
%{
    /****************************************************************************************/
    /*                                                                                      */
    /*                                       Pegasus                                        */
    /*                                                                                      */
    /****************************************************************************************/
    //! \file   as.parser.cpp
    //! \author Kleber Garcia
    //! \brief  Generated file from bison tool.

    //Disable pegasus warning for generated code
    // warning 4065: missing default clause on switch
    //     reason: the generated code has a lot of warnings generated regarding this.
    #pragma warning(disable : 4065)

    #include "Pegasus/Core/Assertion.h"
    #include "Pegasus/Core/Log.h"
    #include "Pegasus/AssetLib/AsCompilerState.h"
    #include "Pegasus/AssetLib/AssetBuilder.h"
    #include "Pegasus/AssetLib/ASTree.h"
    #include "Pegasus/AssetLib/Asset.h"
    #include "Pegasus/Core/Io.h"
    #ifndef YY_EXTRA_TYPE
    #define YY_EXTRA_TYPE Pegasus::AssetLib::AsCompilerState*
    #endif

    #define AS_BUILDER AS_get_extra(scanner)->mBuilder

    #include "Pegasus/AssetLib/as.parser.hpp"
    #include "Pegasus/AssetLib/as.lexer.hpp"

    using namespace Pegasus;
    using namespace Pegasus::Io;
    using namespace Pegasus::AssetLib;


    // Pegasus hooks
    extern bool AS_HasNext(void* scanner);

    #define AS_ERROR(str) PG_LOG('ERR_', "Asset Error: %s, around line %d", str, AS_BUILDER->GetCurrentLine())
    #define AS_parseerror(errorstr) AS_ERROR(errorstr)
    #define AS_error(scanner, errorstr)  AS_ERROR(errorstr)

    //***************************************************//
    //              Let the second insanity begin        //
    //***************************************************//
%}

%union {
    int token;
    int integerValue;
    float floatValue;
    const char* identifierText;
    Pegasus::AssetLib::Object* objVal;
    Pegasus::AssetLib::Array*  arrayVal; 
    Pegasus::AssetLib::Asset*   root;
    Pegasus::AssetLib::VariantType variant;
}

%token <integerValue> I_INT
%token <floatValue> I_FLOAT
%token <identifierText> IDENTIFIER
%token <token> K_LEFT_LACE
%token <token> K_RIGHT_LACE
%token <token> K_LEFT_BRAC
%token <token> K_RIGHT_BRAC
%token <token> K_COMMA
%token <token> K_COLON
%type <root> assetroot
%type <objVal> object
%type <objVal> value_pair_list
%type <objVal> value_pair
%type <objVal> begin_obj
%type <arrayVal> array
%type <arrayVal> comma_list
%type <arrayVal> arr_begin
%type <variant> element

%start assetroot

%%

assetroot : object 
            {
                $$ = AS_BUILDER->BuildAsset( $1 );
                if ($$ == nullptr)
                {
                    AS_ERROR("Error creating asset node.");
                    YYERROR;
                }
            }
          ;

object    : begin_obj value_pair_list K_RIGHT_BRAC 
            {
                if ($1 == nullptr)
                {
                    AS_ERROR("Error creating object node.");
                    YYERROR;
                }
                else
                {
                    $$ = $1; 
                    AS_BUILDER->EndObject(); 
                }
            }
          ;

begin_obj : K_LEFT_BRAC { $$ = AS_BUILDER->BeginObject(); }
          ;

value_pair_list : value_pair_list K_COMMA value_pair { $$ = $1; }
                | value_pair { $$ = $1; }
                | /*empty*/ { $$ = nullptr; }
                ;

array     : arr_begin comma_list K_RIGHT_LACE 
            { 
                if ($1 == nullptr)
                {
                    AS_ERROR("Error creating array.");
                    YYERROR;
                }
                else
                {
                    $$ = $1;
                    AS_BUILDER->EndArray();
                }
            }
          ;

arr_begin : K_LEFT_LACE { $$ = AS_BUILDER->BeginArray(); }
          ;

comma_list : comma_list K_COMMA element 
             {
                 if ($$->GetType() != $3.mType)
                 {
                     AS_ERROR("Array with inconsisten types. All elements must be the same type.");
                     YYERROR;
                 }
                 else
                 {
                     $$ = $1; $$->PushElement($3.v); 
                 }
             }
           | element 
             {
                $$ = AS_BUILDER->GetArray();
                if ($$->GetType() == Array::AS_TYPE_NULL)
                {
                    $$->CommitType($1.mType);
                }
                else if ($$->GetType() != $1.mType)
                {
                    AS_ERROR("Array with inconsisten types. All elements must be the same type.");
                    YYERROR;
                }
                $$->PushElement($1.v);
             }
           | /*empty*/ { $$ = nullptr; }
           ;


value_pair : IDENTIFIER K_COLON element 
            { 
                $$ = AS_BUILDER->GetObject(); 
                if ($$ == nullptr)
                { 
                    AS_ERROR("No contextual object for value pair.");
                    YYERROR;
                }

                switch($3.mType)
                {
                case Array::AS_TYPE_INT:
                    $$->AddInt($1, $3.v.i);
                    break;
                case Array::AS_TYPE_FLOAT:
                    $$->AddFloat($1, $3.v.f);
                    break;
                case Array::AS_TYPE_STRING:
                    $$->AddString($1, $3.v.s);
                    break;
                case Array::AS_TYPE_OBJECT:
                    $$->AddObject($1, $3.v.o);
                    break;
                case Array::AS_TYPE_ARRAY:
                    $$->AddArray($1, $3.v.a);
                    break;
                default:
                    PG_FAILSTR("Unhandled case!");
                }
            }
           ;

element  : I_FLOAT 
            { 
                $$.mType = Array::AS_TYPE_FLOAT;
                $$.v.f = $1;
            }
         | I_INT   
            {   
                $$.mType = Array::AS_TYPE_INT;
                $$.v.i = $1;
            }
         | IDENTIFIER 
            {
                $$.mType = Array::AS_TYPE_STRING;
                $$.v.s = $1;
            }
         | object 
            {
                $$.mType = Array::AS_TYPE_OBJECT;
                $$.v.o = $1;
            }
         | array 
            {
                $$.mType = Array::AS_TYPE_ARRAY;
                $$.v.a = $1;
            }
         ;

%%


void Bison_AssetScriptParse(const Io::FileBuffer* fileBuffer, AssetBuilder* builder) 
{          
    AsCompilerState compilerState;
    compilerState.mFileBuffer = fileBuffer;
    compilerState.mBuilder = builder;

    yyscan_t scanner;
    AS_lex_init_extra(&compilerState, &scanner);

    do 
    {
	    AS_parse(scanner);
    } while (AS_HasNext(scanner) /*&& BS_GlobalBuilder->GetErrorCount() == 0*/);

    //builder->SetScanner(nullptr);
    AS_restart(nullptr, scanner);
    AS_lex_destroy(scanner);

}
