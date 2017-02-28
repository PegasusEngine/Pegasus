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
    #include "Pegasus/BlockScript/EventListeners.h"
    #include "Pegasus/BlockScript/BlockScriptAst.h"
    #include "Pegasus/BlockScript/StackFrameInfo.h"
    #include "Pegasus/BlockScript/Container.h"
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
        yyscan_t scanner = builder->GetScanner();
        Container<IBlockScriptCompilerListener*>& listeners = builder->GetEventListeners();
        for (int i = 0; i < listeners.Size(); ++i)
        {
            listeners[i]->OnCompilationError(builder->GetCurrentLine(), message, scanner ? BS_get_text(scanner) : "");
        }
    }

    #define BS_parseerror(errorstr) BS_ErrorDispatcher(BS_GlobalBuilder, errorstr)
    #define BS_error(scanner, errorstr)  BS_ErrorDispatcher(BS_GlobalBuilder, errorstr)
    
    //***************************************************//
    //              Let the insanity begin               //
    //***************************************************//

/* Line 371 of yacc.c  */
#line 142 "bs.parser.cpp"

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
     K_FOR = 277,
     K_STRUCT = 278,
     K_ENUM = 279,
     K_STATIC_ARRAY = 280,
     K_SIZE_OF = 281,
     K_EXTERN = 282,
     K_A_PAREN = 283,
     O_PLUS = 284,
     O_MINUS = 285,
     O_MUL = 286,
     O_DIV = 287,
     O_MOD = 288,
     O_EQ = 289,
     O_NEQ = 290,
     O_GT = 291,
     O_LT = 292,
     O_GTE = 293,
     O_LTE = 294,
     O_LAND = 295,
     O_LOR = 296,
     O_SET = 297,
     O_DOT = 298,
     O_ACCESS = 299,
     O_INC = 300,
     O_DEC = 301,
     O_METHOD_CALL = 302,
     O_IMPLICIT_CAST = 303,
     O_EXPLICIT_CAST = 304,
     ACCESS_PREC = 305,
     CAST = 306,
     NEG = 307
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 86 "bs.y"

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


/* Line 387 of yacc.c  */
#line 251 "bs.parser.cpp"
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
#line 278 "bs.parser.cpp"

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
#define YYFINAL  49
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   835

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  53
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  27
/* YYNRULES -- Number of rules.  */
#define YYNRULES  84
/* YYNRULES -- Number of states.  */
#define YYNSTATES  201

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   307

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
      45,    46,    47,    48,    49,    50,    51,    52
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    11,    14,    20,    24,
      31,    35,    38,    46,    58,    65,    72,    81,    83,    87,
      89,    92,    94,    96,    98,   106,   113,   114,   116,   122,
     127,   128,   130,   134,   136,   140,   142,   144,   146,   151,
     155,   157,   158,   160,   162,   167,   172,   177,   179,   181,
     184,   187,   194,   201,   205,   209,   213,   217,   221,   225,
     229,   233,   237,   241,   245,   249,   253,   257,   261,   266,
     269,   274,   278,   281,   284,   285,   287,   291,   293,   294,
     299,   301,   305,   308,   309
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      54,     0,    -1,    55,    -1,    55,    56,    -1,    56,    -1,
      -1,    74,    11,    -1,    27,    73,    42,    74,    11,    -1,
      58,    74,    11,    -1,    58,    27,    73,    42,    74,    11,
      -1,    20,    74,    11,    -1,    66,    70,    -1,    62,    12,
      74,    13,    14,    55,    15,    -1,    63,    12,    75,    11,
      75,    11,    75,    13,    14,    55,    15,    -1,    57,     5,
      14,    78,    15,    11,    -1,    24,     5,    14,    69,    15,
      11,    -1,     8,    12,    74,    60,    55,    15,    64,    67,
      -1,    23,    -1,    59,    72,    13,    -1,    28,    -1,    13,
      14,    -1,    77,    -1,    21,    -1,    22,    -1,    64,    65,
      12,    74,    60,    55,    15,    -1,    65,    12,    74,    60,
      55,    15,    -1,    -1,     9,    -1,    61,     5,    12,    76,
      13,    -1,    68,    14,    55,    15,    -1,    -1,    10,    -1,
      69,    18,     5,    -1,     5,    -1,    14,    55,    15,    -1,
      11,    -1,     4,    -1,     3,    -1,    26,    12,    77,    13,
      -1,    72,    18,    74,    -1,    74,    -1,    -1,     5,    -1,
      73,    -1,     5,    12,    72,    13,    -1,     6,    12,    72,
      13,    -1,    25,    37,    77,    36,    -1,     7,    -1,    71,
      -1,    45,    74,    -1,    46,    74,    -1,    74,    47,     5,
      12,    72,    13,    -1,    74,    47,     6,    12,    72,    13,
      -1,    74,    42,    74,    -1,    74,    29,    74,    -1,    74,
      30,    74,    -1,    74,    31,    74,    -1,    74,    32,    74,
      -1,    74,    33,    74,    -1,    74,    34,    74,    -1,    74,
      35,    74,    -1,    74,    40,    74,    -1,    74,    41,    74,
      -1,    74,    37,    74,    -1,    74,    36,    74,    -1,    74,
      39,    74,    -1,    74,    38,    74,    -1,    74,    43,    73,
      -1,    74,    16,    74,    17,    -1,    30,    74,    -1,    12,
      77,    13,    74,    -1,    12,    74,    13,    -1,    74,    45,
      -1,    74,    46,    -1,    -1,    74,    -1,    76,    18,    79,
      -1,    79,    -1,    -1,    77,    16,     4,    17,    -1,     6,
      -1,    78,    79,    11,    -1,    79,    11,    -1,    -1,     5,
      19,    77,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   189,   189,   192,   201,   202,   205,   206,   207,   208,
     223,   224,   225,   229,   233,   234,   251,   272,   275,   278,
     282,   285,   288,   291,   294,   308,   312,   315,   320,   323,
     324,   327,   330,   339,   346,   347,   350,   351,   352,   359,
     368,   369,   372,   375,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,   389,   390,   391,
     392,   393,   394,   395,   396,   397,   398,   399,   400,   401,
     402,   403,   404,   405,   408,   409,   412,   421,   422,   425,
     464,   479,   488,   489,   492
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
  "K_FOR", "K_STRUCT", "K_ENUM", "K_STATIC_ARRAY", "K_SIZE_OF", "K_EXTERN",
  "K_A_PAREN", "O_PLUS", "O_MINUS", "O_MUL", "O_DIV", "O_MOD", "O_EQ",
  "O_NEQ", "O_GT", "O_LT", "O_GTE", "O_LTE", "O_LAND", "O_LOR", "O_SET",
  "O_DOT", "O_ACCESS", "O_INC", "O_DEC", "O_METHOD_CALL",
  "O_IMPLICIT_CAST", "O_EXPLICIT_CAST", "ACCESS_PREC", "CAST", "NEG",
  "$accept", "program", "stmt_list", "stmt", "struct_keyword",
  "annotation_list", "annotation_begin", "if_begin_scope", "fun_type",
  "while_keyword", "for_keyword", "stmt_else_if_tail", "else_if_keyword",
  "fun_declaration", "stmt_else_tail", "else_keyword", "enum_list",
  "fun_stmt_list", "immediate", "exp_list", "ident", "exp", "optional_exp",
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
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    53,    54,    55,    55,    55,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    64,    64,    65,    66,    67,
      67,    68,    69,    69,    70,    70,    71,    71,    71,    72,
      72,    72,    73,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    75,    75,    76,    76,    76,    77,
      77,    78,    78,    78,    79
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     0,     2,     5,     3,     6,
       3,     2,     7,    11,     6,     6,     8,     1,     3,     1,
       2,     1,     1,     1,     7,     6,     0,     1,     5,     4,
       0,     1,     3,     1,     3,     1,     1,     1,     4,     3,
       1,     0,     1,     1,     4,     4,     4,     1,     1,     2,
       2,     6,     6,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     4,     2,
       4,     3,     2,     2,     0,     1,     3,     1,     0,     4,
       1,     3,     2,     0,     3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       5,    37,    36,    42,    80,    47,     0,     0,     0,    22,
      23,    17,     0,     0,     0,     0,    19,     0,     0,     0,
       0,     2,     4,     0,     0,    41,     0,     0,     0,     0,
      48,    43,     0,    21,    41,    41,     0,     0,     0,     0,
       0,     0,     0,     0,    42,     0,    69,    49,    50,     1,
       3,     0,     0,     0,     0,    40,     0,     0,    74,    35,
       5,    11,     6,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    72,
      73,     0,     0,     0,     0,     0,    71,     0,    10,     0,
      80,     0,     0,     0,    83,     0,     8,    18,     0,    78,
       0,    75,     0,     0,     0,    54,    55,    56,    57,    58,
      59,    60,    64,    63,    66,    65,    61,    62,    53,    67,
       0,     0,     0,    44,    45,     0,     5,    70,    33,     0,
      46,    38,     0,     0,     0,     0,     0,    39,     0,    77,
       0,    74,    34,    68,    41,    41,    79,    20,     0,     0,
       0,     7,     0,     0,     0,    82,     0,    28,     0,     5,
       0,     0,     0,    26,    15,    32,    84,    14,    81,     9,
      76,     0,    74,    51,    52,    27,    30,     0,    12,     0,
      31,     0,    16,     0,     0,     0,     0,     5,     0,     5,
       0,     0,     5,     0,     5,    29,     0,    13,     0,    25,
      24
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    20,    21,    22,    23,    24,    25,   126,    26,    27,
      28,   176,   177,    29,   182,   183,   129,    61,    30,    54,
      31,    32,   102,   138,    33,   134,   135
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -150
static const yytype_int16 yypact[] =
{
     374,  -150,  -150,    -4,     3,  -150,    18,   106,   402,  -150,
    -150,  -150,    19,    38,    43,    72,  -150,   402,   402,   402,
      81,   374,  -150,    83,    94,   402,    91,    92,    95,    12,
    -150,  -150,   420,    87,   402,   402,   402,   587,    77,     3,
     453,   100,   102,   102,  -150,    84,     5,     5,     5,  -150,
    -150,   109,    72,   486,    34,   717,   115,   402,   402,  -150,
     374,  -150,  -150,   402,   402,   402,   402,   402,   402,   402,
     402,   402,   402,   402,   402,   402,   402,   402,    72,  -150,
    -150,    48,   124,    58,    61,   622,  -150,   402,  -150,   128,
    -150,     9,    89,   402,   130,   104,  -150,  -150,   402,   130,
     657,   717,   131,   178,   689,   -12,   -12,   564,    -3,   394,
     769,   769,   788,   788,   788,   788,   125,   125,   745,  -150,
     132,   137,   149,  -150,  -150,   155,   374,     5,  -150,   107,
    -150,  -150,   519,   157,    17,   163,   402,   717,    65,  -150,
     173,   402,  -150,  -150,   402,   402,  -150,  -150,   206,   177,
     184,  -150,   102,   180,   181,  -150,   552,  -150,   130,   374,
     183,    71,    74,   186,  -150,  -150,    87,  -150,  -150,  -150,
    -150,   234,   402,  -150,  -150,  -150,    85,   185,  -150,   194,
    -150,   203,  -150,   182,   402,   202,   402,   374,   622,   374,
     622,   262,   374,   290,   374,  -150,   318,  -150,   346,  -150,
    -150
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -150,  -150,   -44,   -18,  -150,  -150,  -150,  -149,  -150,  -150,
    -150,  -150,    41,  -150,  -150,  -150,  -150,  -150,  -150,   -28,
      -6,    -7,  -136,  -150,    -5,  -150,   -85
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      37,    40,    38,    50,    63,   160,    83,    84,    34,    45,
      46,    47,    48,    63,   139,    35,   103,    53,    55,    66,
      67,    63,   133,    59,    41,    82,    60,    55,    55,    85,
      36,    78,   153,    79,    80,    81,   179,    91,    92,   192,
      78,   194,    79,    80,    81,   130,    95,    97,    78,   154,
     100,   101,    98,   120,   121,    43,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   123,   119,   170,   124,    42,    98,    44,   157,    98,
     127,    49,   148,   158,   173,    50,   132,   174,    51,    98,
      87,   137,    98,    82,   175,   180,    56,     1,     2,     3,
      39,     5,   131,    82,    57,    82,     7,    58,    90,     1,
       2,     3,     4,     5,    89,   171,   161,   162,     7,    13,
      14,    52,   149,    94,    17,   150,    93,    99,   122,   156,
      50,    13,    14,   128,   101,   133,    17,    55,    55,    18,
      19,    63,   141,   191,   144,   193,   136,   166,   196,   145,
     198,    18,    19,    50,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,   101,   146,    77,    78,   147,
      79,    80,    81,    50,   155,    50,   152,   188,    50,   190,
      50,     1,     2,     3,     4,     5,     6,   159,   164,   165,
       7,   167,   168,   142,   172,   175,   187,   184,     8,     9,
      10,    11,    12,    13,    14,    15,    16,   185,    17,     1,
       2,     3,     4,     5,     6,   186,   189,   181,     7,     0,
       0,   163,     0,    18,    19,     0,     8,     9,    10,    11,
      12,    13,    14,    15,    16,     0,    17,     1,     2,     3,
       4,     5,     6,     0,     0,     0,     7,     0,     0,   178,
       0,    18,    19,     0,     8,     9,    10,    11,    12,    13,
      14,    15,    16,     0,    17,     1,     2,     3,     4,     5,
       6,     0,     0,     0,     7,     0,     0,   195,     0,    18,
      19,     0,     8,     9,    10,    11,    12,    13,    14,    15,
      16,     0,    17,     1,     2,     3,     4,     5,     6,     0,
       0,     0,     7,     0,     0,   197,     0,    18,    19,     0,
       8,     9,    10,    11,    12,    13,    14,    15,    16,     0,
      17,     1,     2,     3,     4,     5,     6,     0,     0,     0,
       7,     0,     0,   199,     0,    18,    19,     0,     8,     9,
      10,    11,    12,    13,    14,    15,    16,     0,    17,     1,
       2,     3,     4,     5,     6,     0,     0,     0,     7,     0,
       0,   200,     0,    18,    19,     0,     8,     9,    10,    11,
      12,    13,    14,    15,    16,     0,    17,     1,     2,     3,
       4,     5,     6,     0,     0,     0,     7,     0,     0,     0,
       0,    18,    19,     0,     8,     9,    10,    11,    12,    13,
      14,    15,    16,     0,    17,     1,     2,     3,    39,     5,
      63,     0,     0,     0,     7,     0,     0,     0,     0,    18,
      19,     0,     0,    64,    65,    66,    67,    13,    14,     0,
       0,    62,    17,     0,     0,     0,    63,    78,     0,    79,
      80,    81,     0,     0,     0,     0,     0,    18,    19,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    88,    79,    80,    81,     0,    63,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    96,    79,    80,
      81,     0,    63,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
     151,    79,    80,    81,     0,    63,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,   169,    79,    80,    81,     0,    63,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    67,    79,    80,    81,
      86,     0,     0,    63,     0,     0,     0,    78,     0,    79,
      80,    81,     0,     0,     0,     0,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,     0,    79,    80,    81,   125,     0,     0,    63,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,     0,    79,    80,    81,
     140,     0,     0,    63,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,     0,    79,    80,    81,    63,   143,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    63,    79,    80,    81,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    63,    79,    80,    81,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    63,     0,     0,    78,     0,
      79,    80,    81,     0,     0,     0,     0,     0,    64,    65,
      66,    67,    68,     0,    63,    71,    72,    73,    74,     0,
       0,     0,    78,     0,    79,    80,    81,    64,    65,    66,
      67,    68,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    78,     0,    79,    80,    81
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-150)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       7,     8,     7,    21,    16,   141,    34,    35,    12,    15,
      17,    18,    19,    16,    99,    12,    60,    24,    25,    31,
      32,    16,     5,    11,     5,    16,    14,    34,    35,    36,
      12,    43,    15,    45,    46,    47,   172,    42,    43,   188,
      43,   190,    45,    46,    47,    36,    52,    13,    43,   134,
      57,    58,    18,     5,     6,    12,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    13,    78,   158,    13,    37,    18,     5,    13,    18,
      87,     0,   126,    18,    13,   103,    93,    13,     5,    18,
      13,    98,    18,    16,     9,    10,     5,     3,     4,     5,
       6,     7,    13,    16,    12,    16,    12,    12,     6,     3,
       4,     5,     6,     7,    14,   159,   144,   145,    12,    25,
      26,    27,    15,    14,    30,    18,    42,    12,     4,   136,
     148,    25,    26,     5,   141,     5,    30,   144,   145,    45,
      46,    16,    11,   187,    12,   189,    42,   152,   192,    12,
     194,    45,    46,   171,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,   172,    17,    42,    43,    14,
      45,    46,    47,   191,    11,   193,    19,   184,   196,   186,
     198,     3,     4,     5,     6,     7,     8,    14,    11,     5,
      12,    11,    11,    15,    11,     9,    14,    12,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    13,    30,     3,
       4,     5,     6,     7,     8,    12,    14,   176,    12,    -1,
      -1,    15,    -1,    45,    46,    -1,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,     3,     4,     5,
       6,     7,     8,    -1,    -1,    -1,    12,    -1,    -1,    15,
      -1,    45,    46,    -1,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    30,     3,     4,     5,     6,     7,
       8,    -1,    -1,    -1,    12,    -1,    -1,    15,    -1,    45,
      46,    -1,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    -1,    30,     3,     4,     5,     6,     7,     8,    -1,
      -1,    -1,    12,    -1,    -1,    15,    -1,    45,    46,    -1,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    -1,
      30,     3,     4,     5,     6,     7,     8,    -1,    -1,    -1,
      12,    -1,    -1,    15,    -1,    45,    46,    -1,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    -1,    30,     3,
       4,     5,     6,     7,     8,    -1,    -1,    -1,    12,    -1,
      -1,    15,    -1,    45,    46,    -1,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    -1,    30,     3,     4,     5,
       6,     7,     8,    -1,    -1,    -1,    12,    -1,    -1,    -1,
      -1,    45,    46,    -1,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    -1,    30,     3,     4,     5,     6,     7,
      16,    -1,    -1,    -1,    12,    -1,    -1,    -1,    -1,    45,
      46,    -1,    -1,    29,    30,    31,    32,    25,    26,    -1,
      -1,    11,    30,    -1,    -1,    -1,    16,    43,    -1,    45,
      46,    47,    -1,    -1,    -1,    -1,    -1,    45,    46,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    11,    45,    46,    47,    -1,    16,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    11,    45,    46,
      47,    -1,    16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      11,    45,    46,    47,    -1,    16,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    11,    45,    46,    47,    -1,    16,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      16,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    32,    45,    46,    47,
      13,    -1,    -1,    16,    -1,    -1,    -1,    43,    -1,    45,
      46,    47,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    -1,    45,    46,    47,    13,    -1,    -1,    16,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    -1,    45,    46,    47,
      13,    -1,    -1,    16,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    -1,    45,    46,    47,    16,    17,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    16,    45,    46,    47,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    16,    45,    46,    47,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    16,    -1,    -1,    43,    -1,
      45,    46,    47,    -1,    -1,    -1,    -1,    -1,    29,    30,
      31,    32,    33,    -1,    16,    36,    37,    38,    39,    -1,
      -1,    -1,    43,    -1,    45,    46,    47,    29,    30,    31,
      32,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    -1,    45,    46,    47
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,    12,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    30,    45,    46,
      54,    55,    56,    57,    58,    59,    61,    62,    63,    66,
      71,    73,    74,    77,    12,    12,    12,    74,    77,     6,
      74,     5,    37,    12,     5,    73,    74,    74,    74,     0,
      56,     5,    27,    74,    72,    74,     5,    12,    12,    11,
      14,    70,    11,    16,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    45,
      46,    47,    16,    72,    72,    74,    13,    13,    11,    14,
       6,    77,    77,    42,    14,    73,    11,    13,    18,    12,
      74,    74,    75,    55,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    73,
       5,     6,     4,    13,    13,    13,    60,    74,     5,    69,
      36,    13,    74,     5,    78,    79,    42,    74,    76,    79,
      13,    11,    15,    17,    12,    12,    17,    14,    55,    15,
      18,    11,    19,    15,    79,    11,    74,    13,    18,    14,
      75,    72,    72,    15,    11,     5,    77,    11,    11,    11,
      79,    55,    11,    13,    13,     9,    64,    65,    15,    75,
      10,    65,    67,    68,    12,    13,    12,    14,    74,    14,
      74,    55,    60,    55,    60,    15,    55,    15,    55,    15,
      15
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
#line 189 "bs.y"
    { BS_BUILD((yyval.vProgram), CreateProgram()); (yyval.vProgram)->SetStmtList((yyvsp[(1) - (1)].vStmtList)); }
    break;

  case 3:
/* Line 1792 of yacc.c  */
#line 192 "bs.y"
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
#line 201 "bs.y"
    { (yyval.vStmtList) = BS_GlobalBuilder->CreateStmtList(); (yyval.vStmtList)->SetStmt((yyvsp[(1) - (1)].vStmt)); }
    break;

  case 5:
/* Line 1792 of yacc.c  */
#line 202 "bs.y"
    { (yyval.vStmtList) = BS_GlobalBuilder->CreateStmtList(); }
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 205 "bs.y"
    { BS_BUILD((yyval.vStmt), BuildStmtExp((yyvsp[(1) - (2)].vExp))); }
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 206 "bs.y"
    { BS_BUILD((yyval.vStmt), BuildExternVariable((yyvsp[(2) - (5)].vExp),(yyvsp[(4) - (5)].vExp))); }
    break;

  case 8:
/* Line 1792 of yacc.c  */
#line 207 "bs.y"
    { BS_BUILD((yyval.vStmt), BuildDeclarationWithAnnotation((yyvsp[(1) - (3)].vAnnotations), (yyvsp[(2) - (3)].vExp))); }
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 209 "bs.y"
    {
                StmtExp* declaration = nullptr;
                BS_BUILD(declaration, BuildExternVariable((yyvsp[(3) - (6)].vExp), (yyvsp[(5) - (6)].vExp)));
                if (declaration != nullptr)
                {
                    BS_BUILD((yyval.vStmt), BuildDeclarationWithAnnotation((yyvsp[(1) - (6)].vAnnotations), declaration->GetExp()));
                }
                else
                {
                    (yyval.vStmt) = nullptr;
                    BS_parseerror("Syntax error. Invalid expression with annotation.");
                    YYERROR; 
                }
          }
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 223 "bs.y"
    { BS_BUILD((yyval.vStmt), BuildStmtReturn((yyvsp[(2) - (3)].vExp))); }
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 224 "bs.y"
    {BS_BUILD((yyval.vStmt), BindFunImplementation((yyvsp[(1) - (2)].vStmtFunDec), (yyvsp[(2) - (2)].vStmtList)));}
    break;

  case 12:
/* Line 1792 of yacc.c  */
#line 226 "bs.y"
    { 
               BS_BUILD((yyval.vStmt), BuildStmtWhile((yyvsp[(3) - (7)].vExp), (yyvsp[(6) - (7)].vStmtList)));
        }
    break;

  case 13:
/* Line 1792 of yacc.c  */
#line 230 "bs.y"
    {
               BS_BUILD((yyval.vStmt), BuildStmtFor((yyvsp[(3) - (11)].vExp),(yyvsp[(5) - (11)].vExp),(yyvsp[(7) - (11)].vExp),(yyvsp[(10) - (11)].vStmtList)));
        }
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 233 "bs.y"
    { BS_BUILD((yyval.vStmt), BuildStmtStructDef((yyvsp[(2) - (6)].identifierText), (yyvsp[(4) - (6)].vArgList))); }
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 235 "bs.y"
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

  case 16:
/* Line 1792 of yacc.c  */
#line 252 "bs.y"
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

  case 17:
/* Line 1792 of yacc.c  */
#line 272 "bs.y"
    { BS_BUILD((yyval.vFrameInfo), StartNewFrame()); }
    break;

  case 18:
/* Line 1792 of yacc.c  */
#line 275 "bs.y"
    {  BS_BUILD((yyval.vAnnotations), EndAnnotations((yyvsp[(1) - (3)].vAnnotations), (yyvsp[(2) - (3)].vExpList))); }
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 278 "bs.y"
    { BS_BUILD((yyval.vAnnotations), BeginAnnotations()); }
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 282 "bs.y"
    { BS_BUILD((yyval.vFrameInfo), StartNewFrame()); }
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 285 "bs.y"
    { (yyval.vTypeDesc) = (yyvsp[(1) - (1)].vTypeDesc); if ((yyval.vTypeDesc) == nullptr) { BS_parseerror("Syntax error. Invalid function type.");YYERROR; }; if (!BS_GlobalBuilder->StartNewFunction((yyval.vTypeDesc))) {BS_parseerror("cannot declare function within a function"); YYERROR;} }
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 288 "bs.y"
    { BS_GlobalBuilder->StartNewFrame(); }
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 291 "bs.y"
    { BS_GlobalBuilder->StartNewFrame(); }
    break;

  case 24:
/* Line 1792 of yacc.c  */
#line 295 "bs.y"
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

  case 25:
/* Line 1792 of yacc.c  */
#line 309 "bs.y"
    {
                        (yyval.vStmtIfElse) = BS_GlobalBuilder->BuildStmtIfElse((yyvsp[(3) - (6)].vExp), (yyvsp[(5) - (6)].vStmtList), nullptr, (yyvsp[(4) - (6)].vFrameInfo));
                   }
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 312 "bs.y"
    { (yyval.vStmtIfElse) = nullptr; }
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 315 "bs.y"
    { //pop previous frame
                              BS_GlobalBuilder->PopFrame();  
                            }
    break;

  case 28:
/* Line 1792 of yacc.c  */
#line 320 "bs.y"
    {BS_BUILD((yyval.vStmtFunDec), BuildStmtFunDec((yyvsp[(4) - (5)].vArgList), (yyvsp[(1) - (5)].vTypeDesc), (yyvsp[(2) - (5)].identifierText)));}
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 323 "bs.y"
    { BS_BUILD((yyval.vStmtIfElse), BuildStmtIfElse(nullptr, (yyvsp[(3) - (4)].vStmtList), nullptr, (yyvsp[(1) - (4)].vFrameInfo))); }
    break;

  case 30:
/* Line 1792 of yacc.c  */
#line 324 "bs.y"
    { (yyval.vStmtIfElse) = nullptr; }
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 327 "bs.y"
    { BS_GlobalBuilder->PopFrame(); BS_BUILD((yyval.vFrameInfo), StartNewFrame()); }
    break;

  case 32:
/* Line 1792 of yacc.c  */
#line 330 "bs.y"
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

  case 33:
/* Line 1792 of yacc.c  */
#line 339 "bs.y"
    {
                Pegasus::BlockScript::EnumNode* enumNode = BS_GlobalBuilder->GetSymbolTable()->NewEnumNode(); 
                enumNode->mIdd = (yyvsp[(1) - (1)].identifierText);
                (yyval.vEnumNode) = enumNode;
          }
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 346 "bs.y"
    { (yyval.vStmtList) = (yyvsp[(2) - (3)].vStmtList); }
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 347 "bs.y"
    { (yyval.vStmtList) = nullptr; }
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 350 "bs.y"
    { BS_BUILD((yyval.vExp), BuildImmInt((yyvsp[(1) - (1)].integerValue))); }
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 351 "bs.y"
    { BS_BUILD((yyval.vExp), BuildImmFloat((yyvsp[(1) - (1)].floatValue))); }
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 353 "bs.y"
    {
            //figure out size at compile time!
            BS_BUILD((yyval.vExp), BuildImmInt((yyvsp[(3) - (4)].vTypeDesc)->GetByteSize()));
          }
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 359 "bs.y"
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

  case 40:
/* Line 1792 of yacc.c  */
#line 368 "bs.y"
    { (yyval.vExpList) = BS_GlobalBuilder->CreateExpList(); (yyval.vExpList)->SetExp((yyvsp[(1) - (1)].vExp)); }
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 369 "bs.y"
    { (yyval.vExpList) = BS_GlobalBuilder->CreateExpList(); }
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 372 "bs.y"
    { BS_BUILD((yyval.vExp), BuildIdd((yyvsp[(1) - (1)].identifierText))); }
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 375 "bs.y"
    { (yyval.vExp) = (yyvsp[(1) - (1)].vExp); }
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 376 "bs.y"
    { BS_BUILD((yyval.vExp), BuildFunCall((yyvsp[(3) - (4)].vExpList), (yyvsp[(1) - (4)].identifierText))); }
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 377 "bs.y"
    { BS_BUILD((yyval.vExp), BuildFunCall((yyvsp[(3) - (4)].vExpList), (yyvsp[(1) - (4)].identifierText))); }
    break;

  case 46:
/* Line 1792 of yacc.c  */
#line 378 "bs.y"
    { BS_BUILD((yyval.vExp), BuildStaticArrayDec((yyvsp[(3) - (4)].vTypeDesc))); }
    break;

  case 47:
/* Line 1792 of yacc.c  */
#line 379 "bs.y"
    { BS_BUILD((yyval.vExp), BuildStrImm((yyvsp[(1) - (1)].identifierText))); }
    break;

  case 48:
/* Line 1792 of yacc.c  */
#line 380 "bs.y"
    { (yyval.vExp) = (yyvsp[(1) - (1)].vExp); }
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 381 "bs.y"
    { BS_BUILD((yyval.vExp), BuildUnop((yyvsp[(1) - (2)].token), (yyvsp[(2) - (2)].vExp))); }
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 382 "bs.y"
    { BS_BUILD((yyval.vExp), BuildUnop((yyvsp[(1) - (2)].token), (yyvsp[(2) - (2)].vExp))); }
    break;

  case 51:
/* Line 1792 of yacc.c  */
#line 383 "bs.y"
    { BS_BUILD((yyval.vExp), BuildMethodCall((yyvsp[(1) - (6)].vExp), (yyvsp[(3) - (6)].identifierText), (yyvsp[(5) - (6)].vExpList))); }
    break;

  case 52:
/* Line 1792 of yacc.c  */
#line 384 "bs.y"
    { BS_BUILD((yyval.vExp), BuildMethodCall((yyvsp[(1) - (6)].vExp), (yyvsp[(3) - (6)].identifierText), (yyvsp[(5) - (6)].vExpList))); }
    break;

  case 53:
/* Line 1792 of yacc.c  */
#line 385 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 386 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 387 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 56:
/* Line 1792 of yacc.c  */
#line 388 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 389 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 390 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 391 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 60:
/* Line 1792 of yacc.c  */
#line 392 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 61:
/* Line 1792 of yacc.c  */
#line 393 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 62:
/* Line 1792 of yacc.c  */
#line 394 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 63:
/* Line 1792 of yacc.c  */
#line 395 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 64:
/* Line 1792 of yacc.c  */
#line 396 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 65:
/* Line 1792 of yacc.c  */
#line 397 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 66:
/* Line 1792 of yacc.c  */
#line 398 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 67:
/* Line 1792 of yacc.c  */
#line 399 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (3)].vExp), (yyvsp[(2) - (3)].token), (yyvsp[(3) - (3)].vExp))); }
    break;

  case 68:
/* Line 1792 of yacc.c  */
#line 400 "bs.y"
    { BS_BUILD((yyval.vExp), BuildBinop((yyvsp[(1) - (4)].vExp), O_ACCESS, (yyvsp[(3) - (4)].vExp))); }
    break;

  case 69:
/* Line 1792 of yacc.c  */
#line 401 "bs.y"
    { BS_BUILD((yyval.vExp), BuildUnop((yyvsp[(1) - (2)].token), (yyvsp[(2) - (2)].vExp))); }
    break;

  case 70:
/* Line 1792 of yacc.c  */
#line 402 "bs.y"
    { BS_BUILD((yyval.vExp), BuildExplicitCast((yyvsp[(4) - (4)].vExp), (yyvsp[(2) - (4)].vTypeDesc))); }
    break;

  case 71:
/* Line 1792 of yacc.c  */
#line 403 "bs.y"
    { (yyval.vExp) = (yyvsp[(2) - (3)].vExp); }
    break;

  case 72:
/* Line 1792 of yacc.c  */
#line 404 "bs.y"
    { BS_BUILD((yyval.vExp), BuildUnopPost((yyvsp[(1) - (2)].vExp), (yyvsp[(2) - (2)].token))); }
    break;

  case 73:
/* Line 1792 of yacc.c  */
#line 405 "bs.y"
    { BS_BUILD((yyval.vExp), BuildUnopPost((yyvsp[(1) - (2)].vExp), (yyvsp[(2) - (2)].token))); }
    break;

  case 74:
/* Line 1792 of yacc.c  */
#line 408 "bs.y"
    { (yyval.vExp) = nullptr; }
    break;

  case 75:
/* Line 1792 of yacc.c  */
#line 409 "bs.y"
    { (yyval.vExp) = (yyvsp[(1) - (1)].vExp); }
    break;

  case 76:
/* Line 1792 of yacc.c  */
#line 412 "bs.y"
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

  case 77:
/* Line 1792 of yacc.c  */
#line 421 "bs.y"
    { (yyval.vArgList) = BS_GlobalBuilder->CreateArgList(); (yyval.vArgList)->SetArgDec((yyvsp[(1) - (1)].vArgDec)); }
    break;

  case 78:
/* Line 1792 of yacc.c  */
#line 422 "bs.y"
    { (yyval.vArgList) = BS_GlobalBuilder->CreateArgList(); }
    break;

  case 79:
/* Line 1792 of yacc.c  */
#line 425 "bs.y"
    { 
				Pegasus::BlockScript::TypeDesc* resultType = nullptr;
				if ((yyvsp[(1) - (4)].vTypeDesc)->GetModifier() != Pegasus::BlockScript::TypeDesc::M_ARRAY)
				{
					(yyvsp[(1) - (4)].vTypeDesc)->ComputeSize();
					resultType = BS_GlobalBuilder->GetSymbolTable()->CreateArrayType(
					    (yyvsp[(1) - (4)].vTypeDesc)->GetName(), //name
					    (yyvsp[(1) - (4)].vTypeDesc),  // child type
					    (yyvsp[(3) - (4)].integerValue)   //array count
					);
				}        
				else
				{
					Pegasus::BlockScript::TypeDesc* target = (yyvsp[(1) - (4)].vTypeDesc);
					while (target->GetChild()->GetModifier() == Pegasus::BlockScript::TypeDesc::M_ARRAY)
					{
						target = target->GetChild();
					}
					Pegasus::BlockScript::TypeDesc* tmp = target->GetChild();
					resultType = BS_GlobalBuilder->GetSymbolTable()->CreateArrayType(
						(yyvsp[(1) - (4)].vTypeDesc)->GetName(),
						tmp,
						(yyvsp[(3) - (4)].integerValue)
					);
					target->SetChild(resultType);
                    resultType = (yyvsp[(1) - (4)].vTypeDesc);
					(yyvsp[(1) - (4)].vTypeDesc)->ComputeSize();
				}
                
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

  case 80:
/* Line 1792 of yacc.c  */
#line 464 "bs.y"
    {                
                TypeDesc* typeDesc = BS_GlobalBuilder->GetTypeByName((yyvsp[(1) - (1)].identifierText));
                if (typeDesc != nullptr)
                {
					typeDesc->ComputeSize();
                    (yyval.vTypeDesc) = typeDesc;
                }
                else
                {
                    BS_parseerror("Semantics error, unknown type specified.");
                    YYERROR;
                }
            }
    break;

  case 81:
/* Line 1792 of yacc.c  */
#line 479 "bs.y"
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

  case 82:
/* Line 1792 of yacc.c  */
#line 488 "bs.y"
    { (yyval.vArgList) = BS_GlobalBuilder->CreateArgList(); (yyval.vArgList)->SetArgDec((yyvsp[(1) - (2)].vArgDec)); }
    break;

  case 83:
/* Line 1792 of yacc.c  */
#line 489 "bs.y"
    { (yyval.vArgList) = BS_GlobalBuilder->CreateArgList(); }
    break;

  case 84:
/* Line 1792 of yacc.c  */
#line 492 "bs.y"
    { BS_BUILD((yyval.vArgDec), BuildArgDec((yyvsp[(1) - (3)].identifierText), (yyvsp[(3) - (3)].vTypeDesc))); }
    break;


/* Line 1792 of yacc.c  */
#line 2448 "bs.parser.cpp"
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
#line 495 "bs.y"
         

//***************************************************//
//              Let the insanity end                 //
//***************************************************//

extern void BS_restart(FILE* f);


void Bison_BlockScriptParse(const FileBuffer* fileBuffer, BlockScriptBuilder* builder, IFileIncluder* fileIncluder, Container<Preprocessor::Definition>* defList) 
{          
    CompilerState compilerState(builder->GetAllocator());
    compilerState.mBuilder = builder;
    compilerState.mFileBuffer = fileBuffer;
    compilerState.GetPreprocessor().SetFileIncluder(fileIncluder);

    //add definitions pre-added
    if (defList != nullptr)
    {
        for (int i = 0; i < defList->Size(); ++i)
        {
            compilerState.GetPreprocessor().InsertDefinition((*defList)[i]);
        }
    }

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

