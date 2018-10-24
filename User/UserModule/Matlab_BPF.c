
#include "includes.h"
#include "Qed_cgen.h"
/* filter functions are in files 'qed_filt.c'  */
void df2sos(float *pInValues,float *pOutValues,int nValues,BiquadSections *Filt);
//-----------------------------------------------------
//breath
#define BREATH_MWSPT_NSEC 17

float const Breath_BPF_num[BREATH_MWSPT_NSEC][3] = {
  {
    0.07553645223,              0,              0
  },
  {
                1,              0,             -1
  },
  {
    0.07553645223,              0,              0
  },
  {
                1,              0,             -1
  },
  {
    0.07359324396,              0,              0
  },
  {
                1,              0,             -1
  },
  {
    0.07359324396,              0,              0
  },
  {
                1,              0,             -1
  },
  {
    0.07220327109,              0,              0
  },
  {
                1,              0,             -1
  },
  {
    0.07220327109,              0,              0
  },
  {
                1,              0,             -1
  },
  {
    0.07148312032,              0,              0
  },
  {
                1,              0,             -1
  },
  {
    0.07148312032,              0,              0
  },
  {
                1,              0,             -1
  },
  {
                1,              0,              0
  }
};

float const Breath_BPF_den[BREATH_MWSPT_NSEC][3] = {
  {
                1,              0,              0
  },
  {
                1,   -1.918850183,   0.9485595226
  },
  {
                1,              0,              0
  },
  {
                1,   -1.992666721,   0.9931742549
  },
  {
                1,              0,              0
  },
  {
                1,   -1.835830212,   0.8615778089
  },
  {
                1,              0,              0
  },
  {
                1,   -1.978401899,   0.9789578319
  },
  {
                1,              0,              0
  },
  {
                1,   -1.961917996,   0.9625973105
  },
  {
                1,              0,              0
  },
  {
                1,   -1.784281731,   0.8045635223
  },
  {
                1,              0,              0
  },
  {
                1,   -1.941913962,   0.9428442121
  },
  {
                1,              0,              0
  },
  {
                1,   -1.769548178,   0.7840641141
  },
  {
                1,              0,              0
  }
};


float Breath_BPF_m1[BREATH_MWSPT_NSEC];
float Breath_BPF_m2[BREATH_MWSPT_NSEC];

float Breath_BPF_gain = 1.000000000000e+000F; /* initial gain for cascade realization */
                          /* also applies to parallel realization */
float Breath_BPF_pars = 1.000000000000e+000F; /* scale value for parallel sections */

BiquadSections IIR_Breath_BPF = {

     BREATH_MWSPT_NSEC,     /* number of sections                     */
     0,     /* realization method                     */
            /*   0  Cascaded second order sections    */
            /*   1  Parallel second order sections    */
     1,     /* quantization: 0 off, 1 on              */
     1,     /* quantization type                      */
            /*   0  Floating point                    */
            /*   1  Fixed point fractional            */
     0,     /* realization type for cascaded sections only              */
            /*   0  Fixed point    - Transposed Canonic biquad sections */
            /*   1  Fixed point    - Canonic biquad sections            */
            /*   2  Floating point - 4 multiplies                       */
            /*   3  Floating point - 5 multiplies                       */
            /*   4  Floating point - recursive normal                   */
     0,     /* realization type for parallel sections only              */
            /*   0  Fixed point    - Transposed Canonic biquad sections */
            /*   1  Floating point - Transposed Canonic biquad sections */
   &Breath_BPF_gain,    /* pointer to gain for cascade/parallel realizations */
   &Breath_BPF_pars,    /* pointer to scale value for parallel sections      */
   Breath_BPF_num,      /* pointer to numerator coefficients                 */
   Breath_BPF_den,      /* pointer to denominator coefficients               */
   Breath_BPF_m1,       /* pointer to delay line 1                           */
   Breath_BPF_m2,       /* pointer to delay line 2                           */
   df2sos}; /* ptr to filter routine */
//-------------------------------------------------------------------------------

//-----------------------------------------------------
//heart
#define HEART_MWSPT_NSEC 17

float const Heart_BPF_num[HEART_MWSPT_NSEC][3] = {
  {
      0.116608426,              0,              0
  },
  {
                1,              0,             -1
  },
  {
      0.116608426,              0,              0
  },
  {
                1,              0,             -1
  },
  {
     0.1121231839,              0,              0
  },
  {
                1,              0,             -1
  },
  {
     0.1121231839,              0,              0
  },
  {
                1,              0,             -1
  },
  {
     0.1090196371,              0,              0
  },
  {
                1,              0,             -1
  },
  {
     0.1090196371,              0,              0
  },
  {
                1,              0,             -1
  },
  {
     0.1074439362,              0,              0
  },
  {
                1,              0,             -1
  },
  {
     0.1074439362,              0,              0
  },
  {
                1,              0,             -1
  },
  {
                1,              0,              0
  }
};

float const Heart_BPF_den[HEART_MWSPT_NSEC][3] = {
  {
                1,              0,              0
  },
  {
                1,   -1.720857739,   0.9412070513
  },
  {
                1,              0,              0
  },
  {
                1,   -1.909335017,   0.9686184525
  },
  {
                1,              0,              0
  },
  {
                1,   -1.653087854,   0.8450700641
  },
  {
                1,              0,              0
  },
  {
                1,   -1.845225215,   0.9081344604
  },
  {
                1,              0,              0
  },
  {
                1,   -1.632167816,   0.7884326577
  },
  {
                1,              0,              0
  },
  {
                1,   -1.777919054,   0.8509880304
  },
  {
                1,              0,              0
  },
  {
                1,   -1.710575223,   0.8024892807
  },
  {
                1,              0,              0
  },
  {
                1,   -1.655666113,   0.7763270736
  },
  {
                1,              0,              0
  }
};

float Heart_BPF_m1[HEART_MWSPT_NSEC];
float Heart_BPF_m2[HEART_MWSPT_NSEC];

float Heart_BPF_gain = 1.000000000000e+000F; /* initial gain for cascade realization */
                          /* also applies to parallel realization */
float Heart_BPF_pars = 1.000000000000e+000F; /* scale value for parallel sections */

BiquadSections IIR_Heart_BPF = {

     HEART_MWSPT_NSEC,     /* number of sections                     */
     0,     /* realization method                     */
            /*   0  Cascaded second order sections    */
            /*   1  Parallel second order sections    */
     1,     /* quantization: 0 off, 1 on              */
     1,     /* quantization type                      */
            /*   0  Floating point                    */
            /*   1  Fixed point fractional            */
     0,     /* realization type for cascaded sections only              */
            /*   0  Fixed point    - Transposed Canonic biquad sections */
            /*   1  Fixed point    - Canonic biquad sections            */
            /*   2  Floating point - 4 multiplies                       */
            /*   3  Floating point - 5 multiplies                       */
            /*   4  Floating point - recursive normal                   */
     0,     /* realization type for parallel sections only              */
            /*   0  Fixed point    - Transposed Canonic biquad sections */
            /*   1  Floating point - Transposed Canonic biquad sections */
   &Heart_BPF_gain,    /* pointer to gain for cascade/parallel realizations */
   &Heart_BPF_pars,    /* pointer to scale value for parallel sections      */
   Heart_BPF_num,      /* pointer to numerator coefficients                 */
   Heart_BPF_den,      /* pointer to denominator coefficients               */
   Heart_BPF_m1,       /* pointer to delay line 1                           */
   Heart_BPF_m2,       /* pointer to delay line 2                           */
   df2sos}; /* ptr to filter routine */
//-------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//DSC <->
//---------------------------------------------------------------------------
void df2sos
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


    for (i = 0; i < nValues; i++)
    {
        num = Filt->num;
        den = Filt->den;
        x = pInValues[i] * gain;
        for (j = 0; j < nosect; j++)
        {
            temp = ( x    - den[1] * m1[j] - den[2] * m2[j]) / den[0];
            s    = num[0] * temp + num[1] * m1[j] + num[2] * m2[j];
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

