/*==============================================================================

  Library: MSVECG

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Michael Jeulin-Lagarrigue, Kitware Inc.
==============================================================================*/

#ifndef __vtkMSECGButtonsManager_h
#define __vtkMSECGButtonsManager_h

// STD includes
#include <list>
#include <map>
#include <vector>

// VTK includes
#include "vtkButtonWidget.h"
#include "vtkPolyDataReader.h"
#include "vtkSmartPointer.h"

class vtkActor;
class vtkPlatonicSolidSource;
class vtkPolyDataMapper;
class vtkPolyDataReader;

class  vtkMSECGButtonsManager
{
public:
  vtkMSECGButtonsManager(int numberOfButtons = 2);
  ~vtkMSECGButtonsManager();

  /// Set Renderer to process widget
  void SetRenderer(vtkRenderer* ren) {this->renderer = ren;}

  /// Accessors to control the size of our reprensentation
  void SetButtonWidgetSize(double size) {this->buttonWidgetSize = size;}
  double GetbuttonWidgetSize() const {return this->buttonWidgetSize;}

  /// Accessors to control the number of widget buttons in our scene
  void SetNumberOfButtonWidgets(int n) {this->numberOfButtonWidgets = n;}
  int GetNumberOfButtonWidgets() const {return this->numberOfButtonWidgets;}

  /// WidgetCallback is a static function to relay modified events from the Logic
  static void ProcessWidgetsEvents(vtkObject *caller, unsigned long event, void *clientData, void *callData);

  /// Initialize the vtkButtonsWidget corresponding to the ECG data
  void Init(vtkPolyDataReader*);

  /// Clear Buttons Manager
  virtual void Clear();

  void UpdateButtonWidgets(vtkPolyDataReader*);

protected:
  int           numberOfButtonWidgets;
  double        buttonWidgetSize;
  vtkRenderer*  renderer;

private:
  vtkMSECGButtonsManager(const vtkMSECGButtonsManager&);  // Not implemented.
  void operator=(const vtkMSECGButtonsManager&);          // Not implemented.

  class vtkInternal;
  vtkInternal* Internal;
};
#endif
