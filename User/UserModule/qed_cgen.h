/**************************************************************
***************************************************************
*  File qed_cgen.h		Version 3.0
*  Date Oct 8, 1998
*
*  Standard QEDesign C Code Generator 
*
*	filter structures and function prototypes for filter routines
*	  in file qed_cgen.h
*
*  MOMENTUM DATA SYSTEMS
*  17330 Brookhurst St. Suite 140
*  Fountain Valley, CA 92708
*
*  Tel 714/378-5805	FAX 714/378-5985
*  E-Mail: tech@mds.com
*
***************************************************************
**************************************************************/

#ifndef _QED_CGEN_H_
#define _QED_CGEN_H_


typedef struct{
	int fract;
	int shift;
} FractGain;


typedef struct {
	int nosect;		/* number of sections */

	int rMthd;		/* realization method */
					/*   0  Cascaded second order sections */
					/*   1  Parallel second order sections */
	int quant;		/* quantization switch */
					/*   0  quanti
                                         * zation off - double precision */
					/*   1  quantization on */

	int quantType;	/* 0 Floating point         */
					/* 1 Fixed point fractional	*/
	int realType1;	// applies to cascaded sections only
					/* 0 Transposed Canonic Second Order Sections */
					/* 1 Canonic Second Order Sections */
					/* 2 Floating Point - 4 Multiplies */
					/* 3 Floating Point - 5 Multiplies */
					/* Valid Combinations  */
					/* quantType realType   Description */
					/*    0         0      	fixed point	*/
					/*	  0         1		fixed point	*/
					/*    1      	0       block floating point */
					/*    1         1		block floating point */
					/*    1         2		floating point */
					/*    1         3		floating point */
	int realType2;  /* applies to parallel sections only */
					/* 0 Transposed Canonic Second Order Sections */
					/* 1 Transposed Floating Sections */
	void  *gain;	/* pointer to initial gain - cascaded and parallel sections */
	void  *pars;	/* pointer to scaler for parallel form */

	void  *num;		/* pointer to array of numerator values   */
	void  *den;		/* pointer to array of denominator values */
	void  *m1;		/* pointer to intermediate storage        */
	void  *m2;		/* pointer to intermediate storage        */

	void (*filter) ();
/* void (*filter) (float *pInValues, float *pOutValues, int n, BiquadSections *F); */
					/* address of filter routine */
} BiquadSections;	/* second order section structure name */




typedef struct {
	int quant;		/* quantization switch         */
					/*   0  quantization off - double precision */
					/*   1  quantization on        */
	int quantType;	/* 0 Floating point            */
					/* 1 Fixed point fractional	   */
	int realType4;  /* applies to FIR filters only */
					/*   0 - No scaling of filter  */
					/*   1 - Center tap scaled to be > 0.5 and < 1.0  
	                         intended for right shift to restore 0dB gain 
	                          shifts used in fixed point fractional 
	                          implementational only                   
	                         for floating point multiply by gain      */
					/*   2 - Center tap scaled to be 1.0 - gain factor 
							   (essentially largest positive value)  
	                     requires post multiplication of gain value   */
	int shift_cnt;  /* for fixed point fractional implementation only */
	int length;		/* filter length (filter order + 1)     */
	int delay;		/* delay index to delayed samples array */
	void  *gain;    /* initial gain value                   */
	                /*   applies to realType of 1 or 2      */
	void  *coef;	/* pointer to array of coefficients     */
	void  *smpl;	/* pointer to array of delayed samples  */
	void (*filter) ();
/* void (*filter) (float *pInValues, float *pOutValues, int n, FIR_filter *F); */
					/* address of filter routine */
} FIR_filter;		/* fir filter structure name */







typedef struct {
	int length;		/* filter length (filter order + 1)     */
	void  *num;		/* pointer to numerator array	*/
	void  *den;		/* pointer to denominator array */
	void  *buf;		/* buffer for intermediate data */
	void (*filter) ();
/* void (*filter) (float *pInValues, float *pOutValues, int n, RT2_filter *F); */
					/* address of filter routine */
} RT2_filter;		/* ratio of two polynomials filter structure name */






typedef struct {
	int quant;		/* quantization switch  */
					/*   0  quantization off - double precision */
					/*   1  quantization on */
	int quantType;	/* 0 Floating point         */
					/* 1 Fixed point fractional	*/
	int order;		/* filter length (filter order + 1)    */
	void  *gain;    /* initial gain value */
	void  *kappa;	/* pointer to array of lattice coefficients (poles)*/
	void  *gamma;	/* pointer to array of ladder coefficients  (zeros)*/
	void  *f;		/* pointer to array of forward values  */
	void  *b;		/* pointer to array of backward values */
	void (*filter) ();
/* void (*filter) (float *pInValues, float *pOutValues, int n, FIR_filter *F); */
					/* address of filter routine */
} LAT_filter;		/* fir filter structure name */








void init_biquad_float (BiquadSections *F);
void init_fir_float	   (FIR_filter     *F);
void init_rt2_float    (RT2_filter     *F);
void init_lat_float    (LAT_filter     *F);

void cas_float_4mul         
	  (float *pInValues,      /* pointer to input buffer      */
	   float *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   BiquadSections *Filt); /* pointer to filter structure  */
void cas_float_5mul
      (float *pInValues,      /* pointer to input buffer      */
	   float *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   BiquadSections *Filt); /* pointer to filter structure  */
void cas_blkfloat_fm1 
      (float *pInValues,      /* pointer to input buffer      */
	   float *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   BiquadSections *Filt); /* pointer to filter structure  */
void cas_blkfloat_fm2 
      (float *pInValues,      /* pointer to input buffer      */
	   float *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   BiquadSections *Filt); /* pointer to filter structure  */
void par_float_fm1 
      (float *pInValues,      /* pointer to input buffer      */
	   float *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   BiquadSections *Filt); /* pointer to filter structure  */
void par_blkfloat_fm1
      (float *pInValues,      /* pointer to input buffer      */
	   float *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   BiquadSections *Filt); /* pointer to filter structure  */
void fir_float_reg
      (float *pInValues,      /* pointer to input buffer      */
	   float *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   FIR_filter *Filt);     /* pointer to filter structure  */
void rt2_float
      (float *pInValues,      /* pointer to input buffer      */
	   float *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   RT2_filter *Filt);     /* pointer to filter structure  */
void lat_float
      (float *pInValues,      /* pointer to input buffer      */
	   float *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   LAT_filter *Filt);     /* pointer to filter structure  */

void init_biquad_dbl  (BiquadSections *F);
void init_fir_dbl	  (FIR_filter     *F);
void init_rt2_dbl     (RT2_filter     *F);
void init_lat_dbl     (LAT_filter     *F);

void cas_dbl_fm2 
      (double *pInValues,     /* pointer to input buffer      */
	   double *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   BiquadSections *Filt); /* pointer to filter structure  */
void par_dbl_fm1
      (double *pInValues,     /* pointer to input buffer      */
	   double *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   BiquadSections *Filt); /* pointer to filter structure  */
void fir_dbl_reg
      (double *pInValues,     /* pointer to input buffer      */
	   double *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   FIR_filter *Filt);     /* pointer to filter structure  */
void rt2_dbl
      (double *pInValues,     /* pointer to input buffer      */
	   double *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   RT2_filter *Filt);     /* pointer to filter structure  */
void lat_dbl
      (double *pInValues,     /* pointer to input buffer      */
	   double *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   LAT_filter *Filt);     /* pointer to filter structure  */
#endif /* _QED_CGEN_H_ */

