%{

    #include<stdio.h>
	#include<stdlib.h>
	#include<string.h>
	
	
%}


digito   [0-9]
letra    [a-zA-Z]



%%


  /* NUMEROS */

[+-]?{digito}+                                     { printf( "<Integer, %d>\n",    atoi(yytext) ); }
[+-]?{digito}+(.{digito}+)?                        { printf( "<Float, %f>\n",      atof(yytext) ); }
[+-]?{digito}+(.{digito}+)?([eE][+-]?{digito}+)?   { printf( "<Scientific, %f>\n", atof(yytext) ); }


  /* PALABRAS RESERVADAS */

if|then|while|for|"="                              { printf( "<PR, %s>\n", yytext ); }


  /* IDENTIFICADORES */

{letra}({letra}|{digito})*                         { printf( "<Id, %s>\n", yytext ); }


  /* OPERADORES */

"+"|"-"|"*"|"/"                                    { printf( "<OP, %s>\n", yytext ); }


  /* OPERADORES RELACIONALES */
  
"<"|"<="|">"|">="|"<>"                             { printf( "<OP_REL, %s>\n", yytext ); }


  /* IGNORAR ESPACIOS EN BLANCO, TABULADORES Y SALTOS DE LINEA */

[ \t\n]                           { }



  /* DNI */
  
[0-9A-Z]{digito}{7}[A-Z]             { printf("dni: %s\n", yytext); }

  /* EMAIL */
  
([a-zA-Z0-9%_.+\-]+)@([a-zA-Z0-9.\-]+?\.[a-zA-Z]{2,6})  { printf("email: %s\n", yytext); }

  /* TARJETA DE CREDITO */
  
{digito}{4}([ -]?{digito}{4}){3}  { printf("tarjeta: %s\n", yytext); }

  /* MATRICULA DE COCHE */
  
(([A-Z]{1,2})([0-9]{4})([A-Z]{0,2}))|((E)?([0-9]{4})([A-Z]{3}))     { printf("matricula: %s\n", yytext); }

  /* FECHA */

(19|20)[0-9][0-9][-/.](0[1-9]|1[012])[-/.](0[1-9]|[12][0-9]|3[01])  { printf("fecha: %s\n", yytext); }

  /* DIRECCION WEB */

(http:[/][/]|www.)([a-z]|[A-Z]|[0-9]|[/.]|[~])*   { printf("url: %s\n", yytext); }

  /* IP */

(([1-9]?[0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]).){3}([1-9]?[0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])  { printf("ip: %s\n", yytext); }



  /* OTROS CARACTERES */

.                                 { printf( "Otro caracter: %s\n", yytext ); }



%%



void yyerror( const char *msg )
{
    fprintf( stderr, "error %s\n", msg );
}


int yywrap( )
{
    return 1;
}


int main(int argc, char *argv[]) {

  int analizador_lexico = yylex();
    
  while (analizador_lexico != 0)
  {
        
    printf(" %d\n", analizador_lexico);
    analizador_lexico = yylex();
        
  }
    
  return analizador_lexico;

}
