%{

    #include<stdio.h>
    #include<stdlib.h>
    #include<string.h>
    #include"tablasimbolos.h"

	
    char id ;
	
	
%}


digito   [0-9]
letra    [a-zA-Z]



%%


  /* NUMEROS */

[+-]?{digito}+                                     { printf( "<Integer, %d>\n",    atoi(yytext) ); insertarTS(id, atoi(yytext), "int" ) }
[+-]?{digito}+(.{digito}+)?                        { printf( "<Float, %f>\n",      atof(yytext) ); insertarTS(id, atoi(yytext), "float" ) }
[+-]?{digito}+(.{digito}+)?([eE][+-]?{digito}+)?   { printf( "<Scientific, %f>\n", atof(yytext) ); insertarTS(id, atoi(yytext), "scientific" ) }


  /* PALABRAS RESERVADAS */

if|then|while|for|siendo|evalua                { printf( "<palabra_reservada, %s>\n", yytext ); }


  /* IDENTIFICADORES */

{letra}({letra}|{digito})*                         { printf( "<identificador, %s>\n", yytext ); 
						                                         id = yytext ;
						                                        }



  /* OPERADORES */

"+"|"-"|"*"|"/"                                    { printf( "<operador_aritmético, %s>\n", yytext ); }


  /* OPERADORES RELACIONALES */
  
"="|"<"|"<="|">"|">="|"<>"                             { printf( "<operador_asignación, %s>\n", yytext ); }


 /* SEPARADORES */

","                                                { printf( "<separador, %s>\n", yytext ); }

 /* PARÉNTESIS */

"("                                                { printf( "<paréntesis_abierto, %s>\n", yytext ); }

")"                                                { printf( "<paréntesis_cerrado, %s>\n", yytext ); }


  /* IGNORAR ESPACIOS EN BLANCO, TABULADORES Y SALTOS DE LINEA */

[ \t\n]                           { }


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

    mostrarTS();

}