/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

/*!
   \file   as.l
   \author Kleber Garcia
   \date   February 8 2015
   \brief  lexer generator file (input to flex tool). 
           This file contains a regex lexer description of the AssetScript data/scripting language.
           This grammar gets parsed and becomes a cpp file containing the necesary functions for lexing
*/

%option noyywrap
%option reentrant
%option bison-bridge

%{
    #define YY_EXTRA_TYPE class AsCompilerState*

    #include "Pegasus/Core/Assertion.h" 
    #include "Pegasus/Core/Log.h"
    #include "Pegasus/Core/Io.h"
    #include "Pegasus/AssetLib/ASTree.h"
    #include "Pegasus/AssetLib/AsCompilerState.h"
    #include "Pegasus/AssetLib/AssetBuilder.h"
    #include "Pegasus/Utils/Memcpy.h"
    #include "Pegasus/Utils/String.h"
    #include "Pegasus/AssetLib/as.parser.hpp"

    using namespace Pegasus;
    using namespace Pegasus::AssetLib;

    #define AS_TOKEN(t)   return (yylval->token = t);
    #define AS_INT(t)     yylval->integerValue = Pegasus::Utils::Atoi(yytext); return t;
    #define AS_FLOAT(t)   yylval->floatValue = Pegasus::Utils::Atof(yytext); return t;

    //customize input
    #undef YY_INPUT
    #define YY_INPUT(buffer, bytesToRead, maxToRead) AS_readInput(yyg, buffer, bytesToRead, maxToRead)

    /****************************************************************************************/
    /*                                                                                      */
    /*                                       Pegasus                                        */
    /*                                                                                      */
    /****************************************************************************************/
    //! \file   as.lexer.cpp
    //! \author Kleber Garcia
    //! \brief  Generated file from flex tool.

    bool AS_HasNext(void* ptr);

    int AS_readInput(struct yyguts_t* yyg, char * buffer, yy_size_t& result, int maxToRead);

%}

%x STRING_BLOCK
%x LINE_COMMENT

%% 
<STRING_BLOCK>
{
\"  {
        PG_ASSERT(yyextra->mStringAccumulatorPos < 512);
        yyextra->mStringAccumulator[yyextra->mStringAccumulatorPos] = '\0';
        yylval->identifierText = yyextra->mBuilder->CopyString(yyextra->mStringAccumulator);
		BEGIN(INITIAL);
        return IDENTIFIER;
    }
.   {

        if (yyextra->mStringAccumulatorPos >= 511)
        {
            PG_LOG('ERR_', "String too long, exceeding the limit. Line %d", yyextra->mBuilder->GetCurrentLine());
            yyterminate();
        }
		else
		{
			yyextra->mStringAccumulator[yyextra->mStringAccumulatorPos++] = *yytext;
		}
    }
}

<LINE_COMMENT>
{
    \n  { BEGIN(INITIAL); }
    .   ;
}

<INITIAL>
{
\"               { yyextra->mStringAccumulatorPos = 0; BEGIN(STRING_BLOCK); }
[ \t]            ;
[0-9]+\.[0-9]+   { AS_FLOAT(I_FLOAT); }
[0-9]+           { AS_INT(I_INT); }
[_a-zA-Z0-9\.\\\\\/]+    { const char* txt = yyextra->mBuilder->CopyString(yytext); 
                   if (txt == nullptr)
                   {
                        PG_LOG('ERR_', "String too long, exceeding the limit. Line %d", yyextra->mBuilder->GetCurrentLine());
                        yyterminate();
                   }
                   yylval->identifierText = txt;
                   return IDENTIFIER; 
                 }
\n               { yyextra->mBuilder->IncLine(); }
\[               { AS_TOKEN(K_LEFT_LACE); }
\]               { AS_TOKEN(K_RIGHT_LACE); }
\{               { AS_TOKEN(K_LEFT_BRAC); }
\}               { AS_TOKEN(K_RIGHT_BRAC); } 
#                { BEGIN(LINE_COMMENT); }
,                { AS_TOKEN(K_COMMA); }
:                { AS_TOKEN(K_COLON); } 
.                ;
}
%%

bool AS_HasNext(void* ptr)
{
    yyguts_t* yyg = static_cast<yyguts_t*>(ptr);
    return yyextra->mBufferPosition < yyextra->mFileBuffer->GetFileSize();
}

int AS_readInput(struct yyguts_t* yyg, char * buffer, yy_size_t& result, int maxToRead)
{
    int bytesLeft = yyextra->mFileBuffer->GetFileSize() - yyextra->mBufferPosition;
    int bytesRead = maxToRead > bytesLeft ? bytesLeft : maxToRead;
    Pegasus::Utils::Memcpy(buffer, yyextra->mFileBuffer->GetBuffer() + yyextra->mBufferPosition, bytesRead);
    yyextra->mBufferPosition += bytesRead;
    result = static_cast<yy_size_t>(bytesRead);
    return 0;//always correct
}
