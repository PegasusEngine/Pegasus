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
#define YYPURE 0

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
#line 14 "bs.y"

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
    #include "Pegasus/BlockScript/StackFrameInfo.h"
    #include "Pegasus/BlockScript/BsIntrinsics.h"
    #include "Pegasus/BlockScript/TypeTable.h"
    #include "Pegasus/BlockScript/TypeDesc.h"
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

    void BS_error(const char *s) { BS_GlobalBuilder->IncErrorCount(); printf("%d : ERROR: %s near token '%s'\n", BS_line, s, BS_text); }
    
    //***************************************************//
    //              Let the insanity begin               //
    //***************************************************//

/* Line 371 of yacc.c  */
#line 125 "bs.parser.cpp"

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
     I_STRING = 261,
     K_IF = 262,
     K_ELSE_IF = 263,
     K_ELSE = 264,
     K_SEMICOLON = 265,
     K_L_PAREN = 266,
     K_R_PAREN = 267,
     K_L_BRAC = 268,
     K_R_BRAC = 269,
     K_L_LACE = 270,
     K_R_LACE = 271,
     K_COMMA = 272,
     K_COL = 273,
     K_RETURN = 274,
     K_WHILE = 275,
     K_STRUCT = 276,
     O_PLUS = 277,
     O_MINUS = 278,
     O_MUL = 279,
     O_DIV = 280,
     O_MOD = 281,
     O_EQ = 282,
     O_GT = 283,
     O_LT = 284,
     O_GTE = 285,
     O_LTE = 286,
     O_LAND = 287,
     O_LOR = 288,
     O_SET = 289,
     O_DOT = 290,
     O_ACCESS = 291,
     O_ARRAY_CONSTRUCTOR = 292,
     O_TREE = 293,
     O_IMPLICIT_CAST = 294,
     O_EXPLICIT_CAST = 295,
     ACCESS_PREC = 296,
     CAST = 297,
     NEG = 298
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 67 "bs.y"

    int    token;
    int    integerValue;
    float  floatValue;
    char*  identifierText;
    const  Pegasus::BlockScript::StackFrameInfo* vFrameInfo;
    const  Pegasus::BlockScript::TypeDesc*       vTypeDesc;
    #define BS_PROCESS(N) Pegasus::BlockScript::Ast::N* v##N;
    #include "Pegasus/BlockScript/Ast.inl"
    #undef BS_PROCESS


/* Line 387 of yacc.c  */
#line 224 "bs.parser.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE BS_lval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int BS_parse (void *YYPARSE_PARAM);
#else
int BS_parse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int BS_parse (void);
#else
int BS_parse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_BS_BS_PARSER_HPP_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 252 "bs.parser.cpp"

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
#define YYFINAL  29
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   474

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  44
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  22
/* YYNRULES -- Number of rules.  */
#define YYNRULES  63
/* YYNRULES -- Number of states.  */
#define YYNSTATES  138

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   298

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
      35,    36,    37,    38,    39,    40,    41,    42,    43
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    11,    14,    21,    25,
      28,    36,    43,    52,    54,    57,    59,    61,    69,    76,
      77,    79,    85,    90,    91,    93,    97,    99,   101,   103,
     107,   109,   110,   112,   114,   119,   121,   123,   127,   131,
     135,   139,   143,   147,   151,   155,   159,   163,   167,   171,
     175,   179,   184,   187,   192,   196,   200,   202,   203,   208,
     210,   214,   217,   218
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      45,     0,    -1,    46,    -1,    46,    47,    -1,    47,    -1,
      -1,    61,    10,    -1,    60,    38,    11,    59,    12,    10,
      -1,    19,    61,    10,    -1,    54,    57,    -1,    51,    11,
      61,    12,    13,    46,    14,    -1,    48,     5,    13,    64,
      14,    10,    -1,     7,    11,    61,    49,    46,    14,    52,
      55,    -1,    21,    -1,    12,    13,    -1,     5,    -1,    20,
      -1,    52,    53,    11,    61,    49,    46,    14,    -1,    53,
      11,    61,    49,    46,    14,    -1,    -1,     8,    -1,    50,
       5,    11,    62,    12,    -1,    56,    13,    46,    14,    -1,
      -1,     9,    -1,    13,    46,    14,    -1,    10,    -1,     4,
      -1,     3,    -1,    59,    17,    61,    -1,    61,    -1,    -1,
       5,    -1,    60,    -1,     5,    11,    59,    12,    -1,     6,
      -1,    58,    -1,    61,    34,    61,    -1,    61,    22,    61,
      -1,    61,    23,    61,    -1,    61,    24,    61,    -1,    61,
      25,    61,    -1,    61,    26,    61,    -1,    61,    27,    61,
      -1,    61,    32,    61,    -1,    61,    33,    61,    -1,    61,
      29,    61,    -1,    61,    28,    61,    -1,    61,    31,    61,
      -1,    61,    30,    61,    -1,    61,    35,    60,    -1,    61,
      15,    61,    16,    -1,    23,    61,    -1,    11,     5,    12,
      61,    -1,    11,    61,    12,    -1,    62,    17,    65,    -1,
      65,    -1,    -1,    63,    15,     4,    16,    -1,     5,    -1,
      64,    65,    10,    -1,    65,    10,    -1,    -1,     5,    18,
      63,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   154,   154,   157,   166,   167,   170,   171,   172,   173,
     174,   175,   176,   197,   200,   203,   206,   209,   223,   227,
     230,   235,   238,   239,   242,   245,   246,   249,   250,   253,
     262,   263,   266,   269,   270,   271,   272,   273,   274,   275,
     276,   277,   278,   279,   280,   281,   282,   283,   284,   285,
     286,   287,   288,   289,   290,   293,   302,   303,   306,   325,
     340,   349,   350,   353
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "I_FLOAT", "I_INT", "IDENTIFIER",
  "I_STRING", "K_IF", "K_ELSE_IF", "K_ELSE", "K_SEMICOLON", "K_L_PAREN",
  "K_R_PAREN", "K_L_BRAC", "K_R_BRAC", "K_L_LACE", "K_R_LACE", "K_COMMA",
  "K_COL", "K_RETURN", "K_WHILE", "K_STRUCT", "O_PLUS", "O_MINUS", "O_MUL",
  "O_DIV", "O_MOD", "O_EQ", "O_GT", "O_LT", "O_GTE", "O_LTE", "O_LAND",
  "O_LOR", "O_SET", "O_DOT", "O_ACCESS", "O_ARRAY_CONSTRUCTOR", "O_TREE",
  "O_IMPLICIT_CAST", "O_EXPLICIT_CAST", "ACCESS_PREC", "CAST", "NEG",
  "$accept", "program", "stmt_list", "stmt", "struct_keyword",
  "if_begin_scope", "fun_type", "while_keyword", "stmt_else_if_tail",
  "else_if_keyword", "fun_declaration", "stmt_else_tail", "else_keyword",
  "fun_stmt_list", "numeric", "exp_list", "ident", "exp", "arg_list",
  "type_desc", "struct_def_list", "arg_dec", YY_NULL
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
     295,   296,   297,   298
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    44,    45,    46,    46,    46,    47,    47,    47,    47,
      47,    47,    47,    48,    49,    50,    51,    52,    52,    52,
      53,    54,    55,    55,    56,    57,    57,    58,    58,    59,
      59,    59,    60,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    62,    62,    62,    63,    63,
      64,    64,    64,    65
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     0,     2,     6,     3,     2,
       7,     6,     8,     1,     2,     1,     1,     7,     6,     0,
       1,     5,     4,     0,     1,     3,     1,     1,     1,     3,
       1,     0,     1,     1,     4,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     4,     2,     4,     3,     3,     1,     0,     4,     1,
       3,     2,     0,     3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       5,    28,    27,    32,    35,     0,     0,     0,    16,    13,
       0,     0,     2,     4,     0,     0,     0,     0,    36,    33,
       0,    31,     0,    32,    33,     0,    32,     0,    52,     1,
       3,     0,     0,     0,    26,     5,     9,     0,     6,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    30,     0,     0,    54,     8,
      62,    57,     0,     0,    31,     0,    38,    39,    40,    41,
      42,    43,    47,    46,    49,    48,    44,    45,    37,    32,
      50,    34,     0,     0,     5,    53,     0,     0,     0,     0,
      56,     0,    25,     0,    51,    29,    14,     0,     0,     0,
       0,    61,    21,     0,     5,     0,    19,    59,    63,    11,
      60,    55,     0,     7,    20,    23,     0,     0,    10,    24,
       0,    12,     0,     0,     0,     0,     5,     0,    58,     0,
       0,     5,     5,    22,     0,     0,    18,    17
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    11,    12,    13,    14,    84,    15,    16,   115,   116,
      17,   121,   122,    36,    18,    54,    24,    20,    89,   108,
      87,    88
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -54
static const yytype_int16 yypact[] =
{
     436,   -54,   -54,     5,   -54,    -5,   102,   447,   -54,   -54,
     447,     9,   436,   -54,    12,    27,    51,    56,   -54,    32,
     118,   447,   447,    70,   -54,   168,    74,   144,    -4,   -54,
     -54,    89,    92,   447,   -54,   436,   -54,   100,   -54,   447,
     447,   447,   447,   447,   447,   447,   447,   447,   447,   447,
     447,   447,   447,   111,    66,   258,   192,   447,   -54,   -54,
     112,   112,   216,    53,   447,   237,   -10,   -10,    40,    -4,
     439,   298,    65,    65,    65,    65,    -2,    -2,   279,   -54,
     -54,   -54,   447,   105,   436,    -4,   103,    63,   117,    97,
     -54,   116,   -54,    98,   -54,   258,   -54,   331,   131,   127,
     128,   -54,   -54,   112,   436,   129,   147,   -54,   141,   -54,
     -54,   -54,   352,   -54,   -54,    87,   146,   154,   -54,   -54,
     149,   -54,   148,   447,   165,   447,   436,   192,   -54,   192,
     373,   436,   436,   -54,   394,   415,   -54,   -54
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -54,   -54,   -33,   -11,   -54,   -35,   -54,   -54,   -54,    47,
     -54,   -54,   -54,   -54,   -54,    99,     0,    -3,   -54,   -54,
     -54,   -53
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -16
static const yytype_int16 yytable[] =
{
      19,    30,    63,    25,    27,    39,    22,    28,    90,    29,
     -15,    39,    19,    39,    42,    43,    21,    31,    55,    56,
      40,    41,    42,    43,    44,    53,    46,    47,    48,    49,
      62,    53,    32,    53,   100,    19,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
     111,    97,    30,    80,    85,    39,     1,     2,     3,     4,
       5,    55,    33,    19,     6,    43,    34,    92,    86,    35,
      37,   112,     7,     8,     9,    53,    10,    99,    81,    95,
      39,    21,    57,    82,    19,    21,    30,    40,    41,    42,
      43,    44,   131,   130,   132,   114,   119,    19,   134,   135,
      53,    30,    60,    61,    19,     1,     2,    23,     4,   102,
     105,    64,    19,     6,   103,    82,    79,    86,    96,    30,
     127,    98,   129,    30,    30,    10,    19,   101,    38,   104,
      19,    19,    19,    39,    19,    19,   107,   109,   110,   113,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    59,   114,   117,   123,   124,    39,
     125,   126,   120,    93,     0,     0,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      58,   128,     0,    39,     0,     0,     0,     0,     0,     0,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    83,     0,     0,    39,     0,     0,
       0,     0,     0,     0,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    91,     0,
       0,    39,     0,     0,     0,     0,     0,     0,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    39,    94,     0,     0,     0,     0,     0,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    39,     0,     0,     0,     0,     0,     0,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    39,     0,     0,     0,     0,     0,
       0,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    39,    53,     0,     0,     0,     0,     0,
      40,    41,    42,    43,    44,     0,    46,    47,    48,    49,
      50,    51,     0,    53,     1,     2,     3,     4,     5,     0,
       0,     0,     6,     0,     0,   106,     0,     0,     0,     0,
       7,     8,     9,     0,    10,     1,     2,     3,     4,     5,
       0,     0,     0,     6,     0,     0,   118,     0,     0,     0,
       0,     7,     8,     9,     0,    10,     1,     2,     3,     4,
       5,     0,     0,     0,     6,     0,     0,   133,     0,     0,
       0,     0,     7,     8,     9,     0,    10,     1,     2,     3,
       4,     5,     0,     0,     0,     6,     0,     0,   136,     0,
       0,     0,     0,     7,     8,     9,     0,    10,     1,     2,
       3,     4,     5,     0,     0,     0,     6,     0,     0,   137,
       0,     0,     0,     0,     7,     8,     9,     0,    10,     1,
       2,     3,     4,     5,     0,     0,     0,     6,     0,     0,
       1,     2,    26,     4,    39,     7,     8,     9,     6,    10,
       0,    40,    41,    42,    43,     0,     0,     0,     0,     0,
      10,     0,     0,     0,    53
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-54)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       0,    12,    35,     6,     7,    15,    11,    10,    61,     0,
       5,    15,    12,    15,    24,    25,    11,     5,    21,    22,
      22,    23,    24,    25,    26,    35,    28,    29,    30,    31,
      33,    35,     5,    35,    87,    35,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
     103,    84,    63,    53,    57,    15,     3,     4,     5,     6,
       7,    64,    11,    63,    11,    25,    10,    14,     5,    13,
      38,   104,    19,    20,    21,    35,    23,    14,    12,    82,
      15,    11,    12,    17,    84,    11,    97,    22,    23,    24,
      25,    26,   127,   126,   129,     8,     9,    97,   131,   132,
      35,   112,    13,    11,   104,     3,     4,     5,     6,    12,
      12,    11,   112,    11,    17,    17,     5,     5,    13,   130,
     123,    18,   125,   134,   135,    23,   126,    10,    10,    13,
     130,   131,   132,    15,   134,   135,     5,    10,    10,    10,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    10,     8,    15,    11,     4,    15,
      11,    13,   115,    64,    -1,    -1,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      12,    16,    -1,    15,    -1,    -1,    -1,    -1,    -1,    -1,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    12,    -1,    -1,    15,    -1,    -1,
      -1,    -1,    -1,    -1,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    12,    -1,
      -1,    15,    -1,    -1,    -1,    -1,    -1,    -1,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    15,    16,    -1,    -1,    -1,    -1,    -1,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    15,    -1,    -1,    -1,    -1,    -1,    -1,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    15,    -1,    -1,    -1,    -1,    -1,
      -1,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    15,    35,    -1,    -1,    -1,    -1,    -1,
      22,    23,    24,    25,    26,    -1,    28,    29,    30,    31,
      32,    33,    -1,    35,     3,     4,     5,     6,     7,    -1,
      -1,    -1,    11,    -1,    -1,    14,    -1,    -1,    -1,    -1,
      19,    20,    21,    -1,    23,     3,     4,     5,     6,     7,
      -1,    -1,    -1,    11,    -1,    -1,    14,    -1,    -1,    -1,
      -1,    19,    20,    21,    -1,    23,     3,     4,     5,     6,
       7,    -1,    -1,    -1,    11,    -1,    -1,    14,    -1,    -1,
      -1,    -1,    19,    20,    21,    -1,    23,     3,     4,     5,
       6,     7,    -1,    -1,    -1,    11,    -1,    -1,    14,    -1,
      -1,    -1,    -1,    19,    20,    21,    -1,    23,     3,     4,
       5,     6,     7,    -1,    -1,    -1,    11,    -1,    -1,    14,
      -1,    -1,    -1,    -1,    19,    20,    21,    -1,    23,     3,
       4,     5,     6,     7,    -1,    -1,    -1,    11,    -1,    -1,
       3,     4,     5,     6,    15,    19,    20,    21,    11,    23,
      -1,    22,    23,    24,    25,    -1,    -1,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    35
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,    11,    19,    20,    21,
      23,    45,    46,    47,    48,    50,    51,    54,    58,    60,
      61,    11,    11,     5,    60,    61,     5,    61,    61,     0,
      47,     5,     5,    11,    10,    13,    57,    38,    10,    15,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    59,    61,    61,    12,    12,    10,
      13,    11,    61,    46,    11,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,     5,
      60,    12,    17,    12,    49,    61,     5,    64,    65,    62,
      65,    12,    14,    59,    16,    61,    13,    46,    18,    14,
      65,    10,    12,    17,    13,    12,    14,     5,    63,    10,
      10,    65,    46,    10,     8,    52,    53,    15,    14,     9,
      53,    55,    56,    11,     4,    11,    13,    61,    16,    61,
      46,    49,    49,    14,    46,    46,    14,    14
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
      yyerror (YY_("syntax error: cannot back up")); \
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
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
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
		  Type, Value); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
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
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
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
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}




/* The lookahead symbol.  */
int yychar;


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
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
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
#line 154 "bs.y"
    { BS_BUILD((yyval.vProgram), CreateProgram()); (yyval.vProgram)->SetStmtList((yyvsp[(1) - (1)].vStmtList)); }
    break;

  case 3:
/* Line 1792 of yacc.c  */
#line 157 "bs.y"
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
#line 166 "bs.y"
    { (yyval.vStmtList) = BS_GlobalBuilder->CreateStmtList(); (yyval.vStmtList)->SetStmt((yyvsp[(1) - (1)].vStmt)); }
    break;

  case 5:
/* Line 1792 of yacc.c  */
#line 167 "bs.y"
    { (yyval.vStmtList) = BS_GlobalBuilder->CreateStmtList(); }
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 170 "bs.y"
    { BS_BUILD((yyval.vStmt), BuildStmtExp((yyvsp[(1) - (2)].vExp))); }
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 171 "bs.y"
    { BS_BUILD((yyval.vStmt), BuildStmtTreeModifier((yyvsp[(4) - (6)].vExpList), (yyvsp[(1) - (6)].vIdd))); }
    break;

  case 8:
/* Line 1792 of yacc.c  */
#line 172 "bs.y"
    { BS_BUILD((yyval.vStmt), BuildStmtReturn((yyvsp[(2) - (3)].vExp))); }
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 173 "bs.y"
    {BS_BUILD((yyval.vStmt), BindFunImplementation((yyvsp[(1) - (2)].vStmtFunDec), (yyvsp[(2) - (2)].vStmtList)));}
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 174 "bs.y"
    { BS_BUILD((yyval.vStmt), BuildStmtWhile((yyvsp[(3) - (7)].vExp), (yyvsp[(6) - (7)].vStmtList)));}
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 175 "bs.y"
    { BS_BUILD((yyval.vStmt), BuildStmtStructDef((yyvsp[(2) - (6)].identifierText), (yyvsp[(4) - (6)].vArgList))); }
    break;

  case 12:
/* Line 1792 of yacc.c  */
#line 177 "bs.y"
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
#line 197 "bs.y"
    { BS_BUILD((yyval.vFrameInfo), StartNewFrame()); }
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 200 "bs.y"
    { BS_BUILD((yyval.vFrameInfo), StartNewFrame()); }
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 203 "bs.y"
    { (yyval.identifierText) = (yyvsp[(1) - (1)].identifierText); if (!BS_GlobalBuilder->StartNewFunction()) {BS_error("cannot declare function within a function"); YYERROR;} }
    break;

  case 16:
/* Line 1792 of yacc.c  */
#line 206 "bs.y"
    { BS_GlobalBuilder->StartNewFrame(); }
    break;

  case 17:
/* Line 1792 of yacc.c  */
#line 210 "bs.y"
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
#line 224 "bs.y"
    {
                        (yyval.vStmtIfElse) = BS_GlobalBuilder->BuildStmtIfElse((yyvsp[(3) - (6)].vExp), (yyvsp[(5) - (6)].vStmtList), nullptr, (yyvsp[(4) - (6)].vFrameInfo));
                   }
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 227 "bs.y"
    { (yyval.vStmtIfElse) = nullptr; }
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 230 "bs.y"
    { //pop previous frame
                              BS_GlobalBuilder->PopFrame();  
                            }
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 235 "bs.y"
    {BS_BUILD((yyval.vStmtFunDec), BuildStmtFunDec((yyvsp[(4) - (5)].vArgList), (yyvsp[(1) - (5)].identifierText), (yyvsp[(2) - (5)].identifierText)));}
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 238 "bs.y"
    { BS_BUILD((yyval.vStmtIfElse), BuildStmtIfElse(nullptr, (yyvsp[(3) - (4)].vStmtList), nullptr, (yyvsp[(1) - (4)].vFrameInfo))); }
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 239 "bs.y"
    { (yyval.vStmtIfElse) = nullptr; }
    break;

  case 24:
/* Line 1792 of yacc.c  */
#line 242 "bs.y"
    { BS_GlobalBuilder->PopFrame(); BS_BUILD((yyval.vFrameInfo), StartNewFrame()); }
    break;

  case 25:
/* Line 1792 of yacc.c  */
#line 245 "bs.y"
    { (yyval.vStmtList) = (yyvsp[(2) - (3)].vStmtList); }
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 246 "bs.y"
    { (yyval.vStmtList) = nullptr; }
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 249 "bs.y"
    { BS_BUILD((yyval.vExp), BuildImmInt((yyvsp[(1) - (1)].integerValue))); }
    break;

  case 28:
/* Line 1792 of yacc.c  */
#line 250 "bs.y"
    { BS_BUILD((yyval.vExp), BuildImmFloat((yyvsp[(1) - (1)].floatValue))); }
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 253 "bs.y"
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

  case 30:
/* Line 1792 of yacc.c  */
#line 262 "bs.y"
    { (yyval.vExpList) = BS_GlobalBuilder->CreateExpList(); (yyval.vExpList)->SetExp((yyvsp[(1) - (1)].vExp)); }
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 263 "bs.y"
    { (yyval.vExpList) = BS_GlobalBuilder->CreateExpList(); }
    break;

  case 32:
/* Line 1792 of yacc.c  */
#line 266 "bs.y"
    { BS_BUILD((yyval.vIdd), BuildIdd((yyvsp[(1) - (1)].identifierText))); }
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 269 "bs.y"
    { (yyval.vExp) = (yyvsp[(1) - (1)].vIdd); }
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 270 "bs.y"
    { BS_BUILD((yyval.vExp), BuildFunCall((yyvsp[(3) - (4)].vExpList), (yyvsp[(1) - (4)].identifierText))); }
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 271 "bs.y"
    { BS_BUILD((yyval.vExp), BuildStrImm((yyvsp[(1) - (1)].identifierText))); }
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 272 "bs.y"
    { (yyval.vExp) = (yyvsp[(1) - (1)].vExp); }
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 273 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 274 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 275 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 276 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 277 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 278 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 279 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 280 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 281 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 46:
/* Line 1792 of yacc.c  */
#line 282 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 47:
/* Line 1792 of yacc.c  */
#line 283 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 48:
/* Line 1792 of yacc.c  */
#line 284 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 285 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 286 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vIdd))); }
    break;

  case 51:
/* Line 1792 of yacc.c  */
#line 287 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (4)].vExp), O_ACCESS, (yyvsp[(3) - (4)].vExp))); }
    break;

  case 52:
/* Line 1792 of yacc.c  */
#line 288 "bs.y"
    { BS_BUILD((yyval.vExp), BuildUnop((yyvsp[(1) - (2)].token), (yyvsp[(2) - (2)].vExp))); }
    break;

  case 53:
/* Line 1792 of yacc.c  */
#line 289 "bs.y"
    { BS_BUILD((yyval.vExp), BuildExplicitCast((yyvsp[(4) - (4)].vExp), (yyvsp[(2) - (4)].identifierText))); }
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 290 "bs.y"
    { (yyval.vExp) = (yyvsp[(2) - (3)].vExp); }
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 293 "bs.y"
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

  case 56:
/* Line 1792 of yacc.c  */
#line 302 "bs.y"
    { (yyval.vArgList) = BS_GlobalBuilder->CreateArgList(); (yyval.vArgList)->SetArgDec((yyvsp[(1) - (1)].vArgDec)); }
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 303 "bs.y"
    { (yyval.vArgList) = BS_GlobalBuilder->CreateArgList(); }
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 306 "bs.y"
    { 
                Pegasus::BlockScript::TypeTable* typeTable = BS_GlobalBuilder->GetTypeTable();
                const Pegasus::BlockScript::TypeDesc* resultType = typeTable->CreateType(
                    Pegasus::BlockScript::TypeDesc::M_ARRAY,
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
                    BS_error("Fatal error, unknown type specified.");
                    YYERROR;
                }
              }
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 325 "bs.y"
    {
                Pegasus::BlockScript::TypeTable* typeTable = BS_GlobalBuilder->GetTypeTable();
                int typeId = typeTable->GetTypeByName((yyvsp[(1) - (1)].identifierText));
                if (typeId != -1)
                {
                    (yyval.vTypeDesc) = typeTable->GetTypeDesc(typeId);
                }
                else
                {
                    BS_error("Fatal error, unknown type specified.");
                    YYERROR;
                }
            }
    break;

  case 60:
/* Line 1792 of yacc.c  */
#line 340 "bs.y"
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

  case 61:
/* Line 1792 of yacc.c  */
#line 349 "bs.y"
    { (yyval.vArgList) = BS_GlobalBuilder->CreateArgList(); (yyval.vArgList)->SetArgDec((yyvsp[(1) - (2)].vArgDec)); }
    break;

  case 62:
/* Line 1792 of yacc.c  */
#line 350 "bs.y"
    { (yyval.vArgList) = BS_GlobalBuilder->CreateArgList(); }
    break;

  case 63:
/* Line 1792 of yacc.c  */
#line 353 "bs.y"
    { BS_BUILD((yyval.vArgDec), BuildArgDec((yyvsp[(1) - (3)].identifierText), (yyvsp[(3) - (3)].vTypeDesc))); }
    break;


/* Line 1792 of yacc.c  */
#line 2095 "bs.parser.cpp"
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
      yyerror (YY_("syntax error"));
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
        yyerror (yymsgp);
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
		      yytoken, &yylval);
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
		  yystos[yystate], yyvsp);
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
  yyerror (YY_("memory exhausted"));
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
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
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
#line 356 "bs.y"
         

//***************************************************//
//              Let the insanity end                 //
//***************************************************//



void Bison_BlockScriptParse(const FileBuffer* fileBuffer, BlockScriptBuilder* builder) 
{          
    BS_line = 1;
    BS_bufferPosition = 0;
    BS_GlobalBuilder = builder;
    BS_GlobalFileBuffer = fileBuffer;

    //register intrinsic callbacks
    Pegasus::BlockScript::RegisterIntrinsics(builder);

    do 
    {
	    BS_parse();
    } while (BS_HasNext());
    
}

