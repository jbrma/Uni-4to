#include <stdio.h>


extern int yylex();


int main( int argc, const char * argv[] )
{
    
    int analizador_lexico = yylex();
    
    while (analizador_lexico != 0)
    {
        
        printf(" %d\n", analizador_lexico);
        analizador_lexico = yylex();
        
    }
    
    return analizador_lexico;
    
}
