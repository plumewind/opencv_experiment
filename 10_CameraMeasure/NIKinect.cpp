#include  "NIKinect.h"

bool NIKinect::CheckError(XnStatus result,string re_str)
{
    if(result!=XN_STATUS_OK)
    {
	cerr<<re_str<<"Error:"<<xnGetStatusString(result)<<endl;
	return false;
    }
    return true;
}
//NIKinect::NIKinect()//在类进行实例化时调用
//{  
//    startSkelPoints[14]={1,2,6,6,12,17,6,7,12,13,17,18,21,22};
//    endSkelPoints[14]={2,3,12,21,17,21,7,9,13,15,18,20,22,24};
//}
NIKinect::~NIKinect()//在销毁类对象时调用
{
    context.Release();//释放空间
}
bool NIKinect::Initial(NIMode nimode)
{
    //设备初始化
    status=context.Init();
    if(!CheckError(status,"Initial context failed !"))
	return false;
    //初始化kinect参数
//    Kinect_XRes=nxres;Kinect_YRes=nyres;Kinect_FPs=nfps;
    
    if(nimode==Mode_I||nimode==Mode_I_D||nimode==Mode_I_D_G||nimode==Mode_U)
    { //彩色图像生成器初始化
	status=image_generator.Create(context);
	if(!CheckError(status,"Create image generator error!"))
	    return false;
    }
    if(nimode==Mode_D||nimode==Mode_I_D||nimode==Mode_I_D_G||nimode==Mode_U)
    {//深度图像生成器初始化
	status=depth_generator.Create(context);
	if(!CheckError(status,"Create depth generator error!"))
	    return false;
    }
     if(nimode==Mode_I_D||nimode==Mode_I_D_G)
     {//调整视角
	status=depth_generator.GetAlternativeViewPointCap().SetViewPoint(image_generator);
	if(!CheckError(status,"Correcting error!"))
	    return false;
     }
     if(nimode==Mode_G||nimode==Mode_I_D_G)
     {//创建手势生成器
	status = gestureGenerator.Create( context );
	if(!CheckError(status,"Correcting error!"))
	    return false;
     }
     if(nimode==Mode_U)
     {//创建使用者生成器
	status = userGenerator.Create( context );//创建生成器
	if(!CheckError(status,"Correcting error!"))
	    return false;
     }
    return true;
}
bool NIKinect::Start(NIMode nimode)
{
    XnMapOutputMode mapMode; 
    mapMode.nXRes =Kinect_XRes;//设定生成器的参数，分辨率为640*480（标准），30fps采样 
    mapMode.nYRes = Kinect_YRes; 
    mapMode.nFPS = Kinect_FPs; 
    if(nimode==Mode_D||nimode==Mode_I_D||nimode==Mode_I_D_G||nimode==Mode_U)
	status = depth_generator.SetMapOutputMode( mapMode );//设置参数  
    if(nimode==Mode_I||nimode==Mode_I_D||nimode==Mode_I_D_G||nimode==Mode_U)
	status = image_generator.SetMapOutputMode( mapMode );  
    if(nimode==Mode_G||nimode==Mode_I_D_G)
     {//加入要识别的手势和回调函数
	gestureGenerator.AddGesture( "Wave", NULL );
	gestureGenerator.AddGesture( "Click", NULL );
	gestureGenerator.AddGesture( "RaiseHand", NULL );
	//gestureGenerator.AddGesture("MovingHand",NULL);

	//注册识别手势相关的回调函数
	gestureGenerator.RegisterGestureCallbacks( gestureRecog, gestureProgress, NULL, handle );
     }
     if(nimode==Mode_U)
     {//注册使用者生成器回调函数
	XnCallbackHandle userCBHandle;
//	userGenerator.RegisterUserCallbacks( NewUser, LostUser,this, userCBHandle );
	userGenerator.RegisterUserCallbacks(NewUser, LostUser,NULL, userCBHandle );
	
//	skeletonCap = userGenerator.GetSkeletonCap();
	userGenerator.GetSkeletonCap().SetSkeletonProfile( XN_SKEL_PROFILE_ALL );//设置骨架检测文件（检测的骨架数量）	
	userGenerator.GetSkeletonCap().RegisterToCalibrationStart( CalibrationStart,&userGenerator, calibCBHandle );//进行PSI姿势校准
	userGenerator.GetSkeletonCap().RegisterToCalibrationComplete( CalibrationEnd,&userGenerator, calibCBHandle );

         //注册姿态检测生成器回调函数
	XnCallbackHandle poseCBHandle;
	userGenerator.GetPoseDetectionCap().RegisterToPoseDetected( PoseDetected,&userGenerator, poseCBHandle );
     }
    status=context.StartGeneratingAll();
    if(!CheckError(status,"Start generating error !"))
	return false;
    return true;
}
bool NIKinect::UpdateData(NIMode nimode)
{
//    status=context.WaitNoneUpdateAll();
    status=context.WaitAndUpdateAll();
    if(!CheckError(status,"Update data error !"))
	return false;
    //获取数据
    if(nimode==Mode_I||nimode==Mode_I_D||nimode==Mode_I_D_G||nimode==Mode_U)
    {
	image_generator.GetMetaData(imageMD);
    }
    if(nimode==Mode_D||nimode==Mode_I_D||nimode==Mode_I_D_G||nimode==Mode_U)
    {
	depth_generator.GetMetaData(depthMD);
    }
//    if(nimode==Mode_G||nimode==Mode_I_D_G)
//	GestureFlag=false;
    return true;
}
bool NIKinect::Stop()
{
    context.StopGeneratingAll();//关闭数据收集开关
    context.Shutdown();//关闭所有驱动并且正确地清除所有
    
    return true;
}
// callback function for gesture recognized-XN_CALLBACK_TYPE
void  NIKinect::gestureRecog( GestureGenerator &generator,
								   const XnChar *strGesture,
								   const XnPoint3D *pIDPosition,
								   const XnPoint3D *pEndPosition,
								   void *pCookie )
{
//    cout << strGesture<<" from "<<*pIDPosition<<" to "<<*pEndPosition << endl;
    GestureStartX=(int)(Kinect_XRes/2-(pIDPosition->X));
    GestureStartY=(int)(Kinect_YRes/2-(pIDPosition->Y));
    GestureEndX=(int)(Kinect_XRes/2-(pEndPosition->X));
    GestureEndY=(int)(Kinect_YRes/2-(pEndPosition->Y));//设置点绘制坐标
    HandGesture=strGesture;//取得手势
    GestureFlag=true;
}

//    double start,stop,durationTime;
//	start = clock();
//	while(durationTime<=0.5)//等待0.5秒
//	{
//	    stop = clock();
//	    durationTime= ((double)(stop-start))/CLOCKS_PER_SEC;
//	}

// callback function for gesture progress-XN_CALLBACK_TYPE
void  NIKinect::gestureProgress( GestureGenerator &generator,
									  const XnChar *strGesture,
									  const XnPoint3D *pPosition,
									  XnFloat fProgress,
									  void *pCookie )
{
//	cout << strGesture << ":" << fProgress << " at " << *pPosition << endl;
}
// callback function of user generator: new user
void NIKinect::NewUser( UserGenerator& generator, XnUserID user,void* pCookie )
{
//	cout << "New user identified: " << user << endl;
	//userGenerator.GetSkeletonCap().LoadCalibrationDataFromFile( user, "UserCalibration.txt" );
	generator.GetPoseDetectionCap().StartPoseDetection("Psi", user);//开始进行PSI姿势校准
//    NIKinect*  user=(NIKinect*) pCookie;
//    user.generator.GetPoseDetectionCap().StartPoseDetection("Psi", user);//开始进行PSI姿势校准
}

// callback function of user generator: lost user
void NIKinect::LostUser( UserGenerator& generator, XnUserID user,void* pCookie )
{
//	cout << "User " << user << " lost" << endl;
}

// callback function of skeleton: calibration start
void  NIKinect::CalibrationStart(SkeletonCapability& skeleton,XnUserID user,void* pCookie )
{
//	cout << "Calibration start for user " <<  user << endl;//开始标定（校准）
}

// callback function of skeleton: calibration end 
void NIKinect::CalibrationEnd(SkeletonCapability& skeleton,XnUserID user,XnCalibrationStatus calibrationError,void* pCookie )
{
//	cout << "Calibration complete for user " <<  user << ", ";
//	NIKinect*  user=(NIKinect*) pCookie;
	if( calibrationError==XN_CALIBRATION_STATUS_OK )
	{
//		cout << "Success" << endl;
		 skeleton.StartTracking( user );//开始跟踪
//		userGenerator.GetSkeletonCap().SaveCalibrationDataToFile(user, "UserCalibration.txt" );
	}
	else
	{
//		cout << "Failure" << endl;
		//For the current version of OpenNI, only Psi pose is available开始以PSI姿势标定
		((xn::UserGenerator*)pCookie)->GetPoseDetectionCap().StartPoseDetection( "Psi", user );
	}
}

// callback function of pose detection: pose start
void NIKinect::PoseDetected( PoseDetectionCapability& poseDetection,const XnChar* strPose,XnUserID user,void* pCookie)
{
//	cout << "Pose " << strPose << " detected for user " <<  user << endl;
	((xn::UserGenerator*)pCookie)->GetSkeletonCap().RequestCalibration( user, FALSE );
	poseDetection.StopPoseDetection( user );
}
bool NIKinect::GetHumanSkeletonJoint(XnUserID user,XnPoint3D* SkelPoints)
{
    XnPoint3D skelPointsIn[24];//节点
     XnSkeletonJointTransformation mJointTran;//骨架节点转换
    if(userGenerator.GetSkeletonCap().IsTracking(user) )//是否跟踪成功
    {	   
	    for(int iter=0;iter<24;iter++)
	    {
		    //XnSkeletonJoint from 1 to 24			
		    userGenerator.GetSkeletonCap().GetSkeletonJoint( user,XnSkeletonJoint(iter+1), mJointTran );//获得关节节点
		    skelPointsIn[iter]=mJointTran.position.position;//复制节点坐标
	    }
	    depth_generator.ConvertRealWorldToProjective(24,skelPointsIn,SkelPoints);//对节点坐标进行转换
	     return true;
    }
    return false;
}
 void NIKinect::updateTrackedUsers()
 {
//     context.WaitOneUpdateAll(userGenerator);
     XnUserID users[64];
    XnUInt16 nUsers = userGenerator.GetNumberOfUsers();

    TrackedUsers.clear();

    userGenerator.GetUsers(users, nUsers);

    for(int i = 0; i < nUsers; i++)
    {
        if(userGenerator.GetSkeletonCap().IsTracking(users[i]))
        {
            TrackedUsers.push_back(users[i]);
        }
    }
}
unsigned int NIKinect::getNumTrackedUsers()
{
    return TrackedUsers.size();
}
unsigned int NIKinect::getUID(const unsigned int index)
{
    return TrackedUsers[index];
}
Skeleton NIKinect::getSkeleton(const unsigned int uid)
{
    Skeleton result;

    // not tracking user
    if(!userGenerator.GetSkeletonCap().IsTracking(uid))
        return result;

    // Array of available joints
    const unsigned int nJoints = 15;
    XnSkeletonJoint joints[nJoints] = 
    {   XN_SKEL_HEAD,
        XN_SKEL_NECK,
        XN_SKEL_RIGHT_SHOULDER,
        XN_SKEL_LEFT_SHOULDER,
        XN_SKEL_RIGHT_ELBOW,
        XN_SKEL_LEFT_ELBOW,
        XN_SKEL_RIGHT_HAND,
        XN_SKEL_LEFT_HAND,
        XN_SKEL_RIGHT_HIP,
        XN_SKEL_LEFT_HIP,
        XN_SKEL_RIGHT_KNEE,
        XN_SKEL_LEFT_KNEE,
        XN_SKEL_RIGHT_FOOT,
        XN_SKEL_LEFT_FOOT,
        XN_SKEL_TORSO 
    };

    // holds the joint position components
    XnSkeletonJointPosition positions[nJoints];

    for (unsigned int i = 0; i < nJoints; i++)
    {
        userGenerator.GetSkeletonCap().GetSkeletonJointPosition(uid, joints[i], *(positions+i));
    }

    SkeletonPoint points[15];
    convertXnJointsToPoints(positions, points, nJoints);

    result.head              = points[0];
    result.neck              = points[1];
    result.rightShoulder     = points[2];
    result.leftShoulder      = points[3];
    result.rightElbow        = points[4];
    result.leftElbow         = points[5];
    result.rightHand         = points[6];
    result.leftHand          = points[7];
    result.rightHip          = points[8];
    result.leftHip           = points[9];
    result.rightKnee         = points[10];
    result.leftKnee          = points[11];
    result.rightFoot         = points[12];
    result.leftFoot          = points[13];
    result.torso             = points[14];

    return result;
}
void NIKinect::convertXnJointsToPoints(XnSkeletonJointPosition* const joints, SkeletonPoint* const points, unsigned int numPoints)
{
     XnPoint3D xpt;

    for(unsigned int i = 0; i < numPoints; i++)
    {
        xpt = joints[i].position;

//        if(pointModeProjective_)
            depth_generator.ConvertRealWorldToProjective(1, &xpt, &xpt);

        points[i].confidence = joints[i].fConfidence;
        points[i].x = xpt.X;
        points[i].y = xpt.Y;
        points[i].z = xpt.Z;
    }
}
