#include <stdlib.h>
#include <iostream>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <XnCppWrapper.h>

using namespace std;
using namespace cv;

// output for XnPoint3D
ostream& operator<<( ostream& out, const XnPoint3D& rPoint )
{
	out << "(" << rPoint.X << "," << rPoint.Y << "," << rPoint.Z << ")";
	return out;
}

//【4】
// callback function for gesture recognized
void XN_CALLBACK_TYPE gestureRecog( xn::GestureGenerator &generator,
								   const XnChar *strGesture,
								   const XnPoint3D *pIDPosition,
								   const XnPoint3D *pEndPosition,
								   void *pCookie )
{
	cout << strGesture<<" from "<<*pIDPosition<<" to "<<*pEndPosition << endl;

	int imgStartX=0;
	int imgStartY=0;
	int imgEndX=0;
	int imgEndY=0;
	char locationinfo[100];

	imgStartX=(int)(640/2-(pIDPosition->X));
	imgStartY=(int)(480/2-(pIDPosition->Y));
	imgEndX=(int)(640/2-(pEndPosition->X));
	imgEndY=(int)(480/2-(pEndPosition->Y));//设置点绘制坐标

	IplImage* refimage=(IplImage*)pCookie;//获取画布首地址
	if(strcmp(strGesture,"RaiseHand")==0)
	{//发现举起手势就绘制小红点
		cvCircle(refimage,cvPoint(imgStartX,imgStartY),1,CV_RGB(255,0,0),2);
	}
	else if(strcmp(strGesture,"Wave")==0)
	{//发现挥动手势就绘制小黄点
		cvLine(refimage,cvPoint(imgStartX,imgStartY),cvPoint(imgEndX,imgEndY),CV_RGB(255,255,0),6);
	}
	else if(strcmp(strGesture,"Click")==0)
	{//发现前推手势就绘制大蓝点
		cvCircle(refimage,cvPoint(imgStartX,imgStartY),6,CV_RGB(0,0,255),12);
	}

	cvSetImageROI(refimage,cvRect(40,450,640,30));//设置图像的感兴趣区域（矩形），即设置图像操作区域
	CvFont font;
	cvInitFont( &font, CV_FONT_VECTOR0,1, 1, 0, 3, 5);//字体结构初始化
	cvSet(refimage, cvScalar(255,255,255));//设置画布为白色
	sprintf(locationinfo,"From: %d,%d to %d,%d",(int)pIDPosition->X,(int)pIDPosition->Y,(int)(pEndPosition->X),(int)(pEndPosition->Y));
	cvPutText(refimage, locationinfo ,cvPoint(30, 30), &font, CV_RGB(0,0,0));//在画布上写字符串
	cvResetImageROI(refimage);//取消感兴趣区域的设置
}

void clearImg(IplImage* inputimg)
{
	CvFont font;
	cvInitFont( &font, CV_FONT_VECTOR0,1, 1, 0, 3, 5);//初始化字体结构体
	memset(inputimg->imageData,255,640*480*3);//图像RGB值初始化
	cvPutText(inputimg, "Hand Raise!" ,cvPoint(20, 20), &font, CV_RGB(255,0,0));//红色
	cvPutText(inputimg, "Hand Wave!" , cvPoint(20, 50), &font, CV_RGB(255,255,0));//黄色
	cvPutText(inputimg, "Hand Push!" , cvPoint(20, 80), &font, CV_RGB(0,0,255));//蓝色
}

//【5】
// callback function for gesture progress
void XN_CALLBACK_TYPE gestureProgress( xn::GestureGenerator &generator,
									  const XnChar *strGesture,
									  const XnPoint3D *pPosition,
									  XnFloat fProgress,
									  void *pCookie )
{
	cout << strGesture << ":" << fProgress << " at " << *pPosition << endl;
}


int main( int argc, char** argv )
{
	IplImage* drawPadImg=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);
	IplImage* cameraImg=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);

	cvNamedWindow("Gesture",1);
	cvNamedWindow("Camera",1);

	clearImg(drawPadImg);//清空手势绘制屏幕

	XnStatus res;
	char key=0;

	// context
	xn::Context context;
	res = context.Init();//摄像头初始化
	xn::ImageMetaData imgMD

	// create generator 
	xn::ImageGenerator imageGenerator;
	res = imageGenerator.Create( context ); //创建图像生成器
	//【1】
	xn::GestureGenerator gestureGenerator;
	res = gestureGenerator.Create( context );//创建手势生成器

        //【2】
	// Add gesture
	//gestureGenerator.AddGesture( "MovingHand", NULL );
	gestureGenerator.AddGesture( "Wave", NULL );
	gestureGenerator.AddGesture( "Click", NULL );
	gestureGenerator.AddGesture( "RaiseHand", NULL );//加入要识别的手势
	//gestureGenerator.AddGesture("MovingHand",NULL);

        //【3】
	// 6. Register callback functions of gesture generator
	XnCallbackHandle handle;
	//注册识别手势相关的回调函数
	gestureGenerator.RegisterGestureCallbacks( gestureRecog, gestureProgress, (void*)drawPadImg, handle );

	//start generate data
	context.StartGeneratingAll();//开始收集图像数据
	res = context.WaitAndUpdateAll();  

	while( (key!=27) && !(res = context.WaitAndUpdateAll())  ) 
	{  
		if(key=='c')
		{//c键按下就清除画布
			clearImg(drawPadImg);
		}

		imageGenerator.GetMetaData(imgMD);//获取图像数据
		memcpy(cameraImg->imageData,imgMD.Data(),640*480*3);//拷贝图像数据
		cvCvtColor(cameraImg,cameraImg,CV_RGB2BGR);//图像数据格式转换

		cvShowImage("Gesture",drawPadImg);//显示画布
		cvShowImage("Camera",cameraImg);//显示彩色图像

		key=cvWaitKey(20);//等待按键按下

	}
	cvDestroyWindow("Gesture");
	cvDestroyWindow("Camera");
	cvReleaseImage(&drawPadImg);
	cvReleaseImage(&cameraImg);
	context.StopGeneratingAll();
	context.Shutdown();

	return 0;
}