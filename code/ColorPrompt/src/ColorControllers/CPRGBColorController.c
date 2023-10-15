
#include "CPColorController.h"

#include "../CPColorPromptApplication.h"
#include "../CPDesign.h"
#include "../CPTranslations.h"
#include "Displays/CPColorWell1D.h"
#include "Displays/CPColorWell2D.h"
#include "CPRGBColorController.h"

#include "NAApp.h"

#if NA_OS == NA_OS_WINDOWS
  #define cmsscanf sscanf_s
#else
  #define cmsscanf sscanf
#endif

struct CPRGBColorController{
  CPColorController baseController;
  
  CPColorWell2D* colorWell2D;
  
  NASpace* channelSpace;
  NALabel* labelR;
  NALabel* labelG;
  NALabel* labelB;
  NATextField* textFieldR;
  NATextField* textFieldG;
  NATextField* textFieldB;
  CPColorWell1D* colorWell1DR;
  CPColorWell1D* colorWell1DG;
  CPColorWell1D* colorWell1DB;

  NALabel* labelNum;
  NATextField* textFieldHex;
  NATextField* textFieldDec;

  CMLVec3 rgbColor;
};



NABool cmRGBValueEdited(NAReaction reaction){
  CPRGBColorController* con = (CPRGBColorController*)reaction.controller;
  CMLColorMachine* cm = cpGetCurrentColorMachine();

  if(reaction.uiElement == con->textFieldR){
    con->rgbColor[0] = (float)naGetTextFieldDouble(con->textFieldR);
  }else if(reaction.uiElement == con->textFieldG){
    con->rgbColor[1] = (float)naGetTextFieldDouble(con->textFieldG);
  }else if(reaction.uiElement == con->textFieldB){
    con->rgbColor[2] = (float)naGetTextFieldDouble(con->textFieldB);
  }else if(reaction.uiElement == con->textFieldHex){
    NAString* string = naNewStringWithTextFieldText(con->textFieldHex);
    int rgbbuf[3] = {0, 0, 0};
    cmsscanf(naGetStringUTF8Pointer(string), "%02x%02x%02x", &(rgbbuf[0]), &(rgbbuf[1]), &(rgbbuf[2]));
    uint8 rgbbuf2[3] = {rgbbuf[0], rgbbuf[1], rgbbuf[2]};
    cmlData8ToRGB(cm, con->rgbColor, rgbbuf2, 1);
    naDelete(string);
  }else if(reaction.uiElement == con->textFieldDec){
    NAString* string = naNewStringWithTextFieldText(con->textFieldDec);
    int rgbbuf[3] = {0, 0, 0};
    int assigned = cmsscanf(naGetStringUTF8Pointer(string), "%d, %d, %d", &(rgbbuf[0]), &(rgbbuf[1]), &(rgbbuf[2]));
    if(assigned < 3){
      cmsscanf(naGetStringUTF8Pointer(string), "%d %d %d", &(rgbbuf[0]), &(rgbbuf[1]), &(rgbbuf[2]));
    }
    uint8 rgbbuf2[3] = {rgbbuf[0], rgbbuf[1], rgbbuf[2]};
    cmlData8ToRGB(cm, con->rgbColor, rgbbuf2, 1);
    naDelete(string);
  }
  
  cpSetCurrentColorController(&(con->baseController));
  cpUpdateColor();
  
  return NA_TRUE;
}


CPRGBColorController* cmAllocRGBColorController(void){
  CPRGBColorController* con = naAlloc(CPRGBColorController);
  
  cmInitColorController(&(con->baseController), CML_COLOR_RGB);
  
  con->colorWell2D = cmAllocColorWell2D(&(con->baseController), 1);
  
  con->channelSpace = naNewSpace(naMakeSize(1, 1));
  con->labelR = cpNewColorComponentLabel(cpTranslate(CPRGBColorChannelR));
  con->labelG = cpNewColorComponentLabel(cpTranslate(CPRGBColorChannelG));
  con->labelB = cpNewColorComponentLabel(cpTranslate(CPRGBColorChannelB));
  con->textFieldR = cpNewValueTextField(cmRGBValueEdited, con);
  con->textFieldG = cpNewValueTextField(cmRGBValueEdited, con);
  con->textFieldB = cpNewValueTextField(cmRGBValueEdited, con);
  con->colorWell1DR = cmAllocColorWell1D(&(con->baseController), con->rgbColor, 0);
  con->colorWell1DG = cmAllocColorWell1D(&(con->baseController), con->rgbColor, 1);
  con->colorWell1DB = cmAllocColorWell1D(&(con->baseController), con->rgbColor, 2);
  
  con->labelNum = cpNewColorComponentLabel(cpTranslate(CPRGBColorChannelNum));
  con->textFieldHex = cpNewValueTextField(cmRGBValueEdited, con);
  con->textFieldDec = cpNewBigValueTextField(cmRGBValueEdited, con);

  naSetUIElementNextTabElement(con->textFieldR, con->textFieldG);
  naSetUIElementNextTabElement(con->textFieldG, con->textFieldB);
  naSetUIElementNextTabElement(con->textFieldB, con->textFieldR);
  naSetUIElementNextTabElement(con->textFieldHex, con->textFieldDec);
  naSetUIElementNextTabElement(con->textFieldDec, con->textFieldHex);

  cpBeginUILayout(con->channelSpace, naMakeBezel4Zero());
  cpAddUIPos(0, (int)((colorWell2DSize - (4 * 25. + 10.)) / 2.)); // center the channels
  cpAddUIRow(con->labelR, colorValueCondensedRowHeight);
  cpAddUICol(con->textFieldR, colorComponentMarginH);
  cpAddUIColV(cmGetColorWell1DUIElement(con->colorWell1DR), 10, colorWell1DOffset);
  cpAddUIRow(con->labelG, colorValueCondensedRowHeight);
  cpAddUICol(con->textFieldG, colorComponentMarginH);
  cpAddUIColV(cmGetColorWell1DUIElement(con->colorWell1DG), 10, colorWell1DOffset);
  cpAddUIRow(con->labelB, colorValueCondensedRowHeight);
  cpAddUICol(con->textFieldB, colorComponentMarginH);
  cpAddUIColV(cmGetColorWell1DUIElement(con->colorWell1DB), 10, colorWell1DOffset);

  cpAddUIPos(0, 10);
  cpAddUIRow(con->labelNum, colorValueCondensedRowHeight);
  cpAddUICol(con->textFieldHex, colorComponentMarginH);
  cpAddUICol(con->textFieldDec, 10);
  cpEndUILayout();

  cpBeginUILayout(con->baseController.space, colorWellBezel);
  cpAddUIRow(cmGetColorWell2DUIElement(con->colorWell2D), 0);
  cpAddUICol(con->channelSpace, colorWell2DRightMargin);
  cpEndUILayout();
  
  return con;
}



void cmDeallocRGBColorController(CPRGBColorController* con){
  cmDeallocColorWell2D(con->colorWell2D);
  cmDeallocColorWell1D(con->colorWell1DR);
  cmDeallocColorWell1D(con->colorWell1DG);
  cmDeallocColorWell1D(con->colorWell1DB);
  cmClearColorController(&(con->baseController));
  naFree(con);
}



const void* cmGetRGBColorControllerColorData(const CPRGBColorController* con){
  return &(con->rgbColor);
}



void cmSetRGBColorControllerColorData(CPRGBColorController* con, const void* data){
  cmlCpy3(con->rgbColor, data);
}



void cmUpdateRGBColorController(CPRGBColorController* con){
  cpUpdateColorController(&(con->baseController));
  
  CMLColorMachine* cm = cpGetCurrentColorMachine();
  CMLColorType currentColorType = cpGetCurrentColorType();
  const float* currentColorData = cpGetCurrentColorData();
  CMLColorConverter converter = cmlGetColorConverter(CML_COLOR_RGB, currentColorType);
  converter(cm, con->rgbColor, currentColorData, 1);
  
  cpUpdateColorWell2D(con->colorWell2D);
  
  naSetTextFieldText(
    con->textFieldR,
    naAllocSprintf(NA_TRUE, "%1.05f", con->rgbColor[0]));
  naSetTextFieldText(
    con->textFieldG,
    naAllocSprintf(NA_TRUE, "%1.05f", con->rgbColor[1]));
  naSetTextFieldText(
    con->textFieldB,
    naAllocSprintf(NA_TRUE, "%1.05f", con->rgbColor[2]));

  CMLVec3 clamped;
  cmlCpy3(clamped, con->rgbColor);
  cmlClampRGB(clamped, 1);
  uint8 rgbbuf[3] = {0, 0, 0};
  cmlRGBToData8(cm, rgbbuf, clamped, 1);
  naSetTextFieldText(
    con->textFieldHex,
    naAllocSprintf(NA_TRUE, "%02x%02x%02x", (int)rgbbuf[0], (int)rgbbuf[1], (int)rgbbuf[2]));
  naSetTextFieldText(
    con->textFieldDec,
    naAllocSprintf(NA_TRUE, "%d, %d, %d", (int)rgbbuf[0], (int)rgbbuf[1], (int)rgbbuf[2]));

  cpUpdateColorWell1D(con->colorWell1DR);
  cpUpdateColorWell1D(con->colorWell1DG);
  cpUpdateColorWell1D(con->colorWell1DB);
}