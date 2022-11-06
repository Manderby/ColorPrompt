
#include "CMColorConversionsYcdUVW.h"
#include "mainC.h"


// todo: add conversions to main window or even CML.

void convertYcdtoadaptedYuv(float* yuv, const float* Ycd, const float* srcWhitePointYcd, const float* dstWhitePointYcd){
  float cfactor = Ycd[1] * dstWhitePointYcd[1] / srcWhitePointYcd[1];
  float dfactor = Ycd[2] * dstWhitePointYcd[2] / srcWhitePointYcd[2];
  float divisor = (16.518f + 1.481f * cfactor - dfactor);
  yuv[0] = Ycd[0];
  yuv[1] = (10.872f + 0.404f * cfactor - 4.f * dfactor) / divisor;
  yuv[2] = 5.520f / divisor;
}

// ISO 3664 states in forumal D.14 the computation 6X/(X+15Y+3Z). I'm
// pretty sure, they meant  6Y/(X+15Y+3Z) which is according to
// CIE 1960 UCS. This also corresponds to the fact that UVW is based on UCS.
// In CML, this is Yuv.
// UVW is CIE 1964.
void convertYuvtoUVW(float* UVW, float* yuv, const float* whitePointYuv){
  UVW[2] = 25.f * cmlCbrt(yuv[0] * 100.f) - 17.f;
  UVW[0] = 13.f * UVW[2] * (yuv[1] - whitePointYuv[1]);
  UVW[1] = 13.f * UVW[2] * (yuv[2] - whitePointYuv[2]);
}

const NAUTF8Char* getGrade(float value){
  if(value <= .25f){return "A";}
  else if(value <= .5f){return "B";}
  else if(value <= 1.f){return "C";}
  else if(value <= 2.f){return "D";}
  return "E";
}
