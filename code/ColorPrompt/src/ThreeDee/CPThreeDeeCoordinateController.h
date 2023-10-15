
#include "../mainC.h"
#include "CPThreeDeeController.h"

CM_PROTOTYPE(CPWhitePoints);
CM_PROTOTYPE(NASpace);



typedef struct CPThreeDeeCoordinateController CPThreeDeeCoordinateController;

CPThreeDeeCoordinateController* cmAllocThreeDeeCoordinateController(CPThreeDeeController* threeDeeController);
void cmDeallocThreeDeeCoordinateController(CPThreeDeeCoordinateController* con);

NASpace* cmGetThreeDeeCoordinateControllerUIElement(CPThreeDeeCoordinateController* con);

CoordSysType cmGetThreeDeeCoordinateControllerCoordSysType(CPThreeDeeCoordinateController* con);
CMLColorType cmGetThreeDeeCoordinateControllerColorSpaceType(CPThreeDeeCoordinateController* con);
NAInt cmGetThreeDeeCoordinateControllerSteps3D(CPThreeDeeCoordinateController* con);

void cmUpdateThreeDeeCoordinateController(CPThreeDeeCoordinateController* con);
