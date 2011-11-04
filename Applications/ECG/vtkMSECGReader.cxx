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
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkStructuredPointsReader.h>

// MSVTK includes
#include "vtkMSECGReader.h"

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
void vtkMSECGReader::Clear()
{
  this->ClearCartoPoints();
}

//-----------------------------------------------------------------------------
void vtkMSECGReader::ClearCartoPoints()
{
  this->CartoPoints.clear();
  this->CartoPointsReaders.clear();
}
