/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison implementation for Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.7"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         BS_parse
#define yylex           BS_lex
#define yyerror         BS_error
#define yylval          BS_lval
#define yychar          BS_char
#define yydebug         BS_debug
#define yynerrs         BS_nerrs

/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 16 "bs.y"

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

/* Line 371 of yacc.c  */
#line 139 "bs.parser.cpp"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "bs.parser.hpp".  */
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
/* Line 387 of yacc.c  */
#line 83 "bs.y"

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


/* Line 387 of yacc.c  */
#line 242 "bs.parser.cpp"
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

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 269 "bs.parser.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  37
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   567

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  47
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  23
/* YYNRULES -- Number of rules.  */
#define YYNRULES  70
/* YYNRULES -- Number of states.  */
#define YYNSTATES  163

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   301

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    11,    14,    18,    21,
      29,    36,    43,    52,    54,    57,    59,    61,    69,    76,
      77,    79,    85,    90,    91,    93,    97,    99,   103,   105,
     107,   109,   114,   118,   120,   121,   123,   125,   130,   135,
     140,   142,   144,   151,   158,   162,   166,   170,   174,   178,
     182,   186,   190,   194,   198,   202,   206,   210,   214,   219,
     222,   227,   231,   235,   237,   238,   243,   245,   249,   252,
     253
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      48,     0,    -1,    49,    -1,    49,    50,    -1,    50,    -1,
      -1,    65,    11,    -1,    20,    65,    11,    -1,    57,    61,
      -1,    54,    12,    65,    13,    14,    49,    15,    -1,    51,
       5,    14,    68,    15,    11,    -1,    23,     5,    14,    60,
      15,    11,    -1,     8,    12,    65,    52,    49,    15,    55,
      58,    -1,    22,    -1,    13,    14,    -1,    67,    -1,    21,
      -1,    55,    56,    12,    65,    52,    49,    15,    -1,    56,
      12,    65,    52,    49,    15,    -1,    -1,     9,    -1,    53,
       5,    12,    66,    13,    -1,    59,    14,    49,    15,    -1,
      -1,    10,    -1,    60,    18,     5,    -1,     5,    -1,    14,
      49,    15,    -1,    11,    -1,     4,    -1,     3,    -1,    25,
      12,    67,    13,    -1,    63,    18,    65,    -1,    65,    -1,
      -1,     5,    -1,    64,    -1,     5,    12,    63,    13,    -1,
       6,    12,    63,    13,    -1,    24,    33,    67,    32,    -1,
       7,    -1,    62,    -1,    65,    41,     5,    12,    63,    13,
      -1,    65,    41,     6,    12,    63,    13,    -1,    65,    38,
      65,    -1,    65,    26,    65,    -1,    65,    27,    65,    -1,
      65,    28,    65,    -1,    65,    29,    65,    -1,    65,    30,
      65,    -1,    65,    31,    65,    -1,    65,    36,    65,    -1,
      65,    37,    65,    -1,    65,    33,    65,    -1,    65,    32,
      65,    -1,    65,    35,    65,    -1,    65,    34,    65,    -1,
      65,    39,    64,    -1,    65,    16,    65,    17,    -1,    27,
      65,    -1,    12,    67,    13,    65,    -1,    12,    65,    13,
      -1,    66,    18,    69,    -1,    69,    -1,    -1,    67,    16,
       4,    17,    -1,     6,    -1,    68,    69,    11,    -1,    69,
      11,    -1,    -1,     5,    19,    67,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   176,   176,   179,   188,   189,   192,   193,   194,   195,
     196,   197,   214,   235,   238,   241,   244,   247,   261,   265,
     268,   273,   276,   277,   280,   283,   292,   299,   300,   303,
     304,   305,   312,   321,   322,   325,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,   339,   340,   341,
     342,   343,   344,   345,   346,   347,   348,   349,   350,   351,
     352,   353,   356,   365,   366,   369,   386,   400,   409,   410,
     413
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "I_FLOAT", "I_INT", "IDENTIFIER",
  "TYPE_IDENTIFIER", "I_STRING", "K_IF", "K_ELSE_IF", "K_ELSE",
  "K_SEMICOLON", "K_L_PAREN", "K_R_PAREN", "K_L_BRAC", "K_R_BRAC",
  "K_L_LACE", "K_R_LACE", "K_COMMA", "K_COL", "K_RETURN", "K_WHILE",
  "K_STRUCT", "K_ENUM", "K_STATIC_ARRAY", "K_SIZE_OF", "O_PLUS", "O_MINUS",
  "O_MUL", "O_DIV", "O_MOD", "O_EQ", "O_GT", "O_LT", "O_GTE", "O_LTE",
  "O_LAND", "O_LOR", "O_SET", "O_DOT", "O_ACCESS", "O_METHOD_CALL",
  "O_IMPLICIT_CAST", "O_EXPLICIT_CAST", "ACCESS_PREC", "CAST", "NEG",
  "$accept", "program", "stmt_list", "stmt", "struct_keyword",
  "if_begin_scope", "fun_type", "while_keyword", "stmt_else_if_tail",
  "else_if_keyword", "fun_declaration", "stmt_else_tail", "else_keyword",
  "enum_list", "fun_stmt_list", "numeric", "exp_list", "ident", "exp",
  "arg_list", "type_desc", "struct_def_list", "arg_dec", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    47,    48,    49,    49,    49,    50,    50,    50,    50,
      50,    50,    50,    51,    52,    53,    54,    55,    55,    55,
      56,    57,    58,    58,    59,    60,    60,    61,    61,    62,
      62,    62,    63,    63,    63,    64,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    66,    66,    66,    67,    67,    68,    68,    68,
      69
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     0,     2,     3,     2,     7,
       6,     6,     8,     1,     2,     1,     1,     7,     6,     0,
       1,     5,     4,     0,     1,     3,     1,     3,     1,     1,
       1,     4,     3,     1,     0,     1,     1,     4,     4,     4,
       1,     1,     6,     6,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     4,     2,
       4,     3,     3,     1,     0,     4,     1,     3,     2,     0,
       3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       5,    30,    29,    35,    66,    40,     0,     0,     0,    16,
      13,     0,     0,     0,     0,     0,     2,     4,     0,     0,
       0,     0,    41,    36,     0,    15,    34,    34,     0,     0,
       0,     0,     0,     0,     0,     0,    59,     1,     3,     0,
       0,     0,    28,     5,     8,     6,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    33,     0,     0,    61,     0,     7,
       0,    66,     0,     0,    69,    64,     0,     0,     0,    45,
      46,    47,    48,    49,    50,    54,    53,    56,    55,    51,
      52,    44,    35,    57,     0,     0,     0,    37,     0,    38,
       0,     5,    60,    26,     0,    39,    31,     0,     0,     0,
       0,    63,     0,    27,    58,    34,    34,    65,    32,    14,
       0,     0,     0,     0,     0,     0,    68,    21,     0,     5,
       0,     0,    19,    11,    25,    70,    10,    67,    62,     0,
      42,    43,    20,    23,     0,     9,    24,     0,    12,     0,
       0,     0,     5,     0,     0,     0,     5,     5,    22,     0,
       0,    18,    17
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    15,    16,    17,    18,   101,    19,    20,   143,   144,
      21,   148,   149,   104,    44,    22,    63,    23,    24,   110,
      25,   108,   109
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -58
static const yytype_int16 yypact[] =
{
     505,   -58,   -58,    16,    25,   -58,    72,   530,   540,   -58,
     -58,     4,   -18,    76,   540,    86,   505,   -58,    93,    98,
      92,    54,   -58,   -58,   121,    95,   540,   540,   540,   177,
      -2,    25,   150,   101,   110,   110,   -10,   -58,   -58,   116,
     126,   540,   -58,   505,   -58,   -58,   540,   540,   540,   540,
     540,   540,   540,   540,   540,   540,   540,   540,   540,   540,
     131,    61,   136,    -1,   281,    40,   204,   -58,   540,   -58,
     158,   -58,     0,   118,   159,   159,   231,   355,   257,   327,
     327,    66,    -6,    73,    44,    -3,    -3,    -3,    -3,    94,
      94,   305,   -58,   -58,   153,   155,   151,   -58,   540,   -58,
     156,   505,   -10,   -58,   124,   -58,   -58,   152,     3,   161,
      41,   -58,   160,   -58,   -58,   540,   540,   -58,   281,   -58,
     380,   162,   164,   110,   181,   183,   -58,   -58,   159,   505,
      51,    74,   166,   -58,   -58,    95,   -58,   -58,   -58,   405,
     -58,   -58,   -58,    47,   184,   -58,   -58,   185,   -58,   186,
     540,   540,   505,   204,   204,   430,   505,   505,   -58,   455,
     480,   -58,   -58
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -58,   -58,   -39,   -14,   -58,   -57,   -58,   -58,   -58,    52,
     -58,   -58,   -58,   -58,   -58,   -58,   -22,   138,    -7,   -58,
      -4,   -58,   -53
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      29,    32,    38,    30,    77,    65,    46,    36,   107,    33,
      46,    68,    97,    46,    62,    34,    62,    98,   124,    64,
      64,    66,   111,    47,    48,    49,    50,    51,    26,    60,
      72,    73,   105,    60,    76,    61,    60,    27,    61,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    99,   127,   125,   142,   146,    98,   128,
      46,   102,   120,    38,   140,    42,    94,    95,    43,    98,
      47,    48,    49,    50,    51,   138,    53,    54,    55,    56,
      57,    58,    46,    60,    28,    61,    37,   141,    35,    46,
     139,   118,    98,   130,   131,    50,   156,   157,    39,    47,
      48,    49,    50,    40,    41,    60,    38,    61,    64,    64,
      46,    62,    60,   155,    61,    70,    71,   159,   160,   135,
      47,    48,    49,    50,    51,    38,    53,    54,    55,    56,
      74,   106,    45,    60,    62,    61,    92,    46,    75,   121,
      96,    38,   122,   153,   154,    38,    38,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    69,    61,   103,   107,   115,    46,   116,   117,   134,
     119,   123,   126,   133,   129,   142,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      67,    61,   136,    46,   137,   147,   150,   151,    93,     0,
     152,     0,     0,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,   100,    61,     0,
      46,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,   112,    61,     0,    46,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,     0,    61,    46,   114,     0,     0,     0,     0,     0,
       0,     0,     0,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    46,    61,     0,
       0,     0,     0,     0,     0,     0,     0,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    46,    61,     0,     0,     0,     0,     0,     0,     0,
       0,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    46,    60,     0,    61,     0,     0,     0,
       0,     0,     0,     0,     0,    49,    50,     0,     1,     2,
       3,     4,     5,     6,     0,     0,    60,     7,    61,     0,
     113,     0,     0,     0,     0,     8,     9,    10,    11,    12,
      13,     0,    14,     1,     2,     3,     4,     5,     6,     0,
       0,     0,     7,     0,     0,   132,     0,     0,     0,     0,
       8,     9,    10,    11,    12,    13,     0,    14,     1,     2,
       3,     4,     5,     6,     0,     0,     0,     7,     0,     0,
     145,     0,     0,     0,     0,     8,     9,    10,    11,    12,
      13,     0,    14,     1,     2,     3,     4,     5,     6,     0,
       0,     0,     7,     0,     0,   158,     0,     0,     0,     0,
       8,     9,    10,    11,    12,    13,     0,    14,     1,     2,
       3,     4,     5,     6,     0,     0,     0,     7,     0,     0,
     161,     0,     0,     0,     0,     8,     9,    10,    11,    12,
      13,     0,    14,     1,     2,     3,     4,     5,     6,     0,
       0,     0,     7,     0,     0,   162,     0,     0,     0,     0,
       8,     9,    10,    11,    12,    13,     0,    14,     1,     2,
       3,     4,     5,     6,     0,     0,     0,     7,     0,     0,
       0,     0,     0,     0,     0,     8,     9,    10,    11,    12,
      13,     0,    14,     1,     2,     3,     4,     5,     0,     0,
       0,     0,     7,     1,     2,     3,    31,     5,     0,     0,
       0,     0,     7,     0,    12,    13,     0,    14,     0,     0,
       0,     0,     0,     0,    12,    13,     0,    14
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-58)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       7,     8,    16,     7,    43,    27,    16,    14,     5,     5,
      16,    13,    13,    16,    16,    33,    16,    18,    15,    26,
      27,    28,    75,    26,    27,    28,    29,    30,    12,    39,
      34,    35,    32,    39,    41,    41,    39,    12,    41,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    13,    13,   108,     9,    10,    18,    18,
      16,    68,   101,    77,    13,    11,     5,     6,    14,    18,
      26,    27,    28,    29,    30,   128,    32,    33,    34,    35,
      36,    37,    16,    39,    12,    41,     0,    13,    12,    16,
     129,    98,    18,   115,   116,    29,   153,   154,     5,    26,
      27,    28,    29,     5,    12,    39,   120,    41,   115,   116,
      16,    16,    39,   152,    41,    14,     6,   156,   157,   123,
      26,    27,    28,    29,    30,   139,    32,    33,    34,    35,
      14,    13,    11,    39,    16,    41,     5,    16,    12,    15,
       4,   155,    18,   150,   151,   159,   160,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    11,    41,     5,     5,    12,    16,    12,    17,     5,
      14,    19,    11,    11,    14,     9,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      13,    41,    11,    16,    11,   143,    12,    12,    60,    -1,
      14,    -1,    -1,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    13,    41,    -1,
      16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    13,    41,    -1,    16,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    -1,    41,    16,    17,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    16,    41,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    16,    41,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    16,    39,    -1,    41,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    28,    29,    -1,     3,     4,
       5,     6,     7,     8,    -1,    -1,    39,    12,    41,    -1,
      15,    -1,    -1,    -1,    -1,    20,    21,    22,    23,    24,
      25,    -1,    27,     3,     4,     5,     6,     7,     8,    -1,
      -1,    -1,    12,    -1,    -1,    15,    -1,    -1,    -1,    -1,
      20,    21,    22,    23,    24,    25,    -1,    27,     3,     4,
       5,     6,     7,     8,    -1,    -1,    -1,    12,    -1,    -1,
      15,    -1,    -1,    -1,    -1,    20,    21,    22,    23,    24,
      25,    -1,    27,     3,     4,     5,     6,     7,     8,    -1,
      -1,    -1,    12,    -1,    -1,    15,    -1,    -1,    -1,    -1,
      20,    21,    22,    23,    24,    25,    -1,    27,     3,     4,
       5,     6,     7,     8,    -1,    -1,    -1,    12,    -1,    -1,
      15,    -1,    -1,    -1,    -1,    20,    21,    22,    23,    24,
      25,    -1,    27,     3,     4,     5,     6,     7,     8,    -1,
      -1,    -1,    12,    -1,    -1,    15,    -1,    -1,    -1,    -1,
      20,    21,    22,    23,    24,    25,    -1,    27,     3,     4,
       5,     6,     7,     8,    -1,    -1,    -1,    12,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    20,    21,    22,    23,    24,
      25,    -1,    27,     3,     4,     5,     6,     7,    -1,    -1,
      -1,    -1,    12,     3,     4,     5,     6,     7,    -1,    -1,
      -1,    -1,    12,    -1,    24,    25,    -1,    27,    -1,    -1,
      -1,    -1,    -1,    -1,    24,    25,    -1,    27
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,    12,    20,    21,
      22,    23,    24,    25,    27,    48,    49,    50,    51,    53,
      54,    57,    62,    64,    65,    67,    12,    12,    12,    65,
      67,     6,    65,     5,    33,    12,    65,     0,    50,     5,
       5,    12,    11,    14,    61,    11,    16,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    41,    16,    63,    65,    63,    65,    13,    13,    11,
      14,     6,    67,    67,    14,    12,    65,    49,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,     5,    64,     5,     6,     4,    13,    18,    13,
      13,    52,    65,     5,    60,    32,    13,     5,    68,    69,
      66,    69,    13,    15,    17,    12,    12,    17,    65,    14,
      49,    15,    18,    19,    15,    69,    11,    13,    18,    14,
      63,    63,    15,    11,     5,    67,    11,    11,    69,    49,
      13,    13,     9,    55,    56,    15,    10,    56,    58,    59,
      12,    12,    14,    65,    65,    49,    52,    52,    15,    49,
      49,    15,    15
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (scanner, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, scanner)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, scanner); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void* scanner)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, scanner)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    void* scanner;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
  YYUSE (scanner);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
        break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void* scanner)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, scanner)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    void* scanner;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, scanner);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, void* scanner)
#else
static void
yy_reduce_print (yyvsp, yyrule, scanner)
    YYSTYPE *yyvsp;
    int yyrule;
    void* scanner;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       , scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, scanner); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void* scanner)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, scanner)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    void* scanner;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (scanner);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}




/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void* scanner)
#else
int
yyparse (scanner)
    void* scanner;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;


#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
static YYSTYPE yyval_default;
# define YY_INITIAL_VALUE(Value) = Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
/* Line 1792 of yacc.c  */
#line 176 "bs.y"
    { BS_BUILD((yyval.vProgram), CreateProgram()); (yyval.vProgram)->SetStmtList((yyvsp[(1) - (1)].vStmtList)); }
    break;

  case 3:
/* Line 1792 of yacc.c  */
#line 179 "bs.y"
    { 
                    (yyval.vStmtList) = (yyvsp[(1) - (2)].vStmtList);
                    BS_CHECKLIST((yyvsp[(1) - (2)].vStmtList));
                    StmtList* newTail = BS_GlobalBuilder->CreateStmtList();
                    newTail->SetStmt((yyvsp[(2) - (2)].vStmt));
                    StmtList* tailTraversal = (yyvsp[(1) - (2)].vStmtList);
                    while(tailTraversal->GetTail() != nullptr) { tailTraversal = tailTraversal->GetTail(); }
                    tailTraversal->SetTail(newTail);
            }
    break;

  case 4:
/* Line 1792 of yacc.c  */
#line 188 "bs.y"
    { (yyval.vStmtList) = BS_GlobalBuilder->CreateStmtList(); (yyval.vStmtList)->SetStmt((yyvsp[(1) - (1)].vStmt)); }
    break;

  case 5:
/* Line 1792 of yacc.c  */
#line 189 "bs.y"
    { (yyval.vStmtList) = BS_GlobalBuilder->CreateStmtList(); }
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 192 "bs.y"
    { BS_BUILD((yyval.vStmt), BuildStmtExp((yyvsp[(1) - (2)].vExp))); }
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 193 "bs.y"
    { BS_BUILD((yyval.vStmt), BuildStmtReturn((yyvsp[(2) - (3)].vExp))); }
    break;

  case 8:
/* Line 1792 of yacc.c  */
#line 194 "bs.y"
    {BS_BUILD((yyval.vStmt), BindFunImplementation((yyvsp[(1) - (2)].vStmtFunDec), (yyvsp[(2) - (2)].vStmtList)));}
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 195 "bs.y"
    { BS_BUILD((yyval.vStmt), BuildStmtWhile((yyvsp[(3) - (7)].vExp), (yyvsp[(6) - (7)].vStmtList)));}
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 196 "bs.y"
    { BS_BUILD((yyval.vStmt), BuildStmtStructDef((yyvsp[(2) - (6)].identifierText), (yyvsp[(4) - (6)].vArgList))); }
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 198 "bs.y"
    { 
            if (BS_GlobalBuilder->GetSymbolTable()->GetTypeByName((yyvsp[(2) - (6)].identifierText)) == nullptr)
            {
                const Pegasus::BlockScript::TypeDesc* enumType = BS_GlobalBuilder->GetSymbolTable()->CreateEnumType(
                    (yyvsp[(2) - (6)].identifierText),
                    (yyvsp[(4) - (6)].vEnumNode) //the definition!
                );
        
                BS_BUILD((yyval.vStmt), BuildStmtEnumTypeDef(enumType));
            }
            else
            {
                BS_parseerror("Syntax error. Enum type re-declaration. A type with this name already exists");
                YYERROR; 
            }
        }
    break;

  case 12:
/* Line 1792 of yacc.c  */
#line 215 "bs.y"
    { 
                    if ((yyvsp[(8) - (8)].vStmtIfElse) != nullptr && (yyvsp[(7) - (8)].vStmtIfElse) != nullptr)
                    {
                        Pegasus::BlockScript::Ast::StmtIfElse* tail = (yyvsp[(7) - (8)].vStmtIfElse);
                        while (tail->GetTail() != nullptr) tail = tail->GetTail();
                        tail->SetTail((yyvsp[(8) - (8)].vStmtIfElse)); //else is the last statement
                    }
                    if ((yyvsp[(8) - (8)].vStmtIfElse) != nullptr && (yyvsp[(7) - (8)].vStmtIfElse) == nullptr)
                    {
                        BS_BUILD((yyval.vStmt), BuildStmtIfElse((yyvsp[(3) - (8)].vExp), (yyvsp[(5) - (8)].vStmtList), (yyvsp[(8) - (8)].vStmtIfElse), (yyvsp[(4) - (8)].vFrameInfo)));
                    }
                    else
                    {
                        BS_BUILD((yyval.vStmt), BuildStmtIfElse((yyvsp[(3) - (8)].vExp), (yyvsp[(5) - (8)].vStmtList), (yyvsp[(7) - (8)].vStmtIfElse), (yyvsp[(4) - (8)].vFrameInfo)));
                    }

                    BS_GlobalBuilder->PopFrame();
                 }
    break;

  case 13:
/* Line 1792 of yacc.c  */
#line 235 "bs.y"
    { BS_BUILD((yyval.vFrameInfo), StartNewFrame()); }
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 238 "bs.y"
    { BS_BUILD((yyval.vFrameInfo), StartNewFrame()); }
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 241 "bs.y"
    { (yyval.vTypeDesc) = (yyvsp[(1) - (1)].vTypeDesc); if ((yyval.vTypeDesc) == nullptr) { BS_parseerror("Syntax error. Invalid function type.");YYERROR; }; if (!BS_GlobalBuilder->StartNewFunction((yyval.vTypeDesc))) {BS_parseerror("cannot declare function within a function"); YYERROR;} }
    break;

  case 16:
/* Line 1792 of yacc.c  */
#line 244 "bs.y"
    { BS_GlobalBuilder->StartNewFrame(); }
    break;

  case 17:
/* Line 1792 of yacc.c  */
#line 248 "bs.y"
    {
                        (yyval.vStmtIfElse) = (yyvsp[(1) - (7)].vStmtIfElse);
                        BS_CHECKLIST((yyvsp[(1) - (7)].vStmtIfElse));
                        Pegasus::BlockScript::Ast::StmtIfElse* tail = (yyvsp[(1) - (7)].vStmtIfElse);
                        while (tail->GetTail() != nullptr)
                        {
                            tail = tail->GetTail();
                        }
                        
                        tail->SetTail(
                            BS_GlobalBuilder->BuildStmtIfElse((yyvsp[(4) - (7)].vExp), (yyvsp[(6) - (7)].vStmtList), nullptr, (yyvsp[(5) - (7)].vFrameInfo))
                        ); 
                    }
    break;

  case 18:
/* Line 1792 of yacc.c  */
#line 262 "bs.y"
    {
                        (yyval.vStmtIfElse) = BS_GlobalBuilder->BuildStmtIfElse((yyvsp[(3) - (6)].vExp), (yyvsp[(5) - (6)].vStmtList), nullptr, (yyvsp[(4) - (6)].vFrameInfo));
                   }
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 265 "bs.y"
    { (yyval.vStmtIfElse) = nullptr; }
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 268 "bs.y"
    { //pop previous frame
                              BS_GlobalBuilder->PopFrame();  
                            }
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 273 "bs.y"
    {BS_BUILD((yyval.vStmtFunDec), BuildStmtFunDec((yyvsp[(4) - (5)].vArgList), (yyvsp[(1) - (5)].vTypeDesc), (yyvsp[(2) - (5)].identifierText)));}
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 276 "bs.y"
    { BS_BUILD((yyval.vStmtIfElse), BuildStmtIfElse(nullptr, (yyvsp[(3) - (4)].vStmtList), nullptr, (yyvsp[(1) - (4)].vFrameInfo))); }
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 277 "bs.y"
    { (yyval.vStmtIfElse) = nullptr; }
    break;

  case 24:
/* Line 1792 of yacc.c  */
#line 280 "bs.y"
    { BS_GlobalBuilder->PopFrame(); BS_BUILD((yyval.vFrameInfo), StartNewFrame()); }
    break;

  case 25:
/* Line 1792 of yacc.c  */
#line 283 "bs.y"
    {
                Pegasus::BlockScript::EnumNode* enumNode = BS_GlobalBuilder->GetSymbolTable()->NewEnumNode(); 
                enumNode->mIdd = (yyvsp[(3) - (3)].identifierText);
                Pegasus::BlockScript::EnumNode* tailList = (yyvsp[(1) - (3)].vEnumNode);
                while (tailList->mNext != nullptr) { tailList = tailList->mNext; }
                tailList->mNext = enumNode;
                enumNode->mGuid = tailList->mGuid + 1; 
                (yyval.vEnumNode) = (yyvsp[(1) - (3)].vEnumNode);
          }
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 292 "bs.y"
    {
                Pegasus::BlockScript::EnumNode* enumNode = BS_GlobalBuilder->GetSymbolTable()->NewEnumNode(); 
                enumNode->mIdd = (yyvsp[(1) - (1)].identifierText);
                (yyval.vEnumNode) = enumNode;
          }
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 299 "bs.y"
    { (yyval.vStmtList) = (yyvsp[(2) - (3)].vStmtList); }
    break;

  case 28:
/* Line 1792 of yacc.c  */
#line 300 "bs.y"
    { (yyval.vStmtList) = nullptr; }
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 303 "bs.y"
    { BS_BUILD((yyval.vExp), BuildImmInt((yyvsp[(1) - (1)].integerValue))); }
    break;

  case 30:
/* Line 1792 of yacc.c  */
#line 304 "bs.y"
    { BS_BUILD((yyval.vExp), BuildImmFloat((yyvsp[(1) - (1)].floatValue))); }
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 306 "bs.y"
    {
            //figure out size at compile time!
            BS_BUILD((yyval.vExp), BuildImmInt((yyvsp[(3) - (4)].vTypeDesc)->GetByteSize()));
          }
    break;

  case 32:
/* Line 1792 of yacc.c  */
#line 312 "bs.y"
    {
                (yyval.vExpList) = (yyvsp[(1) - (3)].vExpList);
                BS_CHECKLIST((yyvsp[(1) - (3)].vExpList));
                ExpList* newList = BS_GlobalBuilder->CreateExpList(); 
                newList->SetExp((yyvsp[(3) - (3)].vExp));
                ExpList* tailTraversal = (yyvsp[(1) - (3)].vExpList);
                while (tailTraversal->GetTail() != nullptr) { tailTraversal = tailTraversal->GetTail(); }
                tailTraversal->SetTail(newList);
         }
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 321 "bs.y"
    { (yyval.vExpList) = BS_GlobalBuilder->CreateExpList(); (yyval.vExpList)->SetExp((yyvsp[(1) - (1)].vExp)); }
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 322 "bs.y"
    { (yyval.vExpList) = BS_GlobalBuilder->CreateExpList(); }
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 325 "bs.y"
    { BS_BUILD((yyval.vExp), BuildIdd((yyvsp[(1) - (1)].identifierText))); }
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 328 "bs.y"
    { (yyval.vExp) = (yyvsp[(1) - (1)].vExp); }
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 329 "bs.y"
    { BS_BUILD((yyval.vExp), BuildFunCall((yyvsp[(3) - (4)].vExpList), (yyvsp[(1) - (4)].identifierText))); }
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 330 "bs.y"
    { BS_BUILD((yyval.vExp), BuildFunCall((yyvsp[(3) - (4)].vExpList), (yyvsp[(1) - (4)].identifierText))); }
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 331 "bs.y"
    { BS_BUILD((yyval.vExp), BuildStaticArrayDec((yyvsp[(3) - (4)].vTypeDesc))); }
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 332 "bs.y"
    { BS_BUILD((yyval.vExp), BuildStrImm((yyvsp[(1) - (1)].identifierText))); }
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 333 "bs.y"
    { (yyval.vExp) = (yyvsp[(1) - (1)].vExp); }
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 334 "bs.y"
    { BS_BUILD((yyval.vExp), BuildMethodCall((yyvsp[(1) - (6)].vExp), (yyvsp[(3) - (6)].identifierText), (yyvsp[(5) - (6)].vExpList))); }
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 335 "bs.y"
    { BS_BUILD((yyval.vExp), BuildMethodCall((yyvsp[(1) - (6)].vExp), (yyvsp[(3) - (6)].identifierText), (yyvsp[(5) - (6)].vExpList))); }
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 336 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 337 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 46:
/* Line 1792 of yacc.c  */
#line 338 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 47:
/* Line 1792 of yacc.c  */
#line 339 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 48:
/* Line 1792 of yacc.c  */
#line 340 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 341 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 342 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 51:
/* Line 1792 of yacc.c  */
#line 343 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 52:
/* Line 1792 of yacc.c  */
#line 344 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 53:
/* Line 1792 of yacc.c  */
#line 345 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 346 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 347 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 56:
/* Line 1792 of yacc.c  */
#line 348 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 349 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 350 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (4)].vExp), O_ACCESS, (yyvsp[(3) - (4)].vExp))); }
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 351 "bs.y"
    { BS_BUILD((yyval.vExp), BuildUnop((yyvsp[(1) - (2)].token), (yyvsp[(2) - (2)].vExp))); }
    break;

  case 60:
/* Line 1792 of yacc.c  */
#line 352 "bs.y"
    { BS_BUILD((yyval.vExp), BuildExplicitCast((yyvsp[(4) - (4)].vExp), (yyvsp[(2) - (4)].vTypeDesc))); }
    break;

  case 61:
/* Line 1792 of yacc.c  */
#line 353 "bs.y"
    { (yyval.vExp) = (yyvsp[(2) - (3)].vExp); }
    break;

  case 62:
/* Line 1792 of yacc.c  */
#line 356 "bs.y"
    {
                (yyval.vArgList) = (yyvsp[(1) - (3)].vArgList);
                BS_CHECKLIST((yyvsp[(1) - (3)].vArgList));
                ArgList* newList = BS_GlobalBuilder->CreateArgList(); 
                newList->SetArgDec((yyvsp[(3) - (3)].vArgDec));
                ArgList* tailTraversal = (yyvsp[(1) - (3)].vArgList);
                while (tailTraversal->GetTail() != nullptr) { tailTraversal = tailTraversal->GetTail(); }
                tailTraversal->SetTail(newList);
         }
    break;

  case 63:
/* Line 1792 of yacc.c  */
#line 365 "bs.y"
    { (yyval.vArgList) = BS_GlobalBuilder->CreateArgList(); (yyval.vArgList)->SetArgDec((yyvsp[(1) - (1)].vArgDec)); }
    break;

  case 64:
/* Line 1792 of yacc.c  */
#line 366 "bs.y"
    { (yyval.vArgList) = BS_GlobalBuilder->CreateArgList(); }
    break;

  case 65:
/* Line 1792 of yacc.c  */
#line 369 "bs.y"
    { 
                const Pegasus::BlockScript::TypeDesc* resultType = BS_GlobalBuilder->GetSymbolTable()->CreateArrayType(
                    (yyvsp[(1) - (4)].vTypeDesc)->GetName(), //name
                    (yyvsp[(1) - (4)].vTypeDesc),  // child type
                    (yyvsp[(3) - (4)].integerValue)   //array count
                );        
                
                if (resultType != nullptr)
                {
                    (yyval.vTypeDesc) = resultType;
                }
                else
                {
                    BS_parseerror("Semantics error, unknown type specified.");
                    YYERROR;
                }
              }
    break;

  case 66:
/* Line 1792 of yacc.c  */
#line 386 "bs.y"
    {                
                const TypeDesc* typeDesc = BS_GlobalBuilder->GetTypeByName((yyvsp[(1) - (1)].identifierText));
                if (typeDesc != nullptr)
                {
                    (yyval.vTypeDesc) = typeDesc;
                }
                else
                {
                    BS_parseerror("Semantics error, unknown type specified.");
                    YYERROR;
                }
            }
    break;

  case 67:
/* Line 1792 of yacc.c  */
#line 400 "bs.y"
    {
                (yyval.vArgList) = (yyvsp[(1) - (3)].vArgList);
                BS_CHECKLIST((yyvsp[(1) - (3)].vArgList));
                ArgList* newList = BS_GlobalBuilder->CreateArgList(); 
                newList->SetArgDec((yyvsp[(2) - (3)].vArgDec));
                ArgList* tailTraversal = (yyvsp[(1) - (3)].vArgList);
                while (tailTraversal->GetTail() != nullptr) { tailTraversal = tailTraversal->GetTail(); }
                tailTraversal->SetTail(newList);
         }
    break;

  case 68:
/* Line 1792 of yacc.c  */
#line 409 "bs.y"
    { (yyval.vArgList) = BS_GlobalBuilder->CreateArgList(); (yyval.vArgList)->SetArgDec((yyvsp[(1) - (2)].vArgDec)); }
    break;

  case 69:
/* Line 1792 of yacc.c  */
#line 410 "bs.y"
    { (yyval.vArgList) = BS_GlobalBuilder->CreateArgList(); }
    break;

  case 70:
/* Line 1792 of yacc.c  */
#line 413 "bs.y"
    { BS_BUILD((yyval.vArgDec), BuildArgDec((yyvsp[(1) - (3)].identifierText), (yyvsp[(3) - (3)].vTypeDesc))); }
    break;


/* Line 1792 of yacc.c  */
#line 2236 "bs.parser.cpp"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (scanner, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (scanner, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, scanner);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (scanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, scanner);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


/* Line 2055 of yacc.c  */
#line 416 "bs.y"
         

//***************************************************//
//              Let the insanity end                 //
//***************************************************//

extern void BS_restart(FILE* f);


void Bison_BlockScriptParse(const FileBuffer* fileBuffer, BlockScriptBuilder* builder) 
{          
    CompilerState compilerState;
    compilerState.mBuilder = builder;
    compilerState.mFileBuffer = fileBuffer;

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

