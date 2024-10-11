	ld	f2, a 
	add	r1, r0, xtop
loop:	ld	f0, 0(r1)
	multd 	f4, f0, f2
	divd	f10, f8, f0
	sd	0(r1), f4
	sub 	r1, r1, #8
	bnez	r1, loop
	nop
	trap	#0
