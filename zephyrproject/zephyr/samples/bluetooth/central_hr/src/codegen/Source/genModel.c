/* Automatically generated source file */
#include <float.h>
#include "arm_nnfunctions.h"

#include "genNN.h"
#include "genModel.h"

#include "tinyengine_function.h"
//#include "tinyengine_function_fp.h"


/* Variables used by all ops */
ADD_params add_params;
//Conv_Params conv_params;
//Depthwise_Params dpconv_params;
int i;
int8_t *int8ptr;
float *fptr,*fptr2,*fptr3;

signed char* getInput() {
    return &buffer0[4608];
}
signed char* getOutput() {
    return NNoutput;
}
void end2endinference(q7_t* img){
    invoke(NULL);
}
void invoke(float* labels){
/* layer 0:CONV_2D */
convolve_s8_kernel3_stride1_pad1_fpreq(&buffer0[4608],5,5,2,(const q7_t*) weight0,bias0,scales0,7,13,-128,127,&buffer0[0],3,3,512,sbuf,-13);
/* layer 1:MAX_POOL_2D */
max_pooling(&buffer0[0],3,3,512,2,2,1,1,-128,127,&buffer0[4608]);
/* layer 2:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[2048],1,1,512,(const q7_t*) weight1,bias1,scales1,-128,-7,-128,127,&buffer0[1024],1,1,512,sbuf);
/* layer 3:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[1024],1,1,512,(const q7_t*) weight2,bias2,scales2,-128,128,-128,127,&buffer0[0],1,1,1024,sbuf);
/* layer 4:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],1,1,1024,(const q7_t*) weight3,bias3,scales3,-128,128,-128,127,&buffer0[1024],1,1,1024,sbuf);
/* layer 5:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[1024],1,1,1024,(const q7_t*) weight4,bias4,scales4,-128,128,-128,127,&buffer0[0],1,1,512,sbuf);
/* layer 6:CONV_2D */
convolve_1x1_s8_oddch_fpreq(&buffer0[0],1,1,512,(const q7_t*) weight5,bias5,scales5,-128,128,-128,127,&buffer0[512],1,1,1,sbuf);
}
