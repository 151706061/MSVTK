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

// QT includes
#include <QDir>
#include <QString>
#include <QStringList>

// MSECGQT includes
#include "qMSECGApplication.h"
#include "vtkMSECGButtonsManager.h"
#include "vtkMSECGReader.h"


//-----------------------------------------------------------------------------
// qMSECGApplication methods

//-----------------------------------------------------------------------------
qMSECGApplication::qMSECGApplication(int &_argc, char **_argv) :
  Superclass(_argc, _argv),
  reader(vtkMSECGReader::New()),
  buttonsManager(new vtkMSECGButtonsManager),
  cartoPeriod(2500) // ChangeConstDef
{}

//-----------------------------------------------------------------------------
qMSECGApplication* qMSECGApplication::application()
{
  qMSECGApplication* app = qobject_cast<qMSECGApplication*>(QApplication::instance());
  return app;
}

//-----------------------------------------------------------------------------
void qMSECGApplication::ReadCartoData(const QString& dirPath)
{
  this->ReadCartoPoints(dirPath);
  //this->ReadCartoECG(dirPath);

  this->buttonsManager->Init(this->reader->GetCartoPointsReaders().at(0));
}

//-----------------------------------------------------------------------------
void qMSECGApplication::ReadCartoPoints(const QString& dirPath)
{
  QStringList filters;
  filters << "*.vtk";

  std::cout << "ReadCartoDataFolder: " << dirPath.toStdString() << std::endl;

  QDir dir(dirPath);
  if (!dir.cd(QString("CartoPoints")))
    {
    return;
    }
  this->reader->SetPathName(dirPath.toStdString());

  dir.setNameFilters(filters);
  QStringList files = dir.entryList(QDir::Files,QDir::Name);

  std::list<std::string> filePaths;
  QStringList::const_iterator constIt;
  for (constIt = files.constBegin(); constIt != files.constEnd(); ++constIt)
    {
    filePaths.push_back(dir.filePath(*constIt).toLocal8Bit().constData());
    }

  this->reader->ReadCartoPoints(filePaths);
}

//-----------------------------------------------------------------------------
void qMSECGApplication::Clear()
{
  this->reader->Clear();
  this->buttonsManager->Clear();
}
