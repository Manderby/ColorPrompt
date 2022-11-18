
#include "CMColorController.h"
#include "CMColorWell1D.h"
#include "CMColorWell2D.h"
#include "CMHSVColorController.h"
#include "NAApp.h"
#include "CMDesign.h"

struct CMHSVColorController{
  CMColorController baseController;
  
  CMColorWell2D* colorWell2D;

  NALabel* labelH;
  NALabel* labelS;
  NALabel* labelV;
  NATextField* textFieldH;
  NATextField* textFieldS;
  NATextField* textFieldV;
  CMColorWell1D* colorWell1DH;
  CMColorWell1D* colorWell1DS;
  CMColorWell1D* colorWell1DV;

  CMLVec3 hsvColor;
};



NABool cmHSVValueEdited(NAReaction reaction){
  CMHSVColorController* con = (CMHSVColorController*)reaction.controller;
  
  if(reaction.uiElement == con->textFieldH){
    NAString* string = naNewStringWithTextFieldText(con->textFieldH);
    con->hsvColor[0] = atof(naGetStringUTF8Pointer(string));
    naDelete(string);
  }else if(reaction.uiElement == con->textFieldS){
    NAString* string = naNewStringWithTextFieldText(con->textFieldS);
    con->hsvColor[1] = atof(naGetStringUTF8Pointer(string));
    naDelete(string);
  }else if(reaction.uiElement == con->textFieldV){
    NAString* string = naNewStringWithTextFieldText(con->textFieldV);
    con->hsvColor[2] = atof(naGetStringUTF8Pointer(string));
    naDelete(string);
  }
  
  cmSetCurrentColorController(&(con->baseController));
  cmUpdateColor();
  
  return NA_TRUE;
}



CMHSVColorController* cmAllocHSVColorController(void){
  CMHSVColorController* con = naAlloc(CMHSVColorController);
  
  cmInitColorController(&(con->baseController), CML_COLOR_HSV);
  
  con->colorWell2D = cmAllocColorWell2D(&(con->baseController), 2);

  con->labelH = cmNewColorComponentLabel("H");
  con->labelS = cmNewColorComponentLabel("S");
  con->labelV = cmNewColorComponentLabel("V");
  con->textFieldH = cmNewValueTextField(cmHSVValueEdited, con);
  con->textFieldS = cmNewValueTextField(cmHSVValueEdited, con);
  con->textFieldV = cmNewValueTextField(cmHSVValueEdited, con);
  con->colorWell1DH = cmAllocColorWell1D(&(con->baseController), CML_COLOR_HSV, con->hsvColor, 0);
  con->colorWell1DS = cmAllocColorWell1D(&(con->baseController), CML_COLOR_HSV, con->hsvColor, 1);
  con->colorWell1DV = cmAllocColorWell1D(&(con->baseController), CML_COLOR_HSV, con->hsvColor, 2);

  naSetUIElementNextTabElement(con->textFieldH, con->textFieldS);
  naSetUIElementNextTabElement(con->textFieldS, con->textFieldV);
  naSetUIElementNextTabElement(con->textFieldV, con->textFieldH);

  cmBeginUILayout(con->baseController.space, colorWellBezel);
  cmAddUIPos(0, colorValueCondensedRowHeight);
  cmAddUIRow(con->labelH, colorValueCondensedRowHeight);
  cmAddUICol(con->textFieldH, colorComponentMarginH);
  cmAddUIColV(cmGetColorWell1DUIElement(con->colorWell1DH), 10, colorWell1DOffset);
  cmAddUIRow(con->labelS, colorValueCondensedRowHeight);
  cmAddUICol(con->textFieldS, colorComponentMarginH);
  cmAddUIColV(cmGetColorWell1DUIElement(con->colorWell1DS), 10, colorWell1DOffset);
  cmAddUIRow(con->labelV, colorValueCondensedRowHeight);
  cmAddUICol(con->textFieldV, colorComponentMarginH);
  cmAddUIColV(cmGetColorWell1DUIElement(con->colorWell1DV), 10, colorWell1DOffset);
  cmAddUIPos(0, colorValueCondensedRowHeight);
  cmEndUILayout();
  
  naAddSpaceChild(
    con->baseController.space,
    cmGetColorWell2DUIElement(con->colorWell2D),
    naMakePos(10, 5));

  return con;
}



void cmDeallocHSVColorController(CMHSVColorController* con){
  cmClearColorController(&(con->baseController));
  naFree(con);
}



const void* cmGetHSVColorControllerColorData(const CMHSVColorController* con){
  return &(con->hsvColor);
}



void cmSetHSVColorControllerColorData(CMHSVColorController* con, const void* data){
  cmlCpy3(con->hsvColor, data);
}



void cmUpdateHSVColorController(CMHSVColorController* con){
  cmUpdateColorController(&(con->baseController));

  CMLColorMachine* cm = cmGetCurrentColorMachine();
  CMLColorType currentColorType = cmGetCurrentColorType();
  const float* currentColorData = cmGetCurrentColorData();
  CMLColorConverter converter = cmlGetColorConverter(CML_COLOR_HSV, currentColorType);
  converter(cm, con->hsvColor, currentColorData, 1);
  
  cmUpdateColorWell2D(con->colorWell2D);

  naSetTextFieldText(
    con->textFieldH,
    naAllocSprintf(NA_TRUE, "%3.03f", con->hsvColor[0]));
  naSetTextFieldText(
    con->textFieldS,
    naAllocSprintf(NA_TRUE, "%1.05f", con->hsvColor[1]));
  naSetTextFieldText(
    con->textFieldV,
    naAllocSprintf(NA_TRUE, "%1.05f", con->hsvColor[2]));

  cmUpdateColorWell1D(con->colorWell1DH);
  cmUpdateColorWell1D(con->colorWell1DS);
  cmUpdateColorWell1D(con->colorWell1DV);
}
