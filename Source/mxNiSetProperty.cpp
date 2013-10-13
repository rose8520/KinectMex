/*****************************************************************************
*                                                                            *
*  Kinect-MEX 1.3                                                            *
*  Copyright (C) 2011 Hideki Shirai, Yu Ohara@DENSO IT LABORATORY, INC.      *
*                                                                            *
*  This file is part of Kinect-MEX.                                          *
*                                                                            *
*  Kinect-MEX is free software: you can redistribute it and/or modify        *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  Kinect-MEX is distributed in the hope that it will be useful,             *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/


/*****************************************************************************
* mxNiSetProperty.cpp
* Sets the value of property
* 2011.01.24 yohara94
******************************************************************************/

#include "CNiPropertyUtils.h"

#include <XnCppWrapper.h>
#include <mex.h>
#include <string>
#include <map>

using namespace xn;
using namespace NiPropertyUtils;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	CreatePropKeyList();

	mxArray *tmpDepth, *tmpImage, *tmpIR;
	mxArray *context_initialised_array;
	mxArray *has_depth_node_array, *has_image_node_array, *has_ir_node_array;

	DepthGenerator* depth;
	ImageGenerator* image;
	IRGenerator* ir;

	bool context_initialised = false;
	bool has_depth_node = false;
	bool has_image_node = false;
	bool has_ir_node = false;
	char prop_name[256];
	double value;

    //---------------------------------
    // Read input variables
    //---------------------------------
    // depth_obj
	tmpDepth = mxGetField(prhs[0], 0, "depth_obj");
    memcpy((void*)&depth, mxGetPr(tmpDepth), sizeof(DepthGenerator*));

    // image_obj
	tmpImage = mxGetField(prhs[0], 0, "image_obj");
    memcpy((void*)&image, mxGetPr(tmpImage), sizeof(ImageGenerator*));

    // ir_obj
	tmpIR = mxGetField(prhs[0], 0, "ir_obj");
    memcpy((void*)&ir, mxGetPr(tmpIR), sizeof(IRGenerator*));

	// context_initialised
	context_initialised_array = mxGetField(prhs[0], 0, "context_initialised");
	context_initialised = mxGetScalar(context_initialised_array);

	// has_depth_node
	has_depth_node_array = mxGetField(prhs[0], 0, "has_depth_node");
	has_depth_node = mxGetScalar(has_depth_node_array);

	// has_image_node
	has_image_node_array = mxGetField(prhs[0], 0, "has_image_node");
	has_image_node = mxGetScalar(has_image_node_array);

	// has_ir_node
	has_ir_node_array = mxGetField(prhs[0], 0, "has_ir_node");
	has_ir_node = mxGetScalar(has_ir_node_array);

	mxGetString(prhs[1], prop_name, 256);

	value = mxGetScalar(prhs[2]);

	//-------------------------------
	// Create output variables
	//-------------------------------
	PropData prop = GetPropData(prop_name);
	ProductionNode* node;
	ENodeType nodeType = prop.first;
	EType propType = prop.second;
	
	bool hasNode = false;
	if(nodeType == kDepthNode){
		node = depth;
		hasNode = has_depth_node;
	} else if(nodeType == kImageNode){
		node = image;
		hasNode = has_image_node;
	} else if(nodeType == kIRNode){
		node = ir;
		hasNode = has_ir_node;
	}

	if(!context_initialised || !hasNode){
		return;
	}
	
	if(propType == kInt){
		node->SetIntProperty(prop_name, (XnUInt64)value);
	} else if(propType == kReal){
		node->GetRealProperty(prop_name, (XnDouble)value);
	}
}

