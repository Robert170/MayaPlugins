//-
// ==========================================================================
// Copyright 2015 Autodesk, Inc.  All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk
// license agreement provided at the time of installation or download,
// or which otherwise accompanies this software in either electronic
// or hard copy form.
// ==========================================================================
//+

////////////////////////////////////////////////////////////////////////
// DESCRIPTION:
//
// Produces the dependency graph node "yTwist".
//
// This plug-in demonstrates how to create a user-defined deformer.
// A deformer is a node which takes any number of input geometries, deforms
// them, and places the output into the output geometry attribute.
// This example plug-in defines a new deformer node that twists the deformed
// vertices of the input around the y-axis.
//
// To use this node: 
//	(1) Create a sphere or some other object. 
//	(2) Select the object. 
//	(3) Type: "deformer -type yTwist". 
//	(4) Bring up the channel box. 
//	(5) Select the yTwist input. 
//	(6) Change the Angle value of the yTwist input in the channel box. 
//
////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <maya/MIOStream.h>
#include <math.h>

#include <maya/MPxGeometryFilter.h>
#include <maya/MItGeometry.h>

#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>

#include <maya/MFnNumericAttribute.h>
#include <maya/MFnPlugin.h>
#include <maya/MFnDependencyNode.h>

#include <maya/MPoint.h>
#include <maya/MMatrix.h>


#define McheckErr(stat,msg)		\
	if ( MS::kSuccess != stat ) {	\
		cerr << msg;				\
		return MS::kFailure;		\
	}



class yTwist : public MPxGeometryFilter
{
public:
  yTwist();
  ~yTwist() override;

  static  void* creator();
  static  MStatus		initialize();

  // deformation function
  //
  MStatus   	deform(MDataBlock& block,
                     MItGeometry& iter,
                     const MMatrix& mat,
                     unsigned int 	multiIndex) override;

public:
  // yTwist attributes
  //
  static  MObject     angle;  		// angle to twist

  static  MTypeId		id;

private:

};

MTypeId     yTwist::id(0x001386c6);

////////////////////////
// yTwist attributes  //
////////////////////////

MObject     yTwist::angle;


yTwist::yTwist()
//
//	Description:
//		constructor
//
{
}

yTwist::~yTwist()
//
//	Description:
//		destructor
//
{}

void* yTwist::creator()
//
//	Description:
//		create the yTwist
//
{
  return new yTwist();
}

MStatus yTwist::initialize()
//
//	Description:
//		initialize the attributes
//
{
  // local attribute initialization
  //
  MFnNumericAttribute nAttr;
  angle = nAttr.create("angle", "fa", MFnNumericData::kDouble);
  nAttr.setDefault(0.0);
  nAttr.setKeyable(true);
  addAttribute(angle);

  // affects
  //
  attributeAffects(yTwist::angle, yTwist::outputGeom);

  return MS::kSuccess;
}

MStatus
yTwist::deform(MDataBlock& block,
  MItGeometry& iter,
  const MMatrix& /*m*/,
  unsigned int /*multiIndex*/)
  //
  // Method: deform
  //
  // Description:   Deform the point with a yTwist algorithm
  //
  // Arguments:
  //   block		: the datablock of the node
  //	 iter		: an iterator for the geometry to be deformed
  //   m    		: matrix to transform the point into world space
  //	 multiIndex : the index of the geometry that we are deforming
  //
  //
{
  MStatus status = MS::kSuccess;

  // determine the angle of the yTwist
  //
  MDataHandle angleData = block.inputValue(angle, &status);
  McheckErr(status, "Error getting angle data handle\n");
  double magnitude = angleData.asDouble();

  // determine the envelope (this is a global scale factor)
  //
  MDataHandle envData = block.inputValue(envelope, &status);
  McheckErr(status, "Error getting envelope data handle\n");
  float env = envData.asFloat();

  // iterate through each point in the geometry
  //
  for (; !iter.isDone(); iter.next()) {

    MPoint pt = iter.position();

    // do the twist
    //
    double ff = magnitude * pt.y * env;
    if (ff != 0.0) {
      double cct = cos(ff);
      double cst = sin(ff);
      double tt = pt.x * cct - pt.z * cst;
      pt.z = pt.x * cst + pt.z * cct;
      pt.x = tt;;
    }

    iter.setPosition(pt);
  }
  return status;
}

// standard initialization procedures
//

MStatus initializePlugin(MObject obj)
{
  MStatus result;
  MFnPlugin plugin(obj, PLUGIN_COMPANY, "3.0", "Any");
  result = plugin.registerNode("yTwist", yTwist::id, yTwist::creator,
                                yTwist::initialize, MPxNode::kDeformerNode);
  return result;
}

MStatus uninitializePlugin(MObject obj)
{
  MStatus result;
  MFnPlugin plugin(obj);
  result = plugin.deregisterNode(yTwist::id);
  return result;
}



//#include <string.h>
//#include <maya/MIOStream.h>
//#include <maya/MStringArray.h>
//#include <math.h>
//
//#include <maya/MPxDeformerNode.h>
//#include <maya/MItGeometry.h>
//#include <maya/MPxLocatorNode.h>
//
//#include <maya/MFnNumericAttribute.h>
//#include <maya/MFnMatrixAttribute.h>
//#include <maya/MFnMatrixData.h>
//
//#include <maya/MFnPlugin.h>
//#include <maya/MFnDependencyNode.h>
//
//#include <maya/MTypeId.h>
//#include <maya/MPlug.h>
//
//#include <maya/MDataBlock.h>
//#include <maya/MDataHandle.h>
//#include <maya/MArrayDataHandle.h>
//
//#include <maya/MPoint.h>
//#include <maya/MVector.h>
//#include <maya/MMatrix.h>
//
//#include <maya/MDagModifier.h>
//
//#include <maya/MPxGPUDeformer.h>
//#include <maya/MGPUDeformerRegistry.h>
//#include <maya/MOpenCLInfo.h>
//#include <maya/MViewport2Renderer.h>
//#include <maya/MFnMesh.h>
//#include <clew/clew_cl.h>
//#include <vector>
//
//class RandomCircle : public MPxDeformerNode
//{
// public:
//  RandomCircle();
//  ~RandomCircle() override;
//  MStatus deform(MDataBlock&    block,
//                 MItGeometry&   iter,
//                 const MMatrix& mat,
//                 unsigned int multiIndex) override;
//
//  static	void* creator();
//  static	MStatus		initialize();
//  static	MObject	deformingMesh;		// Reference mesh for splat deforming
//  static  MObject parallelEnabled;	// Boolean indicating whether the parallel compute is to be used
//
//
//public:
//
//  static const	MTypeId		id;
//};
//
//const MTypeId RandomCircle::id(0x001386c6);
//
//
//// The creator method creates an instance of the RandomCircleNode class
//// and is the first method called by Maya
//// when a RandomCircleNode needs to be created.
////
//void* RandomCircle::creator()
//{
//  return new RandomCircle();
//}
//
//// The initialize routine is called after the node has been created.
//// It sets up the input and output attributes and adds them to the node.
//// Finally the dependencies are arranged so that when the inputs
//// change Maya knowns to call compute to recalculate the output values.
//// The inputs are: input, scale, frames
//// The outputs are: sineOutput, cosineOutput
////
//MStatus RandomCircle::initialize()
//{
//  
//  return MS::kSuccess;
//}
//
//// The constructor does nothing
////
//RandomCircle::RandomCircle() {}
//
//// The destructor does nothing
////
//RandomCircle::~RandomCircle() {}
//
//MStatus RandomCircle::deform(MDataBlock& block, 
//                             MItGeometry& iter, 
//                             const MMatrix& mat, 
//                             unsigned int multiIndex) {
//  MStatus returnStatus;
//
//  float env = block.inputValue( envelope ).asFloat();
//
//    // Iterate through each point in the geometry.
//    //
//  for (; !iter.isDone(); iter.next()) {
//    MPoint pt = iter.position();
//
//    // Perform some calculation on pt.
//    // e.g.
//     pt.x += 50.0f * env;
//
//    // Set the final position.
//    iter.setPosition(pt);
//  }
//  return returnStatus;
//}
//
//
//// The initializePlugin method is called by Maya when the RandomCircleNode
//// plugin is loaded.  It registers the RandomCircleNode which provides
//// Maya with the creator and initialize methods to be called when
//// a RandomCircleNode is created.
////
//MStatus initializePlugin(MObject obj)
//{
//  MStatus result;
//
//  MFnPlugin plugin(obj, "Roberto", "3.0", "Any");
//  result = plugin.registerNode("RandomCircle",
//                               RandomCircle::id,
//                               &RandomCircle::creator,
//                               &RandomCircle::initialize,
//                               MPxNode::kDeformerNode);
//  if (!result) {
//    result.perror("registerNode");
//    return result;
//  }
//
//  return result;
//}
//
//// The unitializePlugin is called when Maya needs to unload the plugin.
//// It basically does the opposite of initialize by calling
//// the deregisterCommand to remove it.
////
//MStatus 
//uninitializePlugin(MObject obj) {
//  
//  MStatus result;
//
//  MFnPlugin plugin(obj);
//  result = plugin.deregisterNode(RandomCircle::id);
//  if (!result) {
//    result.perror("deregisterNode");
//    return result;
//  }
//
//  return result;
//}
//
//int main() {
//  return 0;
//}