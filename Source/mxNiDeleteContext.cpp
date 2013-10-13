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
* mxNiDeleteContext.cpp
* Delete context pointer
* 2011.01.07 yohara94
******************************************************************************/

#include <XnCppWrapper.h>
#include <mex.h>

using namespace xn;
static int call_count = 0;

void StopGenerating(Generator* generator)
{
	if(!generator || !generator->IsGenerating()) return;
	generator->StopGenerating();
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	mxArray *tmpContext, *tmpDepth, *tmpImage;
	mxArray *tmpDepthMD, *tmpImageMD;
	mxArray *tmpIR, *tmpIRMD;
	mxArray *tmpUser;
	mxArray *tmpScene;
	mxArray* tmpHands;

	mxArray* context_initialised_array;
	mxArray* has_depth_node_array;
	mxArray* has_image_node_array;
	mxArray* has_ir_node_array;
	mxArray* has_scene_node_array;
	mxArray* has_hands_node_array;
	mxArray* has_user_node_array;
	mxArray* sensor_index_array;
	mxArray* num_sensors_array;

	Context* context;
	DepthGenerator* depth;
	ImageGenerator* image;
	IRGenerator* ir;
	UserGenerator* user;
	SceneAnalyzer* scene;
	HandsGenerator* hands;
	DepthMetaData* depthMD;
	ImageMetaData* imageMD;
	IRMetaData* irMD;

	bool context_initialised = false;
	bool has_depth_node = false;
	bool has_image_node = false;
	bool has_ir_node = false;
	bool has_scene_node = false;
	bool has_hands_node = false;
	bool has_user_node = false;

	int sensorIndex = 0;
	int numSensors = 0;
	//---------------------------------
    // Read input variables
    //---------------------------------
	has_depth_node_array = mxGetField(prhs[0], 0, "has_depth_node");
	has_depth_node = mxGetScalar(has_depth_node_array);

	has_image_node_array = mxGetField(prhs[0], 0, "has_image_node");
	has_image_node = mxGetScalar(has_image_node_array);

	has_ir_node_array = mxGetField(prhs[0], 0, "has_ir_node");
	has_ir_node = mxGetScalar(has_ir_node_array);

	has_scene_node_array = mxGetField(prhs[0], 0, "has_scene_node");
	has_scene_node = mxGetScalar(has_scene_node_array);

	has_hands_node_array = mxGetField(prhs[0], 0, "has_hands_node");
	has_hands_node = mxGetScalar(has_hands_node_array);

	has_user_node_array = mxGetField(prhs[0], 0, "has_user_node");
	has_user_node = mxGetScalar(has_user_node_array);

	// ni_context_obj
	tmpContext = mxGetField(prhs[0], 0, "ni_context_obj");
    memcpy((void*)&context, mxGetPr(tmpContext), sizeof(Context*));
	
    // depth_obj
	tmpDepth = mxGetField(prhs[0], 0, "depth_obj");
    memcpy((void*)&depth, mxGetPr(tmpDepth), sizeof(DepthGenerator*));

    // image_obj
	tmpImage = mxGetField(prhs[0], 0, "image_obj");
    memcpy((void*)&image, mxGetPr(tmpImage), sizeof(ImageGenerator*));

    // ir_obj
	tmpIR = mxGetField(prhs[0], 0, "ir_obj");
	memcpy((void*)&ir, mxGetPr(tmpIR), sizeof(IRGenerator*));

    // user_obj
	tmpUser = mxGetField(prhs[0], 0, "user_obj");
	memcpy((void*)&user, mxGetPr(tmpUser), sizeof(UserGenerator*));

    // scene_obj
	tmpScene = mxGetField(prhs[0], 0, "scene_obj");
	memcpy((void*)&scene, mxGetPr(tmpScene), sizeof(SceneAnalyzer*));

    // hands_obj
	tmpHands = mxGetField(prhs[0], 0, "hands_obj");
	memcpy((void*)&hands, mxGetPr(tmpHands), sizeof(HandsGenerator*));

	// depthMD_obj
	tmpDepthMD = mxGetField(prhs[0], 0, "depthMD_obj");
    memcpy((void*)&depthMD, mxGetPr(tmpDepthMD), sizeof(DepthMetaData*));

    // imageMD_obj
	tmpImageMD = mxGetField(prhs[0], 0, "imageMD_obj");
    memcpy((void*)&imageMD, mxGetPr(tmpImageMD), sizeof(ImageMetaData*));

    // irMD_obj
	tmpIRMD = mxGetField(prhs[0], 0, "irMD_obj");
	memcpy((void*)&irMD, mxGetPr(tmpIRMD), sizeof(IRMetaData*));

	// context_initialised
	context_initialised_array = mxGetField(prhs[0], 0, "context_initialised");
	context_initialised = mxGetScalar(context_initialised_array);

	// sensor_index
	sensor_index_array = mxGetField(prhs[0], 0, "sensor_index");
	sensorIndex = mxGetScalar(sensor_index_array);

	//num_sensors
	num_sensors_array = mxGetField(prhs[0], 0, "num_sensors");
	numSensors = mxGetScalar(num_sensors_array);

	if(hands && has_hands_node) StopGenerating(hands);
	if(scene && has_scene_node) StopGenerating(scene);
	if(image && has_image_node) StopGenerating(image);
	if(depth && has_depth_node) StopGenerating(depth);
	if(ir && has_ir_node) StopGenerating(ir);

	if(context_initialised && call_count == numSensors -1){
		context->Shutdown();
	}

	if(has_ir_node) delete irMD;
	if(has_image_node) delete imageMD;
	if(has_depth_node) delete depthMD;
	if(hands) delete hands;
	if(scene) delete scene;
	if(user) delete user;
	if(ir) delete ir;
	if(image) delete image;
	if(depth) delete depth;
	if(context && call_count == numSensors -1){
		delete context;
		call_count = 0;
	} else{
		call_count++;
	}
}
