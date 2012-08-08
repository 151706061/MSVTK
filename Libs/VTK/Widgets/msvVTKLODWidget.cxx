/*==============================================================================

  Program: MSVTK

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

==============================================================================*/

#include "msvVTKLODWidget.h"
#include "msvVTKProp3DButtonRepresentation.h"
#include "vtkAssemblyPath.h"
#include "vtkAssemblyNode.h"
#include "vtkCallbackCommand.h"
//#include "vtkCompositeActor.h"
#include "vtkCompositeDataSet.h"
#include "vtkCompositePolyDataMapper2.h"
#include "vtkDataObject.h"
#include "vtkHardwareSelector.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkMapper.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSelection.h"
#include "vtkSelectionNode.h"
#include "vtkSmartPointer.h"
#include "vtkWidgetEventTranslator.h"
#include "vtkWidgetCallbackMapper.h"
#include "vtkWidgetRepresentation.h"
#include "vtkEvent.h"
#include "vtkWidgetEvent.h"

#include <cassert>

vtkStandardNewMacro(msvVTKLODWidget);

//-------------------------------------------------------------------------
msvVTKLODWidget::msvVTKLODWidget()
{
  this->WidgetState = msvVTKLODWidget::Start;
  this->SelectX = 0;
  this->SelectY = 0;
  this->BlockInteractorStyle = true;
  this->Interactive = true;
  this->LastSelection = 0;
  this->HighlightColor[0] = 1.;
  this->HighlightColor[1] = 0.;
  this->HighlightColor[2] = 0.;
  this->HighlightColor[3] = 1.;
  this->LastHighlightedCompositeIndex = vtkIdType(-1);
  this->ColorBeforeHighlight[0] = 1.;
  this->ColorBeforeHighlight[1] = 1.;
  this->ColorBeforeHighlight[2] = 1.;
  this->ColorBeforeHighlight[3] = 1.;

  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
                                          vtkWidgetEvent::Select,
                                          this, msvVTKLODWidget::SelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,
                                          vtkWidgetEvent::EndSelect,
                                          this, msvVTKLODWidget::EndSelectAction);
  //this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent,
  //                                        vtkWidgetEvent::Select,
  //                                        this, msvVTKLODWidget::SelectAction);
  //this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonReleaseEvent,
  //                                        vtkWidgetEvent::EndSelect,
  //                                        this, msvVTKLODWidget::EndSelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,
                                          vtkWidgetEvent::Move,
                                          this, msvVTKLODWidget::MouseHoverAction);
}

//-------------------------------------------------------------------------
msvVTKLODWidget::~msvVTKLODWidget()
{
  if (this->LastSelection != 0)
    {
    this->LastSelection->Delete();
    this->LastSelection = 0;
    }
}

//-------------------------------------------------------------------------
void msvVTKLODWidget::SetCursor(int cState)
{
/*
  if(!this->Resizable && cState != vtkBorderRepresentation::Inside)
    {
    this->RequestCursorShape(VTK_CURSOR_DEFAULT);
    return;
    }

  switch (cState)
    {
    case vtkBorderRepresentation::AdjustingP0:
      this->RequestCursorShape(VTK_CURSOR_SIZESW);
      break;
    case vtkBorderRepresentation::AdjustingP1:
      this->RequestCursorShape(VTK_CURSOR_SIZESE);
      break;
    case vtkBorderRepresentation::AdjustingP2:
      this->RequestCursorShape(VTK_CURSOR_SIZENE);
      break;
    case vtkBorderRepresentation::AdjustingP3:
      this->RequestCursorShape(VTK_CURSOR_SIZENW);
      break;
    case vtkBorderRepresentation::AdjustingE0:
    case vtkBorderRepresentation::AdjustingE2:
      this->RequestCursorShape(VTK_CURSOR_SIZENS);
      break;
    case vtkBorderRepresentation::AdjustingE1:
    case vtkBorderRepresentation::AdjustingE3:
      this->RequestCursorShape(VTK_CURSOR_SIZEWE);
      break;
    case vtkBorderRepresentation::Inside:
      if ( reinterpret_cast<vtkBorderRepresentation*>(this->WidgetRep)->GetMoving() )
        {
        this->RequestCursorShape(VTK_CURSOR_SIZEALL);
        }
      else
        {
        this->RequestCursorShape(VTK_CURSOR_HAND);
        }
      break;
    default:
      this->RequestCursorShape(VTK_CURSOR_DEFAULT);
    }
*/
}

//-------------------------------------------------------------------------
void msvVTKLODWidget::SelectAction(vtkAbstractWidget *w)
{
  msvVTKLODWidget *self = reinterpret_cast<msvVTKLODWidget*>(w);

  if ( self->SubclassSelectAction() ||
       self->WidgetState != msvVTKLODWidget::Selectable)
    {
    return;
    }

  // We are definitely selected
  self->GrabFocus(self->EventCallbackCommand);
  self->WidgetState = msvVTKLODWidget::Selected;

  // Picked something inside the widget
  self->SelectX = self->Interactor->GetEventPosition()[0];
  self->SelectY = self->Interactor->GetEventPosition()[1];

  // This is redundant but necessary on some systems (windows) because the
  // cursor is switched during OS event processing and reverts to the default
  // cursor (i.e., the MoveAction may have set the cursor previously, but this
  // method is necessary to maintain the proper cursor shape)..
  self->SetCursor(self->WidgetRep->GetInteractionState());

  //self->SelectRegion(eventPos);

  self->EventCallbackCommand->SetAbortFlag(1);
  //self->StartInteraction();
  //self->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
}

//-------------------------------------------------------------------------
void msvVTKLODWidget::EndSelectAction(vtkAbstractWidget *w)
{
  msvVTKLODWidget *self = reinterpret_cast<msvVTKLODWidget*>(w);

  if ( self->SubclassEndSelectAction() ||
       self->WidgetState != msvVTKLODWidget::Selected)
    {
    return;
    }

  // Return state to not selected
  self->ReleaseFocus();
  // stop adjusting
  self->EventCallbackCommand->SetAbortFlag(1);
  //self->EndInteraction();
  //self->InvokeEvent(vtkCommand::EndInteractionEvent,NULL);

  // compute some info we need for all cases
  int endSelectX = self->Interactor->GetEventPosition()[0];
  int endSelectY = self->Interactor->GetEventPosition()[1];
  vtkAssemblyPath* pickedPath =
    self->GetRepresentation()->GetRenderer()->PickProp(endSelectX, endSelectY);
  bool cursorOverLODObject = (pickedPath != 0);
  self->WidgetState = cursorOverLODObject ? msvVTKLODWidget::Selectable : msvVTKLODWidget::Start;

  vtkNew<vtkHardwareSelector> hardwareSelector;
  hardwareSelector->SetFieldAssociation(vtkDataObject::FIELD_ASSOCIATION_POINTS);
  hardwareSelector->SetRenderer(self->GetRepresentation()->GetRenderer());

  hardwareSelector->SetArea(static_cast<unsigned int>(self->SelectX),
    static_cast<unsigned int>(self->SelectY),
    static_cast<unsigned int>(endSelectX),
    static_cast<unsigned int>(endSelectY));

  vtkSelection *res = hardwareSelector->Select();
  // /todo handle more than 1 selection node
  if (res->GetNumberOfNodes())
    {
    vtkSelectionNode* node = res->GetNode(0);
    vtkObjectBase* object = node->GetProperties()->Get(vtkSelectionNode::PROP());
    vtkActor* actor = vtkActor::SafeDownCast(object);
    vtkCompositePolyDataMapper2* composite = vtkCompositePolyDataMapper2::SafeDownCast(actor->GetMapper());
    if (composite)
      {
      vtkIdType composite = node->GetProperties()->Get(vtkSelectionNode::COMPOSITE_INDEX());
      std::cout << "Composite index: " << composite << std::endl;
      }
    }
  //res->Print(std::cout);
}

//-------------------------------------------------------------------------
void msvVTKLODWidget::MouseHoverAction(vtkAbstractWidget *w)
{
  msvVTKLODWidget *self = reinterpret_cast<msvVTKLODWidget*>(w);
  if ( self->SubclassMouseHoverAction() ||
       self->WidgetState == msvVTKLODWidget::Selected)
    {
    return;
    }

  // compute some info we need for all cases
  int mouseHoverX = self->Interactor->GetEventPosition()[0];
  int mouseHoverY = self->Interactor->GetEventPosition()[1];

  vtkCompositeDataSet* hoveredCompositeDataSet = 0;
  if (self->BlockInteractorStyle || self->Interactive)
    {
    hoveredCompositeDataSet =
      self->PickCompositeDataSet(mouseHoverX, mouseHoverY);
    if (hoveredCompositeDataSet != 0)
      {
      // Return state to not selected
      self->GrabFocus(self->EventCallbackCommand);
      self->EventCallbackCommand->SetAbortFlag(1);
      self->WidgetState = msvVTKLODWidget::Selectable;
      }
    else
      {
      // Return state to not selected
      self->ReleaseFocus();
      self->WidgetState = msvVTKLODWidget::Start;
      }
    }
  if (self->GetInteractive() &&
      hoveredCompositeDataSet != 0)
    {
    vtkIdType compositeIndex = self->PickCompositeIndex(mouseHoverX, mouseHoverY);
    if (compositeIndex != vtkIdType(-1))
      {
      self->HighlightCompositePiece(
        self->GetActor(self->LastSelection->GetNode(0)),
        compositeIndex);
      }
    }
}

//----------------------------------------------------------------------
void msvVTKLODWidget::CreateDefaultRepresentation()
{
  if ( ! this->WidgetRep )
    {
    this->WidgetRep = msvVTKProp3DButtonRepresentation::New();
    }
}

//-------------------------------------------------------------------------
void msvVTKLODWidget::SelectRegion(double* vtkNotUsed(eventPos[2]))
{
  this->InvokeEvent(vtkCommand::WidgetActivateEvent,NULL);
}

//-------------------------------------------------------------------------
void msvVTKLODWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//-------------------------------------------------------------------------
vtkCompositeDataSet* msvVTKLODWidget
::PickCompositeDataSet(unsigned int pickX, unsigned int pickY)
{
  vtkAssemblyPath* pickedPath =
    this->GetRepresentation()->GetRenderer()->PickPropFrom(pickX, pickY, 0);
  return pickedPath && pickedPath->GetFirstNode() ?
    vtkCompositeDataSet::SafeDownCast(pickedPath->GetFirstNode()->GetViewProp()) :
    0;
}

//-------------------------------------------------------------------------
vtkIdTypeArray* msvVTKLODWidget
::PickCompositeIndexes(unsigned int startX, unsigned int startY,
       unsigned int endX, unsigned int endY)
{
  vtkNew<vtkHardwareSelector> hardwareSelector;
  hardwareSelector->SetFieldAssociation(vtkDataObject::FIELD_ASSOCIATION_POINTS);
  hardwareSelector->SetRenderer(this->GetRepresentation()->GetRenderer());

  hardwareSelector->SetArea(startX, startY, endX, endY);

  // Warning, pickedCompositeIndexes must be deleted from calling function
  vtkIdTypeArray* pickedCompositeIndexes = vtkIdTypeArray::New();
  if (this->LastSelection != 0)
    {
    this->LastSelection->Delete();
    this->LastSelection = 0;
    }
  this->LastSelection = hardwareSelector->Select();
  for (unsigned int nodeIt = 0; nodeIt < this->LastSelection->GetNumberOfNodes();
       ++nodeIt)
    {
    vtkSelectionNode* node = this->LastSelection->GetNode(nodeIt);
    vtkCompositePolyDataMapper2* composite = this->GetMapper(node);
    if (composite)
      {
      vtkIdType compositeIndex =
        node->GetProperties()->Get(vtkSelectionNode::COMPOSITE_INDEX());
      pickedCompositeIndexes->InsertNextValue(compositeIndex);
      std::cout << "Composite index: " << composite << std::endl;
      }
    }
  return pickedCompositeIndexes;
}

//-------------------------------------------------------------------------
vtkIdType msvVTKLODWidget
::PickCompositeIndex(unsigned int pickX, unsigned int pickY)
{
  vtkSmartPointer<vtkIdTypeArray> pickedCompositeIndexes;
  pickedCompositeIndexes.TakeReference(
    this->PickCompositeIndex(pickX, pickY, pickX, pickY));
  return pickedCompositeIndexes->GetNumberOfTuples() ?
    pickedCompositeIndexes->GetValue(0) : vtkIdType(-1);
}

//-------------------------------------------------------------------------
/*vtkCompositeActor* msvVTKLODWidget
::GetActor(vtkSelectionNode* node)
{
  assert(node && node->GetProperties()->Get(vtkSelectionNode::PROP()));
  vtkObjectBase* prop = node->GetProperties()->Get(vtkSelectionNode::PROP());
  return vtkCompositeActor::SafeDownCast(prop);
}
*/
//-------------------------------------------------------------------------
vtkCompositePolyDataMapper2* msvVTKLODWidget
::GetMapper(vtkSelectionNode* node)
{
  vtkCompositeActor* actor = msvVTKLODWidget::GetActor(node);
  return vtkCompositePolyDataMapper2::SafeDownCast(
    actor ? actor->GetMapper() : 0);
}

//-------------------------------------------------------------------------
void msvVTKLODWidget
::HighlightCompositePiece(vtkCompositeActor* actor,
                          vtkIdType compositeIndex)
{
  if (this->LastHighlightedCompositeIndex == compositeIndex)
    {
    return;
    }
  if (this->LastHighlightedCompositeIndex != -1)
    {
    this->SetCompositePieceColor(actor, this->LastHighlightedCompositeIndex,
                                 this->ColorBeforeHighlight);
    }
  if (compositeIndex != -1)
    {
    this->GetCompositePieceColor(actor, compositeIndex, this->ColorBeforeHighlight);
    this->SetCompositePieceColor(actor, compositeIndex, this->HighlightColor);
    }
  this->LastHighlightedCompositeIndex = compositeIndex;
}

//-------------------------------------------------------------------------
void msvVTKLODWidget
::SetCompositePieceColor(vtkCompositeActor* actor,
                         vtkIdType compositeIndex,
                         double color[4])
{
  vtkProperty* prop = actor->GetPieceProperty(compositeIndex)->Clone();
  prop->SetColor(color);
  prop->SetOpacity(color[3]);
  actor->SetPieceProperty(compositeIndex, prop);
  prop->Delete();
}

//-------------------------------------------------------------------------
void msvVTKLODWidget
::GetCompositePieceColor(vtkCompositeActor* actor,
                         vtkIdType compositeIndex,
                         double color[4])
{
  vtkProperty* prop = actor->GetPieceProperty(compositeIndex);
  prop->GetColor(color);
  color[3] = prop->GetOpacity();
}
