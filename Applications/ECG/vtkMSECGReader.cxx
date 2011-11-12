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


// MSVTK includes
#include "vtkMSECGReader.h"

// VTK includes
#include <vtkFloatArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkStructuredPointsReader.h>
#include <vtkTable.h>

// STD includes
#include <cstdlib>

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkMSECGReader);

//-----------------------------------------------------------------------------
vtkMSECGReader::vtkMSECGReader() : PathName(""),
                                   NumberOfCartoPoints(0),
                                   NumberOfCartoSignals(0)
{}

//-----------------------------------------------------------------------------
vtkMSECGReader::~vtkMSECGReader()
{
  this->ClearCartoPoints();
}

//-----------------------------------------------------------------------------
void vtkMSECGReader::ReadCartoPoints(std::list<std::string>& filePaths)
{
  this->ClearCartoPoints();

  this->CartoPoints.reserve(filePaths.size());
  this->CartoPointsReaders.reserve(filePaths.size());

  for (std::list<std::string>::iterator it = filePaths.begin();
       it != filePaths.end(); ++it)
    {
    std::cout << "FilePath: " << *it << std::endl;

    // Reader
    vtkNew<vtkPolyDataReader> reader;
    reader->SetFileName(it->c_str());
    reader->Update();
    this->CartoPointsReaders.push_back(reader.GetPointer());

    // PolyDataMapper
    vtkNew<vtkPolyDataMapper> dataMapper;
    dataMapper->SetInput(reader->GetOutput());
    dataMapper->ScalarVisibilityOff();
    this->CartoPoints.push_back(dataMapper.GetPointer());
    }

  this->NumberOfCartoPoints = this->CartoPoints.size();
  std::cout << "Number of pointfiles: " << this->NumberOfCartoPoints << std::endl;
}

//-----------------------------------------------------------------------------
vtkMSECGReader::SmartECGs vtkMSECGReader::GetCartoSignals()
{
  vtkMSECGReader::SmartECGs signals;
  const vtkIdType signalCount = 5;
  for (vtkIdType i; i < signalCount; ++i)
    {
    signals.push_back(this->GetCartoSignal(i));
    }
  return signals;
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkTable> vtkMSECGReader::GetCartoSignal(vtkIdType vtkNotUsed(index))
{
  vtkSmartPointer<vtkTable> signal = vtkSmartPointer<vtkTable>::New();
  vtkNew<vtkFloatArray> time;
  time->SetName("Time");
  signal->AddColumn(time.GetPointer());
  vtkNew<vtkFloatArray> values;
  values->SetName("Values");
  signal->AddColumn(values.GetPointer());

  // Test charting with a few more points...
  const int timeSpanInMSecs = 2500;
  const int timeStepInMSecs = 1;
  const vtkIdType numPoints = timeSpanInMSecs * timeStepInMSecs;
  signal->SetNumberOfRows(numPoints);
  for (vtkIdType i = 0; i < numPoints; ++i)
    {
    signal->SetValue(i, 0, i);
    signal->SetValue(i, 1, rand() - RAND_MAX / 2);
    }
  signal->Update();

  return signal;
}

//-----------------------------------------------------------------------------
void vtkMSECGReader::Clear()
{
  this->ClearCartoPoints();
}

//-----------------------------------------------------------------------------
void vtkMSECGReader::ClearCartoPoints()
{
  this->CartoPoints.clear();
  this->CartoPointsReaders.clear();
  this->NumberOfCartoPoints = 0;
}

void vtkMSECGReader::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}
