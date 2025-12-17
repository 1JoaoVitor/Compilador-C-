/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SRC_PARSER_TAB_H_INCLUDED
# define YY_YY_SRC_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    IF = 258,                      /* IF  */
    ELSE = 259,                    /* ELSE  */
    INT = 260,                     /* INT  */
    RETURN = 261,                  /* RETURN  */
    VOID = 262,                    /* VOID  */
    WHILE = 263,                   /* WHILE  */
    SOMA = 264,                    /* SOMA  */
    SUB = 265,                     /* SUB  */
    MULT = 266,                    /* MULT  */
    DIV = 267,                     /* DIV  */
    MENOR = 268,                   /* MENOR  */
    MENORIGUAL = 269,              /* MENORIGUAL  */
    MAIOR = 270,                   /* MAIOR  */
    MAIORIGUAL = 271,              /* MAIORIGUAL  */
    IGUAL = 272,                   /* IGUAL  */
    DIFERENTE = 273,               /* DIFERENTE  */
    ATRIB = 274,                   /* ATRIB  */
    PONTOVIRGULA = 275,            /* PONTOVIRGULA  */
    VIRGULA = 276,                 /* VIRGULA  */
    APAREN = 277,                  /* APAREN  */
    FPAREN = 278,                  /* FPAREN  */
    ACOLCH = 279,                  /* ACOLCH  */
    FCOLCH = 280,                  /* FCOLCH  */
    ACHAVE = 281,                  /* ACHAVE  */
    FCHAVE = 282,                  /* FCHAVE  */
    ID = 283,                      /* ID  */
    NUM = 284,                     /* NUM  */
    LOWER_THAN_ELSE = 285          /* LOWER_THAN_ELSE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 26 "src/parser.y"

    TreeNode * node;
    char * string;
    int val;
    int op; // token

#line 101 "src/parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SRC_PARSER_TAB_H_INCLUDED  */
