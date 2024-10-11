%{
    #include<stdio.h>
    int mayusculas = 0;
    int minusculas = 0;
    int especial = 0;
    int numero = 0;
%}

mayusculas [A-Z]
minusculas [a-z]
especial [$#@%&]
numero [0-9]

%%

{mayusculas} { mayusculas++;}
{minusculas} { minusculas++; }
{especial} { especial++; }
{numero} { numero++; }

\n  {  if(mayusculas == 3 && minusculas == 2 && especial == 2 && numero == 1){
            printf("Contraseña válida\n");
        }
        else{
            printf("Contraseña inválida\n");
        }
        mayusculas = 0;
        minusculas = 0;
        especial = 0;
        numero = 0;
    }
\t { }
. { }

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
}
