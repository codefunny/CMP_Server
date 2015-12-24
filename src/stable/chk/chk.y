%{
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

extern int yylex(void);
extern void yyerror(char *msg);

extern int opt;
extern int parentheses;
extern char * chkexpstr;
extern int CHK_ExprCalc (int iExprId);

static int ign = 0;
%}

%token NUMBER NOT AND OR
%left OR
%left AND
%left NOT
%nonassoc UMINUS

%%
statment: expression    
            {
                return ($1 << 4);
            }
expression: expression OR { ign = $1 ? 1 : ign; } expression 
            { 
                if (ign) {
                    $$ = 1;
                } else {
                    $$ = $1 || $4; 
                }
                ign = 0;
            } 
      |     expression AND { ign = $1 ? ign : 1; } expression 
            { 
                if (ign) {
                    $$ = 0;
                } else {
                    $$ = $1 && $4; 
                }
                ign = 0;
            } 
      |     NOT expression            
            {
                $$ = !$2;
            }
      |     '(' expression ')'        
            {
                $$ = $2; 
            }
      |     NUMBER                    
            {
                if (!ign) {
                    $$ = CHK_ExprCalc ($1);
                } else {
                    $$ = 0;
                }
            }
      ;
%%

void yyerror(char* msg)
{
      printf("error: %s encountered \n", msg);
}

/***
main (int argc, char *argv[])
{
    int i;
    for (i = 1; i < argc ; i++) {
        opt = 0;
        parentheses = 0;
        chkexpstr = argv[i]; 
        printf ("ret %d\n", yyparse());
        //printf ("ret %d\n", yylex());
    }
}
***/
