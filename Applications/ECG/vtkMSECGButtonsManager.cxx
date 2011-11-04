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

// VTK includes
#include <vtkButtonWidget.h>
#include <vtkCallbackCommand.h>
#include <vtkCubeSource.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3DButtonRepresentation.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkWidgetEvent.h>

// MSVTK includes
#include "vtkMSECGButtonsManager.h"

class vtkMSECGButtonsManager::vtkInternal
{
public:
  vtkInternal(vtkMSECGButtonsManager* external);
  ~vtkInternal();

  void CreateButtonWidgets();
  void SetupButtonWidgets(vtkPolyDataReader*);
  void ClearButtons();

  struct ButtonProp : vtkObjectBase
    {
    ButtonProp();

    // vtkProp for the first state of our button
    vtkSmartPointer<vtkCubeSource> Cube;
    vtkSmartPointer<vtkPolyDataMapper>      CubeMapper;
    vtkSmartPointer<vtkActor>               CubeActor;
    };

  struct ButtonHandleReprensentation : vtkObjectBase
    {
    ButtonHandleReprensentation();

    vtkSmartPointer<ButtonProp> PropButton;
    vtkIdType LinkedPointID;
    };

  typedef std::map<vtkSmartPointer<vtkButtonWidget>,
                   vtkSmartPointer<ButtonHandleReprensentation> > SmartButtonWidgets;
  SmartButtonWidgets      ButtonsWidgets;
  vtkMSECGButtonsManager* External;
};

//---------------------------------------------------------------------------
// vtkInternal methods

//---------------------------------------------------------------------------
vtkMSECGButtonsManager::vtkInternal::vtkInternal(vtkMSECGButtonsManager* _external)
{
  this->External = _external;
}

//---------------------------------------------------------------------------
vtkMSECGButtonsManager::vtkInternal::~vtkInternal()
{
  this->ClearButtons();
}

//-----------------------------------------------------------------------------
vtkMSECGButtonsManager::vtkInternal::ButtonProp::ButtonProp()
{
  this->Cube = vtkCubeSource::New();
  this->CubeMapper = vtkPolyDataMapper::New();
  this->CubeMapper->SetInputConnection(Cube->GetOutputPort());
  this->CubeMapper->SetScalarRange(0,19);
  this->CubeActor = vtkActor::New();
  this->CubeActor->SetMapper(CubeMapper);
}

//-----------------------------------------------------------------------------
vtkMSECGButtonsManager::vtkInternal::ButtonHandleReprensentation::ButtonHandleReprensentation()
{
  this->PropButton = new ButtonProp();
  LinkedPointID = 0;
}

//-----------------------------------------------------------------------------
void vtkMSECGButtonsManager::vtkInternal::CreateButtonWidgets()
{
  if(this->External->numberOfButtonWidgets < 1 ||
     !this->External->renderer || !this->External->renderer->GetRenderWindow())
    {
    return;
    }

  // Define callback
  vtkSmartPointer<vtkCallbackCommand> widgetCallback =
    vtkSmartPointer<vtkCallbackCommand>::New();
  widgetCallback->SetCallback(vtkMSECGButtonsManager::ProcessWidgetsEvents);

  // Main loop instantiation
  for(int i=0; i<this->External->numberOfButtonWidgets; ++i)
    {
    // Instantiate the ButtonHandleRepresentation
    vtkSmartPointer<ButtonHandleReprensentation>
      buttonHandle = new ButtonHandleReprensentation();

    // Instantiate the ButtonRepresentation
    vtkNew<vtkProp3DButtonRepresentation > rep;
    rep->SetNumberOfStates(1);
    rep->SetButtonProp(0, buttonHandle->PropButton->CubeActor);
    rep->SetState(0);
    rep->SetPlaceFactor(1);
    rep->SetDragable(0);

    // The Manager has to manage the destruction of the widgets
    vtkNew<vtkButtonWidget> buttonWidget;
    buttonWidget->SetInteractor(this->External->renderer->GetRenderWindow()->GetInteractor());
    buttonWidget->SetRepresentation(rep.GetPointer());
    buttonWidget->SetEnabled(1);

    // Associate the current button to the callBackCommand
    buttonWidget->AddObserver(vtkCommand::StateChangedEvent, widgetCallback);

    this->ButtonsWidgets.insert(std::pair<vtkSmartPointer<vtkButtonWidget>,
      vtkSmartPointer<ButtonHandleReprensentation> >
      (buttonWidget.GetPointer(), buttonHandle.GetPointer()));
    }
}

//-----------------------------------------------------------------------------
void vtkMSECGButtonsManager::vtkInternal::SetupButtonWidgets(vtkPolyDataReader* polyDataReader)
{
  if (polyDataReader->GetOutput()->GetNumberOfPoints() < this->External->numberOfButtonWidgets)
  {
  return;
  }

  int index = 0;
  int numberOfPoints = polyDataReader->GetOutput()->GetNumberOfPoints();
  int step  = numberOfPoints / this->External->numberOfButtonWidgets; // Choose point spreaded

  for (SmartButtonWidgets::iterator it = this->ButtonsWidgets.begin();
       it != this->ButtonsWidgets.end(); ++it , ++index )
    {
    it->second->LinkedPointID = index * step;
    }
}

//-----------------------------------------------------------------------------
void vtkMSECGButtonsManager::vtkInternal::ClearButtons()
{
  // We Have to first delete the HandleReprensentation of each vtkButtonWidget
  for (SmartButtonWidgets::iterator it = this->ButtonsWidgets.begin();
       it != this->ButtonsWidgets.end(); ++it )
    {
    (*it->second).Delete();
    }

  this->ButtonsWidgets.clear();
}

//---------------------------------------------------------------------------
// vtkMRMLSliceModelDisplayableManager methods

//-----------------------------------------------------------------------------
vtkMSECGButtonsManager::vtkMSECGButtonsManager(int numberOfButtons) :
  numberOfButtonWidgets(numberOfButtons), buttonWidgetSize(5), renderer(0)
{
  this->Internal = new vtkInternal(this);
}

//-----------------------------------------------------------------------------
vtkMSECGButtonsManager::~vtkMSECGButtonsManager()
{
  delete this->Internal;
}

//-----------------------------------------------------------------------------
void vtkMSECGButtonsManager::Clear()
{
  this->Internal->ClearButtons();
}

//-----------------------------------------------------------------------------
void vtkMSECGButtonsManager::Init(vtkPolyDataReader* polyDataReader)
{
  if (!polyDataReader)
    {
    return;
    }

  this->Internal->CreateButtonWidgets();
  this->Internal->SetupButtonWidgets(polyDataReader);
}

//-----------------------------------------------------------------------------
void vtkMSECGButtonsManager::UpdateButtonWidgets(vtkPolyDataReader* polyDataReader)
{
  if (!polyDataReader ||
      !polyDataReader->GetOutput() ||
      this->Internal->ButtonsWidgets.empty() ||
      polyDataReader->GetOutput()->GetNumberOfPoints() <
      this->Internal->ButtonsWidgets.size() // rbegin()->second->LinkedPointID
      )
    {
    return;
    }

  double center[3];
  double size = this->buttonWidgetSize;
  vtkMSECGButtonsManager::vtkInternal::SmartButtonWidgets::iterator it;
  for (it = this->Internal->ButtonsWidgets.begin();
       it != this->Internal->ButtonsWidgets.end(); ++it)
    {
    it->first->GetRepresentation()->VisibilityOn();

    polyDataReader->GetOutput()->GetPoint(it->second->LinkedPointID,center);
    double bounds[6] = {bounds[0] = center[0] - size / 2,
                        bounds[1] = center[0] + size / 2,
                        bounds[2] = center[1] - size / 2,
                        bounds[3] = center[1] + size / 2,
                        bounds[4] = center[2] - size / 2,
                        bounds[5] = center[2] + size / 2};

    it->first->GetRepresentation()->PlaceWidget(bounds);
    }
}

//----------------------------------------------------------------------------
void vtkMSECGButtonsManager::ProcessWidgetsEvents(vtkObject *caller,
                                                  unsigned long vtkNotUsed(event),
                                                  void *vtkNotUsed(clientData),
                                                  void *vtkNotUsed(calldata))
{
  vtkButtonWidget* widget = vtkButtonWidget::SafeDownCast(caller);
  if(widget)
    widget->Print(std::cout);
}
