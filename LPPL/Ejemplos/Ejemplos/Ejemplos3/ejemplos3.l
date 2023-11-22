%{

    #include<stdio.h>
    #include<string.h>
    #include "tabla.h"
    
    int linea_actual=0 ;
	
%}


letra      [a-zA-Z]
digito     [0-9]
alphanum   [a-zA-Z_0-9]
blanco     [ \t]
otros      (@|&)



%%



"("                  return PIZ;
")"                  return PDE;
"+"                  return SUM;
"-"                  return SUB;
"*"                  return MUL;
"/"                  return DIV;
":="                 return ASI;
{letra}{alphanum}*   return ID;
{digito}+            return ENT;
{blanco}+            return BL;
\n                   ++linea_actual ;
{otros}              printf("\n(Linea %d) Error lexico: token %s\n", linea_actual, yytext);



%%


void yyerror( const char *msg )
{
    fprintf( stderr, "error %s\n", msg );
}

int yywrap( )
{
    return 1;
}
