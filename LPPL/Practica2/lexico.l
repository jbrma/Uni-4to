%{
    #include "sintactico.tab.h"
    #include <stdio.h>
    #include <stdlib.h>

%}

let [A-Za-z_]
dig [0-9]

%%

siendo                                      { return SIEN; }

evalua                                      { return EVAL; }

if                                          { return IF; }

then                                        { return THEN; }

else                                        { return ELSE; }

switch                                      { return SWITCH; }

case                                        { return CSE; }

default                                     { return DFLT; }

[+-]?{dig}+                                 { yylval.numero_real = atof(yytext); return INT; }

[+-]?{dig}+(.{dig}+)?                       { yylval.numero_real = atof(yytext); return REAL; }

[+-]?{dig}+(.{dig}+)?([eE][+-]?{dig}+)?     { yylval.numero_real = atof(yytext); return EXP; }

{let}({let}|{dig})*                         { yylval.identificador = yytext; return ID; }

"<="                                        { return LEQ; }

">="                                        { return GEQ; }                            

"=="                                        { return EQI; }

"!="                                        { return NEQ; }

"="                                         { return EQU; }

"+"                                         { return ADD; }

"-"                                         { return SUB; }  

"*"                                         { return MULT; }

"/"                                         { return DIV; }

","                                         { return SEP; }

"("                                         { return PO; }

")"                                         { return PC; } 

"{"                                         { return CPO; }    

"}"                                         { return CPC; }

"<"                                         { return LT; }

">"                                         { return GT; }

";"                                         { return PCOMA; }

":"                                         { return DP; }

"\n"                                        {}

.                                           {};        

%% 