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
* CNiPropertyUtils.cpp
* Create the list of property's key 
* 2011.01.24 yohara94
******************************************************************************/
#include <string>
#include <map>
	enum EType{
	kInt, 
	kReal,
	kUInt16Mat,
	kInvalidType
	};

namespace NiPropertyUtils {


	enum ENodeType{
		kDepthNode, 
		kImageNode,
		kIRNode,
		kInvalidNodeType
	};

	typedef std::pair<ENodeType, EType> PropData;
	typedef std::map<std::string, PropData > PropKeyType;
	static  PropKeyType prop_keys;
	
	static bool fInitialised = false;

	// Register the property's name and its type
	void CreatePropKeyList()
	{
		if(fInitialised) return;

		prop_keys["Gain"] = std::make_pair(kDepthNode, kInt);
		prop_keys["HoleFilter"] = std::make_pair(kDepthNode, kInt);
		prop_keys["MinDepthValue"] = std::make_pair(kDepthNode, kInt);
		prop_keys["MaxDepthValue"] = std::make_pair(kDepthNode, kInt);
		prop_keys["ShadowValue"] = std::make_pair(kDepthNode, kInt);
		prop_keys["NoSampleValue"] = std::make_pair(kDepthNode, kInt);
		prop_keys["Registration"] = std::make_pair(kDepthNode, kInt);
		prop_keys["RegistrationType"] = std::make_pair(kDepthNode, kInt);
		prop_keys["WhiteBalancedEnabled"] = std::make_pair(kDepthNode, kInt);
		prop_keys["ConstShift"] = std::make_pair(kDepthNode, kInt);
		prop_keys["PixelSizeFactor"] = std::make_pair(kDepthNode, kInt);
		prop_keys["MaxShift"] = std::make_pair(kDepthNode, kInt);
		prop_keys["ParamCoeff"] = std::make_pair(kDepthNode, kInt);
		prop_keys["ShiftScale"] = std::make_pair(kDepthNode, kInt);
		prop_keys["DeviceMaxDepth"] = std::make_pair(kDepthNode, kInt);
		prop_keys["ZPD"] = std::make_pair(kDepthNode, kInt);
		prop_keys["ZPPS"] = std::make_pair(kDepthNode, kReal);
		prop_keys["LDDIS"] = std::make_pair(kDepthNode, kReal);
		prop_keys["GmcMode"] = std::make_pair(kDepthNode, kInt);
		prop_keys["S2D"] = std::make_pair(kDepthNode, kUInt16Mat);
		prop_keys["OutputFormat"] = std::make_pair(kDepthNode, kInt);
		fInitialised = true;
	}

	// Get the value of the property by the given name
	PropData GetPropData(const std::string& name){
		 PropKeyType::iterator iter = prop_keys.find(name);
		if(iter ==  prop_keys.end()) return PropData(kInvalidNodeType, kInvalidType);

		return iter->second;
	}

}

