/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef OIMIMPORT_H_
#define OIMIMPORT_H_



#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif



#include <vector>
#include <string>

#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/MXATypes.h>

#include "EbsdLib/TSL/H5AngImporter.h"
#include "EbsdLib/HKL/H5CtfImporter.h"

#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Common/AbstractPipeline.h"


/**
 * @class OIMImport OIMImport.h AIM/ANG/OIMImport.h
 * @brief This class is used to import multiple TSL .ang files into an HDF5 file.
 * @author Michael A. Jackson for BlueQuartz Software
 * @author Dr. Michael Groeber, USAFRL*
 * @date March 23, 2011
 * @version 1.2
 *
 */
class OIMImport : public AbstractPipeline
{
  public:
    MXA_SHARED_POINTERS(OIMImport)
    MXA_TYPE_MACRO(OIMImport)
    MXA_STATIC_NEW_MACRO(OIMImport)

    virtual ~OIMImport();

    MXA_INSTANCE_STRING_PROPERTY(OutputFile)
    MXA_INSTANCE_PROPERTY(int, ZStartIndex)
    MXA_INSTANCE_PROPERTY(int, ZEndIndex)
    MXA_INSTANCE_PROPERTY(float, ZResolution)
    MXA_INSTANCE_PROPERTY(std::vector<std::string>, AngFileList);

    /**
     * @brief Main method to run the operation
     */
    void execute();

  protected:
    OIMImport();


  private:

    OIMImport(const OIMImport&); // Copy Constructor Not Implemented
    void operator=(const OIMImport&); // Operator '=' Not Implemented
};


#endif /* OIMIMPORT_H_ */
