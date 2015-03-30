/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_BS_BS_PARSER_HPP_INCLUDED
# define YY_BS_BS_PARSER_HPP_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int BS_debug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     I_FLOAT = 258,
     I_INT = 259,
     IDENTIFIER = 260,
     TYPE_IDENTIFIER = 261,
     I_STRING = 262,
     K_IF = 263,
     K_ELSE_IF = 264,
     K_ELSE = 265,
     K_SEMICOLON = 266,
     K_L_PAREN = 267,
     K_R_PAREN = 268,
     K_L_BRAC = 269,
     K_R_BRAC = 270,
     K_L_LACE = 271,
     K_R_LACE = 272,
     K_COMMA = 273,
     K_COL = 274,
     K_RETURN = 275,
     K_WHILE = 276,
     K_STRUCT = 277,
     K_ENUM = 278,
     K_STATIC_ARRAY = 279,
     K_SIZE_OF = 280,
     O_PLUS = 281,
     O_MINUS = 282,
     O_MUL = 283,
     O_DIV = 284,
     O_MOD = 285,
     O_EQ = 286,
     O_GT = 287,
     O_LT = 288,
     O_GTE = 289,
     O_LTE = 290,
     O_LAND = 291,
     O_LOR = 292,
     O_SET = 293,
     O_DOT = 294,
     O_ACCESS = 295,
     O_METHOD_CALL = 296,
     O_IMPLICIT_CAST = 297,
     O_EXPLICIT_CAST = 298,
     ACCESS_PREC = 299,
     CAST = 300,
     NEG = 301
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2058 of yacc.c  */
#line 84 "bs.y"

    int    token;
    int    integerValue;
    float  floatValue;
    char*  identifierText;
    Pegasus::BlockScript::StackFrameInfo*      vFrameInfo;
    const  Pegasus::BlockScript::TypeDesc*     vTypeDesc;
    Pegasus::BlockScript::EnumNode*            vEnumNode;
    #define BS_PROCESS(N) Pegasus::BlockScript::Ast::N* v##N;
    #include "Pegasus/BlockScript/Ast.inl"
    #undef BS_PROCESS


/* Line 2058 of yacc.c  */
#line 117 "bs.parser.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int BS_parse (void *YYPARSE_PARAM);
#else
int BS_parse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int BS_parse (void* scanner);
#else
int BS_parse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_BS_BS_PARSER_HPP_INCLUDED  */
