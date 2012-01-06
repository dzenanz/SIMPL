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

#ifndef LOADVOLUME_H_
#define LOADVOLUME_H_

#include <vector>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/DataContainer.h"

/**
 * @class LoadVolume LoadVolume.h DREAM3DLib/GenericFilters/LoadVolume.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT LoadVolume : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(LoadVolume);
    DREAM3D_STATIC_NEW_MACRO(LoadVolume);
    DREAM3D_TYPE_MACRO_SUPER(LoadVolume, AbstractFilter);

    virtual ~LoadVolume();

    DREAM3D_INSTANCE_STRING_PROPERTY(InputFile)

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::StatisticsFilters; }
    virtual const std::string getHumanLabel() { return "Load Voxel Volume"; }

    virtual void setupFilterOptions();

    /**
    * @brief Reimplemented from @see AbstractFilter class
    */
    virtual void execute();


  protected:
    LoadVolume();

    void initializeAttributes();
    void initializeGrains();

  private:
    int32_t* grain_indicies;
    int32_t* phases;
    float* euler1s;
    float* euler2s;
    float* euler3s;
    int8_t* surfacevoxels;
    int32_t* neighbors;
    float* quats;

    LoadVolume(const LoadVolume&); // Copy Constructor Not Implemented
    void operator=(const LoadVolume&); // Operator '=' Not Implemented
};

#endif /* LOADVOLUME_H_ */
