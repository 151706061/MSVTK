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

==============================================================================*/

// Qt includes
#include "QFileDialog"
#include "QTimer"

// MSECG includes
#include "vtkMSECGButtonsManager.h"

// MSVTK includes
#include "qMSECGMainWindow.h"
#include "ui_qMSECGMainWindow.h"
#include "qMSECGApplication.h"
#include "vtkMSECGReader.h"

// VTK includes
#include "vtkActor.h"
#include "vtkNew.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkSmartPointer.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
class qMSECGMainWindowPrivate: public Ui_qMSECGMainWindow
{
  Q_DECLARE_PUBLIC(qMSECGMainWindow);
protected:
  qMSECGMainWindow* const q_ptr;

  vtkSmartPointer<vtkRenderer>            threeDRenderer;
  std::vector<vtkSmartPointer<vtkActor> > cartoPointsActors;

  int lastFrame;  // Convenient member to avoid going through the list of actors;
  QTimer *timer;  // Timer to process the player

public:
  qMSECGMainWindowPrivate(qMSECGMainWindow& object);
  ~qMSECGMainWindowPrivate() {this->reset();}
  virtual void setupUi(QMainWindow * mainWindow);
  virtual void setupApi();
  virtual void updateUi();
  virtual void loadThreeDViewData();
  virtual void reset();
};

//-----------------------------------------------------------------------------
// qMSECGMainWindowPrivate methods

qMSECGMainWindowPrivate::qMSECGMainWindowPrivate(qMSECGMainWindow& object)
  : q_ptr(&object)
{
  this->threeDRenderer = vtkRenderer::New();
  this->threeDRenderer->SetBackground(0.1, 0.2, 0.4);

  this->timer = new QTimer();
  this->lastFrame = 0;
}

//-----------------------------------------------------------------------------
void qMSECGMainWindowPrivate::setupUi(QMainWindow * mainWindow)
{
  this->Ui_qMSECGMainWindow::setupUi(mainWindow);

  // Initialize framSlider
  this->currentFrameSlider->setDecimals(0);
  this->currentFrameSlider->setMaximum(0);
  this->currentFrameSlider->setSingleStep(1);
}

//-----------------------------------------------------------------------------
void qMSECGMainWindowPrivate::setupApi()
{
  this->threeDView->GetRenderWindow()->AddRenderer(this->threeDRenderer);
}

//-----------------------------------------------------------------------------
void qMSECGMainWindowPrivate::updateUi()
{
  Q_Q(qMSECGMainWindow);

  this->currentFrameSlider->setMaximum(static_cast<double>(
    q->application->GetReader()->GetNumberOfCartoPoints()-1));
  this->currentFrameSlider->setValue(this->lastFrame);
}

//-----------------------------------------------------------------------------
void qMSECGMainWindowPrivate::loadThreeDViewData()
{
  Q_Q(qMSECGMainWindow);

  vtkMSECGReader::SmartPolyDataMappers CartoPoints =
    q->application->GetReader()->GetCartoPoints();

  if (CartoPoints.empty())
    {
    return;
    }

  // We create the actors correspondings to the CartoPoints and add it to the scene
  // and to a vector of Actors.
  this->cartoPointsActors.reserve(CartoPoints.size());
  vtkMSECGReader::SmartPolyDataMappers::iterator it;
  for (it = CartoPoints.begin(); it != CartoPoints.end() ; ++it)
    {
    vtkNew<vtkActor> cartoPointsActor;
    cartoPointsActor->SetMapper(*it);
    cartoPointsActor->VisibilityOff();

    this->cartoPointsActors.push_back(cartoPointsActor.GetPointer());
    this->threeDRenderer->AddActor(cartoPointsActor.GetPointer());
    }

  // Refocus the camera on the data
  this->threeDRenderer->ResetCamera(
    this->cartoPointsActors.at(this->lastFrame)->GetBounds());
}

//-----------------------------------------------------------------------------
void qMSECGMainWindowPrivate::reset()
{
  Q_Q(qMSECGMainWindow);

  this->threeDRenderer->RemoveAllViewProps(); // clean up the renderer
  this->cartoPointsActors.clear();            // clean Actors references
  q->application->Clear();                    // clean application core

  this->lastFrame = 0;
}

//-----------------------------------------------------------------------------
// qMSECGMainWindow methods

//-----------------------------------------------------------------------------
qMSECGMainWindow::qMSECGMainWindow(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new qMSECGMainWindowPrivate(*this))
{
  Q_D(qMSECGMainWindow);

  d->setupUi(this);
  d->setupApi();
  this->setupMenuActions();
}

//-----------------------------------------------------------------------------
qMSECGMainWindow::~qMSECGMainWindow()
{}

//-----------------------------------------------------------------------------
void qMSECGMainWindow::setupMenuActions()
{
  Q_D(qMSECGMainWindow);

  // Connect Menu ToolBars actions
  this->connect(d->actionOpenCartoData, SIGNAL(triggered()), this, SLOT(openCartoData()));
  this->connect(d->actionClose, SIGNAL(triggered()), this, SLOT(closeScene()));
  this->connect(d->actionExit, SIGNAL(triggered()), this, SLOT(close()));

  // Connect ThreeDView controller
  this->connect(d->currentFrameSlider, SIGNAL(valueChanged(double)), this, SLOT(updateThreeDViewData(double)));

  // Playback Controller
  this->connect(d->playToolButton, SIGNAL(pressed()), this, SLOT(playCartoData()));
  this->connect(d->timer, SIGNAL(timeout()), this, SLOT(playStep()));
}

//-----------------------------------------------------------------------------
void qMSECGMainWindow::SetApplication(qMSECGApplication* app)
{
  Q_D(qMSECGMainWindow);
  if (!app)
    {
    return;
    }

  application = app;
  this->GetApplication()->GetButtonsManager()->SetRenderer(d->threeDRenderer);
}

//-----------------------------------------------------------------------------
void qMSECGMainWindow::openCartoData()
{
  Q_D(qMSECGMainWindow);

  QString dir = QFileDialog::getExistingDirectory(
    this, tr("Select root CartoData Folder"), QDir::homePath(),
    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if (dir.isNull())
    {
    return;
    }

  d->reset(); // Clean Up Scene & Application before loading new data

  this->application->ReadCartoData(dir);

  d->loadThreeDViewData();
  d->updateUi();
  this->updateThreeDViewData(d->lastFrame);
}

//-----------------------------------------------------------------------------
void qMSECGMainWindow::closeScene()
{
  Q_D(qMSECGMainWindow);

  d->reset();
  this->updateThreeDViewData();
}

//-----------------------------------------------------------------------------
void qMSECGMainWindow::updateThreeDViewData(double frame)
{
  Q_D(qMSECGMainWindow);

  if (frame >= d->cartoPointsActors.size() ||
      frame >= this->GetApplication()->GetReader()->GetCartoPointsReaders().size())
    {
    d->threeDView->GetRenderWindow()->Render();
    return;
    }

  d->cartoPointsActors.at(d->lastFrame)->VisibilityOff();

  int curFrame = static_cast<int>(frame);

  vtkActor* actor = d->cartoPointsActors.at(curFrame).GetPointer();
  actor->VisibilityOn();

  this->application->GetButtonsManager()->UpdateButtonWidgets(
    this->GetApplication()->GetReader()->GetCartoPointsReaders().at(curFrame));

  d->threeDView->GetRenderWindow()->Render();
  d->lastFrame = curFrame;
}

//-----------------------------------------------------------------------------
void qMSECGMainWindow::playCartoData()
{
  Q_D(qMSECGMainWindow);

  if(this->application->GetReader()->GetNumberOfCartoPoints() < 2)
    {
    return;
    }

  d->timer->start(this->application->GetCartoPeriod() /
                  this->application->GetReader()->GetNumberOfCartoPoints()-1);
}

//-----------------------------------------------------------------------------
void qMSECGMainWindow::playStep()
{
  Q_D(qMSECGMainWindow);

  if (d->currentFrameSlider->value() ==
        this->application->GetReader()->GetNumberOfCartoPoints()-1)
    {
    d->timer->stop();
    return;
    }

  d->currentFrameSlider->setValue(d->currentFrameSlider->value()+1);
}
