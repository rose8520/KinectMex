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
* mxNiIRImage.cpp
* Acquire ir and depth image by Kinect
* 2011.01.26 yohara94
******************************************************************************/

#include <XnCppWrapper.h>
#include <mex.h>
using namespace xn;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	mxArray *tmpContext;
	mxArray *tmpIR, *tmpIRMD;
	mxArray *tmpDepth, *tmpDepthMD;
	mxArray *context_initialised_array;
	mxArray *has_ir_node_array;
	mxArray *has_depth_node_array;

	Context* context;

	IRGenerator* ir;
	IRMetaData* irMD;

	DepthGenerator* depth;
	DepthMetaData* depthMD;

	unsigned short* output_ir = 0;
	unsigned short* output_depth = 0;

	mwSize dims2_ir[2];
	mwSize dims2_depth[2];
	
	bool context_initialised = false;
	bool has_ir_node = false;
	bool has_depth_node = false;

    //---------------------------------
    // Read input variables
    //---------------------------------
    // ni_context_obj
	tmpContext = mxGetField(prhs[0], 0, "ni_context_obj");
    memcpy((void*)&context, mxGetPr(tmpContext), sizeof(Context*));

    // ir_obj
	tmpIR = mxGetField(prhs[0], 0, "ir_obj");
    memcpy((void*)&ir, mxGetPr(tmpIR), sizeof(IRGenerator*));

    // irMD_obj
	tmpIRMD = mxGetField(prhs[0], 0, "irMD_obj");
    memcpy((void*)&irMD, mxGetPr(tmpIRMD), sizeof(IRMetaData*));

    // depth_obj
	tmpDepth = mxGetField(prhs[0], 0, "depth_obj");
    memcpy((void*)&depth, mxGetPr(tmpDepth), sizeof(DepthGenerator*));

    // depthMD_obj
	tmpDepthMD = mxGetField(prhs[0], 0, "depthMD_obj");
    memcpy((void*)&depthMD, mxGetPr(tmpDepthMD), sizeof(DepthMetaData*));

	// context_initialised
	context_initialised_array = mxGetField(prhs[0], 0, "context_initialised");
	context_initialised = mxGetScalar(context_initialised_array);

	// has_ir_node
	has_ir_node_array = mxGetField(prhs[0], 0, "has_ir_node");
	has_ir_node = mxGetScalar(has_ir_node_array);

	// has_depth_node
	has_depth_node_array = mxGetField(prhs[0], 0, "has_depth_node");
	has_depth_node = mxGetScalar(has_depth_node_array);

	//-------------------------------
	// Create output variables
	//-------------------------------	
	bool initialised = context_initialised;

	int ir_width = 640;
	int ir_height = 480;

	int depth_width = 640;
	int depth_height = 480;

	if(initialised){
		XnStatus rc;
		
		if(has_ir_node){
			ir->GetMetaData(*irMD);

			ir_width = irMD->XRes();
			ir_height = irMD->YRes();
		}

		if(has_depth_node){
			depth->GetMetaData(*depthMD);

			depth_width = depthMD->XRes();
			depth_height = depthMD->YRes();
		}
	} 

	dims2_ir[0] = ir_height;
	dims2_ir[1] = ir_width;
	plhs[0] = mxCreateNumericArray(2, dims2_ir, mxUINT16_CLASS, mxREAL);
	output_ir = (unsigned short*)mxGetPr(plhs[0]);

	if(nlhs >= 2){
		dims2_depth[0] = depth_height;
		dims2_depth[1] = depth_width;
		plhs[1] = mxCreateNumericArray(2, dims2_depth, mxUINT16_CLASS, mxREAL);
		output_depth = (unsigned short*)mxGetPr(plhs[1]);
	}
	
	if(!initialised) return;

	if(has_ir_node && output_ir != 0){
		const XnIRPixel* pIR = irMD->Data();
	
		int ir_image_size = ir_width*ir_height;
		for(int i=0;i<ir_height;i++){
			for(int j = 0;j < ir_width;j++){
				output_ir[j*ir_height+i] = pIR[i*ir_width+j];
			}
		}
	}

	if(has_depth_node && output_depth != 0){
		const XnDepthPixel* pDepth = depthMD->Data();
	
		int depth_image_size = depth_width*depth_height;
		for(int i=0;i<depth_height;i++){
			for(int j = 0;j < depth_width;j++){
				output_depth[j*depth_height+i] = pDepth[i*depth_width+j];
			}
		}
	}

}

