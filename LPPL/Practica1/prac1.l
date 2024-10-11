%{

    #include<stdio.h>
    #include<stdlib.h>
    #include<string.h>
    #include"tablasimbolos.h"

    int siendo ;
    char *id ;
	
	
%}


digito   [0-9]
letra    [a-zA-Z]



%%


  /* NUMEROS */

[+-]?{digito}+                                     { printf( "<Integer, %d>\n",    atoi(yytext) ); 
                                                     if (siendo == 1) insertarTS(id, atof(yytext), "int" ); 
                                                   }
[+-]?{digito}+(.{digito}+)?                        { printf( "<Float, %f>\n",      atof(yytext) );
						     if (siendo == 1) insertarTS(id, atof(yytext), "float" ); 
 					           }
[+-]?{digito}+(.{digito}+)?([eE][+-]?{digito}+)?   { printf( "<Scientific, %f>\n", atof(yytext) );
						     if (siendo == 1) insertarTS(id, atof(yytext), "scientific" ); 
						   }


  /* PALABRAS RESERVADAS */

if|then|while|for                                  { printf( "<palabra_reservada, %s>\n", yytext ); }
siendo                                             { printf( "<palabra_reservada, %s>\n", yytext ); siendo = 1 ; }
evalua                                             { printf( "<palabra_reservada, %s>\n", yytext ); siendo = 0 ; }


  /* IDENTIFICADORES */

{letra}({letra}|{digito})*                         { printf( "<identificador, %s>\n", yytext ); 
						     id = (char *) malloc(32); 
                                                     strcpy(id, yytext);
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

.                                 { }



%%



void yyerror( const char *msg )
{
    fprintf( stderr, "error %s\n", msg );
}


int yywrap( )
{
    return 1;
}


int main( int argc, char **argv ) 
{ 
     ++argv, --argc;  /* se salta el nombre del programa */
    if ( argc > 0 )
            yyin = fopen( argv[0], "r" );
    else
            yyin = stdin;

    printf("\n");
    
    yylex();

    printf("\n");

    mostrarTS();
}