%{
    #include<stdio.h>
    #include<stdlib.h>
    #include<string.h>
    
%}

NUM    [0-9]
ID     [_a-zA-Z][a-zA-Z0-9_]*

%%

{NUM}+      { printf("numero encontrado: %s\n", yytext); }

{ID}        { printf("identificador encontrado: %s\n", yytext); }


%%

void yyerror( const char *msg )
{
    fprintf( stderr, "error %s\n", msg );
}

int yywrap( )
{
    return 1;
}

