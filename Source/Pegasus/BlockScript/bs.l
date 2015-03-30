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
    #include "Pegasus/BlockScript/IFileIncluder.h"
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
%x PREPROCESSOR_IGNORE_CODE

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

<PREPROCESSOR_IGNORE_CODE>
{
#  {
        BEGIN(PREPROCESSOR);
   }
\n { 
        yyextra->mBuilder->IncrementLine(); 
   }
. ;
}

<PREPROCESSOR_DEFINE_CAPTURE>
{
\n  {
        yyextra->mBuilder->IncrementLine(); 
        if (yyextra->mStringAccumulatorPos >= 511)
        {
            BS_ErrorDispatcher(yyextra->mBuilder, "Out of string block memory! keep macro strings within 512 chars.");
            yyterminate();
        }
        else
        {
             Pegasus::BlockScript::Preprocessor& pp = yyextra->GetPreprocessor();
             yyextra->mStringAccumulator[yyextra->mStringAccumulatorPos++] = '\0';
             const char* copiedString = yyextra->mBuilder->AllocStrImm(yyextra->mStringAccumulator);
             pp.PushCode(copiedString);
             yyextra->mStringAccumulatorPos = 0;
             const char* msg = nullptr;
             if (!pp.FlushCommand(&msg))
             {
                 BS_ErrorDispatcher(yyextra->mBuilder, msg); yyterminate();
             }
             else
             {
			     if (pp.IsIfActive())
				 {
				     BEGIN(yyextra->PopLexerState()); 
				 }
				 else
				 {
					 BEGIN(PREPROCESSOR_IGNORE_CODE);
				 }
                 
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
             if (*yytext != 32)
                yyextra->mStringAccumulator[yyextra->mStringAccumulatorPos++] = *yytext;
        }
    }
}

<PREPROCESSOR>
{
"include" {
                 Pegasus::BlockScript::Preprocessor& pp = yyextra->GetPreprocessor();
                 if (pp.GetCmd() != Pegasus::BlockScript::Preprocessor::PP_CMD_NONE)
                 {
                     BS_ErrorDispatcher(yyextra->mBuilder, "Can't queue preprocessor command 'include' "); yyterminate();
                 }
                 else
                 {
                     pp.IncludeCmd(); 
                 }
           }
"define"   {  
                 Pegasus::BlockScript::Preprocessor& pp = yyextra->GetPreprocessor();
                 if (pp.GetCmd() != Pegasus::BlockScript::Preprocessor::PP_CMD_NONE)
                 {
                     BS_ErrorDispatcher(yyextra->mBuilder, "Can't queue preprocessor command 'define' "); yyterminate();
                 }
                 else
                 {
                     pp.DefineCmd(); 
                 }
           }
"ifdef"    {
                Pegasus::BlockScript::Preprocessor& pp = yyextra->GetPreprocessor();
                if (pp.GetCmd() != Pegasus::BlockScript::Preprocessor::PP_CMD_NONE)
                {
                    BS_ErrorDispatcher(yyextra->mBuilder, "Can't queue preprocessor command 'ifdef' "); yyterminate();
                }
                else
                {
                    bool ifActive = pp.IsIfActive();
                    pp.NewState();
                    pp.IfCmd();
                    pp.SetChosePath(false);
                    pp.SetIfActive(ifActive);
                }
           }
"else"     {
                Pegasus::BlockScript::Preprocessor& pp = yyextra->GetPreprocessor();
                if (pp.GetCmd() != Pegasus::BlockScript::Preprocessor::PP_CMD_NONE)
                {
                    BS_ErrorDispatcher(yyextra->mBuilder, "Can't queue preprocessor command 'else' "); yyterminate();
                }
                else
                {
                    if (pp.StateCount() >= 2)
                    {
                        bool chosePrevIfStmt = pp.ChosePath(); //was a previous condition true?
                        pp.PopState();
                        bool isActive = pp.IsIfActive();
                        pp.NewState();
                        pp.ElseCmd();
                        pp.SetChosePath(chosePrevIfStmt);
                        pp.SetIfActive(!chosePrevIfStmt && isActive);
                    }
                    else
                    {
                        BS_ErrorDispatcher(yyextra->mBuilder, "Can't queue preprocessor command 'else' "); yyterminate();
                    }
                }
                
           }
"elif"     {
                Pegasus::BlockScript::Preprocessor& pp = yyextra->GetPreprocessor();
                if (pp.GetCmd() != Pegasus::BlockScript::Preprocessor::PP_CMD_NONE)
                {
                    BS_ErrorDispatcher(yyextra->mBuilder, "Can't queue preprocessor command 'elif' "); yyterminate();
                }
                else
                {
                    if (pp.StateCount() >= 2)
                    {
                        bool chosePrevIfStmt = pp.ChosePath(); //was a previous condition true?
                        pp.PopState();
                        bool isActive = pp.IsIfActive();
                        pp.NewState();
                        pp.ElseIfCmd();
                        pp.SetChosePath(chosePrevIfStmt);
                        pp.SetIfActive(!chosePrevIfStmt && isActive);
                    }
                    else
                    {
                        BS_ErrorDispatcher(yyextra->mBuilder, "Can't queue preprocessor command 'elif' "); yyterminate();
                    }
                }
           }
"endif"    {
                Pegasus::BlockScript::Preprocessor& pp = yyextra->GetPreprocessor();
                if (pp.GetCmd() != Pegasus::BlockScript::Preprocessor::PP_CMD_NONE)
                {
                    BS_ErrorDispatcher(yyextra->mBuilder, "Can't queue preprocessor command 'endif' "); yyterminate();
                }
                else
                {
                    pp.EndIfCmd();
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
                Pegasus::BlockScript::Preprocessor& pp = yyextra->GetPreprocessor();
                if (pp.GetCmd() == Pegasus::BlockScript::Preprocessor::PP_CMD_IF || pp.GetCmd() == Pegasus::BlockScript::Preprocessor::PP_CMD_ELSEIF)
                {
                   
                    const char* preprocessorDefine = pp.GetStringArg();
                    pp.NoneCmd();

                    if (preprocessorDefine == nullptr)
                    {
                        BS_ErrorDispatcher(yyextra->mBuilder, "Invalid argument for PP command"); yyterminate();
                    }
                
                    if (pp.IsIfActive() && !pp.ChosePath())
                    {
                        bool isDefined = pp.FindDefinitionByName( preprocessorDefine ) != nullptr;
                        if (isDefined)
                        {
                            pp.SetChosePath(true);
                            BEGIN(yyextra->PopLexerState());
                        }
                        else
                        {
                            pp.SetIfActive(false);
                            BEGIN(PREPROCESSOR_IGNORE_CODE);
                        }
                    }
                    else
                    {
                        BEGIN(PREPROCESSOR_IGNORE_CODE);
                    }
                    
                }
                else if (pp.GetCmd() == Pegasus::BlockScript::Preprocessor::PP_CMD_ELSE)
                {
                    pp.NoneCmd();
                    if (pp.IsIfActive() && !pp.ChosePath())
                    {
                        BEGIN(yyextra->PopLexerState());
                    }
                    else
                    {
                        pp.SetIfActive(false);
                        BEGIN(PREPROCESSOR_IGNORE_CODE);
                    }
                }
                else if (pp.GetCmd() == Pegasus::BlockScript::Preprocessor::PP_CMD_ENDIF)
                {
                    if (pp.StateCount() <= 1)
                    {
                        BS_ErrorDispatcher(yyextra->mBuilder, "Can't queue endif preprocessor command"); yyterminate();
                    }
                    else
                    {
                        pp.PopState();
                        pp.NoneCmd();
                        if (pp.IsIfActive())
                        {
                            BEGIN(yyextra->PopLexerState());
                        }
                        else
                        {
                            BEGIN(PREPROCESSOR_IGNORE_CODE);
                        }
                    }
                }
                else
                {
                    const char* msg = nullptr;

                    if (!pp.FlushCommand(&msg))
                    {
                        BS_ErrorDispatcher(yyextra->mBuilder, msg); yyterminate();
                    }
                    else
                    {
                        if (pp.HasIncludeBuffer())
                        {
                            const char* buffer = nullptr;
                            int bufferSize = 0;
                            yyextra->PushDefineStack(YY_CURRENT_BUFFER, pp.GetIncludeDefinition());
                            yypush_buffer_state(yy_create_buffer(NULL, YY_BUF_SIZE, yyscanner), yyscanner);
                        }

                        if (pp.IsIfActive())
                        {
                            BEGIN(yyextra->PopLexerState()); 
                        }
                        else
                        {
                            BEGIN(PREPROCESSOR_IGNORE_CODE);
                        }
                    }
                }
           }
[ \t]      ;
[_a-zA-Z0-9]+  {
                    Pegasus::BlockScript::Preprocessor& pp = yyextra->GetPreprocessor();
                    if (
                            pp.GetCmd() != Pegasus::BlockScript::Preprocessor::PP_CMD_DEFINE &&
                            pp.GetCmd() != Pegasus::BlockScript::Preprocessor::PP_CMD_IF     &&
                            pp.GetCmd() != Pegasus::BlockScript::Preprocessor::PP_CMD_ELSEIF 
                       )
                    {
                        BS_ErrorDispatcher(yyextra->mBuilder, "Invalid identifier token for preprocessor command."); yyterminate();
                    }
                    else
                    {
                        char * str = yyextra->mBuilder->GetStringPool().AllocateString();
                        if (str == nullptr) { BS_ErrorDispatcher( yyextra->mBuilder, "Out of identifier memory!"); yyterminate(); }
						str[0] = '\0';
                        Pegasus::Utils::Strcat(str, yytext);
                        pp.PushString(yyextra->mBuilder->AllocStrImm(yytext));
                        
                        if (pp.GetCmd() != Pegasus::BlockScript::Preprocessor::PP_CMD_IF &&
                            pp.GetCmd() != Pegasus::BlockScript::Preprocessor::PP_CMD_ELSEIF)
                        {
                            yyextra->mStringAccumulatorPos = 0;
                            yyextra->mStringAccumulator[yyextra->mStringAccumulatorPos] = '\0';

                            BEGIN(PREPROCESSOR_DEFINE_CAPTURE);
                        }
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
                        
                        const Pegasus::BlockScript::Preprocessor::Definition* preprocessorDefinition = yyextra->GetPreprocessor().FindDefinitionByName(str);
                        if (preprocessorDefinition != nullptr)
                        {
                            yyextra->PushDefineStack(YY_CURRENT_BUFFER, preprocessorDefinition);
                            yypush_buffer_state(yy_create_buffer(NULL, YY_BUF_SIZE, yyscanner), yyscanner);
                        }
                        else
                        {
                            isTypeString = yyextra->mBuilder->GetSymbolTable()->GetTypeByName(str) != nullptr;
                            return isTypeString ? TYPE_IDENTIFIER : IDENTIFIER;
                        }
                    }
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

<<EOF>>         {
                    if (yyextra->GetDefineStackCount() > 0)
                    {
                        yy_delete_buffer(YY_CURRENT_BUFFER, yyscanner); 
                        yy_switch_to_buffer((YY_BUFFER_STATE)yyextra->PopDefineStack(), yyscanner);
                    }
					else
					{
						yyterminate();
					}
                }
%%

bool BS_HasNext(void* ptr)
{
    yyguts_t* yyg = static_cast<yyguts_t*>(ptr);
    return yyextra->mBufferPosition < yyextra->mFileBuffer->GetFileSize();
}

int BS_readInput(struct yyguts_t* yyg, char * buffer, yy_size_t& result, int maxToRead)
{
    if (yyextra->GetDefineStackCount() > 0)
    {
        Pegasus::BlockScript::CompilerState::DefineBufferEl* bufferEl = yyextra->GetDefineStackTop();
        const Pegasus::BlockScript::Preprocessor::Definition* def = bufferEl->mDef;
        if (def->mValue != nullptr)
        {
            if (bufferEl->mBufferPosition >= def->mBufferSize)
            {
                result = 0;
            }
            else
            {
				int bytesLeft = def->mBufferSize - bufferEl->mBufferPosition;
                int bytesRead = maxToRead > bytesLeft ? bytesLeft : maxToRead;
                Pegasus::Utils::Memcpy(buffer, def->mValue + bufferEl->mBufferPosition, bytesRead);
                bufferEl->mBufferPosition += bytesRead;
                result = static_cast<yy_size_t>(bytesRead);

                if (def->mIsInclude && bufferEl->mBufferPosition >= def->mBufferSize)
                {
                    yyextra->GetPreprocessor().GetFileIncluder()->Close(def->mValue);
                }
            }
        }
        else
        {
            result = 0;
        }
    }
    else
    {
        int bytesLeft = yyextra->mFileBuffer->GetFileSize() - yyextra->mBufferPosition;
        int bytesRead = maxToRead > bytesLeft ? bytesLeft : maxToRead;
        Pegasus::Utils::Memcpy(buffer, yyextra->mFileBuffer->GetBuffer() + yyextra->mBufferPosition, bytesRead);
        yyextra->mBufferPosition += bytesRead;
        result = static_cast<yy_size_t>(bytesRead);
    }
    return 0;//always correct
}
