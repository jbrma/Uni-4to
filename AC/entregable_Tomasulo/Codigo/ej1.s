		ld	f2, a 
		add	r1, r0, xtop
loop:	ld	f0, 0(r1)
		sub r1, r1, #8
		multd f4, f0, f2
		bnez r1, loop
		sd	8(r1), f4
		trap	#0



