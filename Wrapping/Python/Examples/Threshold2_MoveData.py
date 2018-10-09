# Based on Threshold2 MoveData example
# 

# These are the simpl_py python modules

import dream3d
import dream3d.core as d3d
import dream3d.dream3d.simpl as simpl
import dream3d.utils.simpl_common as sc
import dream3d.utils.simpl_test_dirs as sd
import dream3d.dream3d.orientationanalysis as orientationanalysis
import dream3d.orientation_analysis as orientationanalysis_pythonic
import dream3d.dream3d.sampling as sampling
import dream3d.sampling as sampling_pythonic
import dream3d.dream3d.statistics as statistics
import dream3d.statistics as statistics_pythonic


def Threshold2_MoveDataTest():
  # Create Data Container Array
  dca = simpl.DataContainerArray.New()

  # Pipeline Annotation 1
  err = d3d.pipeline_annotation(dca, "This pipeline shows using the newer threshold objects filter and moving data around in the data structure.")
  if err < 0:
      print("PipelineAnnotation 1 ErrorCondition: %d" % err)

  # ReadAngData
  err = orientationanalysis_pythonic.read_ang_data(dca, "Small IN100", "Phase Data", "EBSD Scan Data",
   False, sd.GetBuildDirectory() + "/Debug/Data/SmallIN100/Slice_1.ang")
  if err < 0:
    print("ReadAngData ErrorCondition: %d" % err)

  err = sampling_pythonic.rotate_sample_ref_frame(dca, simpl.DataArrayPath("Small IN100", "EBSD Scan Data", ""),
   simpl.FloatVec3(0.0, 1.0, 0.0), 180.0, False)
  if err < 0:
    print("RotateSampleRefFrame ErrorCondition: %d" % err)

  err = orientationanalysis_pythonic.rotate_euler_ref_frame(dca, simpl.FloatVec3(0.0, 0.0, 1.0), 90.0,
   simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "EulerAngles"))
  if err < 0:
    print("RotateEulerRefFrame ErrorCondition: %d" % err)

  # MultiThresholdObjects filter  
  err = sc.MultiThresholdObjects2(dca, ["Small IN100", "EBSD Scan Data", ""], "Mask",
   ["AND", ("Confidence Index", 1, 0.1), ("Image Quality", 1, 400), ["OR", ("Image Quality", 0, 0)]])
  if err < 0:
    print("MultiThresholdObjects ErrorCondition: %d" % err)

  # Pipeline Annotation 2
  err = d3d.pipeline_annotation(dca, "These next filters show creating a new DataContainer and AttributeMatrix, \
   then we move some data arrays from the Small IN100/EBSD Scan Data Attribute Matrix to the new attributeMatrix using the \
   \"Move Multi Data\" filter and the \"Move Data\" filter.")
  if err < 0:
      print("PipelineAnnotation 2 ErrorCondition: %d" % err) 
  
  # Create Data Container
  # Improve helper function for creation of data containers to include a dca
  err = d3d.create_data_container(dca, "Extra_DataContainer")
  if err < 0:
    print("CreateDataContainer ErrorCondition: %d" % err)
  
  # Create Attribute Matrix
  # Using helper function
  dynamicTableData = sc.CreateDynamicTableData([[189, 201, 1]], ["0", "1", "2"], ["1"])
  err = d3d.create_attribute_matrix(dca, simpl.DataArrayPath("Extra_DataContainer","Positions",""), 13, dynamicTableData)

  if err < 0:
    print("CreateAttributeMatrix ErrorCondition: %d" % err)
  
  # Move Multiple Data Arrays
  err = sc.MoveMultiData(dca, sc.WhatToMove.AttributeArray, [("Small IN100", "EBSD Scan Data", "X Position"), 
  simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "Y Position")], simpl.DataArrayPath("Extra_DataContainer","Positions",""))
  if err < 0:
    print("MoveMultiData ErrorCondition: %d" % err)
  
  # Move Data Arrays
  err = sc.MoveData(dca, "Attribute Array", simpl.DataArrayPath("Small IN100", "EBSD Scan Data", "SEM Signal"),
   simpl.DataArrayPath("Extra_DataContainer", "Positions", ""))
  if err < 0:
    print("MoveData ErrorCondition: %d" % err)

  # Pipeline Annotation 3
  err = d3d.pipeline_annotation(dca, "This next part simply changes (arbitrarily) the Origin \
  and Resolution of the Image Geometry that the data resides on.")
  if err < 0:
      print("PipelineAnnotation 3 ErrorCondition: %d" % err) 

  # Set Origin & Resolution (Image)
  err = d3d.set_origin_resolution_image_geom(dca, "Small IN100", True, simpl.FloatVec3(50, 25, 10), 
  True, simpl.FloatVec3(1, 1, 25))
  if err < 0:
      print("SetOriginResolutionImageGeom ErrorCondition: %d" % err)

  # Write to DREAM3D File 1
  err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + "/Debug/Data/Output/Example/OriginChange.dream3d", dca)
  if err < 0:
    print("WriteDREAM3DFile 1 ErrorCondition: %d" % err)

  # Pipeline Annotation 4
  err = d3d.pipeline_annotation(dca, "This next part simply scales the image geometry by some arbitrary amount.")
  if err < 0:
      print("PipelineAnnotation 4 ErrorCondition: %d" % err) 
  
  # Change Scaling of Volume
  err = d3d.scale_volume(dca, "Small IN100", "", True, False,simpl.FloatVec3(2, 2, 2))
  if err < 0:
      print("ScaleVolume ErrorCondition: %d" % err) 

  # Write to DREAM3D File 2
  err = sc.WriteDREAM3DFile(sd.GetBuildDirectory() + "/Debug/Data/Output/Example/ScaleVolume.dream3d", dca)
  if err < 0:
    print("WriteDREAM3DFile 2 ErrorCondition: %d" % err)
	
"""
Main entry point for python script
"""
if __name__ == "__main__":
  Threshold2_MoveDataTest()