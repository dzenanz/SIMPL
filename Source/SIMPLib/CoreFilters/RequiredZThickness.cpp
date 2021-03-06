/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "RequiredZThickness.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RequiredZThickness::RequiredZThickness()
: m_DataContainerSelection("")
, m_NumZVoxels(-1)
, m_PreflightCheck(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RequiredZThickness::~RequiredZThickness() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RequiredZThickness::setupFilterParameters()
{
  FilterParameterVector parameters = getFilterParameters();

  {
    DataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DC_SELECTION_FP("DataContainer", DataContainerSelection, FilterParameter::RequiredArray, RequiredZThickness, req));
  }
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Minimum Z Dimension", NumZVoxels, FilterParameter::Parameter, RequiredZThickness, 0));
  parameters.push_back(SIMPL_NEW_BOOL_FP("Preflight Check", PreflightCheck, FilterParameter::Parameter, RequiredZThickness));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RequiredZThickness::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  AbstractDecisionFilter::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setDataContainerSelection(reader->readString("DataContainerSelection", getDataContainerSelection()));
  setNumZVoxels(reader->readValue("NumZVoxels", getNumZVoxels()));
  setPreflightCheck(reader->readValue("PreflightCheck", getPreflightCheck()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RequiredZThickness::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RequiredZThickness::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer dataContainer = getDataContainerArray()->getPrereqDataContainer(this, getDataContainerSelection());
  if(getErrorCondition() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = dataContainer->getGeometryAs<ImageGeom>();
  if(nullptr == image.get())
  {
    setErrorCondition(-7789);
    notifyErrorMessage(getHumanLabel(), "Missing Image Geometry in the selected DataContainer", getErrorCondition());
    return;
  }

  size_t dims[3] = {0, 0, 0};
  std::tie(dims[0], dims[1], dims[2]) = image->getDimensions();

  if(dims[2] < getNumZVoxels() && m_PreflightCheck)
  {
    setErrorCondition(-7787);
    QString str;
    QTextStream ss(&str);
    ss << "Number of Z Voxels does not meet required value during preflight of the filter. \n";
    ss << "  Required Z Voxels: " << m_NumZVoxels << "\n";
    ss << "  Current Z Voxels: " << dims[2];

    notifyErrorMessage(getHumanLabel(), str, getErrorCondition());
  }
  else if(dims[2] < getNumZVoxels() && !m_PreflightCheck)
  {
    QString str;
    QTextStream ss(&str);
    ss << "Number of Z Voxels does not meet required value during preflight but will be checked during pipeline execution.\n";
    ss << "  Required Z Voxels: " << m_NumZVoxels << "\n";
    ss << "  Current Z Voxels: " << dims[2];

    setWarningCondition(-7788);
    notifyWarningMessage(getHumanLabel(), str, getWarningCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RequiredZThickness::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RequiredZThickness::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer dataContainer = getDataContainerArray()->getPrereqDataContainer(this, getDataContainerSelection());
  if(getErrorCondition() < 0)
  {
    return;
  }

  ImageGeom::Pointer image = dataContainer->getGeometryAs<ImageGeom>();

  size_t dims[3] = {0, 0, 0};
  std::tie(dims[0], dims[1], dims[2]) = image->getDimensions();

  if(dims[2] < getNumZVoxels())
  {
    QString str;
    QTextStream ss(&str);
    ss << "Number of Z Voxels does not meet required value during execution of the filter. \n";
    ss << "  Required Z Voxels: " << m_NumZVoxels << "\n";
    ss << "  Current Z Voxels: " << dims[2];

    setErrorCondition(-7788);
    notifyErrorMessage(getHumanLabel(), str, getErrorCondition());
    bool needMoreData = true;
    emit decisionMade(needMoreData);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RequiredZThickness::newFilterInstance(bool copyFilterParameters) const
{
  RequiredZThickness::Pointer filter = RequiredZThickness::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RequiredZThickness::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RequiredZThickness::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid RequiredZThickness::getUuid()
{
  return QUuid("{1fe19578-6856-55f2-adc8-2236fac22c25}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RequiredZThickness::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MiscFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RequiredZThickness::getHumanLabel() const
{
  return "Required Z Dimension (Image Geometry)";
}
