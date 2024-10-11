	lf	f6, 	34(r2)
	lf	f2, 	45(r3)
	multf	f0, 	f2, 	f4
	subf	f8, 	f6, 	f2
	divf 	f10,	f0, 	f6
	addf	f6, 	f8, 	f2
	trap	#0
