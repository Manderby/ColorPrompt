
#include "CMColorController.h"
#include "CMColorWell2D.h"
#include "CMDesign.h"
#include "NAApp.h"
#include "NAMath/NAVectorAlgebra.h"



struct CMColorWell2D{
  NAOpenGLSpace* display;
  
  GLuint wellTex;
  
  CMColorController* colorController;
  size_t fixedIndex;
};



void cmInitColorWell2D(void* data){
  CMColorWell2D* well = (CMColorWell2D*)data;
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glGenTextures(1, &(well->wellTex));
  glBindTexture(GL_TEXTURE_2D, well->wellTex);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}



NABool cmDragColorWell2D(NAReaction reaction){
  CMColorWell2D* well = (CMColorWell2D*)reaction.controller;
 
  const NAMouseStatus* mouseStatus = naGetMouseStatus();
  if(mouseStatus->leftPressed){
    CMLColorType colorType = cmGetColorControllerColorType(well->colorController);
    CMLNormedConverter outputConverter = cmlGetNormedCartesianOutputConverter(colorType);
    CMLNormedConverter inputConverter = cmlGetNormedCartesianInputConverter(colorType);
    CMLColorMutator clamper = cmlGetClamper(colorType);

    CMLVec3 normedColorValues;
    outputConverter(normedColorValues, cmGetColorControllerColorData(well->colorController), 1);

    NARect displayRect = naGetUIElementRect(well->display, naGetApplication(), NA_FALSE);
    switch(well->fixedIndex){
    case 0:
      normedColorValues[1] = (mouseStatus->pos.x - displayRect.pos.x) / displayRect.size.width;
      normedColorValues[2] = (mouseStatus->pos.y - displayRect.pos.y) / displayRect.size.height;
      break;
    case 1:
      normedColorValues[0] = (mouseStatus->pos.x - displayRect.pos.x) / displayRect.size.width;
      normedColorValues[2] = (mouseStatus->pos.y - displayRect.pos.y) / displayRect.size.height;
      break;
    case 2:
      normedColorValues[0] = (mouseStatus->pos.x - displayRect.pos.x) / displayRect.size.width;
      normedColorValues[1] = (mouseStatus->pos.y - displayRect.pos.y) / displayRect.size.height;
      break;
    }

    CMLVec3 newColorValues;
    inputConverter(newColorValues, normedColorValues, 1);
    clamper(newColorValues, 1);
    
    cmSetColorControllerColorData(well->colorController, newColorValues);
    cmSetCurrentColorController(well->colorController);
    cmUpdateColor();

    return NA_TRUE;
  }
  
  return NA_FALSE;
}



NABool cmDrawColorWell2D(NAReaction reaction){
  CMColorWell2D* well = (CMColorWell2D*)reaction.controller;
  CMLColorMachine* cm = cmGetCurrentColorMachine();
  CMLColorMachine* sm = cmGetCurrentScreenMachine();

  NASize viewSize = naGetUIElementRect(well->display, NA_NULL, NA_FALSE).size;
  glViewport(0, 0, (GLsizei)viewSize.width, (GLsizei)viewSize.height);

  glClear(GL_DEPTH_BUFFER_BIT);

  CMLColorType colorType = cmGetColorControllerColorType(well->colorController);
  CMLNormedConverter outputConverter = cmlGetNormedCartesianOutputConverter(colorType);
  CMLNormedConverter inputConverter = cmlGetNormedCartesianInputConverter(colorType);

  float inputValues[colorWell2DSize * colorWell2DSize * 3];
  float* inputPtr = inputValues;
  CMLVec3 normedColorValues;
  outputConverter(normedColorValues, cmGetColorControllerColorData(well->colorController), 1);
  
  float fixedValueA;
  float fixedValueB;

  switch(well->fixedIndex){
  case 0:
    for(int y = 0; y < colorWell2DSize; ++y){
      float yValue = (float)y / (float)colorWell2DSize;
      for(int x = 0; x < colorWell2DSize; ++x){
        fixedValueA = normedColorValues[1];
        fixedValueB = normedColorValues[2];
        float xValue = (float)x / (float)colorWell2DSize;
        *inputPtr++ = normedColorValues[0];
        *inputPtr++ = xValue;
        *inputPtr++ = yValue;
      }
    }
    break;
  case 1:
    for(int y = 0; y < colorWell2DSize; ++y){
      float yValue = (float)y / (float)colorWell2DSize;
      for(int x = 0; x < colorWell2DSize; ++x){
        fixedValueA = normedColorValues[0];
        fixedValueB = normedColorValues[2];
        float xValue = (float)x / (float)colorWell2DSize;
        *inputPtr++ = xValue;
        *inputPtr++ = normedColorValues[1];
        *inputPtr++ = yValue;
      }
    }
    break;
  case 2:
    for(int y = 0; y < colorWell2DSize; ++y){
      float yValue = (float)y / (float)colorWell2DSize;
      for(int x = 0; x < colorWell2DSize; ++x){
        fixedValueA = normedColorValues[0];
        fixedValueB = normedColorValues[1];
        float xValue = (float)x / (float)colorWell2DSize;
        *inputPtr++ = xValue;
        *inputPtr++ = yValue;
        *inputPtr++ = normedColorValues[2];
      }
    }
    break;
  }

  // Convert the given values to screen RGBs.
  float rgbValues[colorWell2DSize * colorWell2DSize * 3];
  fillRGBFloatArrayWithArray(
    cm,
    sm,
    rgbValues,
    inputValues,
    colorType,
    inputConverter,
    colorWell2DSize * colorWell2DSize,
    NA_FALSE,
    NA_FALSE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, colorWell2DSize, colorWell2DSize, 0, GL_RGB, GL_FLOAT, rgbValues);

  glEnable(GL_TEXTURE_2D);
  glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0., 0.);
    glVertex2f(-1., -1.);
    glTexCoord2f(0., 1.);
    glVertex2f(-1., +1.);
    glTexCoord2f(1., 0.);
    glVertex2i(+1., -1.);
    glTexCoord2f(1., 1.);
    glVertex2i(+1., +1.);
  glEnd();

  const float whiteR = 2. * 4. / (float)colorWell2DSize;
  const float blackR = 2. * 5. / (float)colorWell2DSize;
  const int subdivisions = 16;

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH);

  // Draw the spectrum
  if((colorType == CML_COLOR_Yupvp) || (colorType == CML_COLOR_Yxy)){
    float imin = CML_DEFAULT_INTEGRATION_MIN;
    float imax = CML_DEFAULT_INTEGRATION_MAX;
    int32 intervals = (int32)((imax - imin) / CML_DEFAULT_INTEGRATION_STEPSIZE) + 1;
    
    CMLColorConverter coordConverter = cmlGetColorConverter(colorType, CML_COLOR_XYZ);
    CMLNormedConverter normedConverter = cmlGetNormedOutputConverter(colorType);

    glBegin(GL_LINE_STRIP);
    for(int32 iStep = 0; iStep <= intervals; iStep++){
      float l = imin + (((imax - imin) * iStep) / intervals);
      CMLVec3 curXYZ;
      cmlGetSpectralXYZColor(cm, curXYZ, l);
      if(curXYZ[1] > 0.f){

        CMLVec3 curRGB;
        CMLVec3 curcoords;
        CMLVec3 curNormedCoords;
        coordConverter(cm, curcoords, curXYZ, 1);
        normedConverter(curNormedCoords, curcoords, 1);
        cmlXYZToRGB(cm, curRGB, curXYZ, 1);
        cmlMul3(curRGB, .7f);
        cmlClampRGB(curRGB, 1);
        cmlMul3(curRGB, .75f);
        glColor3fv(curRGB);

        switch(well->fixedIndex){
        case 0:
          glVertex2f(curNormedCoords[1] * 2. - 1., curNormedCoords[2] * 2. - 1.);
          break;
        case 1:
          glVertex2f(curNormedCoords[0] * 2. - 1., curNormedCoords[2] * 2. - 1.);
          break;
        case 2:
          glVertex2f(curNormedCoords[0] * 2. - 1., curNormedCoords[1] * 2. - 1.);
          break;
        }
      }
    }
    glEnd();
  }

  glBegin(GL_LINE_LOOP);
    glColor4f(1., 1., 1., 1.);
    for(int i = 0; i < subdivisions; ++i){
      float ang = NA_PI2 * (float)i / (float)subdivisions;
      glVertex2f(fixedValueA * 2. - 1. + whiteR * naCos(ang), fixedValueB * 2. - 1. + whiteR * naSin(ang));
    }
    glColor4f(0., 0., 0., 1.);
    for(int i = 0; i < subdivisions; ++i){
      float ang = NA_PI2 * (float)i / (float)subdivisions;
      glVertex2f(fixedValueA * 2. - 1. + blackR * naCos(ang), fixedValueB * 2. - 1. + blackR * naSin(ang));
    }
  glEnd();

  naSwapOpenGLSpaceBuffer(well->display);

  return NA_TRUE;
}



CMColorWell2D* cmAllocColorWell2D(CMColorController* colorController, size_t fixedIndex){
  CMColorWell2D* well = naAlloc(CMColorWell2D);
  
  well->display = naNewOpenGLSpace(naMakeSize(colorWell2DSize, colorWell2DSize), cmInitColorWell2D, well);
  naAddUIReaction(well->display, NA_UI_COMMAND_REDRAW, cmDrawColorWell2D, well);
  naAddUIReaction(well->display, NA_UI_COMMAND_MOUSE_DOWN, cmDragColorWell2D, well);
  naAddUIReaction(well->display, NA_UI_COMMAND_MOUSE_MOVED, cmDragColorWell2D, well);
  
  well->colorController = colorController;
  well->fixedIndex = fixedIndex;
  
  return well;
}



void cmDeallocColorWell2D(CMColorWell2D* well){
  glDeleteTextures(1, &(well->wellTex));
}



NAOpenGLSpace* cmGetColorWell2DUIElement(CMColorWell2D* well){
  return well->display;
}



void cmUpdateColorWell2D(CMColorWell2D* well){
  naRefreshUIElement(well->display, 0.);
}
