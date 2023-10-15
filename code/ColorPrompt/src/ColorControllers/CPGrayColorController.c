
#include "CPColorController.h"

#include "../CPColorPromptApplication.h"
#include "../CPDesign.h"
#include "../CPTranslations.h"
#include "CPGrayColorController.h"
#include "Displays/CPColorWell1D.h"
#include "Displays/CPGrayColorWell.h"

#include "NAApp.h"

struct CPGrayColorController{
  CPColorController baseController;
  
  CPGrayColorWell* display;
  
  NASpace* channelSpace;
  NALabel* labelGray;
  NATextField* textFieldGray;
  CPColorWell1D* colorWell1DGray;

  float grayColor;
};



NABool cp_GrayValueEdited(NAReaction reaction){
  CPGrayColorController* con = (CPGrayColorController*)reaction.controller;
  
  if(reaction.uiElement == con->textFieldGray){
    con->grayColor = (float)naGetTextFieldDouble(con->textFieldGray);
  }
  
  cpSetCurrentColorController(&(con->baseController));
  cpUpdateColor();
  
  return NA_TRUE;
}



CPGrayColorController* cpAllocGrayColorController(void){
  CPGrayColorController* con = naAlloc(CPGrayColorController);
  
  cpInitColorController(&(con->baseController), CML_COLOR_Gray);
  
  con->display = cpAllocGrayColorWell(&(con->baseController));
  
  con->channelSpace = naNewSpace(naMakeSize(1, 1));
  con->labelGray = cpNewColorComponentLabel(cpTranslate(CPGrayColorChannelGr));
  con->textFieldGray = cpNewValueTextField(cp_GrayValueEdited, con);
  con->colorWell1DGray = cpAllocColorWell1D(&(con->baseController), &(con->grayColor), 0);

  cpBeginUILayout(con->channelSpace, naMakeBezel4Zero());
  cpAddUIPos(0, (int)((colorWell2DSize - (1 * 25.)) / 2.)); // center the channels
  cpAddUIRow(con->labelGray, colorValueCondensedRowHeight);
  cpAddUICol(con->textFieldGray, colorComponentMarginH);
  cpAddUIColV(cpGetColorWell1DUIElement(con->colorWell1DGray), 10, colorWell1DOffset);
  cpAddUIPos(0, 2 * colorValueCondensedRowHeight);
  cpEndUILayout();
  
  cpBeginUILayout(con->baseController.space, colorWellBezel);
  cpAddUIRow(cpGetGrayColorWellUIElement(con->display), 0);
  cpAddUICol(con->channelSpace, colorWell2DRightMargin);
  cpEndUILayout();

  con->grayColor = .5;

  return con;
}



void cpDeallocGrayColorController(CPGrayColorController* con){
  cpDeallocGrayColorWell(con->display);
  cpDeallocColorWell1D(con->colorWell1DGray);
  cpClearColorController(&(con->baseController));
  naFree(con);
}



const void* cpGetGrayColorControllerColorData(const CPGrayColorController* con){
  return &(con->grayColor);
}



void cpSetGrayColorControllerColorData(CPGrayColorController* con, const void* data){
  con->grayColor = *(const float*)data;
}



void cpUpdateGrayColorController(CPGrayColorController* con){
  cpUpdateColorController(&(con->baseController));

  CMLColorMachine* cm = cpGetCurrentColorMachine();
  CMLColorType currentColorType = cpGetCurrentColorType();
  const float* currentColorData = cpGetCurrentColorData();
  CMLColorConverter converter = cmlGetColorConverter(CML_COLOR_Gray, currentColorType);
  converter(cm, &(con->grayColor), currentColorData, 1);
  
  naSetTextFieldText(
    con->textFieldGray,
    naAllocSprintf(NA_TRUE, "%1.05f", con->grayColor));

  cpUpdateGrayColorWell(con->display);
  cpUpdateColorWell1D(con->colorWell1DGray);
}
