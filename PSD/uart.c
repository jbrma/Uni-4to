
#include <s3c44b0x.h>
#include <uart.h>

void uart0_init( void )
{
    UFCON0 = 0x1;
    UMCON0 = 0x0;
    ULCON0 = 0x3;
    UBRDIV0 = 0x22;
    UCON0 = 0x5;
}

void uart0_putchar( char ch )
{
    while( UFSTAT0 & (1<<9) );
    UTXH0 = ch; //envía el character
}        

char uart0_getchar( void )
{
    while( (UFSTAT0 & 0XF) == 0);
    return URXH0; //recibe el character y lo devuelve
}

void uart0_puts( char *s )
{
    while(*s != '\0'){ //envía cadena de caracteres
    	uart0_putchar(*s);
    	s++;
    }
}

void uart0_putint( int32 i )
{
	char buf[11 + 1];
	char *p = buf + 11;
	uint8 c;
	boolean neg = (i<0);
	*p = '\0';

	if(neg) i *= -1;
	if(i == 0)*--p = '0';
	while(i> 0){
		c = i % 10;
		i = i / 10;
		*--p = '0' + c;
	}

	if(neg) *--p = '-';

	uart0_puts( p ); //envía un entero con signo como una cadena de digitos decimales
}

void uart0_puthex( uint32 i )
{
    char buf[8 + 1];
    char *p = buf + 8;
    uint8 c;

    *p = '\0';

    do {
        c = i & 0xf;
        if( c < 10 )
            *--p = '0' + c;
        else
            *--p = 'a' + c - 10;
        i = i >> 4;
    } while( i );

    uart0_puts( p ); //envía un entero sin signo como una cadena de digitos hexadecimales
}

void uart0_gets( char *s )
{
	char ch;
	while((ch= uart0_getchar()) != '\n'){ //recibe una cadena de caracteres y la almacena
		*s = ch;
		s++;
	}
	*s = '\0';
}

int32 uart0_getint( void )
{
	//Espera la recepción de una cadena de dígitos decimales que
	//interpreta como un entero con signo que devuelve

    int32 outp = 0;
    int32 sig;
    char car = uart0_getchar();
    boolean neg= FALSE;
    if(car == '-'){
    	neg = TRUE;
    	car = uart0_getchar();
    }
    while(car != '\n'){
    	sig = car - '0';
    	outp = outp*10;
    	if(neg) outp-= sig;
    	else outp += sig;
    	car = uart0_getchar();
    }
    return outp;
}

uint32 uart0_gethex( void )
{
	//Espera la recepción de una cadena de dígitos hexadecimales que
	//interpreta como un entero sin signo que devuelve

	int32 num = 0;
		char caracter;

		while((caracter = uart0_getchar()) != '\n'){
			if (caracter - '0' < 10){
				num = num*16 + (caracter - '0');
			}
			else if(caracter<='Z'){
				num = num*16 + (caracter - 'A' + 10);
			}
			else{
				num = num*16 + (caracter - 'a' + 10);
			}

		}
		return num;
}
