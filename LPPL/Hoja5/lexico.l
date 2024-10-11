%{

   #include <stdio.h>
   #include <stdlib.h>

   #include "sintactico.tab.h"


%}

DIGITO [0-9]
LETRA  [A-Za-z_]

%%
 

"int"                      { return ENTERO; }

"float"                    { return REAL; }

"char"                     { return CARAC; }

","                        { return COMA; }

";"                        { return PUNTOCOMA; }

{LETRA}({LETRA}|{DIGITO})* { yylval.nombre = yytext;
                              return ID; }

"\n"                       {}

" "                        {}

%%


