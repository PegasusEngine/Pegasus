/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

/*!
   \file   bs.l
   \author Kleber Garcia
   \date   August 27th 2014
   \brief  lexer generator file (input to flex tool). 
           This file contains a regex lexer description of the BlockScript scripting language.
           This grammar gets parsed and becomes a cpp file containing the necesary functions for lexing
*/

%option noyywrap
%option reentrant
%option bison-bridge

%{
    #define YY_EXTRA_TYPE class CompilerState*

    /****************************************************************************************/
    /*                                                                                      */
    /*                                       Pegasus                                        */
    /*                                                                                      */
    /****************************************************************************************/
    //! \file   bs.lexer.cpp
    //! \author Kleber Garcia
    //! \brief  Generated file from flex tool.

    #include "Pegasus/BlockScript/BlockScriptBuilder.h"
    #include "Pegasus/BlockScript/BlockScriptAst.h"
    #include "Pegasus/BlockScript/Preprocessor.h"
    #include "Pegasus/BlockScript/bs.parser.hpp"
    #include "Pegasus/Utils/String.h"
    #include "Pegasus/Utils/Memcpy.h"
    #include "Pegasus/BlockScript/CompilerState.h"
    #include "Pegasus/Core/Io.h"

    #define BS_TOKEN(t)   return (yylval->token = t);
    #define BS_INT(t)     yylval->integerValue = Pegasus::Utils::Atoi(yytext); return t;
    #define BS_FLOAT(t)   yylval->floatValue = Pegasus::Utils::Atof(yytext); return t;

    //customize input
    #undef YY_INPUT
    #define YY_INPUT(buffer, bytesToRead, maxToRead) BS_readInput(yyg, buffer, bytesToRead, maxToRead)

    using namespace Pegasus::BlockScript;
    using namespace Pegasus::Io;

    
    extern void BS_ErrorDispatcher(BlockScriptBuilder* builder, const char* message) ;


    bool BS_HasNext(void* ptr);

    int BS_readInput(struct yyguts_t* yyg, char * buffer, yy_size_t& result, int maxToRead);
%}

%x IN_LINE_COMMENT
%x MULTI_COMMENT
%x STRING_BLOCK
%x PREPROCESSOR
%x PREPROCESSOR_DEFINE_CAPTURE

%%
<IN_LINE_COMMENT>
{
\n   { yyextra->mBuilder->IncrementLine(); BEGIN(yyextra->PopLexerState()); }
.    ;
}
<MULTI_COMMENT>
{
\*\/ { BEGIN(yyextra->PopLexerState()); }
\n   { yyextra->mBuilder->IncrementLine(); }
.    ;
}

<STRING_BLOCK>
{
\"  {
        BEGIN(yyextra->PopLexerState()); 
        PG_ASSERT(yyextra->mStringAccumulatorPos < 512);
        yyextra->mStringAccumulator[yyextra->mStringAccumulatorPos] = '\0';
        yylval->identifierText = yyextra->mStringAccumulator;

        if (YYSTATE == PREPROCESSOR)
        {
            const char* strImm = yyextra->mBuilder->AllocStrImm(yyextra->mStringAccumulator);
            if (strImm != nullptr)
            {
                yyextra->GetPreprocessor().PushCode(yyextra->mBuilder->AllocStrImm(strImm));
            }
            else
            {
                yyterminate();
            }
        }
        else
        {
            return I_STRING;
        }
    }
.   {
        if (yyextra->mStringAccumulatorPos >= 511)
        {
            BS_ErrorDispatcher(yyextra->mBuilder, "Out of string block memory! keep imm strings within 512 chars.");
            yyterminate();
        }
        else
        {
             yyextra->mStringAccumulator[yyextra->mStringAccumulatorPos++] = *yytext;
        }
    }
}

<PREPROCESSOR_DEFINE_CAPTURE>
{
\n  {
        if (yyextra->mStringAccumulatorPos >= 511)
        {
            BS_ErrorDispatcher(yyextra->mBuilder, "Out of string block memory! keep macro strings within 512 chars.");
            yyterminate();
        }
        else
        {
             yyextra->mStringAccumulator[yyextra->mStringAccumulatorPos++] = '\0';
             const char* copiedString = yyextra->mBuilder->AllocStrImm(yyextra->mStringAccumulator);
             yyextra->GetPreprocessor().PushCode(copiedString);
             yyextra->mStringAccumulatorPos = 0;
             yyextra->mBuilder->IncrementLine(); 
             const char* msg = nullptr;
             if (!yyextra->GetPreprocessor().FlushCommand(&msg))
             {
                 BS_ErrorDispatcher(yyextra->mBuilder, msg); yyterminate();
             }
             else
             {
                 BEGIN(yyextra->PopLexerState()); 
             }
        }
    }
.   {
        if (yyextra->mStringAccumulatorPos >= 511)
        {
            BS_ErrorDispatcher(yyextra->mBuilder, "Out of string block memory! keep macro strings within 512 chars.");
            yyterminate();
        }
        else
        {
             yyextra->mStringAccumulator[yyextra->mStringAccumulatorPos++] = *yytext;
        }
    }
}

<PREPROCESSOR>
{
"include" {
                if (yyextra->GetPreprocessor().GetCmd() != Pegasus::BlockScript::Preprocessor::PP_CMD_NONE)
                {
                    BS_ErrorDispatcher(yyextra->mBuilder, "Can't queue preprocessor command 'include' "); yyterminate();
                }
                else
                {
                    yyextra->GetPreprocessor().IncludeCmd(); 
                }
           }
"define"   {  
                if (yyextra->GetPreprocessor().GetCmd() != Pegasus::BlockScript::Preprocessor::PP_CMD_NONE)
                {
                    BS_ErrorDispatcher(yyextra->mBuilder, "Can't queue preprocessor command 'define' "); yyterminate();
                }
                else
                {
                    yyextra->GetPreprocessor().DefineCmd(); 
                }
           }
\/\/       { yyextra->PushLexerState(YYSTATE);BEGIN(IN_LINE_COMMENT);}
\/\*       { yyextra->PushLexerState(YYSTATE);BEGIN(MULTI_COMMENT);  }
\"         { 
                if (yyextra->GetPreprocessor().GetCmd() != Pegasus::BlockScript::Preprocessor::PP_CMD_INCLUDE) 
                {
                    BS_ErrorDispatcher(yyextra->mBuilder, "Invalid argument for PP command"); yyterminate();
                }
                else
                {
                    yyextra->mStringAccumulatorPos = 0; yyextra->PushLexerState(YYSTATE); BEGIN(STRING_BLOCK); 
                }
           }
\n         { 
                yyextra->mBuilder->IncrementLine(); 
                const char* msg = nullptr;
                if (!yyextra->GetPreprocessor().FlushCommand(&msg))
                {
                    BS_ErrorDispatcher(yyextra->mBuilder, msg); yyterminate();
                }
                else
                {
                    BEGIN(yyextra->PopLexerState()); 
                }
           }
[ \t]      ;
[_a-zA-Z0-9]+  {
                    if (yyextra->GetPreprocessor().GetCmd() != Pegasus::BlockScript::Preprocessor::PP_CMD_DEFINE)
                    {
                        BS_ErrorDispatcher(yyextra->mBuilder, "Invalid identifier token for preprocessor command."); yyterminate();
                    }
                    else
                    {
                        char * str = yyextra->mBuilder->GetStringPool().AllocateString();
                        if (str == nullptr) { BS_ErrorDispatcher( yyextra->mBuilder, "Out of identifier memory!"); yyterminate(); }
                        Pegasus::Utils::Strcat(str, yytext);
                        yyextra->GetPreprocessor().PushString(yyextra->mBuilder->AllocStrImm(yytext));
                        yyextra->mStringAccumulatorPos = 0;
                        yyextra->mStringAccumulator[yyextra->mStringAccumulatorPos] = '\0';

                        BEGIN(PREPROCESSOR_DEFINE_CAPTURE);
                    } 
                }
.          { BS_ErrorDispatcher( yyextra->mBuilder, "Invalid token for preprocessor."); yyterminate(); }
}

<INITIAL>
{
#               { yyextra->PushLexerState(YYSTATE); BEGIN(PREPROCESSOR);}
\/\/            { yyextra->PushLexerState(YYSTATE);BEGIN(IN_LINE_COMMENT);}
\/\*            { yyextra->PushLexerState(YYSTATE);BEGIN(MULTI_COMMENT);  }
\"              { yyextra->mStringAccumulatorPos = 0; yyextra->PushLexerState(YYSTATE);BEGIN(STRING_BLOCK); }
[ \t]            ;
\n              { yyextra->mBuilder->IncrementLine();       }
if              { return K_IF;     }
elif            { return K_ELSE_IF;}
else            { return K_ELSE;   }
return          { return K_RETURN; }
struct          { return K_STRUCT; }
enum            { return K_ENUM;   }
while           { return K_WHILE;  }
static_array    { return K_STATIC_ARRAY; }
sizeof          { return K_SIZE_OF;      }
[0-9]+\.[0-9]+  { BS_FLOAT(I_FLOAT);     }
[0-9]+          { BS_INT(I_INT);         }
;               { BS_TOKEN(K_SEMICOLON); }
[_a-zA-Z0-9]+   { 
                    bool isTypeString = false;
                    int strLen = Pegasus::Utils::Strlen(yytext) + 1;
                    if (strLen >=  Pegasus::BlockScript::IddStrPool::sCharsPerString){
                        BS_ErrorDispatcher(yyextra->mBuilder, "Identifier string too long!\n");
                        yyterminate();
                    }else{
                        char * str = yyextra->mBuilder->GetStringPool().AllocateString();
                        if (str == nullptr) { BS_ErrorDispatcher( yyextra->mBuilder, "Out of identifier memory!"); yyterminate(); }
                        yylval->identifierText = str;
                        str[0] = '\0';
                        Pegasus::Utils::Strcat(str, yytext);
                        isTypeString = yyextra->mBuilder->GetSymbolTable()->GetTypeByName(str) != nullptr;
                    }
                    return isTypeString ? TYPE_IDENTIFIER : IDENTIFIER;
                }
\+              { BS_TOKEN(O_PLUS);  }
\-              { BS_TOKEN(O_MINUS); }
\*              { BS_TOKEN(O_MUL);   }
\/              { BS_TOKEN(O_DIV);   }
%               { BS_TOKEN(O_MOD);   }
==              { BS_TOKEN(O_EQ);    }
>               { BS_TOKEN(O_GT);    }
\<              { BS_TOKEN(O_LT);    }
>=              { BS_TOKEN(O_GTE);   }
\<=             { BS_TOKEN(O_LTE);   }
&&              { BS_TOKEN(O_LAND); }
\|\|            { BS_TOKEN(O_LOR);  }
=               { BS_TOKEN(O_SET);  }
->              { BS_TOKEN(O_METHOD_CALL); }
\.              { BS_TOKEN(O_DOT); }
\(              { return K_L_PAREN; }
\)              { return K_R_PAREN; }
\{              { return K_L_BRAC;  }
\}              { return K_R_BRAC;  }
\[              { return K_L_LACE;  }
\]              { return K_R_LACE;  }
,               { return K_COMMA;   }
:               { return K_COL;     }
.               ;
}

%%

bool BS_HasNext(void* ptr)
{
    yyguts_t* yyg = static_cast<yyguts_t*>(ptr);
    return yyextra->mBufferPosition < yyextra->mFileBuffer->GetFileSize();
}

int BS_readInput(struct yyguts_t* yyg, char * buffer, yy_size_t& result, int maxToRead)
{
    int bytesLeft = yyextra->mFileBuffer->GetFileSize() - yyextra->mBufferPosition;
    int bytesRead = maxToRead > bytesLeft ? bytesLeft : maxToRead;
    Pegasus::Utils::Memcpy(buffer, yyextra->mFileBuffer->GetBuffer() + yyextra->mBufferPosition, bytesRead);
    yyextra->mBufferPosition += bytesRead;
    result = static_cast<yy_size_t>(bytesRead);
    return 0;//always correct
}
