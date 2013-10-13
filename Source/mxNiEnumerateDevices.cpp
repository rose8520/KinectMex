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
* mxNiEnumerateDevices.cpp
* Return the value of property
* 2011.03.31 yohara94
******************************************************************************/
#include <XnCppWrapper.h>
#include <mex.h>
#include <string>
#include <map>

using namespace xn;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	XnStatus rc;

	NodeInfoList infoList;
	Context context;
	context.Init();
	int numDevices = 0;

	rc = context.EnumerateProductionTrees(XN_NODE_TYPE_IMAGE,NULL,infoList, NULL);
	int count = 0;
	for (xn::NodeInfoList::Iterator nodeIt=infoList.Begin(); nodeIt != infoList.End();++nodeIt) {
		count++;
	}
	numDevices = count;
	context.Shutdown();

	plhs[0] = mxCreateDoubleScalar(numDevices);
}

