%{
#include "y.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

extern int yylval;
extern char *chkexpstr;
extern int opt;
extern int parentheses;

#ifdef USE_FLEX
    #undef YY_INPUT
    #define YY_INPUT(buf,result,max_size) \
                { \
                int c = '*', n; \
                for ( n = 0; n < max_size && \
                             (c = toupper(*(chkexpstr++))) != '\0'; ++n ) \
                        buf[n] = (char) c; \
                result = n; \
                } 
#else
    #undef input
    #undef unput
    #define input() (toupper(*(chkexpstr++)))  
    #define unput(c) (*(--chkexpstr)=c) 
#endif 

%}

%%
A[0-9]+    {
              yylval = atoi (yytext+1); 
              return NUMBER;
          }

\!       {
              opt = NOT;
              return NOT;
          }

\&\&       {
              opt = AND;
              return AND;
          }

\|\|        {
              opt = OR;
              return OR;
          }

\(         {
              parentheses += 1;
              return '('; 
          }

\)         {
              parentheses -= 1;
              return ')'; 
          }

[ \t] ;
.         {
             return (yytext[0]);
          }
%%
