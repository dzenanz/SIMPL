/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FindLocalMisorientationGradients.h"

#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"

const static float m_pi = M_PI;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindLocalMisorientationGradients::FindLocalMisorientationGradients() :
    AbstractFilter()
{
  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*>(m_HexOps.get()));

  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*>(m_CubicOps.get()));

  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(dynamic_cast<OrientationMath*>(m_OrthoOps.get()));
  setupFilterOptions();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindLocalMisorientationGradients::~FindLocalMisorientationGradients()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalMisorientationGradients::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Kernel Size");
    option->setPropertyName("KernelSize");
    option->setWidgetType(FilterOption::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
  setFilterOptions(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLocalMisorientationGradients::execute()
{
  setErrorCondition(0);

  DataContainer* m = getDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }

  int64_t totalPoints = m->totalPoints();
  int32_t* grain_indicies = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, this);
  if(NULL == grain_indicies)
  {
    return;
  }
  int32_t* phases = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::Phases, totalPoints, this);
  if(NULL == phases)
  {
    return;
  }
  float* quats = m->getVoxelDataSizeCheck<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::Quats, (totalPoints * 5), this);
  if(NULL == quats)
  {
    return;
  }

  float* kernelmisorientations =
      m->createVoxelData<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::KernelAverageMisorientations, totalPoints, 1, this);
  if(NULL == kernelmisorientations)
  {
    return;
  }

  float* grainmisorientations = m->createVoxelData<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainMisorientations, totalPoints, 1, this);
  if(NULL == grainmisorientations)
  {
    return;
  }

  float* misorientationgradients = m->createVoxelData<float, FloatArrayType, AbstractFilter>(DREAM3D::VoxelData::MisorientationGradients, totalPoints, 1, this);
  if(NULL == misorientationgradients)
  {
    return;
  }

  // We need to keep a reference to the wrapper DataArray class in addition to the raw pointer
  FloatArrayType* m_KernelMisorientations =
      FloatArrayType::SafeObjectDownCast<IDataArray*, FloatArrayType*>(m->getVoxelData(DREAM3D::VoxelData::KernelAverageMisorientations).get());

  std::vector<float> gamVec(totalPoints);
  float* gam = &(gamVec.front());

  float** avgmiso = new float *[m->m_Grains.size()];
  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
    avgmiso[i] = new float[2];
    for (int j = 0; j < 2; j++)
    {
      avgmiso[i][j] = 0.0;
    }
  }
  for (int i = 0; i < totalPoints; ++i)
  {
    gam[i] = 0.0;
  }

  float q1[5];
  float q2[5];
  int numVoxel; // number of voxels in the grain...
  int numchecks; // number of voxels in the grain...
  int good = 0;

  int point;
  float w, totalmisorientation;
  float n1, n2, n3;
  Ebsd::CrystalStructure phase1 = Ebsd::UnknownCrystalStructure;
  Ebsd::CrystalStructure phase2 = Ebsd::UnknownCrystalStructure;

  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType xPoints = static_cast<DimType>(udims[0]);
  DimType yPoints = static_cast<DimType>(udims[1]);
  DimType zPoints = static_cast<DimType>(udims[2]);

  size_t neighbor = 0;
//  int m_KernelSize = 1;
  int jStride;
  int kStride;
  for (DimType col = 0; col < xPoints; col++)
  {
    for (DimType row = 0; row < yPoints; row++)
    {
      for (DimType plane = 0; plane < zPoints; plane++)
      {
        point = (plane * xPoints * yPoints) + (row * xPoints) + col;
        if(grain_indicies[point] > 0 && phases[point] > 0)
        {
          totalmisorientation = 0.0;
          numVoxel = 0;
          q1[1] = quats[point * 5 + 1];
          q1[2] = quats[point * 5 + 2];
          q1[3] = quats[point * 5 + 3];
          q1[4] = quats[point * 5 + 4];
          phase1 = m->crystruct[phases[point]];
          for (int j = -m_KernelSize; j < m_KernelSize + 1; j++)
          {
            jStride = j * xPoints * yPoints;
            for (int k = -m_KernelSize; k < m_KernelSize + 1; k++)
            {
              kStride = k * xPoints;
              for (int l = -m_KernelSize; l < m_KernelSize + 1; l++)
              {
                good = 1;
                neighbor = point + (jStride) + (kStride) + (l);
                if(plane + j < 0) good = 0;
                else if(plane + j > zPoints - 1) good = 0;
                else if(row + k < 0) good = 0;
                else if(row + k > yPoints - 1) good = 0;
                else if(col + l < 0) good = 0;
                else if(col + l > xPoints - 1) good = 0;
                if(good == 1)
                {
                  w = 10000.0;
                  q2[1] = quats[neighbor * 5 + 1];
                  q2[2] = quats[neighbor * 5 + 2];
                  q2[3] = quats[neighbor * 5 + 3];
                  q2[4] = quats[neighbor * 5 + 4];
                  phase2 = m->crystruct[phases[neighbor]];
                  if(phase1 == phase2) w = m_OrientationOps[phase1]->getMisoQuat(q1, q2, n1, n2, n3);
                  if(w < 5.0)
                  {
                    totalmisorientation = totalmisorientation + w;
                    numVoxel++;
                  }
                }
              }
            }
          }
          if(numVoxel == 0)
          {
            kernelmisorientations[point] = 0;
          }
          else if(neighbor < m_KernelMisorientations->GetNumberOfTuples())
          {
            kernelmisorientations[point] = totalmisorientation / (float)numVoxel;
          }
          else
          {
            kernelmisorientations[point] = 0;
          }

          q2[0] = 1;
          q2[1] = m->m_Grains[grain_indicies[point]]->avg_quat[1] / m->m_Grains[grain_indicies[point]]->avg_quat[0];
          q2[2] = m->m_Grains[grain_indicies[point]]->avg_quat[2] / m->m_Grains[grain_indicies[point]]->avg_quat[0];
          q2[3] = m->m_Grains[grain_indicies[point]]->avg_quat[3] / m->m_Grains[grain_indicies[point]]->avg_quat[0];
          q2[4] = m->m_Grains[grain_indicies[point]]->avg_quat[4] / m->m_Grains[grain_indicies[point]]->avg_quat[0];
          w = m_OrientationOps[phase1]->getMisoQuat(q1, q2, n1, n2, n3);
          grainmisorientations[point] = w;
          gam[point] = w;
          avgmiso[grain_indicies[point]][0]++;
          avgmiso[grain_indicies[point]][1] = avgmiso[grain_indicies[point]][1] + w;
        }
        if(grain_indicies[point] == 0 || phases[point] == 0)
        {
          kernelmisorientations[point] = 0;
          grainmisorientations[point] = 0;
          gam[point] = 0;
        }
      }
    }
  }

  size_t grainsSize = m->m_Grains.size();
  for (size_t i = 1; i < grainsSize; i++)
  {
    m->m_Grains[i]->averagemisorientation = avgmiso[i][1] / avgmiso[i][0];
    if(avgmiso[i][0] == 0) m->m_Grains[i]->averagemisorientation = 0.0;
  }

//  m_KernelSize = 1;
  for (DimType col = 0; col < xPoints; col++)
  {
    for (DimType row = 0; row < yPoints; row++)
    {
      for (DimType plane = 0; plane < zPoints; plane++)
      {
        point = (plane * xPoints * yPoints) + (row * xPoints) + col;
        if(grain_indicies[point] > 0 && phases[point] > 0)
        {
          totalmisorientation = 0.0;
          numchecks = 0;
          for (int j = -m_KernelSize; j < m_KernelSize + 1; j++)
          {
            jStride = j * xPoints * yPoints;
            for (int k = -m_KernelSize; k < m_KernelSize + 1; k++)
            {
              kStride = k * xPoints;
              for (int l = -m_KernelSize; l < m_KernelSize + 1; l++)
              {
                good = 1;
                neighbor = point + (jStride) + (kStride) + (l);
                if(plane + j < 0) good = 0;
                if(plane + j > zPoints - 1) good = 0;
                if(row + k < 0) good = 0;
                if(row + k > yPoints - 1) good = 0;
                if(col + l < 0) good = 0;
                if(col + l > xPoints - 1) good = 0;
                if(good == 1 && grain_indicies[point] == grain_indicies[neighbor])
                {
                  numchecks++;
                  totalmisorientation = totalmisorientation + fabs(gam[point] - gam[neighbor]);
                }
              }
            }
          }
          misorientationgradients[point] = totalmisorientation / (float)numchecks;
        }
        if(grain_indicies[point] == 0 || phases[point] == 0)
        {
          misorientationgradients[point] = 0;
        }
      }
    }
  }

  // Clean up all the heap allocated memory
  for (size_t i = 1; i < m->m_Grains.size(); i++)
  {
    delete[] avgmiso[i];
  }
  delete avgmiso;

  notify("FindLocalMisorientationGradients Completed", 0, Observable::UpdateProgressMessage);
}
