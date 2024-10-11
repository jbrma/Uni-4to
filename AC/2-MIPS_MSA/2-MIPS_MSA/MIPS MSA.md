# 1. Emulación MIPS MSA (opcional)

## 1.1 Instalación de herramientas
Seguir las siguientes instrucciones para instalar el compilador cruzado MIPS64 y el emulador QEMU MIPS64.

1. Instalar [Docker Desktop](https://www.docker.com/products/docker-desktop/) y lanzar el demonio
2. Ejecutar (desde una consola de sistema o terminal) un contenedor de Ubuntu 20:04 LTS (focal) desde el directorio MIPS_MSA/code

```
docker run -it -v .:/home -w /home ubuntu:focal /bin/bash
```

3. Instalar los paquetes necesarios

```
root@74e63dd00e90:/# apt update
root@74e63dd00e90:/# apt -y upgrade
root@74e63dd00e90:/# apt -y install gcc-10-mipsisa64r6-linux-gnuabi64
root@74e63dd00e90:/# apt -y install qemu-user
```

## 1.2 Ejemplo: solución ej13

### Versión scalar

Compilar el programa, ejecutarlo con código scalar y guardar la salida de referencia

```
root@74e63dd00e90:/# mipsisa64r6-linux-gnuabi64-gcc-10 -mmsa ej13.c -o ej13-scalar.elf -static
root@74e63dd00e90:/# qemu-mips64 -cpu I6400 ej13-scalar.elf > ej13-scalar.out
```

Siendo el código:

```c
for (i=0;i<128;i++) {
   for (j=0; j<128; j++){
      if (red[i][j]< 250) 
		  red[i][j] += 5;
	  else 
		  red[i][j] = 255;
   }
}
```

### Versión ensamblador (ADDVI)

Compilar el programa en la versión ensamblador con ADDVI empleando *inline assembly* ([link1](https://gcc.gnu.org/onlinedocs/gcc/extensions-to-the-c-language-family/how-to-use-inline-assembly-language-in-c-code.html), [link2](https://gcc.gnu.org/onlinedocs/gcc/extensions-to-the-c-language-family/how-to-use-inline-assembly-language-in-c-code.html) GNU GCC Online Docs)

```
root@74e63dd00e90:/# mipsisa64r6-linux-gnuabi64-gcc-10 -mmsa ej13.c -o ej13-addvi.elf -static \
-DADDVI_VERSION
```

Siendo el código:

```c
__asm volatile(  	
"				li	 		$4,0x4000	\n"	
"				move		$5,%[Red]	\n"	
"				li	 		$6,0x10		\n"
"				ldi.b  	$w1,0xfa	\n"
"   		ldi.b		$w2,0xff	\n"				
"loop: 	ld.b    $w3,0($5)	\n" 	
"				clt_u.b $w4,$w3,$w1	\n" 
"				addvi.b $w5,$w3,0x5	\n"
"   		bsel.v  $w4,$w2,$w5 \n"
"				st.b    $w4,0($5)	\n" 
"				sub	 		$4,$4,$6	\n"
"				dadd    $5,$5,$6	\n"
"				bgtz		$4,loop		\n"
"				nop								\n"
: 
: [Red] "r" (red)
: "memory","$4","$5","$6"
);
```

*Notas: (i) la matriz de 128x128 se recorre con un sólo bucle ya que sería lo óptimo (ii) la nop del delay slot se puede eliminar.*

### Versión ensamblador (ADDS)

Compilar el programa en la versión ensamblador con ADDS empleando [*inline assembly* en GCC](https://gcc.gnu.org/onlinedocs/gcc/extensions-to-the-c-language-family/how-to-use-inline-assembly-language-in-c-code.html):

```
root@74e63dd00e90:/# mipsisa64r6-linux-gnuabi64-gcc-10 -mmsa ej13.c -o ej13-adds.elf -static \
-DADDS_VERSION
```

Siendo el código:

```c
__asm volatile(  	
"				li			$4,0x4000		\n"		
"				li			$5,0x10			\n"
"				move		$6,%[Red]		\n"
"				ldi.b		$w0,0x5			\n" 			
"loop:	ld.b 		$w1,0($6)		\n" 	
"				adds_u.b $w1,$w1,$w0 \n" 	
"				st.b   	$w1,0($6)		\n" 
"				sub	 		$4,$4,$5		\n"
"				dadd    $6,$6,$5		\n"
"				bgtz		$4,loop			\n"	
"				nop									\n"	
: 
: [Red] "r" (red)
: "memory","$4","$5","$6", "$w0", "$w1"
);
```

*Nota: la nop del delay slot se puede eliminar.*

## 1.3 Ejemplo: solución ej14

### Versión scalar

Compilar el programa, ejecutarlo con código scalar y guardar la salida de referencia

```
root@74e63dd00e90:/# mipsisa64r6-linux-gnuabi64-gcc-10 -mmsa ej14.c -o ej14-scalar.elf -static
root@74e63dd00e90:/# qemu-mips64 -cpu I6400 ej14-scalar.elf > ej14-scalar.out
```

Siendo el código:

```c
for (i=0; i<256; i++) {
    a[i]=b[i]+c[i];
    if (a[i]==b[i])
       d[i]=a[i]*3;
    b[i]=a[i]-5;
}
```

### Versión ensamblador

Compilar el programa en la versión ensamblador empleando [*inline assembly* en GCC](https://gcc.gnu.org/onlinedocs/gcc/extensions-to-the-c-language-family/how-to-use-inline-assembly-language-in-c-code.html):

```
root@74e63dd00e90:/# mipsisa64r6-linux-gnuabi64-gcc-10 -mmsa ej14.c -o ej14-msa.elf -static \
-DMSA_VERSION
```

Siendo el código:

```c
__asm volatile(  	
"				ld.d		$w7,0(%[Rthree]) \n"
"				ld.d		$w8,0(%[Rfive])	\n"
"				li 			$4,128					\n"
"				li			$5,16						\n"
"				li			$6,1						\n"
"Loop:	ld.d 		$w1,0(%[Rb])		\n"
"				ld.d 		$w2,0(%[Rc])		\n"
"				fadd.d 	$w3,$w1,$w2			\n"
"				st.d 		$w3,0(%[Ra])		\n"
"				fceq.d 	$w5,$w3,$w1			\n"
"				fmul.d 	$w4,$w3,$w7			\n"
"				ld.d 		$w6,0(%[Rd])		\n"
"  			bmnz.v	$w6,$w4,$w5  		\n"
"  			st.d 		$w6,0(%[Rd])		\n"
"  			fsub.d	$w5,$w3,$w8			\n"
"  			st.d 		$w5,0(%[Rb])		\n"
"  			dadd 		%[Ra],%[Ra],$5  \n"
"  			dadd 		%[Rb],%[Rb],$5  \n"
"  			dadd 		%[Rc],%[Rc],$5  \n"
"  			dadd 		%[Rd],%[Rd],$5  \n"
"  			sub 		$4,$4,$6  			\n"
"  			bnez 		$4,Loop  				\n"
"  			nop											\n"	
: 
: [Ra] "r" (a),
  [Rb] "r" (b),
  [Rc] "r" (c),
  [Rd] "r" (d),
  [Rthree] "r" (three),
  [Rfive] "r" (five)
: "memory", "$4", "$5", "$6"
);
```

*Nota: la nop del delay slot se puede eliminar.*

### Versión intrínsecas

Compilar el programa en la versión ensamblador empleando 

```
root@74e63dd00e90:/# mipsisa64r6-linux-gnuabi64-gcc-10 -mmsa ej14.c -o ej14-intrinsic.elf -static \
-DINTRINSIC_VERSION
```

Siendo el código:

```c
register v2f64 va, vb, vc, vd;
register v2f64 v2, v3, v5, v6;
register v2i64 v1;

v3 = (v2f64) __msa_ld_d(three,0);
v5 = (v2f64) __msa_ld_d(five,0);

for (i=0; i<256; i+=2) {	
 	va = (v2f64) __msa_ld_d(&a[i],0);
	vb = (v2f64) __msa_ld_d(&b[i],0);
	vc = (v2f64) __msa_ld_d(&c[i],0);
	vd = (v2f64) __msa_ld_d(&d[i],0);
	
  //a[i]=b[i]+c[i];
	va = __msa_fadd_d(vb,vc);
	__msa_st_d((v2i64)va,&a[i],0);
  
  //if (a[i]==b[i])
	v1 = __msa_fceq_d(va,vb);	
  		
  		//d[i]=a[i]*3;
			v2 = __msa_fmul_d(va,v3);
			v6 = (v2f64) __msa_bmnz_v((v16u8)vd,(v16u8)v2,(v16u8)v1);
			__msa_st_d((v2i64)v6,&d[i],0);
  
  //b[i]=a[i]-5;
	vb = __msa_fsub_d(va,v5);
	__msa_st_d((v2i64)vb,&b[i],0);
```

# 2. Ejercicio

Codificar empleando MIPS MSA el cuerpo de la función `convolution()` del fichero `covolution.c`. Se valorará la optimización de código y el número de instrucciones resultantes.

```c
    for (int i = 0; i < MATRIX_SIZE - 1; i++) {
        for (int j = 0; j < MATRIX_SIZE - 1; j++) {
            output[i][j] = input[i][j] * filter[0][0] +
                           input[i][j + 1] * filter[0][1] +
                           input[i + 1][j] * filter[1][0] +
                           input[i + 1][j + 1] * filter[1][1];
        }
    }
```

*Aclaraciones:*

* *Se pueden usar las simplificaciones vistas en clase y por lo tanto se puede entregar un código no conforme 100% con el repertorio MIPS64 MSA.*
* *No es por lo tanto obligatorio que el código esté libre de errores en el compilador cruzado ni emplear el emulador, aunque se valorará positivamente las utilización de ellos.*
