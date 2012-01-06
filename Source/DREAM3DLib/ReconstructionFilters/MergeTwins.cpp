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

#include "MergeTwins.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/OrientationMath.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"
#include "DREAM3DLib/Common/NeighborList.hpp"

#include "DREAM3DLib/GenericFilters/FindNeighbors.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"


#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1

const static float m_pi = M_PI;


#define NEW_SHARED_ARRAY(var, type, size)\
  boost::shared_array<type> var##Array(new type[size]);\
  type* var = var##Array.get();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeTwins::MergeTwins() :
            AbstractFilter()
{

  m_HexOps = HexagonalOps::New();
  m_OrientationOps.push_back(m_HexOps.get());
  m_CubicOps = CubicOps::New();
  m_OrientationOps.push_back(m_CubicOps.get());
  m_OrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back(m_OrthoOps.get());
  setupFilterOptions();


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeTwins::~MergeTwins()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::setupFilterOptions()
{
  std::vector<FilterOption::Pointer> options;
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Axis Tolerance");
    option->setPropertyName("AxisTolerance");
    option->setWidgetType(FilterOption::DoubleWidget);
    option->setValueType("float");
    options.push_back(option);
  }
  {
    FilterOption::Pointer option = FilterOption::New();
    option->setHumanLabel("Angle Tolerance");
    option->setPropertyName("AngleTolerance");
    option->setWidgetType(FilterOption::DoubleWidget);
    option->setValueType("float");
    options.push_back(option);
  }

  setFilterOptions(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::execute()
{
  DataContainer* m = getDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }
  setErrorCondition(0);

  float* totalsurfacearea = m->getEnsembleDataSizeCheck<float, FloatArrayType, AbstractFilter>(DREAM3D::EnsembleData::TotalSurfaceArea, (m->crystruct.size()), this);
  if (NULL == totalsurfacearea) { return; }

  merge_twins();
  characterize_twins();
  renumber_grains();

  // If there is an error set this to something negative and also set a message
  notify("MergeTwins Completed", 0, Observable::UpdateProgressMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeTwins::merge_twins()
{
  // Since this method is called from the 'execute' and the DataContainer validity
  // was checked there we are just going to get the Shared Pointer to the DataContainer
  DataContainer* m = getDataContainer();
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  NeighborList<int>* neighListPtr = NeighborList<int>::SafeObjectDownCast<IDataArray*, NeighborList<int>* >(m->getFieldData(DREAM3D::FieldData::NeighborList).get());
  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int>& neighborlist = *neighListPtr;

  int32_t* grain_indicies = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, m->totalPoints(), this);
  if (NULL == grain_indicies) { return; }

 // float angcur = 180.0f;
  std::vector<int> twinlist;
  float w;
  float n1, n2, n3;
  float angtol = 2.0f;
  float axistol = 2.0f*M_PI/180.0f;
  float q1[5];
  float q2[5];
  size_t numgrains = m->m_Grains.size();
  Ebsd::CrystalStructure phase1, phase2;
  twinnewnumbers.resize(numgrains, -1);

  for (size_t i = 1; i < numgrains; i++)
  {
	if (twinnewnumbers[i] == -1 && m->m_Grains[i]->phase > 0)
    {
      twinlist.push_back(i);
      for (size_t j = 0; j < twinlist.size(); j++)
      {
        int firstgrain = twinlist[j];
        int size = int(neighborlist[firstgrain].size());
        for (int l = 0; l < size; l++)
        {
       //   angcur = 180.0f;
          int twin = 0;
          size_t neigh = neighborlist[firstgrain][l];
          if (neigh != i && twinnewnumbers[neigh] == -1 && m->m_Grains[neigh]->phase > 0)
          {
            w = 10000.0f;
            q1[1] = m->m_Grains[firstgrain]->avg_quat[1]/m->m_Grains[firstgrain]->avg_quat[0];
            q1[2] = m->m_Grains[firstgrain]->avg_quat[2]/m->m_Grains[firstgrain]->avg_quat[0];
            q1[3] = m->m_Grains[firstgrain]->avg_quat[3]/m->m_Grains[firstgrain]->avg_quat[0];
            q1[4] = m->m_Grains[firstgrain]->avg_quat[4]/m->m_Grains[firstgrain]->avg_quat[0];
            phase1 = m->crystruct[m->m_Grains[firstgrain]->phase];
            q2[1] = m->m_Grains[neigh]->avg_quat[1]/m->m_Grains[neigh]->avg_quat[0];
            q2[2] = m->m_Grains[neigh]->avg_quat[2]/m->m_Grains[neigh]->avg_quat[0];
            q2[3] = m->m_Grains[neigh]->avg_quat[3]/m->m_Grains[neigh]->avg_quat[0];
            q2[4] = m->m_Grains[neigh]->avg_quat[4]/m->m_Grains[neigh]->avg_quat[0];
            phase2 = m->crystruct[m->m_Grains[neigh]->phase];
            if (phase1 == phase2 && phase1 > 0) w = m_OrientationOps[phase1]->getMisoQuat( q1, q2, n1, n2, n3);
//			OrientationMath::axisAngletoRod(w, n1, n2, n3, r1, r2, r3);
			float axisdiff111 = acosf(fabs(n1)*0.57735f+fabs(n2)*0.57735f+fabs(n3)*0.57735f);
			float angdiff60 = fabs(w-60.0f);
            if (axisdiff111 < axistol && angdiff60 < angtol) twin = 1;
            if (twin == 1)
            {
              twinnewnumbers[neigh] = i;
              twinlist.push_back(neigh);
            }
          }
        }
      }
    }
    twinlist.clear();
  }
  size_t totalPoints = static_cast<size_t>(m->totalPoints());
  for (size_t k = 0; k < totalPoints; k++)
  {
    int grainname = grain_indicies[k];
	if (twinnewnumbers[grainname] != -1) { grain_indicies[k] = twinnewnumbers[grainname];}
  }
}

void MergeTwins::renumber_grains()
{
  DataContainer* m = getDataContainer();
  if (NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << getNameOfClass() << " DataContainer was NULL";
    setErrorMessage(ss.str());
    return;
  }

  int32_t* grain_indicies = m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, m->totalPoints(), this);
  if (NULL == grain_indicies) { return; }

  size_t numgrains = m->m_Grains.size();
  int graincount = 1;
  std::vector<int > newnames(numgrains);
  for (size_t i = 1; i < numgrains; i++)
  {
    if (twinnewnumbers[i] == -1)
    {
      newnames[i] = graincount;
      float ea1good = m->m_Grains[i]->euler1;
      float ea2good = m->m_Grains[i]->euler2;
      float ea3good = m->m_Grains[i]->euler3;
      int size = m->m_Grains[i]->numvoxels;
      int numneighbors = m->m_Grains[i]->numneighbors;
      m->m_Grains[graincount]->numvoxels = size;
      m->m_Grains[graincount]->numneighbors = numneighbors;
      m->m_Grains[graincount]->euler1 = ea1good;
      m->m_Grains[graincount]->euler2 = ea2good;
      m->m_Grains[graincount]->euler3 = ea3good;
      graincount++;
    }
  }
#if 0
  tbb::parallel_for(tbb::blocked_range<size_t>(0, totalpoints ), ParallelRenumberGrains( this) );
#else
  int64_t totalPoints = m->totalPoints();
 for (int j = 0; j < totalPoints; j++)
  {
    int grainname = grain_indicies[j];
    if (grainname >= 1)
    {
      int newgrainname = newnames[grainname];
      grain_indicies[j] = newgrainname;
    }
  }
#endif
}

void MergeTwins::characterize_twins()
{
  DataContainer* m = getDataContainer();
  size_t numgrains = m->m_Grains.size();
  for (size_t i = 0; i < numgrains; i++)
  {

  }
}

