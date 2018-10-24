/**************************************************************
***************************************************************
*  File qed_filt.c 	     Version 3.0
*  Date Oct 8, 1998
*
*  Standard QEDesign C Code Generator 
*
*  This file contains routines for implementing filters
*     Second order sections: cascade and parallel 
*     FIR filter 
*     Ratio of two polynomials
*
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

#include "qed_cgen.h"

/**************************************************************
*  initialize biquad sections
*  clears delay lines to zero
*  must be called prior to calling filter routine
**************************************************************/

void init_biquad_float(BiquadSections *Filt) {
    
float *m1;	  /* pointer to delay line1 */ 
float *m2;	  /* pointer to delay line2 */
int i;

m1 = Filt->m1;
m2 = Filt->m2;

for (i = 0; i < Filt->nosect; i++) {
	m1[i] = (float) 0.0;
	m2[i] = (float) 0.0;
	}
}



/*  filter routine for 4 multiplier 
    floating point cascaded sections
	
    This uses form II i.e.
    the incoming sample is added to the feedback loops
	Note: minus signs are used for the feedback loops

	If code is used for coefficients from an flt file,
	  these signs must be changed to positive. All feedback
	  coefficients in flt files have complemented signs.
*/

void cas_float_4mul 
      (float *pInValues,      /* pointer to input buffer      */
	   float *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   BiquadSections *Filt)  /* pointer to filter structure  */
{

float *pGain; /* pointer to gain value of filter */
float gain;   /* gain value of filter            */
int nosect;   /* number of sections in the filter */
float *m1;	  /* pointer to delay line1 */ 
float *m2;	  /* pointer to delay line2 */
float *num;	  /* pointer to numerator coefficients   */
float *den;	  /* pointer to denominator coefficients */
float x;	  /* gain * input sample */
float temp;   /* scratch variable */
float s;      /* scratch variable */
int i;        /* input sample counter */
int j;        /* section counter */


/* runtime initialization for filter routine */

m1 = Filt->m1;
m2 = Filt->m2;

pGain  = Filt->gain;
gain   = *pGain;
nosect = Filt->nosect;


for (i = 0; i < nValues; i++)  {
	num = Filt->num;
    den = Filt->den;
	x = pInValues[i] * gain;
	for (j = 0; j < nosect; j++) {
		temp = x - den[0] * m1[j] - den[1] * m2[j];
		s = temp + num[0] * m1[j] + num[1] * m2[j];
	    m2[j] = m1[j];
		m1[j] = temp;    
		x = s;
		num += 2;
	    den += 2;
	}
	pOutValues[i] = s;
}
return;
}




/*  filter routine for 5 multiplier 
    floating point cascaded sections
	
    This uses form II i.e.
    the incoming sample is added to the feedback loops
	Note: minus signs are used for the feedback loops

	If code is used for coefficients from an flt file,
	  these signs must be changed to positive. All feedback
	  coefficients in flt files have complemented signs.
*/

void cas_float_5mul 
      (float *pInValues,      /* pointer to input buffer      */
	   float *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   BiquadSections *Filt)  /* pointer to filter structure  */
{

float *pGain; /* pointer to gain value of filter */
float  gain;  /* gain value of filter */
int nosect;   /* number of sections in the filter */
float *m1;	  /* pointer to delay line1 */ 
float *m2;	  /* pointer to delay line2 */
float *num;	  /* pointer to numerator coefficients   */
float *den;	  /* pointer to denominator coefficients */
float x;	  /* gain * input sample */
float temp;   /* scratch variable */
float s;      /* scratch variable */
int i;        /* input sample counter */
int j;        /* section counter */


/* runtime initialization for filter routine */

m1 = Filt->m1;
m2 = Filt->m2;
pGain = Filt->gain;
gain  = *pGain;
nosect = Filt->nosect;


for (i = 0; i < nValues; i++)  {
	num = Filt->num;
    den = Filt->den;
	x = pInValues[i] * gain;
	for (j = 0; j < nosect; j++) {
		temp = x - den[0] * m1[j] - den[1] * m2[j];
		s = temp * num[0] + num[1] * m1[j] + num[2] * m2[j];
	    m2[j] = m1[j];
		m1[j] = temp;
	  	x = s;
        num += 3;
	    den += 2;
	}
	pOutValues[i] = s;
}
return;
}





/*  filter routine for 5 multiplier block floating point
	 cascaded sections
	
    This uses form I . This is the transpose of Form II

	If code is used for coefficients from an flt file,
	  these signs must be changed to positive. All feedback
	  coefficients in flt files have complemented signs.

*/

void cas_blkfloat_fm1 
      (float *pInValues,      /* pointer to input buffer      */
	   float *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   BiquadSections *Filt)  /* pointer to filter structure  */
{

float *pGain; /* pointer to gain value of filter */
float gain;	  /* gain value of filter */
int nosect;   /* number of sections in the filter */
float *m1;	  /* pointer to delay line1 */ 
float *m2;	  /* pointer to delay line2 */
float *num;	  /* pointer to numerator coefficients   */
float *den;	  /* pointer to denominator coefficients */
float x;	  /* gain * input sample */
float s;      /* scratch variable */
int i;        /* input sample counter */
int j;        /* section counter */


/* runtime initialization for filter routine */

m1 = Filt->m1;
m2 = Filt->m2;
pGain = Filt->gain;
gain  = *pGain;
nosect = Filt->nosect;


for (i = 0; i < nValues; i++)  {
	num = Filt->num;
    den = Filt->den;
	x = pInValues[i] * gain;
	for (j = 0; j < nosect; j++) {
	   	  s     = (num[0] * x + m1[j]) / den[0];
	      m1[j] =  num[1] * x - den[1] * s + m2[j];
	      m2[j] =  num[2] * x - den[2] * s;
	      x = s;
		  num += 3;
		  den += 3;

	}
	pOutValues[i] = s;
}
return;
}





/*  filter routine for 5 multiplier block floating point
	 cascaded sections
	
	This uses form II i.e.
    the incoming sample is added to the feedback loops


	If code is used for coefficients from an flt file,
	  these signs must be changed to positive. All feedback
	  coefficients in flt files have complemented signs.

*/

void cas_blkfloat_fm2 
      (float *pInValues,      /* pointer to input buffer      */
	   float *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   BiquadSections *Filt)  /* pointer to filter structure  */
{

float *pGain; /* pointer to gain value of filter */
float gain;	  /* gain value of filter */
int nosect;   /* number of sections in the filter */
float *m1;	  /* pointer to delay line1 */ 
float *m2;	  /* pointer to delay line2 */
float *num;	  /* pointer to numerator coefficients   */
float *den;	  /* pointer to denominator coefficients */
float x;	  /* gain * input sample */
float temp;   /* scratch variable */
float s;      /* scratch variable */
int i;        /* input sample counter */
int j;        /* section counter */


/* runtime initialization for filter routine */

m1 = Filt->m1;
m2 = Filt->m2;
pGain = Filt->gain;
gain  = *pGain;
nosect = Filt->nosect;


for (i = 0; i < nValues; i++)  {
	num = Filt->num;
    den = Filt->den;
	x = pInValues[i] * gain;
	for (j = 0; j < nosect; j++) {
	   	temp = (den[0] * x    - den[1] * m1[j] - den[2] * m2[j]) / den[0];
		s    = (num[0] * temp + num[1] * m1[j] + num[2] * m2[j]) / den[0];
	    m2[j] = m1[j];
		m1[j] = temp;
	  	x = s;
        num += 3;
	    den += 3;
	}
	pOutValues[i] = s;
}
return;
}



/*  filter routine for 5 multiplier 
    floating point sum of  sections
	
    This uses form I 
 
	Note: minus signs are used for the feedback loops

	If code is used for coefficients from an flt file,
	  these signs must be changed to positive. All feedback
	  coefficients in flt files have complemented signs.
*/

void par_float_fm1 
      (float *pInValues,      /* pointer to input buffer      */
	   float *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   BiquadSections *Filt)  /* pointer to filter structure  */
{

float *pGain; /* pointer to gain value of filter  */
float gain;   /* gain value for sum of sections   */
float *pPars; /* pointer to scale value for parallel sections */
float pars;	  /* scale value of filter                        */
int nosect;   /* number of sections in the filter */
float *m1;	  /* pointer to delay line1 */ 
float *m2;	  /* pointer to delay line2 */
float *num;	  /* pointer to numerator coefficients   */
float *den;	  /* pointer to denominator coefficients */
float x;	  /* input sample */
float sum;    /* scratch variable initialized to gain * sum */
float s;      /* scratch variable     */
int i;        /* input sample counter */
int j;        /* section counter      */


/* runtime initialization for filter routine */

m1 = Filt->m1;
m2 = Filt->m2;
pPars  = Filt->pars;
pars   = *pPars;
pGain  = Filt->gain;
gain   = *pGain;
nosect = Filt->nosect;


for (i = 0; i < nValues; i++)  {
	num = Filt->num;
    den = Filt->den;
	x   = pInValues[i];
	sum = gain * x;
	for (j = 0; j < nosect; j++) {
	   	s     =  num[0] * x + m1[j] ;
	    m1[j] =  num[1] * x - den[0] * s + m2[j];
	    m2[j] =  num[2] * x - den[1] * s;
	    sum += s;
		num += 3;
		den += 2;

	}
	pOutValues[i] = sum * pars;
}
return;
}


/*  filter routine for 5 multiplier 
    block floating point sum of  sections
	
    This uses form I i.e.
 
	Note: minus signs are used for the feedback loops

	If code is used for coefficients from an flt file,
	  these signs must be changed to positive. All feedback
	  coefficients in flt files have complemented signs.
*/

void par_blkfloat_fm1 
      (float *pInValues,      /* pointer to input buffer      */
	   float *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   BiquadSections *Filt)  /* pointer to filter structure  */
{

float *pGain; /* pointer to gain value of filter */
float gain;   /* gain value for sum of sections  */
float *pPars; /* pointer to scale value for parallel sections */
float pars;	  /* scale value of filter            */
int nosect;   /* number of sections in the filter */
float *m1;	  /* pointer to delay line1 */ 
float *m2;	  /* pointer to delay line2 */
float *num;	  /* pointer to numerator coefficients   */
float *den;	  /* pointer to denominator coefficients */
float x;	  /* input sample */
float sum;    /* scratch variable initialized to gain * sum */
float s;      /* scratch variable     */
int i;        /* input sample counter */
int j;        /* section counter      */


/* runtime initialization for filter routine */

m1 = Filt->m1;
m2 = Filt->m2;
pPars = Filt->pars;
pars  = *pPars;
pGain = Filt->gain;
gain  = *pGain;
nosect = Filt->nosect;


for (i = 0; i < nValues; i++)  {
	num = Filt->num;
    den = Filt->den;
	x   = pInValues[i];
	sum = gain * x;
	for (j = 0; j < nosect; j++) {
	   	s     = (num[0] * x + m1[j]) / den[0];
	    m1[j] =  num[1] * x - den[1] * s + m2[j];
	    m2[j] =  num[2] * x - den[2] * s;
	    sum += s;
		num += 3;
		den += 3;

	}
	pOutValues[i] = sum * pars;
}
return;
}





void fir_float_reg 
      (float *pInValues,      /* pointer to input buffer      */
	   float *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   FIR_filter *Filt)      /* pointer to filter structure  */
{
	int i;			/* counter of number of samples     */
	int j;			/* counter on filter values         */
	int length;		/* filter length                    */
	int delay;		/* index into delay line            */
	int rtype4;     /* realization type                 */
	float temp;		/* filter summation variable        */
	float *pGain;   /* pointer to gain value            */
	float gain;     /* gain value                       */
	float *smpl;	/* pointer to delay line of samples */
	float *coef;	/* pointer to filter coefficients   */

/**********************************************************************
*   Let x(n) = at time n and y(n) = output at time n
*   then y(n) = sum of h(j)*x(n-j) for j= 1...length
*
*   x(n) corresponds to pInValues[i]
*   y(n) corresponds to pOutValues[i]
*
*   In this program, h(j) is coef(j)
*     and x(n-j) is the array smpl x(n-j) is implemented using 
*     the delay index and decrementing backward until delay = - 1
*     when delay = -1, it is reset to length - 1 for the next
*     iteration.
*     In effect the use of the variable delay and pointer smpl
*     is a circular buffer for the input samples
*
*   On each sample, the sample is stored in the smpl delay line 
*	  using the subscript value in Filt->delay. This value is then 
*     incremented for the next sample. If the end of the array is
*     reached after incrementing, Filt->delay is set to zero. This
*     forces the next sample to be stored at the first entry of smpl 
*************************************************************************/


	length = Filt->length;
	pGain  = Filt->gain;
	gain   = *pGain;
	rtype4 = Filt->realType4;
	smpl   = Filt->smpl;
	coef   = Filt->coef;

	for (i = 0; i < nValues; i++)  {
		delay = Filt->delay++;		 /* get delay index and increment index value */
									 /*   in filter structure                     */
		if (Filt->delay == length)   /* check for buffer address reset            */
			Filt->delay = 0;		 /* reset buffer address in filter structure  */
		smpl[delay] = pInValues[i];  /* put sample into delay line of samples     */
	    temp  = (float) 0.0;
		for (j = 0; j < length; j++) {
				temp += coef[j] * smpl[delay--]; /* form sum of products          */
				if (delay < 0)		  /* check for buffer address reset           */
					delay += length;  /* reset buffer address to get rest         */
									  /*   of delayed samples                     */
			}
		pOutValues[i] = temp * gain;
	}
    return;
}
	

void init_fir_float (FIR_filter *Filt) {
	int i; float *smpl;
	Filt->delay = 0;				   /* set delay index to zero */
	smpl = Filt->smpl;
	for (i = 0; i < Filt->length; i++) /* zero out the delay line */
		smpl[i] = (float) 0.0;
}




void rt2_float 
      (float *pInValues,      /* pointer to input buffer      */
	   float *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   RT2_filter *Filt)      /* pointer to filter structure  */
{	
	int i, j;
	int order; 
	float x, y, *num, *den, *buf;

	buf   = Filt->buf;
	order = Filt->length - 1;

	for (i = 0; i < nValues; i++)  {

		num   = Filt->num;
		den   = Filt->den;

	    x = pInValues[i];     /* get current sample           */
		y = (x * (*num++) + buf[0]) * (*den++);

		for (j = 0; j < order; j++)
			buf[j] = buf[j+1] + x * (*num++) - y * (*den++);
		buf[order] = x * (*num++) - y * (*den++);
		pOutValues[i] = y;
	}
}


void init_rt2_float (RT2_filter *F) {
	int i;
	float *buf; buf = F->buf;
	for (i = 0; i < F->length; i++) 
		buf[i] = (float) 0.0;
}





/********************************************************************
*  double precision rourines
********************************************************************/

/********************************************************************
*  initialize biquad sections
*  clears delay lines to zero
*  must be called prior to calling filter routine
********************************************************************/

void init_biquad_dbl(BiquadSections *Filt) {
    
double *m1;	  /* pointer to delay line1 */ 
double *m2;	  /* pointer to delay line2 */
int i;

m1 = Filt->m1;
m2 = Filt->m2;

for (i = 0; i < Filt->nosect; i++) {
	m1[i] = (double) 0.0;
	m2[i] = (double) 0.0;
	}
}



/********************************************************************
*   filter routine for 4 multiplier 
*   floating point cascaded sections
*	
*   This uses form II i.e.
*   the incoming sample is added to the feedback loops
*	Note: minus signs are used for the feedback loops
*
*	If code is used for coefficients from an flt file,
*	  these signs must be changed to positive. All feedback
*	  coefficients in flt files have complemented signs.
********************************************************************/

void cas_dbl_fm2 
      (double *pInValues,     /* pointer to input buffer      */
	   double *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   BiquadSections *Filt)  /* pointer to filter structure  */
{

double *pGain; /* pointer to gain value of filter */
double gain;   /* gain value of filter            */
int nosect;    /* number of sections in the filter */
double *m1;	   /* pointer to delay line1 */ 
double *m2;	   /* pointer to delay line2 */
double *num;   /* pointer to numerator coefficients   */
double *den;   /* pointer to denominator coefficients */
double x;	   /* gain * input sample */
double temp;   /* scratch variable */
double s;      /* scratch variable */
int i;         /* input sample counter */
int j;         /* section counter */


/* runtime initialization for filter routine */

m1 = Filt->m1;
m2 = Filt->m2;

pGain  = Filt->gain;
gain   = *pGain;
nosect = Filt->nosect;


for (i = 0; i < nValues; i++)  {
	num = Filt->num;
    den = Filt->den;
	x = pInValues[i] * gain;
	for (j = 0; j < nosect; j++) {
		temp = x - den[0] * m1[j] - den[1] * m2[j];
		s = temp + num[0] * m1[j] + num[1] * m2[j];
	    m2[j] = m1[j];
		m1[j] = temp;    
		x = s;
		num += 2;
	    den += 2;
	}
	pOutValues[i] = s;
}
return;
}







/********************************************************************
*   filter routine for 5 multiplier 
*    floating point sum of  sections
*	
*   This uses form I 
* 
*	Note: minus signs are used for the feedback loops
*
*	If code is used for coefficients from an flt file,
*	  these signs must be changed to positive. All feedback
*	  coefficients in flt files have complemented signs.
********************************************************************/

void par_dbl_fm1 
      (double *pInValues,     /* pointer to input buffer      */
	   double *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   BiquadSections *Filt)  /* pointer to filter structure  */
{

double *pGain; /* pointer to gain value of filter  */
double gain;   /* gain value for sum of sections   */
double *pPars; /* pointer to scale value for parallel sections */
double pars;   /* scale value of filter                        */
int nosect;    /* number of sections in the filter */
double *m1;	   /* pointer to delay line1 */ 
double *m2;	   /* pointer to delay line2 */
double *num;   /* pointer to numerator coefficients   */
double *den;   /* pointer to denominator coefficients */
double x;	   /* input sample */
double sum;    /* scratch variable initialized to gain * sum */
double s;      /* scratch variable     */
int i;         /* input sample counter */
int j;         /* section counter      */


/* runtime initialization for filter routine */

m1 = Filt->m1;
m2 = Filt->m2;
pPars  = Filt->pars;
pars   = *pPars;
pGain  = Filt->gain;
gain   = *pGain;
nosect = Filt->nosect;


for (i = 0; i < nValues; i++)  {
	num = Filt->num;
    den = Filt->den;
	x   = pInValues[i];
	sum = gain * x;
	for (j = 0; j < nosect; j++) {
	   	s     =  num[0] * x + m1[j] ;
	    m1[j] =  num[1] * x - den[0] * s + m2[j];
	    m2[j] =  num[2] * x - den[1] * s;
	    sum += s;
		num += 3;
		den += 2;

	}
	pOutValues[i] = sum * pars;
}
return;
}







void fir_dbl_reg 
      (double *pInValues,     /* pointer to input buffer      */
	   double *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   FIR_filter *Filt)      /* pointer to filter structure  */
{
	int i;			/* counter of number of samples */
	int j;			/* counter on filter length */
	int length;		/* filter length */
	int delay;		/* index into delay line */
	double temp;	/* filter summation variable */
	double *smpl;	/* pointer to delay line of samples */
	double *coef;	/* pointer to filter coefficients */
/**********************************************************************
*   Let x(n) = at time n and y(n) = output at time n
*   then y(n) = sum of h(j)*x(n-j) for j= 1...length
*
*   x(n) corresponds to pInValues[i]
*   y(n) corresponds to pOutValues[i]
*
*   In this program, h(j) is coef(j)
*     and x(n-j) is the array smpl x(n-j) is implemented using 
*     the delay index and decrementing backward until delay = - 1
*     when delay = -1, it is reset to length - 1 for the next
*     iteration.
*     In effect the use of the variable delay and pointer smpl
*     is a circular buffer for the input samples
*
*   On each sample, the sample is stored in the smpl delay line 
*	  using the subscript value in Filt->delay. This value is then 
*     incremented for the next sample. If the end of the array is
*     reached after incrementing, Filt->delay is set to zero. This
*     forces the next sample to be stored at the first entry of smpl 
*************************************************************************/


	length = Filt->length;
	smpl   = Filt->smpl;
	coef   = Filt->coef;

	for (i = 0; i < nValues; i++)  {
		delay = Filt->delay++;		 /* get delay index and increment index value */
									 /*   in filter structure                     */
		if (Filt->delay == length)   /* check for buffer address reset            */
			Filt->delay = 0;		 /* reset buffer address in filter structure  */
		smpl[delay] = pInValues[i];  /* put sample into delay line of samples     */
	    temp  = (double) 0.0;
		for (j = 0; j < length; j++) {
				temp += coef[j] * smpl[delay--]; /* form sum of products          */
				if (delay < 0)		  /* check for buffer address reset           */
					delay += length;  /* reset buffer address to get rest         */
									  /*   of delayed samples                     */
			}
		pOutValues[i] = temp;
	}
    return;
}
	

void init_fir_dbl (FIR_filter *Filt) {
	int i; double *smpl;
	Filt->delay = 0;				   /* set delay index to zero  */
	smpl = Filt->smpl;
	for (i = 0; i < Filt->length; i++) /* zero out the delay line  */
		smpl[i] = (double) 0.0;
}




void rt2_dbl 
      (double *pInValues,     /* pointer to input buffer      */
	   double *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   RT2_filter *Filt)      /* pointer to filter structure  */
{	
	int i, j;
	int order; 
	double x, y, *num, *den, *buf;

	buf   = Filt->buf;
	order = Filt->length - 1;

	for (i = 0; i < nValues; i++)  {

		num   = Filt->num;
		den   = Filt->den;

	    x = pInValues[i];     /* get current sample  */
		y = (x * (*num++) + buf[0]) * (*den++);

		for (j = 0; j < order; j++)
			buf[j] = buf[j+1] + x * (*num++) - y * (*den++);
		buf[order] = x * (*num++) - y * (*den++);
		pOutValues[i] = y;
	}
}


void init_rt2_dbl (RT2_filter *F) {
	int i;
	double *buf; buf = F->buf;
	for (i = 0; i < F->length; i++) 
		buf[i] = (double) 0.0;
}


void init_lat_float (LAT_filter *Filt) {
	int i;
	float *back, *forw;

	forw  = Filt->f;
	back  = Filt->b;

	for (i = 0; i <= Filt->order; i++)  /* zero out the delay line */
		back[i] = forw[i] = (float) 0.0;
}

void init_lat_dbl (LAT_filter *Filt) {
	int i;
	double *back, *forw;

	forw  = Filt->f;
	back  = Filt->b;

	for (i = 0; i <= Filt->order; i++)  /* zero out the delay line */
		back[i] = forw[i] = (double) 0.0;
}


void lat_float 
      (float *pInValues,      /* pointer to input buffer      */
	   float *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   LAT_filter *Filt)      /* pointer to filter structure  */
{	
	int i, j;		/* loop control variables  */
	int order;		/* order of lattice filter */
	int index;		/* order - 1               */ 
	float x;		/* input variable          */
	float y;		/* output variable         */
	float *kappa;   /* pointer to lattice coeficients */
	float *gamma;	/* pointer to ladder coefficients */
	float *back;	/* pointer to backward lattice function values */
	float *forw;	/* pointer to forward  lattice function values */
	float *gain;	/* pointer to initial gain value  */
	float temp;	    /* temporary variable */

	kappa  = Filt->kappa;
	gamma  = Filt->gamma;
	gain   = Filt->gain;
	order  = Filt->order;
	forw  = Filt->f;
	back  = Filt->b;
	index = order - 1;

	for (i = 0; i < nValues; i++)  {

	    x = pInValues[i];  /* get current sample  */
		forw[0] = (*gain) * x;

		for (j = 0; j < order; j++) {
			temp = kappa [index - j];
			forw [j + 1] = forw[j]     - temp * back[j + 1];
			back[j]      = back[j + 1] + temp * forw[j + 1];
		}
		back[order] = forw[order];
		y = 0.0;
		for (j = 0; j <= order; j++) {
			y += back[j] * gamma[order - j];
		}

		pOutValues[i] = y;
	}
}




void lat_dbl 
      (double *pInValues,     /* pointer to input buffer      */
	   double *pOutValues,	  /* pointer to output buffer     */
	   int nValues,			  /* number of samples to process */
	   LAT_filter *Filt)      /* pointer to filter structure  */
{	
	int i, j;		/* loop control variables  */
	int order;		/* order of lattice filter */
	int index;		/* order - 1               */ 
	double x;		/* input variable          */
	double y;		/* output variable         */
	double *kappa;  /* pointer to lattice coeficients */
	double *gamma;	/* pointer to ladder coefficients */
	double *back;	/* pointer to backward lattice function values */
	double *forw;	/* pointer to forward  lattice function values */
	double *gain;	/* pointer to initial gain value  */
	double temp;	/* temporary variable */

	kappa  = Filt->kappa;
	gamma  = Filt->gamma;
	gain   = Filt->gain;
	order  = Filt->order;
	forw  = Filt->f;
	back  = Filt->b;
	index = order - 1;

	for (i = 0; i < nValues; i++)  {

	    x = pInValues[i];  /* get current sample */
		forw[0] = (*gain) * x;

		for (j = 0; j < order; j++) {
			temp = kappa [index - j];
			forw [j + 1] = forw[j]     - temp * back[j + 1];
			back[j]      = back[j + 1] + temp * forw[j + 1];
		}
		back[order] = forw[order];
		y = 0.0;
		for (j = 0; j <= order; j++) {
			y += back[j] * gamma[order - j];
		}

		pOutValues[i] = y;
	}
}