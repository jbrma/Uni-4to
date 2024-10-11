#include <stdio.h>
#include <msa.h>

__attribute__((aligned(16))) double a[256], b[256], c[256], d[256];
__attribute__((aligned(16))) double three[] = {3.0,3.0};
__attribute__((aligned(16))) double five[] = {5.0,5.0};

int main()
{

int i,j;

// fake test image (red channel)
for (i=0; i<256; i++) {
    b[i] = c[i]= (double)(i)/10;
    d[i] = 0.0;
}

c[5] = 0.0;

//#define SIMD_VERSION

#ifdef MSA_VERSION
__asm volatile(  	
"		LD.D	$w7,0(%[Rthree])	\n"
"		LD.D	$w8,0(%[Rfive])		\n"
"		LI 	$4,128					\n"
"		LI	$5,16					\n"
"		LI	$6,1					\n"
"Loop:	LD.D 	$w1,0(%[Rb])		\n"
"		LD.D 	$w2,0(%[Rc])		\n"
"		FADD.D	$w3,$w1,$w2			\n"
"		ST.D 	$w3,0(%[Ra])		\n"
"		FCEQ.D 	$w5,$w3,$w1			\n"
"		FMUL.D	$w4,$w3,$w7			\n"
"		LD.D 	$w6,0(%[Rd])		\n"
"  		BMNZ.V	$w6,$w4,$w5  		\n"
"  		ST.D 	$w6,0(%[Rd])		\n"
"  		FSUB.D	$w5,$w3,$w8			\n"
"  		ST.D 	$w5,0(%[Rb])		\n"
"  		DADD 	%[Ra],%[Ra],$5  	\n"
"  		DADD 	%[Rb],%[Rb],$5  	\n"
"  		DADD 	%[Rc],%[Rc],$5  	\n"
"  		DADD 	%[Rd],%[Rd],$5  	\n"
"  		SUB 	$4,$4,$6  			\n"
"  		BNEZ 	$4,Loop  			\n"
"  		NOP							\n"	
: 
: [Ra] "r" (a),
  [Rb] "r" (b),
  [Rc] "r" (c),
  [Rd] "r" (d),
  [Rthree] "r" (three),
  [Rfive] "r" (five)
: "memory", "$4", "$5", "$6"
);
#elif INTRINSIC_VERSION
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
}
#else
for (i=0; i<256; i++) {
    a[i]=b[i]+c[i];
    if (a[i]==b[i])
       d[i]=a[i]*3;
    b[i]=a[i]-5;
}
#endif


// check output
for (i=0; i<256;i++) {
    printf(" a:%lf b:%lf c:%lf d:%lf\n",a[i],b[i],c[i],d[i]);
}

return 0;
}
