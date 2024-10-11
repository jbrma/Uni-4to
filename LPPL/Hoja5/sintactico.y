%{

   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>
   
   extern int  yylex(void);
   extern char *yytext;
   int yyerror(char *s);

  
%}
             
%union
{

  char* nombre;

}

%start S

%token <nombre> ID
%token COMA
%token PUNTOCOMA
%token ENTERO
%token REAL
%token CARAC

%type <nombre> T

%%

S : T ID { printf("var %s", $2); } L { printf(": %s", $1);} PUNTOCOMA S
  |
  ;

L : COMA ID    { printf(", %s", $2); } L
  |            
  ;

T : ENTERO      { $$ = "integer;\n"; }
  | REAL        { $$ = "real;\n"; }
  | CARAC       { $$ = "char;\n"; }
  ;

%%


int main() {

  yyparse();

}
   

int yyerror(char *s)
{
  
  printf("Error %s", s);

}

             
int yywrap()  
{  

  return 1;  

}  
