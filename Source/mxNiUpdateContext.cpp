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
* mxNiCreateContext.cpp
* Initialise to get images by Kinect
* 2011.01.07 yohara94
******************************************************************************/

#include <XnCppWrapper.h>
#include <mex.h>

using namespace xn;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	mxArray *tmpContext, *tmpDepth, *tmpImage;

	Context* context;
	DepthGenerator* depth;
	ImageGenerator* image;

	mxArray *has_image_node_array;
	mxArray *has_depth_node_array;
	mxArray *context_initialised_array;

	bool context_initialised = false;
	bool has_image_node = false;
	bool has_depth_node = false;

	//---------------------------------
    // Read input variables
    //---------------------------------
	has_depth_node_array = mxGetField(prhs[0], 0, "has_depth_node");
	has_depth_node = mxGetScalar(has_depth_node_array);

	has_image_node_array = mxGetField(prhs[0], 0, "has_image_node");
	has_image_node = mxGetScalar(has_image_node_array);

	// ni_context_obj
	tmpContext = mxGetField(prhs[0], 0, "ni_context_obj");
    memcpy((void*)&context, mxGetPr(tmpContext), sizeof(Context*));
	
    // depth_obj
	tmpDepth = mxGetField(prhs[0], 0, "depth_obj");
    memcpy((void*)&depth, mxGetPr(tmpDepth), sizeof(DepthGenerator*));

	// image_obj
	tmpImage = mxGetField(prhs[0], 0, "image_obj");
    memcpy((void*)&image, mxGetPr(tmpImage), sizeof(ImageGenerator*));

	// context_initialised
	context_initialised_array = mxGetField(prhs[0], 0, "context_initialised");
	context_initialised = mxGetScalar(context_initialised_array);
	bool image_initialised = (context_initialised &&  has_image_node && has_depth_node);

	bool useOption = (nrhs >= 2);
	if(useOption && image_initialised){
		mxArray* overlay_flg_array = mxGetField(prhs[1], 0, "adjust_view_point"); 
		bool adjust_view_point_flg = mxGetScalar(overlay_flg_array);
		bool use_image_view_point = depth->GetAlternativeViewPointCap().IsViewPointAs(*image);
	
		if(use_image_view_point != adjust_view_point_flg){
			if(adjust_view_point_flg){
				depth->GetAlternativeViewPointCap().SetViewPoint(*image);
			} else{
				depth->GetAlternativeViewPointCap().ResetViewPoint();
			}
		}
	}

	if(context_initialised){
		context->WaitAnyUpdateAll();
		context->WaitAndUpdateAll();
	}
}

