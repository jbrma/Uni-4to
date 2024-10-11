// JORGE BRAVO MATEOS

/* Pseudocódigo:

Para cada fila i de la matriz de entrada:
    Para cada columna j de la matriz de entrada:
        output[i][j] = input[i][j] * filter[0][0] +
                       input[i][j + 1] * filter[0][1] +
                       input[i + 1][j] * filter[1][0] +
                       input[i + 1][j + 1] * filter[1][1]
*/

#ifdef ADDS_VERSION
__asm volatile(
"    li      $4, 0                    \n"   // Inicializa el contador i
"    li      $7, 0                    \n"   // Inicializa el contador j
"    move    $8, %[Input]             \n"   // Carga la dirección base de la matriz de entrada
"    move    $9, %[Output]            \n"   // Carga la dirección base de la matriz de salida
"    li      $10, 1                   \n"   // Constante para la suma
"outer_loop:                           \n"   // Inicio del bucle externo
"    li      $7, 0                    \n"   // Reinicia el contador j para cada iteración de i
"inner_loop:                           \n"   // Inicio del bucle interno
"    lq      $11, 0($8)               \n"   // Carga input[i][j] y input[i][j+1] en $11
"    lw      $13, %[Filter]            \n"   // Carga el filtro en $13
"    lq      $12, 8($8)               \n"   
"    lq      $14, 16($8)              \n"   // Carga input[i+1][j] y input[i+1][j+1] en $14
"    mul.q   $11, $11, $13            \n"   // Multiplica los elementos de $11 por los elementos del filtro
"    mul.q   $14, $14, $13            \n"   // Multiplica los elementos de $14 por los elementos del filtro
"    addu.q  $15, $11, $14            \n"   // Suma los resultados de las multiplicaciones
"    sq      $15, 0($9)               \n"   // Guarda el resultado en output[i][j]
"    addi    $7, $7, 2                \n"   // Incrementa j
"    addi    $8, $8, 16               \n"   // Avanza a la siguiente columna
"    addi    $9, $9, 16               \n"
"    blt     $7, %[SizeMinusOne], inner_loop \n" // Comprueba si se ha alcanzado el final de la fila
"    addi    $4, $4, 1                \n"   // Incrementa i
"    add     $8, $8, %[Stride]        \n"   // Avanza a la siguiente fila
"    add     $9, $9, %[Stride]        \n"
"    blt     $4, %[SizeMinusOne], outer_loop \n" // Comprueba si se ha alcanzado el final de la matriz
: // No hay argumentos de salida
: [Input] "r" (input), [Output] "r" (output), [Filter] "r" (filter), [Stride] "r" (16 * MATRIX_SIZE - 16), [SizeMinusOne] "r" (MATRIX_SIZE - 1)
: // Registros que se modifican
);
#endif


.text
.globl convolution

convolution:
    # Carga los valores del filtro en registros vectoriales
    lv.d  v0, 0($a0)   # Carga filtro[0] (a0 apunta al arreglo filter)
    lv.d  v1, 0($a0 + 16) # Carga filtro[1]

    # Bucle a través de la matriz de entrada (iteraciones MATRIX_SIZE - 1)
loop:
    addi $sp, $sp, -32  # Reserva espacio para variables temporales en la pila

    # Carga elementos de entrada en registros vectoriales
    ld.h  $v2, 0($a1)   # Carga input[i] (a1 apunta a la matriz input)
    lv.d  v3, 0($a1 + 8) # Carga input[i + 1]

    # Calcula el producto para el primer elemento (output[i][j])
    fmul.d v4, v0, v2  # v4 = filtro[0] * input[i]
    fadd.d v4, v4, v1  # v4 = v4 + filtro[1] * input[i + 1]

    # Calcula el producto para el segundo elemento (output[i][j + 1])
    fmul.d v5, v0, v3  # v5 = filtro[0] * input[i + 1]
    fadd.d v5, v5, v1  # v5 = v5 + filtro[1] * input[i]

    # Almacena los resultados en la matriz de salida
    sv.d  v4, 0($a2)   # Almacena output[i][j]
    sv.d  v5, 0($a2 + 8) # Almacena output[i][j + 1]

    addi $a1, $a1, 16  # Incrementa el puntero de la matriz de entrada
    addi $a2, $a2, 16  # Incrementa el puntero de la matriz de salida
    bnez $s1, loop   # Bucle hasta que se procesen todas las filas

    addi $sp, $sp, 32  # Libera espacio de la pila

    jr $ra



