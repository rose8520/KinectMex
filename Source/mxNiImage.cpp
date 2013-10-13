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
* mxNiImage.cpp
* Get rgb and depth image by Kinect
* 2011.01.07 yohara94
******************************************************************************/

#include <XnCppWrapper.h>
#include <mex.h>
#define MAX_DEPTH 10000
float g_pDepthHist[MAX_DEPTH];

using namespace xn;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	mxArray *tmpContext, *tmpImage, *tmpImageMD, *tmpDepth, *tmpDepthMD;
	mxArray *context_initialised_array;
	mxArray *has_image_node_array;
	mxArray *has_depth_node_array;
	Context* context;

	DepthGenerator* depth;
	ImageGenerator* image;
	DepthMetaData* depthMD;
	ImageMetaData* imageMD;
	bool overlay = false;

	unsigned char* output_rgb;
	unsigned short* output_depth;
	mwSize dims2_rgb[3];
	mwSize dims2_depth[2];
	bool context_initialised = false;
	bool has_image_node = false;
	bool has_depth_node = false;

    //---------------------------------
    // Read input mex variables
    //---------------------------------
    // ni_context_obj
	tmpContext = mxGetField(prhs[0], 0, "ni_context_obj");
    memcpy((void*)&context, mxGetPr(tmpContext), sizeof(Context*));

    // depth_obj
	tmpDepth = mxGetField(prhs[0], 0, "depth_obj");
    memcpy((void*)&depth, mxGetPr(tmpDepth), sizeof(DepthGenerator*));

	// image_obj
	tmpImage = mxGetField(prhs[0], 0, "image_obj");
    memcpy((void*)&image, mxGetPr(tmpImage), sizeof(ImageGenerator*));

    // imageMD_obj
	tmpImageMD = mxGetField(prhs[0], 0, "imageMD_obj");
    memcpy((void*)&imageMD, mxGetPr(tmpImageMD), sizeof(ImageMetaData*));

	// depthMD_obj
	tmpDepthMD = mxGetField(prhs[0], 0, "depthMD_obj");
	memcpy((void*)&depthMD, mxGetPr(tmpDepthMD), sizeof(DepthMetaData*));

	// context_initialised
	context_initialised_array = mxGetField(prhs[0], 0, "context_initialised");
	context_initialised = mxGetScalar(context_initialised_array);

	has_depth_node_array = mxGetField(prhs[0], 0, "has_depth_node");
	has_depth_node = mxGetScalar(has_depth_node_array);

	has_image_node_array = mxGetField(prhs[0], 0, "has_image_node");
	has_image_node = mxGetScalar(has_image_node_array);

	//-------------------------------
	// Output
	//-------------------------------
	bool initialised = (context_initialised &&  has_image_node && has_depth_node);
	int image_width = 640;
	int image_height = 480;

	int depth_width = 640;
	int depth_height = 480;

	if(initialised){
		image->GetMetaData(*imageMD);
		depth->GetMetaData(*depthMD);

		image_width = imageMD->XRes();
		image_height = imageMD->YRes();

		depth_width = depthMD->XRes();
		depth_height = depthMD->YRes();

	}

	dims2_rgb[0] = image_height;
	dims2_rgb[1] = image_width;
	dims2_rgb[2] = 3;

	plhs[0] = mxCreateNumericArray(3, dims2_rgb, mxUINT8_CLASS, mxREAL);
	output_rgb = (unsigned char*)mxGetPr(plhs[0]);
	
	dims2_depth[0] = depth_height;
	dims2_depth[1] = depth_width;
	plhs[1] = mxCreateNumericArray(2, dims2_depth, mxUINT16_CLASS, mxREAL);
	output_depth = (unsigned short*)mxGetPr(plhs[1]);
	
	if(!initialised) return;

	const XnDepthPixel* pDepth = depthMD->Data();
	const XnUInt8* pImage = imageMD->Data();

	int rgb_image_size = image_width*image_height;
	for(int i=0;i<image_height;i++){
		for(int j = 0;j <image_width;j++){
			for(int k = 0;k < 3;k++){
				output_rgb[j*image_height+i+k*rgb_image_size] = pImage[3*(i*image_width+j)+k];
			}
		}
	}

	for(int i=0;i<depth_height;i++){
		for(int j = 0;j <depth_width;j++){
			output_depth[j*depth_height+i] = pDepth[i*depth_width+j];
		}
	}

}

