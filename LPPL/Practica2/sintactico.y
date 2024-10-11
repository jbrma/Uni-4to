%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    
    #include "tablasimbolos.h"
    
    extern int  yylex(void);
    extern char *yytext;

    /* Here go the variables I need
    char* id;
    */
    char* id;
    char* id1;
    char* id2;

   int yyerror(char *s);
%}

%union {
    float numero_real;
    char* identificador;
    int condition;
}

%start S;

%token SIEN EVAL IF THEN ELSE LEQ GEQ EQI NEQ EQU ADD SUB MULT DIV SEP PO PC CPO CPC LT GT SWITCH CSE DFLT PCOMA DP;
%token <numero_real> INT REAL EXP;
%token <identificador> ID;

%type <condition> cond;
%type <numero_real> DECL E0 E1 E2;

%left ADD SUB;
%left MULT DIV;

%%

S     : EVAL DECL                  { printf("El resultado de evaluar el programa es %.2f\n", $2); } 
      | SIEN LDs EVAL DECL               { printf("El resultado de evaluar el programa es %.2f\n", $4); }
      ;

LDs   : LDs SEP D
      | D
      ;


DECL  : IF PO cond PC THEN CPO DECL CPC ELSE CPO DECL CPC         { if($3) $$ = $7;
                                                           else $$ = $11;} 
      | SWITCH PO ID                    {     id = NULL;
                                          id = (char*)malloc(32);
                                          sprintf(id, "%s", $3); }
        PC CPO CSE DECL DP DECL PCOMA CSE DECL DP DECL PCOMA DFLT DP DECL PCOMA CPC 
                                          { float num = buscarTS(id);
                                            float num1 = $8;
                                            float toreturn1 = $10;
                                            float num2 = $13;
                                            float toreturn2 = $15;
                                            float toreturn3 = $19;

                                            if(num == num1) $$ = toreturn1;
                                            else if(num == num2) $$ = toreturn2;
                                            else $$ = toreturn3;
                                          }
      | E0                               { $$ = $1; };

D     : ID                                { id = NULL;
                                            id = (char*)malloc(32);
                                            sprintf(id, "%s", $1); }
        EQU E0                            { insertarTS(id, $4);
                                            printf("El valor del identificador %s es %.2f\n", id, $4); }
      | LT ID                             { id1 = NULL;
                                            id1 = (char *)malloc(32); 
                                            sprintf(id1, "%s", $2); }
        SEP ID                            { id2 = NULL;
                                            id2 = (char *)malloc(32);
                                            sprintf(id2, "%s", $5); }
        GT EQU LT E0                      { insertarTS(id1, $10);
                                            printf("El valor del identificador %s es %.2f\n", id1, $10); }
        SEP E0                            { insertarTS(id2, $13);
                                            printf("El valor del identificador %s es %.2f\n", id2, $13); }
        GT         
      ;

E0    : E0 ADD E1                         { $$ = $1 + $3; }
      | E0 SUB E1                         { $$ = $1 - $3; }
      | E1                                { $$ = $1; }
      ;

cond  : E0 LEQ E0                         { if($1 <= $3) $$ = 1;
                                            else $$ = 0; }
      | E0 GEQ E0                         { if($1 >= $3) $$ = 1;
                                            else $$ = 0; }
      | E0 EQI E0                         { if($1 == $3) $$ = 1;
                                            else $$ = 0; }
      | E0 NEQ E0                         { if($1 != $3) $$ = 1;
                                            else $$ = 0; }
      | E0 LT E0                          { if($1 < $3) $$ = 1;
                                            else $$ = 0; }
      | E0 GT E0                          { if($1 > $3) $$ = 1;
                                            else $$ = 0; }
      | E0                                { if($1) $$ = 1;
                                            else $$ = 0; }
      ;

E1    : E1 MULT E2                        { $$ = $1 * $3; }
      | E1 DIV E2                         { $$ = $1 / $3; }
      | E2                                { $$ = $1; }
      ;

E2    : INT                               { $$ = $1; }
      | REAL                              { $$ = $1; }
      | EXP                               { $$ = $1; }
      | ID                                { $$ = buscarTS($1); }
      | PO E0 PC                          { $$ = $2; }
      ;
      
%%

int main() {

  yyparse();

}

          

int yyerror (char *s)
{

  printf ("%s\n", s);

  return 0;

}


             
int yywrap()  
{  

  return 1;  

}  