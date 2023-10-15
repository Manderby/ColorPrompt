
#include "CPThreeDeePerspectiveController.h"

#include "../CPDesign.h"
#include "../CPTranslations.h"

#include "CML.h"

#include "NAUtility/NABinaryData.h"
#include "NAUtility/NAMemory.h"
#include "NAUtility/NAString.h"

struct CPThreeDeePerspectiveController{
  NASpace* space;
  CPThreeDeeController* parent;

  NALabel* rotationLabel;
  NAButton* rotationButton;
  NASlider* rotationSlider;

  double rotationStep;
  double anglePol;
  double angleEqu;
  double zoom;
};



void cmStepRotation(void* data){
  CPThreeDeePerspectiveController* con = (CPThreeDeePerspectiveController*)data;
  
  if(con->rotationStep != 0.){
    con->angleEqu -= con->rotationStep * .015f;
    naCallApplicationFunctionInSeconds(cmStepRotation, data, 1./60.);
    cpUpdateThreeDeeController(con->parent);
  }
}



NABool cmPressRotationButton(NAReaction reaction){
  CPThreeDeePerspectiveController* con = (CPThreeDeePerspectiveController*)reaction.controller;

  if(reaction.uiElement == con->rotationButton){
    con->rotationStep = 0.;
  }

  cpUpdateThreeDeeController(con->parent);

  return TRUE;
}



NABool cmChangeRotationSlider(NAReaction reaction){
  CPThreeDeePerspectiveController* con = (CPThreeDeePerspectiveController*)reaction.controller;

  if(reaction.uiElement == con->rotationSlider){
    NABool needsRotationStart = (con->rotationStep == 0.);
    con->rotationStep = (float)naGetSliderValue(con->rotationSlider);
    if(con->rotationStep < .1 && con->rotationStep > -.1){con->rotationStep = 0.;}
    if(needsRotationStart){cmStepRotation(con);}
  }
  
  cpUpdateThreeDeeController(con->parent);

  return TRUE;
}



void cmFixThreeDeeViewParameters(CPThreeDeePerspectiveController* con){
  if(con->zoom <= .025f){con->zoom = .025f;}
  if(con->zoom >= 2.f){con->zoom = 2.f;}
  if(con->angleEqu < -NA_PIf){con->angleEqu += NA_PI2f;}
  if(con->angleEqu > NA_PIf){con->angleEqu -= NA_PI2f;}
  if(con->anglePol <= NA_SINGULARITYf){con->anglePol = NA_SINGULARITYf;}
  if(con->anglePol >= NA_PIf - NA_SINGULARITYf){con->anglePol = NA_PIf - NA_SINGULARITYf;}
}



NABool cmMoveRotationMouse(NAReaction reaction){
  CPThreeDeePerspectiveController* con = (CPThreeDeePerspectiveController*)reaction.controller;

  const NAMouseStatus* status = naGetMouseStatus();
  if(status->leftPressed){
    
    NAPos mouseDiff = naMakePos(status->pos.x - status->prevPos.x, status->pos.y - status->prevPos.y);
    double scaleFactor = cpGetUIScaleFactorForWindow(naGetUIElementNativePtr(con->space));

    con->angleEqu -= (float)(mouseDiff.x * .01 * scaleFactor);
    con->anglePol += (float)(mouseDiff.y * .01 * scaleFactor);

    cmFixThreeDeeViewParameters(con);
    cmRefreshThreeDeeDisplay(con->parent);
  }
  return NA_TRUE;
}



NABool cmScrollRotation(NAReaction reaction){
  CPThreeDeePerspectiveController* con = (CPThreeDeePerspectiveController*)reaction.controller;

  const NAMouseStatus* status = naGetMouseStatus();
  
  con->zoom *= 1.f + (float)(status->pos.y - status->prevPos.y) * .01f;
  cmFixThreeDeeViewParameters(con);
  cmRefreshThreeDeeDisplay(con->parent);

  return NA_TRUE;
}



CPThreeDeePerspectiveController* cpAllocThreeDeePerspectiveController(CPThreeDeeController* parent){
  CPThreeDeePerspectiveController* con = naAlloc(CPThreeDeePerspectiveController);

  con->parent = parent;

  con->space = naNewSpace(naMakeSize(1, 1));
  naSetSpaceAlternateBackground(con->space, NA_FALSE);

  con->rotationLabel = naNewLabel(cpTranslate(CPRotation), threeDeeRotationLabelWidth);
  con->rotationSlider = naNewSlider(threeDeeControlWidth);
  con->rotationButton = naNewTextPushButton(cpTranslate(CPStop), 70);
  naSetSliderRange(con->rotationSlider, -1., +1., 0);
  naAddUIReaction(con->rotationButton, NA_UI_COMMAND_PRESSED, cmPressRotationButton, con);
  naAddUIReaction(con->rotationSlider, NA_UI_COMMAND_EDITED, cmChangeRotationSlider, con);

  // layout

  cpBeginUILayout(con->space, threeDeeBezel);
  
  cpAddUIRow(con->rotationLabel, uiElemHeight);
  cpAddUICol(con->rotationButton, 0.);
  cpAddUICol(con->rotationSlider, 0.);

  cpEndUILayout();

  // initial values
  float scaleFactor = (float)cpGetUIScaleFactorForWindow(naGetUIElementNativePtr(con->space));

  con->rotationStep = 0.;
  con->anglePol = 1.3f;
  con->angleEqu = NA_PIf / 4.f;
  con->zoom = 1.f / scaleFactor;

  return con;
}



void cpDeallocThreeDeePerspectiveController(CPThreeDeePerspectiveController* con){
  naFree(con);
}



NASpace* cpGetThreeDeePerspectiveControllerUIElement(CPThreeDeePerspectiveController* con){
  return con->space;
}



double cpGetThreeDeePerspectiveControllerRotationStep(CPThreeDeePerspectiveController* con){
  return con->rotationStep;
}
double cpGetThreeDeePerspectiveControllerRotationAnglePol(CPThreeDeePerspectiveController* con){
  return con->anglePol;
}
double cpGetThreeDeePerspectiveControllerRotationAngleEqu(CPThreeDeePerspectiveController* con){
  return con->angleEqu;
}
double cpGetThreeDeePerspectiveControllerZoom(CPThreeDeePerspectiveController* con){
  return con->zoom;
}
void cpSetThreeDeePerspectiveControllerZoom(CPThreeDeePerspectiveController* con, double zoom){
  con->zoom = zoom;
}



void cpUpdateThreeDeePerspectiveController(CPThreeDeePerspectiveController* con)
{
  naSetSliderValue(con->rotationSlider, con->rotationStep);
}
