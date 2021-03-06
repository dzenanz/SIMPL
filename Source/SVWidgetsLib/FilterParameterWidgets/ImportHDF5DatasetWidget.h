/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"

#include "hdf5.h"

#include "SVWidgetsLib/SVWidgetsLib.h"

//-- UIC generated Header
#include <ui_ImportHDF5DatasetWidget.h>

#define READ_HDF5_EXTRA_FEATURES 1

class IH5DataWindow;
class QDockWidget;
class ImportHDF5DatasetFilterParameter;
class ImportHDF5Dataset;

/**
 * @brief The ImportHDF5DatasetWidget class
 */
class SVWidgetsLib_EXPORT ImportHDF5DatasetWidget : public FilterParameterWidget, private Ui::ImportHDF5DatasetWidget
{
  Q_OBJECT

public:
  ImportHDF5DatasetWidget(FilterParameter* parameter, AbstractFilter* filter = nullptr, QWidget* parent = nullptr);

  /**
   * @brief getCurrentFile
   * @return
   */
  QString getCurrentFile()
  {
    return m_CurrentOpenFile;
  }

  /**
   * @brief verifyPathExists
   * @param filePath
   * @param lineEdit
   * @return
   */
  bool verifyPathExists(QString filePath, QtSFSDropLabel* lineEdit);

public slots:
  void beforePreflight();
  void afterPreflight();
  void filterNeedsInputParameters(AbstractFilter* filter);

  /**
   * @brief on_value_fileDropped
   * @param text
   */
  void on_value_fileDropped(const QString& text);

  /**
   * @brief on_selectBtn_clicked
   */
  void on_selectBtn_clicked();

  /**
   * @brief on_showLocationBtn_clicked
   */
  void on_showLocationBtn_clicked();

protected:
  /**
   * @brief Drag and drop implementation
   */
  void dragEnterEvent(QDragEnterEvent*) override;

  /**
   * @brief Drag and drop implementation
   */
  void dropEvent(QDropEvent*) override;

  /**
   * @brief Initializes some of the GUI elements with selections or other GUI related items
   */
  void setupGui() override;

  /**
   * @brief initWithFile Initializes the window by trying to open and populate the window
   * with values from the passed in hdf5 file
   * @param hdf5File
   */
  bool initWithFile(QString hdf5File);

  /**
  * @brief Returns the best guess at component dimensions for the given path.  This requires a valid AttributeMatrix, ImageGeometry, and HDF5 path
  * @param path
  * @return
  */
  std::tuple<herr_t, QString> bestGuessCDims(const QString& path);

protected slots:
  void on_cDimsLE_valueChanged(QString text);

private slots:
  /**
   * @brief Slot to catch events when the DataRecord TreeView selection is changed
   * @param current The currently selected Index
   * @param previous The previously selected Index
   */
  void hdfTreeView_currentChanged(const QModelIndex& current, const QModelIndex& previous);

private:
  static QString m_OpenDialogLastDirectory; // Stores the last directory the user visited
  QString m_CurrentOpenFile;                // Stores the currently open HDF5 File
  std::string m_CurrentHDFDataPath;         // Stores the currently viewed HDF data path
  hid_t m_FileId;
  QMap<QString, QString> m_ComponentDimsMap;
  QStringList m_CurrentPathsWithErrors;

  ImportHDF5Dataset* m_Filter = nullptr;

  ImportHDF5DatasetFilterParameter* m_FilterParameter = nullptr;

  /**
   * @brief Updates the QGraphicsView based on the current Data Dimension and Data record values
   * @param path The path to the HDF data set
   */
  void _updateViewFromHDFPath(std::string path);

  herr_t updateAttributeTable(const QString& datasetPath);
  herr_t updateGeneralTable(const QString& path);
  herr_t updateComponentDimensions(const QString& datasetPath);
  void addRow(QTableWidget* table, int row, const QString& key, const QString& value);

  void initializeHDF5Paths();

  /**
   * @brief Breaks the specified number, n, down into its prime factors.  These are numbers that are
   * only divisible by 1 and themselves.
   * @param n The number to calculate the prime factors of.
   * @param primeFactors The vector to store the prime factors into.
   * @return
   */
  void calculatePrimeFactors(int n, QVector<int>& primeFactors);

  ~ImportHDF5DatasetWidget() override;

public:
  ImportHDF5DatasetWidget(const ImportHDF5DatasetWidget&) = delete; // Copy Constructor Not Implemented
  ImportHDF5DatasetWidget(ImportHDF5DatasetWidget&&) = delete;      // Move Constructor Not Implemented
  ImportHDF5DatasetWidget& operator=(const ImportHDF5DatasetWidget&) = delete; // Copy Assignment Not Implemented
  ImportHDF5DatasetWidget& operator=(ImportHDF5DatasetWidget&&) = delete;      // Move Assignment Not Implemented
};
