	add   r1, r0, #8   ; i
loop1: 	add   r2, r0, #48  ; j
	ld    f0, b(r1) ; tmp = b[i]
loop2:	sll   r3, r2, #1
	add   r3, r3, r1
	ld    f4, x(r2) ;x[j]
	ld    f2, w(r3) ;w[i][j]
	sub   r2, r2, #8 ; j-1
	multd f6, f2, f4  ; w[i][j]*x[j]
	bnez  r2, loop2  ; j!= 0 pc<-loop2
	addd  f0, f0, f6  ; tmp + w + x	
	ld    f2, u(r1)  ; f2<-u[i]
	gtd   f2, f0  ;FPSR <- u(i)<tmp
	bfpt  else  
	sd    y(r1), f0  ; y[i] = tmp
	j     endif
else:   sw    y(r1), #0  ; y[i] = tmp
endif:	bnez  r1, loop1   ; i != 0 pc <- loop1
	sub   r1, r1, #8  ; i-1