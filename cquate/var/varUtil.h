
#pragma once

#include "variable.h"

// display to cmd line
bool QuietShow(cContainer& var);
bool QuietShow(cMatrix& var);
bool QuietShow(cCube& var);
bool QuietShow(cString& var);
bool QuietShow(cContainer::cUserHash& var);

bool Show(cContainer& var);
bool Show(cMatrix& var);
bool Show(cCube& var);
bool Show(cString& var);
bool Show(cContainer::cUserHash& var);
bool History();

bool Figure(cMatrix& v);
bool CurrentFigure(cMatrix& v);
bool Which(cString& v);
bool Scripts();
bool Fcns();
bool OpenFiles();
bool OpenFileIds();

