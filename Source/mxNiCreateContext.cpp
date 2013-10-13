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

static Context* context = 0;
DepthGenerator* depth;
ImageGenerator* image;
UserGenerator* user;
IRGenerator* ir;
SceneAnalyzer* scene;
HandsGenerator* hands;

DepthMetaData* depthMD;
ImageMetaData* imageMD;
IRMetaData* irMD;

//static bool context_initialised = false;
bool context_initialised = false;
bool has_image_node = false;
bool has_depth_node = false;
bool has_ir_node = false;
bool has_scene_node = false;
bool has_hands_node = false;
bool has_user_node = false;
bool use_xml = false;
int numSensors = 1;

static NodeInfoList imageNodeInfoList;
static NodeInfoList depthNodeInfoList;
static NodeInfoList irNodeInfoList;
static NodeInfoList sceneNodeInfoList;
static NodeInfoList handsNodeInfoList;
static NodeInfoList userNodeInfoList;

struct SGeneratorInfo {
	SGeneratorInfo();

	XnMapOutputMode f_mode;
	bool f_mirror;
};

SGeneratorInfo::SGeneratorInfo()
{
	f_mode.nXRes = 640;
	f_mode.nYRes = 480;
	f_mode.nFPS = 30;
	f_mirror = true;
}

XnStatus InitProductionNode(NodeInfoList* nodeInfoList, Context* context,
						ProductionNode* productionNode, int sensorIndex);

XnStatus StartMapGenerator(MapGenerator* mapGenerator, 
							const SGeneratorInfo& info);
XnStatus StartGenerator(Generator* generator, const SGeneratorInfo& info);

XnStatus LoadFromXml(char* xml_filename);
XnStatus LoadFromStructure(const mxArray* info, int sensorIndex);
NodeInfo GetNodeInfo(int sensorIndex, const NodeInfoList& nodeInfoList);

XnStatus LoadFromXml_MultipleDevices(char* xml_filename, 
									const mxArray* structure, 
									int sensorIndex);

XnStatus InitProductionNode(NodeInfoList* nodeInfoList, Context* context,
						ProductionNode* productionNode, int sensorIndex)
{
	XnStatus rc;
	int count = 0;
	for (xn::NodeInfoList::Iterator nodeIt=nodeInfoList->Begin(); nodeIt != nodeInfoList->End();++nodeIt) {
		if(count == sensorIndex){
			NodeInfo nodeInfo = *nodeIt;
			const XnProductionNodeDescription& description = nodeInfo.GetDescription();
	
			rc = context->CreateProductionTree(nodeInfo);
			rc = nodeInfo.GetInstance(*productionNode);
		}
		count++;
	}
	numSensors = count;
	return rc;
}

XnStatus StartMapGenerator(MapGenerator* mapGenerator, 
							const SGeneratorInfo& info)
{
	XnStatus rc;
	mapGenerator->SetMapOutputMode(info.f_mode);
	rc = mapGenerator->StartGenerating();
	MirrorCapability mirror = mapGenerator->GetMirrorCap();
	mirror.SetMirror(info.f_mirror);

	return rc;
}

XnStatus StartGenerator(Generator* generator,
						const SGeneratorInfo& info)
{
	XnStatus rc = generator->StartGenerating();
	MirrorCapability mirror = generator->GetMirrorCap();
	mirror.SetMirror(info.f_mirror);
	return rc;
}

XnStatus LoadFromXml_MultipleDevices(char* xml_filename, 
									const mxArray* structure, 
									int sensorIndex)
{
	XnStatus rc;
	rc = LoadFromStructure(structure, sensorIndex);
	return XN_STATUS_OK;
}


XnStatus LoadFromXml(char* xml_filename)
{
	XnStatus rc;
	// initialise by xml file
	EnumerationErrors errors;
	rc = context->InitFromXmlFile(xml_filename, &errors);

	if (rc == XN_STATUS_NO_NODE_PRESENT){
		XnChar strError[1024];
		errors.ToString(strError, 1024);
		mexPrintf("%s\n", strError);
		return rc;
	} else if (rc != XN_STATUS_OK){
		mexPrintf("Open failed: %s\n Check whether %s is available.\n", xnGetStatusString(rc), xml_filename);
		return rc;
	}

	rc = context->FindExistingNode(XN_NODE_TYPE_DEPTH, *depth);
	has_depth_node = (rc == XN_STATUS_OK);
	if(has_depth_node){
		depth->GetMetaData(*depthMD);
	}

	rc = context->FindExistingNode(XN_NODE_TYPE_IMAGE, *image);
	has_image_node = (rc == XN_STATUS_OK);
	if(has_image_node){
		image->GetMetaData(*imageMD);
	}

	rc = context->FindExistingNode(XN_NODE_TYPE_IR, *ir);
	has_ir_node = (rc == XN_STATUS_OK);
	if(rc == XN_STATUS_OK){
		ir->GetMetaData(*irMD);
	}

	rc = context->FindExistingNode(XN_NODE_TYPE_USER, *user);
	if (rc != XN_STATUS_OK){
		has_user_node = true;
		user->Create(*context);
	}

	rc = context->FindExistingNode(XN_NODE_TYPE_SCENE, *scene);
	has_scene_node = (rc == XN_STATUS_OK);

	rc = context->FindExistingNode(XN_NODE_TYPE_HANDS, *hands);
	has_hands_node = (rc == XN_STATUS_OK);

	context->StartGeneratingAll();
	context->WaitAnyUpdateAll();
	
	return XN_STATUS_OK;
}

void LoadGeneratorInfo(mxArray* infoArray, SGeneratorInfo* info)
{
	int width;
	int height;
	int fps;
	bool mirror;

	mxArray* width_array;
	mxArray* height_array;
	mxArray* fps_array;
	mxArray* mirror_array;

	width_array = mxGetField(infoArray, 0, "width");
	if(width_array){
		width = mxGetScalar(width_array);
	}
	
	height_array = mxGetField(infoArray, 0, "height");
	if(height_array){
		height = mxGetScalar(height_array);
	}

	fps_array = mxGetField(infoArray, 0, "fps");
	if(fps_array){
		fps = mxGetScalar(fps_array);
	}

	mirror_array = mxGetField(infoArray, 0, "mirror");
	if(mirror_array){
		mirror = mxGetScalar(mirror_array);
	}

	info->f_mode.nXRes = width;
	info->f_mode.nYRes = height;
	info->f_mode.nFPS = fps;
	info->f_mirror = mirror;
}

XnStatus LoadFromStructure(const mxArray* structure, int sensorIndex)
{
	XnStatus rc;

	mxArray *image_node_array;
	mxArray *depth_node_array;
	mxArray *ir_node_array;
	mxArray *scene_node_array;
	mxArray *hands_node_array;
	mxArray *user_node_array;

	SGeneratorInfo imageInfo;
	SGeneratorInfo depthInfo;
	SGeneratorInfo irInfo;
	SGeneratorInfo sceneInfo;
	SGeneratorInfo handsInfo;
	SGeneratorInfo userInfo;
	
	if(sensorIndex == 0){
		rc = context->Init();

		if(!use_xml){
			rc = context->EnumerateProductionTrees(XN_NODE_TYPE_IMAGE,NULL,imageNodeInfoList, NULL);
			rc = context->EnumerateProductionTrees(XN_NODE_TYPE_DEPTH,NULL,depthNodeInfoList, NULL);
			rc = context->EnumerateProductionTrees(XN_NODE_TYPE_IR,NULL,irNodeInfoList, NULL);
			rc = context->EnumerateProductionTrees(XN_NODE_TYPE_SCENE,NULL,sceneNodeInfoList, NULL);
			rc = context->EnumerateProductionTrees(XN_NODE_TYPE_USER,NULL,userNodeInfoList, NULL);
			rc = context->EnumerateProductionTrees(XN_NODE_TYPE_HANDS,NULL,handsNodeInfoList, NULL);
		}
	}

	image_node_array = mxGetField(structure, 0, "image_node");
	if(image_node_array){
		LoadGeneratorInfo(image_node_array, &imageInfo);
		rc = InitProductionNode(&imageNodeInfoList, context,
									image, sensorIndex);
		rc = StartMapGenerator(image, imageInfo);
		has_image_node = true;
	}

	depth_node_array = mxGetField(structure, 0, "depth_node");
	if(depth_node_array){
		LoadGeneratorInfo(depth_node_array, &depthInfo);
		rc = InitProductionNode(&depthNodeInfoList, context,
									depth, sensorIndex);
		rc = StartMapGenerator(depth, depthInfo);
		has_depth_node = true;
	}

	ir_node_array = mxGetField(structure, 0, "ir_node");
	if(ir_node_array){
		LoadGeneratorInfo(ir_node_array, &irInfo);
		rc = InitProductionNode(&irNodeInfoList, context,
									ir, sensorIndex);
		rc = StartMapGenerator(ir, irInfo);
		has_ir_node = true;
	}

	scene_node_array = mxGetField(structure, 0, "scene_node");
	if(scene_node_array){
		LoadGeneratorInfo(scene_node_array, &sceneInfo);
		rc = InitProductionNode(&sceneNodeInfoList, context,
									scene, sensorIndex);
		rc = StartMapGenerator(scene, sceneInfo);
		has_scene_node = true;
	}

	hands_node_array = mxGetField(structure, 0, "hands_node");
	if(hands_node_array){
		LoadGeneratorInfo(hands_node_array, &handsInfo);
		rc = InitProductionNode(&handsNodeInfoList, context,
									hands, sensorIndex);
		has_hands_node = true;
	}

	user_node_array = mxGetField(structure, 0, "user_node");
	if(user_node_array){
		LoadGeneratorInfo(user_node_array, &userInfo);
		rc = InitProductionNode(&userNodeInfoList, context,
									user, sensorIndex);
		user->Create(*context);
		has_user_node = true;
	}

	return XN_STATUS_OK;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	XnStatus rc;

	mxArray *tmpContext, *tmpDepth, *tmpImage;
	mxArray *tmpDepthMD, *tmpImageMD;
	mxArray *tmpIR, *tmpIRMD;
	mxArray* tmpHands;
	mxArray* tmpUser;
	mxArray *tmpScene;

	mxArray *context_initialised_array;
	mxArray *has_image_node_array;
	mxArray *has_depth_node_array;
	mxArray *has_ir_node_array;
	mxArray *has_scene_node_array;
	mxArray* has_hands_node_array;
	mxArray* has_user_node_array;
	mxArray* sensor_index_array;
	mxArray* num_sensors_array;

	depth = new DepthGenerator;
	image = new ImageGenerator;
	ir = new IRGenerator;
	user = new UserGenerator;
	scene = new SceneAnalyzer;
	hands = new HandsGenerator;
	depthMD = new DepthMetaData;
	imageMD = new ImageMetaData;
	irMD = new IRMetaData;

	char *field_name[19];
	field_name[0] = "ni_context_obj";
	field_name[1] = "depth_obj";
	field_name[2] = "image_obj";
	field_name[3] = "ir_obj";
	field_name[4] = "user_obj";
	field_name[5] = "scene_obj";
	field_name[6] = "hands_obj";
	field_name[7] = "depthMD_obj";
	field_name[8] = "imageMD_obj";
	field_name[9] = "irMD_obj";
	field_name[10] = "context_initialised";
	field_name[11] = "has_depth_node";
	field_name[12] = "has_image_node";
	field_name[13] = "has_ir_node";
	field_name[14] = "has_scene_node";
	field_name[15] = "has_hands_node";
	field_name[16] = "has_user_node";
	field_name[17] = "sensor_index";
	field_name[18] = "num_sensors";

	char xml_filename[256];
	int sensorIndex = 0;
	
	if(mxIsChar(prhs[0])){
		mxGetString(prhs[0], xml_filename, 256);	//xml file name
		use_xml = true;
	} else{
		if(nrhs >= 2){
			sensorIndex = mxGetScalar(prhs[1]);
		}
	}

	if(sensorIndex == 0){
		context = new Context;
	}

	//---------------------------------
    // Create output variabels
    //---------------------------------
	plhs[0] = mxCreateStructMatrix(1, 1, 19, (const char**)field_name);
    // ni_context_obj
    tmpContext = mxCreateNumericMatrix(1, sizeof(Context*), mxUINT8_CLASS, mxREAL);
	memcpy(mxGetPr(tmpContext), (void*)&context, sizeof(Context*));
	mxSetField(plhs[0], 0, "ni_context_obj", tmpContext);

    // depth_obj
    tmpDepth = mxCreateNumericMatrix(1, sizeof(DepthGenerator*), mxUINT8_CLASS, mxREAL);
	memcpy(mxGetPr(tmpDepth), (void*)&depth, sizeof(DepthGenerator*));
	mxSetField(plhs[0], 0, "depth_obj", tmpDepth);

    // image_obj
    tmpImage = mxCreateNumericMatrix(1, sizeof(ImageGenerator*), mxUINT8_CLASS, mxREAL);
	memcpy(mxGetPr(tmpImage), (void*)&image, sizeof(ImageGenerator*));
	mxSetField(plhs[0], 0, "image_obj", tmpImage);

    // ir_obj
    tmpIR = mxCreateNumericMatrix(1, sizeof(IRGenerator*), mxUINT8_CLASS, mxREAL);
	memcpy(mxGetPr(tmpIR), (void*)&ir, sizeof(IRGenerator*));
	mxSetField(plhs[0], 0, "ir_obj", tmpIR);

    // user_obj
    tmpUser = mxCreateNumericMatrix(1, sizeof(UserGenerator*), mxUINT8_CLASS, mxREAL);
	memcpy(mxGetPr(tmpUser), (void*)&user, sizeof(UserGenerator*));
	mxSetField(plhs[0], 0, "user_obj", tmpUser);

    // scene_obj
    tmpScene = mxCreateNumericMatrix(1, sizeof(SceneAnalyzer*), mxUINT8_CLASS, mxREAL);
	memcpy(mxGetPr(tmpScene), (void*)&scene, sizeof(SceneAnalyzer*));
	mxSetField(plhs[0], 0, "scene_obj", tmpScene);

	//hands_obj
    tmpHands = mxCreateNumericMatrix(1, sizeof(HandsGenerator*), mxUINT8_CLASS, mxREAL);
	memcpy(mxGetPr(tmpHands), (void*)&hands, sizeof(HandsGenerator*));
	mxSetField(plhs[0], 0, "hands_obj", tmpHands);
	
	// depthMD_obj
    tmpDepthMD = mxCreateNumericMatrix(1, sizeof(DepthMetaData*), mxUINT8_CLASS, mxREAL);
	memcpy(mxGetPr(tmpDepthMD), (void*)&depthMD, sizeof(DepthMetaData*));
	mxSetField(plhs[0], 0, "depthMD_obj", tmpDepthMD);

	// imageMD_obj
    tmpImageMD = mxCreateNumericMatrix(1, sizeof(ImageMetaData*), mxUINT8_CLASS, mxREAL);
	memcpy(mxGetPr(tmpImageMD), (void*)&imageMD, sizeof(ImageMetaData*));
	mxSetField(plhs[0], 0, "imageMD_obj", tmpImageMD);

	// irMD_obj
    tmpIRMD = mxCreateNumericMatrix(1, sizeof(IRMetaData*), mxUINT8_CLASS, mxREAL);
	memcpy(mxGetPr(tmpIRMD), (void*)&irMD, sizeof(IRMetaData*));
	mxSetField(plhs[0], 0, "irMD_obj", tmpIRMD);

	context_initialised_array = mxCreateDoubleScalar(context_initialised);
	mxSetField(plhs[0], 0, "context_initialised", context_initialised_array);
	context_initialised = (int*)mxGetPr(context_initialised_array);

	has_image_node_array = mxCreateDoubleScalar(has_image_node);
	mxSetField(plhs[0], 0, "has_image_node", has_image_node_array);

	has_depth_node_array = mxCreateDoubleScalar(has_depth_node);
	mxSetField(plhs[0], 0, "has_depth_node", has_depth_node_array);

	has_ir_node_array = mxCreateDoubleScalar(has_ir_node);
	mxSetField(plhs[0], 0, "has_ir_node", has_ir_node_array);

	has_scene_node_array = mxCreateDoubleScalar(has_scene_node);
	mxSetField(plhs[0], 0, "has_scene_node", has_scene_node_array);

	has_hands_node_array = mxCreateDoubleScalar(has_hands_node);
	mxSetField(plhs[0], 0, "has_hands_node", has_hands_node_array);

	has_user_node_array = mxCreateDoubleScalar(has_user_node);
	mxSetField(plhs[0], 0, "has_user_node", has_user_node_array);

	sensor_index_array = mxCreateDoubleScalar(sensorIndex);
	mxSetField(plhs[0], 0, "sensor_index", sensor_index_array);

	num_sensors_array = mxCreateDoubleScalar(numSensors);
	mxSetField(plhs[0], 0, "num_sensors", num_sensors_array);

//////////////////////////////
	if(use_xml){
		rc = LoadFromXml(xml_filename);
	} else{
		rc = LoadFromStructure(prhs[0], sensorIndex);
	}	
	
	if(rc != XN_STATUS_OK){
		return;
	}

//////////////////////////////	

	context_initialised = true;

	context_initialised_array = mxCreateDoubleScalar(context_initialised);
	mxSetField(plhs[0], 0, "context_initialised", context_initialised_array);

	has_depth_node_array = mxCreateDoubleScalar(has_depth_node);
	mxSetField(plhs[0], 0, "has_depth_node", has_depth_node_array);

	has_image_node_array = mxCreateDoubleScalar(has_image_node);
	mxSetField(plhs[0], 0, "has_image_node", has_image_node_array);

	has_ir_node_array = mxCreateDoubleScalar(has_ir_node);
	mxSetField(plhs[0], 0, "has_ir_node", has_ir_node_array);

	has_scene_node_array = mxCreateDoubleScalar(has_scene_node);
	mxSetField(plhs[0], 0, "has_scene_node", has_scene_node_array);

	has_hands_node_array = mxCreateDoubleScalar(has_hands_node);
	mxSetField(plhs[0], 0, "has_hands_node", has_hands_node_array);

	has_user_node_array = mxCreateDoubleScalar(has_user_node);
	mxSetField(plhs[0], 0, "has_user_node", has_user_node_array);

	sensor_index_array = mxCreateDoubleScalar(sensorIndex);
	mxSetField(plhs[0], 0, "sensor_index", sensor_index_array);

	num_sensors_array = mxCreateDoubleScalar(numSensors);
	mxSetField(plhs[0], 0, "num_sensors", num_sensors_array);
}

