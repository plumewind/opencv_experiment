#include <stdlib.h>
#include <iostream>
#include <vector>

#include <XnCppWrapper.h>
#include <XnModuleCppInterface.h> 
#include "opencv/cv.h"
#include "opencv/highgui.h"

using namespace std;
using namespace cv;

//#pragma comment (lib,"cv210")
//#pragma comment (lib,"cxcore210")
//#pragma comment (lib,"highgui210")
//#pragma comment (lib,"OpenNI")

//【1】设置生成器
xn::UserGenerator userGenerator;
xn::DepthGenerator depthGenerator;
xn::ImageGenerator imageGenerator;

/*
  XN_SKEL_HEAD                     = 1,    XN_SKEL_NECK                           = 2,
  XN_SKEL_TORSO                   = 3,    XN_SKEL_WAIST                         = 4,
  XN_SKEL_LEFT_COLLAR        = 5,    XN_SKEL_LEFT_SHOULDER        = 6,
  XN_SKEL_LEFT_ELBOW         = 7,    XN_SKEL_LEFT_WRIST                = 8,
  XN_SKEL_LEFT_HAND           = 9,    XN_SKEL_LEFT_FINGERTIP         =10,
  XN_SKEL_RIGHT_COLLAR     =11,   XN_SKEL_RIGHT_SHOULDER      =12,
  XN_SKEL_RIGHT_ELBOW      =13,   XN_SKEL_RIGHT_WRIST              =14,
  XN_SKEL_RIGHT_HAND        =15,   XN_SKEL_RIGHT_FINGERTIP       =16,
  XN_SKEL_LEFT_HIP               =17,   XN_SKEL_LEFT_KNEE                   =18,
  XN_SKEL_LEFT_ANKLE          =19,   XN_SKEL_LEFT_FOOT                   =20,
  XN_SKEL_RIGHT_HIP            =21,   XN_SKEL_RIGHT_KNEE                =22,
  XN_SKEL_RIGHT_ANKLE       =23,   XN_SKEL_RIGHT_FOOT               =24    
*/
//a line will be drawn between start point and corresponding end point
int startSkelPoints[14]={1,2,6,6,12,17,6,7,12,13,17,18,21,22};
int endSkelPoints[14]={2,3,12,21,17,21,7,9,13,15,18,20,22,24};

// callback function of user generator: new user
void XN_CALLBACK_TYPE NewUser( xn::UserGenerator& generator, XnUserID user,void* pCookie )
{
	cout << "New user identified: " << user << endl;
	//userGenerator.GetSkeletonCap().LoadCalibrationDataFromFile( user, "UserCalibration.txt" );
	generator.GetPoseDetectionCap().StartPoseDetection("Psi", user);//开始进行PSI姿势校准
}

// callback function of user generator: lost user
void XN_CALLBACK_TYPE LostUser( xn::UserGenerator& generator, XnUserID user,void* pCookie )
{
	cout << "User " << user << " lost" << endl;
}

// callback function of skeleton: calibration start
void XN_CALLBACK_TYPE CalibrationStart( xn::SkeletonCapability& skeleton,XnUserID user,void* pCookie )
{
	cout << "Calibration start for user " <<  user << endl;//开始标定（校准）
}

// callback function of skeleton: calibration end 
void XN_CALLBACK_TYPE CalibrationEnd( xn::SkeletonCapability& skeleton,XnUserID user,XnCalibrationStatus calibrationError,void* pCookie )
{
	cout << "Calibration complete for user " <<  user << ", ";
	if( calibrationError==XN_CALIBRATION_STATUS_OK )
	{
		cout << "Success" << endl;
		skeleton.StartTracking( user );//开始跟踪
		//userGenerator.GetSkeletonCap().SaveCalibrationDataToFile(user, "UserCalibration.txt" );
	}
	else
	{
		cout << "Failure" << endl;
		//For the current version of OpenNI, only Psi pose is available开始以PSI姿势标定
		((xn::UserGenerator*)pCookie)->GetPoseDetectionCap().StartPoseDetection( "Psi", user );
	}
}

// callback function of pose detection: pose start
void XN_CALLBACK_TYPE PoseDetected( xn::PoseDetectionCapability& poseDetection,const XnChar* strPose,XnUserID user,void* pCookie)
{
	cout << "Pose " << strPose << " detected for user " <<  user << endl;
	((xn::UserGenerator*)pCookie)->GetSkeletonCap().RequestCalibration( user, FALSE );
	poseDetection.StopPoseDetection( user );
}

void clearImg(IplImage* inputimg)
{
	CvFont font;
	cvInitFont( &font, CV_FONT_VECTOR0,1, 1, 0, 3, 5);
	memset(inputimg->imageData,255,640*480*3);
}


int main( int argc, char** argv )
{
	char key=0;
	int imgPosX=0;
	int imgPosY=0;//图像坐标XY

	// initial context
	xn::Context context;
	context.Init();//kinect摄像头初始化
	xn::ImageMetaData imageMD;

	IplImage* cameraImg=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);
	cvNamedWindow("Camera",1);

	// map output mode
	XnMapOutputMode mapMode;
	mapMode.nXRes = 640;
	mapMode.nYRes = 480;
	mapMode.nFPS = 30;//设置图像输出格式

	// create generator
	depthGenerator.Create( context );
	depthGenerator.SetMapOutputMode( mapMode );
	imageGenerator.Create( context );
	userGenerator.Create( context );//创建生成器

        //【2】// Register callback functions of user generator
	XnCallbackHandle userCBHandle;//注册使用者生成器回调函数
	userGenerator.RegisterUserCallbacks( NewUser, LostUser, NULL, userCBHandle );

         //【3】// Register callback functions of skeleton capability
	xn::SkeletonCapability skeletonCap = userGenerator.GetSkeletonCap();
	skeletonCap.SetSkeletonProfile( XN_SKEL_PROFILE_ALL );//设置骨架检测文件（检测的骨架数量）
	XnCallbackHandle calibCBHandle;//注册骨架校准生成器回调函数
//	skeletonCap.RegisterCalibrationCallbacks( CalibrationStart, CalibrationEnd,&userGenerator, calibCBHandle );
	skeletonCap.RegisterToCalibrationStart( CalibrationStart,&userGenerator, calibCBHandle );//进行PSI姿势校准
	skeletonCap.RegisterToCalibrationComplete( CalibrationEnd,&userGenerator, calibCBHandle );

         //【4】// Register callback functions of Pose Detection capability
	XnCallbackHandle poseCBHandle;//注册姿态检测生成器回调函数
	userGenerator.GetPoseDetectionCap().RegisterToPoseDetected( PoseDetected,&userGenerator, poseCBHandle );


	// start generate data
	context.StartGeneratingAll();//打开图像收集开关
	while( key!=27 )
	{
		context.WaitAndUpdateAll();//设置图像更新方式
		imageGenerator.GetMetaData(imageMD);//得到摄像头彩色图像数据
		memcpy(cameraImg->imageData,imageMD.Data(),640*480*3);
		cvCvtColor(cameraImg,cameraImg,CV_RGB2BGR);//转换彩色图像数据
		// get users
		XnUInt16 userCounts = userGenerator.GetNumberOfUsers();//获得使用者数量
		if( userCounts > 0 )
		{
			XnUserID* userID = new XnUserID[userCounts];//获得使用者ID号
			userGenerator.GetUsers( userID, userCounts );//获得使用者信息数据
			for( int i = 0; i < userCounts; ++i )
			{
				//【5】// if is tracking skeleton
				if( skeletonCap.IsTracking( userID[i] ) )//是否跟踪成功
				{
					XnPoint3D skelPointsIn[24],skelPointsOut[24];//节点
					XnSkeletonJointTransformation mJointTran;//骨架节点转换
					for(int iter=0;iter<24;iter++)
					{
						//XnSkeletonJoint from 1 to 24			
                                                			skeletonCap.GetSkeletonJoint( userID[i],XnSkeletonJoint(iter+1), mJointTran );//获得关节节点
						skelPointsIn[iter]=mJointTran.position.position;//复制节点坐标
					}
					depthGenerator.ConvertRealWorldToProjective(24,skelPointsIn,skelPointsOut);//对节点坐标进行转换

					//【6】
					for(int d=0;d<14;d++)
					{
						CvPoint startpoint = cvPoint(skelPointsOut[startSkelPoints[d]-1].X,skelPointsOut[startSkelPoints[d]-1].Y);
						CvPoint endpoint = cvPoint(skelPointsOut[endSkelPoints[d]-1].X,skelPointsOut[endSkelPoints[d]-1].Y);
			
						cvCircle(cameraImg,startpoint,3,CV_RGB(0,0,255),12);//画关节圆点
						cvCircle(cameraImg,endpoint,3,CV_RGB(0,0,255),12);//画关节另一个原点
						cvLine(cameraImg,startpoint,endpoint,CV_RGB(0,0,255),4);//画关节直线
					}
				}
			}
			delete [] userID;
		}
		cvShowImage("Camera",cameraImg);//显示彩色图像

		key=cvWaitKey(20);


	}
	// stop and shutdown
	cvDestroyWindow("Camera");
	cvReleaseImage(&cameraImg);
	context.StopGeneratingAll();
	context.Shutdown();

	return 0;
}
