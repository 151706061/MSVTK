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

#ifndef __vtkMSECGReader_h
#define __vtkMSECGReader_h

// STD includes
#include <vector>
#include <list>

// VTK includes
#include "vtkPolyDataReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkSmartPointer.h"

class  vtkMSECGReader : public vtkPolyDataReader
{
public:
  vtkMSECGReader();
  ~vtkMSECGReader();
  void PrintSelf(ostream& os, vtkIndent indent){Superclass::PrintSelf(os,indent);}

  // Description:
  // Specify file name.
  void SetPathName(const std::string& pName){PathName = pName;}
  const std::string& GetPathName() const {return PathName;}

  // Description:
  // Get/Set the data type.  The options are:
  // - VTK_FLOAT (default) single precision floating point.
  // - VTK_DOUBLE double precision floating point.
  vtkSetClampMacro(DataType, int, VTK_FLOAT, VTK_DOUBLE);
  vtkGetMacro(DataType, int);
  void SetDataTypeToFloat() {this->SetDataType(VTK_FLOAT);}
  void SetDataTypeToDouble() {this->SetDataType(VTK_DOUBLE);}

  // Description
  // Return the number of CartoPoints files loaded
  vtkGetMacro(NumberOfCartoPoints,size_t);

  // Description
  // Return CartonPoints as PolyDataMappers
  typedef std::vector<vtkSmartPointer<vtkPolyDataReader> > SmartPolyDataReaders;
  typedef std::vector<vtkSmartPointer<vtkPolyDataMapper> > SmartPolyDataMappers;
  SmartPolyDataMappers GetCartoPoints() const {return CartoPoints;}
  SmartPolyDataReaders GetCartoPointsReaders() const {return CartoPointsReaders;}

  void ReadCartoPoints(std::list<std::string>&);

  /// Reset Reader
  virtual void Clear();

protected:
  void ClearCartoPoints();

  std::string PathName;             // Current directory
  size_t NumberOfCartoPoints;
  size_t NumberOfCartoSignals;

  SmartPolyDataMappers CartoPoints;
  SmartPolyDataReaders CartoPointsReaders;

  // Description:
  // Used to specify the data type.
  int DataType;

private:
  vtkMSECGReader(const vtkMSECGReader&);  // Not implemented.
  void operator=(const vtkMSECGReader&);  // Not implemented.
};
#endif
