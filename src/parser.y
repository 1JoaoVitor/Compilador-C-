%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "util.h"

extern int line_num;
extern int yylex();
extern char* yytext;
TreeNode * savedTree;

void yyerror(const char *s);

TreeNode * addSibling(TreeNode * t, TreeNode * s) {
    if (t != NULL) {
        TreeNode * p = t;
        while (p->sibling != NULL) p = p->sibling;
        p->sibling = s;
        return t;
    }
    return s;
}
%}

%union {
    TreeNode * node;
    char * string;
    int val;
    int op; // token
}

%define parse.error verbose


%token IF ELSE INT RETURN VOID WHILE
%token SOMA SUB MULT DIV
%token MENOR MENORIGUAL MAIOR MAIORIGUAL IGUAL DIFERENTE ATRIB
%token PONTOVIRGULA VIRGULA APAREN FPAREN ACOLCH FCOLCH ACHAVE FCHAVE


%token <string> ID
%token <val> NUM


%type <node> declaration_list declaration var_declaration fun_declaration
%type <node> params param_list param compound_stmt local_declarations statement_list
%type <node> statement expression_stmt selection_stmt iteration_stmt return_stmt
%type <node> expression var simple_expression additive_expression term factor call args arg_list
%type <op> relop addop mulop type_specifier

/* Cria uma regra para resolver a ambiguidade de IF, ideia da "maior igualdade"/ "maior caminho" */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%





/* CFG de fato */

programa: declaration_list
        { savedTree = $1; }
        ;

declaration_list: declaration_list declaration
                { $$ = addSibling($1, $2); }
                | declaration
                { $$ = $1; }
                ;

declaration: var_declaration { $$ = $1; }
           | fun_declaration { $$ = $1; }
           ;

var_declaration: type_specifier ID PONTOVIRGULA
               {
                   $$ = newDecNode(VarK);
                   $$->type = ($1 == INT) ? Integer : Void;
                   $$->attr.name = $2; 
               }
               | type_specifier ID ACOLCH NUM FCOLCH PONTOVIRGULA
               {
                   $$ = newDecNode(VarK);
                   $$->type = ($1 == INT) ? Integer : Void;
                   $$->attr.name = $2;
                   TreeNode * t = newExpNode(ConstK);
                   t->attr.val = $4;
                   $$->child[0] = t; 
               }
               | error PONTOVIRGULA
               {
                   yyerrok;
                   $$ = NULL;
               }
               ;

type_specifier: INT { $$ = INT; }
              | VOID { $$ = VOID; }
              ;

fun_declaration: type_specifier ID APAREN params FPAREN compound_stmt
               {
                   $$ = newDecNode(FunK);
                   $$->type = ($1 == INT) ? Integer : Void;
                   $$->attr.name = $2;
                   $$->child[0] = $4; 
                   $$->child[1] = $6;
               }
               ;

params: param_list { $$ = $1; }
      | VOID { $$ = NULL; }
      ;

param_list: param_list VIRGULA param
          { $$ = addSibling($1, $3); }
          | param
          { $$ = $1; }
          ;

param: type_specifier ID
     {
         $$ = newDecNode(ParamK);
         $$->type = ($1 == INT) ? Integer : Void;
         $$->attr.name = $2;
     }
     | type_specifier ID ACOLCH FCOLCH
     {
         $$ = newDecNode(ParamK);
         $$->type = ($1 == INT) ? Integer : Void; 
         $$->attr.name = $2;
     }
     ;

compound_stmt: ACHAVE local_declarations statement_list FCHAVE
             {
                 $$ = newStmtNode(CompoundK);
                 $$->child[0] = $2; 
                 $$->child[1] = $3; 
             }
             ;

local_declarations: local_declarations var_declaration
                  { $$ = addSibling($1, $2); }
                  | // nada
                  { $$ = NULL; }
                  ;

statement_list: statement_list statement
              { $$ = addSibling($1, $2); }
              | // nada
              { $$ = NULL; }
              ;

statement: expression_stmt { $$ = $1; }
         | compound_stmt { $$ = $1; }
         | selection_stmt { $$ = $1; }
         | iteration_stmt { $$ = $1; }
         | return_stmt { $$ = $1; }
         ;

expression_stmt: expression PONTOVIRGULA { $$ = $1; }
               | PONTOVIRGULA { $$ = NULL; }
               | error PONTOVIRGULA
               {
                   yyerrok;
                   $$ = NULL;
               }
               ;

selection_stmt: IF APAREN expression FPAREN statement %prec LOWER_THAN_ELSE
              {
                  $$ = newStmtNode(IfK);
                  $$->child[0] = $3; 
                  $$->child[1] = $5; 
              }
              | IF APAREN expression FPAREN statement ELSE statement
              {
                  $$ = newStmtNode(IfK);
                  $$->child[0] = $3; 
                  $$->child[1] = $5; 
                  $$->child[2] = $7; 
              }
              ;

iteration_stmt: WHILE APAREN expression FPAREN statement
              {
                  $$ = newStmtNode(WhileK);
                  $$->child[0] = $3; 
                  $$->child[1] = $5; 
              }
              ;

return_stmt: RETURN PONTOVIRGULA
           { $$ = newStmtNode(ReturnK); }
           | RETURN expression PONTOVIRGULA
           {
               $$ = newStmtNode(ReturnK);
               $$->child[0] = $2; 
           }
           ;

expression: var ATRIB expression
          {
              $$ = newExpNode(OpK);
              $$->attr.op = ATRIB;
              $$->child[0] = $1; 
              $$->child[1] = $3; 
          }
          | simple_expression { $$ = $1; }
          ;

var: ID
   {
       $$ = newExpNode(IdK);
       $$->attr.name = $1;
   }
   | ID ACOLCH expression FCOLCH
   {
       $$ = newExpNode(IdK);
       $$->attr.name = $1;
       $$->child[0] = $3; 
   }
   ;

simple_expression: additive_expression relop additive_expression
                 {
                     $$ = newExpNode(OpK);
                     $$->attr.op = $2;
                     $$->child[0] = $1;
                     $$->child[1] = $3;
                 }
                 | additive_expression { $$ = $1; }
                 ;

relop: MENORIGUAL { $$ = MENORIGUAL; }
     | MENOR { $$ = MENOR; }
     | MAIOR { $$ = MAIOR; }
     | MAIORIGUAL { $$ = MAIORIGUAL; }
     | IGUAL { $$ = IGUAL; }
     | DIFERENTE { $$ = DIFERENTE; }
     ;

additive_expression: additive_expression addop term
                   {
                       $$ = newExpNode(OpK);
                       $$->attr.op = $2;
                       $$->child[0] = $1;
                       $$->child[1] = $3;
                   }
                   | term { $$ = $1; }
                   ;

addop: SOMA { $$ = SOMA; }
     | SUB { $$ = SUB; }
     ;

term: term mulop factor
    {
        $$ = newExpNode(OpK);
        $$->attr.op = $2;
        $$->child[0] = $1;
        $$->child[1] = $3;
    }
    | factor { $$ = $1; }
    ;

mulop: MULT { $$ = MULT; }
     | DIV { $$ = DIV; }
     ;

factor: APAREN expression FPAREN { $$ = $2; }
      | var { $$ = $1; }
      | call { $$ = $1; }
      | NUM
      {
          $$ = newExpNode(ConstK);
          $$->attr.val = $1;
      }
      ;

call: ID APAREN args FPAREN
    {
        $$ = newExpNode(CallK);
        $$->attr.name = $1;
        $$->child[0] = $3; 
    }
    ;

args: arg_list { $$ = $1; }
    | { $$ = NULL; }
    ;

arg_list: arg_list VIRGULA expression
        { $$ = addSibling($1, $3); }
        | expression
        { $$ = $1; }
        ;

%%

void yyerror(const char *s) {
    printf("ERRO SINTATICO: %s LINHA: %d\n", s, line_num);
}