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

#ifndef _comparisoninputsadvanced_h_
#define _comparisoninputsadvanced_h_

#include <memory>

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QMetaType>
#include <QtCore/QObject>
#include <QtCore/QVector>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/Filtering/AbstractComparison.h"

/**
 * @class ComparisonInputsAdvanced ComparisonInputsAdvanced.h DREAM3DLib/Common/ComparisonInputsAdvanced.h
 * @brief  This class
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Mar 3, 2017
 * @version 1.0
 */
class SIMPLib_EXPORT ComparisonInputsAdvanced : public QObject
{
  Q_OBJECT

public:
  ComparisonInputsAdvanced();
  ComparisonInputsAdvanced(const ComparisonInputsAdvanced& rhs);
  // explicit ComparisonInputsAdvanced(ComparisonInputsAdvanced& rhs);

  virtual ~ComparisonInputsAdvanced();

  int size();

  void addInput(int unionOperator, const QString arrayName, int compOperator, double compValue);
  void addInput(int unionOperator, bool invertComparison, QVector<AbstractComparison::Pointer> comparisons);
  void addInput(const AbstractComparison::Pointer input);

  AbstractComparison::Pointer getInput(int index);
  QVector<AbstractComparison::Pointer>& getInputs();

  void setInputs(QVector<AbstractComparison::Pointer> comparisons);

  QString getDataContainerName();
  QString getAttributeMatrixName();

  void setDataContainerName(QString dcName);
  void setAttributeMatrixName(QString amName);

  void operator=(const ComparisonInputsAdvanced&);

  AbstractComparison::Pointer operator[](int index);

  bool hasComparisonValue();
  QVector<AbstractComparison::Pointer> getComparisonValues();

  bool shouldInvert();
  void setInvert(bool invert);

  void readJson(QJsonObject obj);
  void writeJson(QJsonObject& obj);

private:
  QVector<AbstractComparison::Pointer> m_Inputs;
  QString m_dataContainerName;
  QString m_attributeMatrixName;

  bool m_invert;
};

Q_DECLARE_METATYPE(ComparisonInputsAdvanced)

#endif /* _ComparisonInputsAdvanced_H_ */