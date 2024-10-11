foo:	ld 	f2, 0(r1)
	multd 	f4, f2, f0
	ld	f6, 0(r2)
	addd    f6, f4, f6
	sd      0(r2), f6
	addi 	r1, r1, 8
	addi 	r2, r2, 8
	sgti	r3, r1, done
	beqz	r3, foo
	nop
	trap	#0
