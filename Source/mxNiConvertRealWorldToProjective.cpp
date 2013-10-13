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
* mxNiConvertRealWorldToProjective.cpp
* Convert XYZ on world coordinate system to pixel (u,v) and depth value[mm]
* 2011.01.07 yohara94
******************************************************************************/

#include <XnCppWrapper.h>
#include <mex.h>

using namespace xn;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	int i, j;
	int width, height;

	mxArray *tmpDepth;
	mxArray *context_initialised_array;
	mxArray *has_depth_node_array;
	unsigned short* projective;
	float* real_world_coord;
	XnPoint3D* ni_projective;
	XnPoint3D* ni_real_world_coord;

	DepthGenerator* depth;

	bool context_initialised = false;
	bool has_depth_node = false;

	mwSize dims2[3];
	const mwSize *dims;

	dims = mxGetDimensions(prhs[1]);
	width = dims[0];
	height  = dims[1];

    //---------------------------------
    // Read input variables
    //---------------------------------
    // depth_obj
	tmpDepth = mxGetField(prhs[0], 0, "depth_obj");
    memcpy((void*)&depth, mxGetPr(tmpDepth), sizeof(DepthGenerator*));

	// context_initialised
	context_initialised_array = mxGetField(prhs[0], 0, "context_initialised");
	context_initialised = mxGetScalar(context_initialised_array);

	has_depth_node_array = mxGetField(prhs[0], 0, "has_depth_node");
	has_depth_node = mxGetScalar(has_depth_node_array);

	real_world_coord = (float*)mxGetPr(prhs[1]);

	//-------------------------------
	// Create output variables
	//-------------------------------
	dims2[0] = width;
	dims2[1] = height;
	dims2[2] = 3;

	plhs[0] = mxCreateNumericArray(3, dims2, mxUINT16_CLASS, mxREAL);
	projective = (unsigned short*)mxGetPr(plhs[0]);

	if(!context_initialised || !has_depth_node){
		return;
	}
	//-------------------------------
	// Run
	//-------------------------------

	ni_projective = new XnPoint3D[height*width];
	ni_real_world_coord = new XnPoint3D[height*width];

	int size = width*height;
		
	for(i = 0;i < height;i++){
		for(j = 0;j < width;j++){
			ni_real_world_coord[i*width+j].X = (float)real_world_coord[i*width+j];
			ni_real_world_coord[i*width+j].Y = (float)real_world_coord[i*width+j+size];
			ni_real_world_coord[i*width+j].Z = (float)real_world_coord[i*width+j+2*size];
		}
	}

	XnStatus status = depth->ConvertRealWorldToProjective(size, ni_real_world_coord, ni_projective);
	if(status == XN_STATUS_OK){
		for(i=0;i<height;i++){
			for(j = 0;j < width;j++){
				projective[i*width+j] = (unsigned short)ni_projective[i*width+j].X;
				projective[i*width+j+size] = (unsigned short)ni_projective[i*width+j].Y;
				projective[i*width+j+2*size] = (unsigned short)ni_projective[i*width+j].Z;
			}
		}
	}

	delete []ni_real_world_coord;
	delete []ni_projective;
}
