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
%{
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
    #include "Pegasus/BlockScript/bs.parser.hpp"
    #include "Pegasus/Utils/String.h"
    #include "Pegasus/Utils/Memcpy.h"
    #include "Pegasus/Core/Io.h"

    #define BS_TOKEN(t)   return (BS_lval.token = t);
    #define BS_INT(t)     BS_lval.integerValue = Pegasus::Utils::Atoi(BS_text); return t;
    #define BS_FLOAT(t)   BS_lval.floatValue = Pegasus::Utils::Atof(BS_text); return t;
    #define BS_STRING(t)  int strLen = Pegasus::Utils::Strlen(BS_text) + 1;\
                          if (strLen >=  Pegasus::BlockScript::IddStrPool::sCharsPerString){\
                            BS_ErrorDispatcher(BS_GlobalBuilder, "Identifier string too long!\n");\
                            yyterminate();\
                          }else{\
                            char * str = BS_GlobalBuilder->GetStringPool().AllocateString();\
                            if (str == nullptr) { BS_ErrorDispatcher( BS_GlobalBuilder, "Out of identifier memory!"); yyterminate(); }\
                            BS_lval.identifierText = str;\
                            str[0] = '\0';\
                            Pegasus::Utils::Strcat(str, BS_text);\
                          }\
                          return t;\
                                
                            
                             

    //customize input
    #undef YY_INPUT
    #define YY_INPUT(buffer, bytesToRead, maxToRead) BS_readInput(buffer, bytesToRead, maxToRead)

    using namespace Pegasus::BlockScript;
    using namespace Pegasus::Io;

    int BS_bufferPosition = 0;
    extern const FileBuffer* BS_GlobalFileBuffer;
    extern BlockScriptBuilder* BS_GlobalBuilder;

    extern void BS_ErrorDispatcher(BlockScriptBuilder* builder, const char* message) ;


    bool BS_HasNext()
    {
        return BS_bufferPosition < BS_GlobalFileBuffer->GetFileSize();
    }

    int BS_readInput(char * buffer, yy_size_t& result, int maxToRead)
    {
        int bytesLeft = BS_GlobalFileBuffer->GetFileSize() - BS_bufferPosition;
        int bytesRead = maxToRead > bytesLeft ? bytesLeft : maxToRead;
        Pegasus::Utils::Memcpy(buffer, BS_GlobalFileBuffer->GetBuffer() + BS_bufferPosition, bytesRead);
        BS_bufferPosition += bytesRead;
        result = static_cast<yy_size_t>(bytesRead);
        return 0;//always correct
    }

    char BS_StringBlockAccumulator[512];
    int  BS_StringBlockAccumulatorPos = 0;

%}

%x IN_LINE_COMMENT
%x MULTI_COMMENT
%x STRING_BLOCK

%%
<IN_LINE_COMMENT>
{
\n   { BS_GlobalBuilder->IncrementLine(); BEGIN(INITIAL); }
.    ;
}
<MULTI_COMMENT>
{
\*\/ { BEGIN(INITIAL); }
\n   { BS_GlobalBuilder->IncrementLine(); }
.    ;
}

<STRING_BLOCK>
{
\"  {
        BEGIN(INITIAL); 
        PG_ASSERT(BS_StringBlockAccumulatorPos < 512);
        BS_StringBlockAccumulator[BS_StringBlockAccumulatorPos] = '\0';
        BS_lval.identifierText = BS_StringBlockAccumulator;
        return I_STRING;
    }
.   {
        if (BS_StringBlockAccumulatorPos >= 511)
        {
            BS_ErrorDispatcher(BS_GlobalBuilder, "Out of string block memory! keep imm strings within 512 chars.");
            yyterminate();
        }
        else
        {
             BS_StringBlockAccumulator[BS_StringBlockAccumulatorPos++] = *BS_text;
        }
    }
}

<INITIAL>
{
\/\/            { BEGIN(IN_LINE_COMMENT);}
\/\*            { BEGIN(MULTI_COMMENT);  }
\"              { BS_StringBlockAccumulatorPos = 0; BEGIN(STRING_BLOCK); }
[ \t]            ;
\n              { BS_GlobalBuilder->IncrementLine();       }
if              { return K_IF;     }
elif            { return K_ELSE_IF;}
else            { return K_ELSE;   }
return          { return K_RETURN; }
struct          { return K_STRUCT; }
while           { return K_WHILE;  }
[0-9]+\.[0-9]+  { BS_FLOAT(I_FLOAT);     }
[0-9]+          { BS_INT(I_INT);         }
;               { BS_TOKEN(K_SEMICOLON); }
[_a-zA-Z0-9]+   { BS_STRING(IDENTIFIER); }
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
->              { BS_TOKEN(O_TREE); }
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
