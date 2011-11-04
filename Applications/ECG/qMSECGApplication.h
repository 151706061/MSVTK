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

#ifndef __qMSECGApplication_h
#define __qMSECGApplication_h

// Qt includes
#include <QApplication>

class QString;
class QMainWindow;
class vtkMSECGReader;

class qMSECGApplication : public QApplication
{
  Q_OBJECT
public:
  typedef QApplication Superclass;
  qMSECGApplication(int &argc, char **argv);
  virtual ~qMSECGApplication(){}

  /// Return a reference to the application singleton
  static qMSECGApplication* application();

  /// Read data from CartoData folder
  void ReadCartoData(const QString&);

  /// Reset application
  virtual void Clear();

  /// Return a pointer on the main window of the application if any.
  QMainWindow* mainWindow()const{return 0;}

  /// Return a pointer on the application reader
  vtkMSECGReader* GetReader() const {return reader;}

  /// Return the period of the current CartoData
  size_t GetCartoPeriod() const {return cartoPeriod;}

private:
  void ReadCartoPoints(const QString&);

  vtkMSECGReader*         reader;
  size_t                  cartoPeriod;
};
#endif
