#include <stdlib.h>
#include <iostream>
#include <string>//C++语言标准头文件
//【1】
#include <XnCppWrapper.h>//openni头文件
#include "opencv/cv.h"//opencv头文件
#include "opencv/highgui.h"

using namespace std;
using namespace cv;

void CheckOpenNIError( XnStatus result, string status )
{ 
	if( result != XN_STATUS_OK ) 
		cerr << status << " Error: " << xnGetStatusString( result ) << endl;
}

int main( int argc, char** argv )
{
	XnStatus result = XN_STATUS_OK;  
	xn::DepthMetaData depthMD;//kinect输出的深度图像数据
	xn::ImageMetaData imageMD;//kinect输出的彩色图像数据

	//【1】OpenCV定义存储相关图像数据的参数
	IplImage*  imgDepth16u=cvCreateImage(cvSize(640,480),IPL_DEPTH_16U,1);
	IplImage* imgRGB8u=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);
	IplImage*  depthShow=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,1);
	IplImage* imageShow=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);
	cvNamedWindow("depth",1);//定义CV图像显示窗口
	cvNamedWindow("image",1);
	char key=0;

	//【2】context 
	xn::Context context; 
	result = context.Init(); //初始化所有传感器驱动
	CheckOpenNIError( result, "initialize context" );  

	// creategenerator  
	xn::DepthGenerator depthGenerator;//深度图像生成器  
	result = depthGenerator.Create( context ); //创建深度图像生成器
	CheckOpenNIError( result, "Create depth generator" );  
	xn::ImageGenerator imageGenerator;//彩色图像生成器
	result = imageGenerator.Create( context ); //创建
	CheckOpenNIError( result, "Create image generator" );

	//【3】map mode  
	XnMapOutputMode mapMode; 
	mapMode.nXRes = 640;//设定生成器的参数，分辨率为640*480（标准），30fps采样 
	mapMode.nYRes = 480; 
	mapMode.nFPS = 30; 
	result = depthGenerator.SetMapOutputMode( mapMode );//设置参数  
	result = imageGenerator.SetMapOutputMode( mapMode );  

	//【4】 correct view port  调整视角，把深度生成器的视角设定为彩色生成器的视角
	depthGenerator.GetAlternativeViewPointCap().SetViewPoint( imageGenerator ); 

	//【5】read data
	result = context.StartGeneratingAll();//打开开关，生成器开始工作
	//【6】设置更新 context 下所有的 node 的数据的条件：强制更新（不管有没有新数据）
	result = context.WaitNoneUpdateAll();

	while( (key!=27) && !(result = context.WaitNoneUpdateAll( ))  ) //Ese键按下即退出循环
	{  
		//get meta data获取kinect图像数据
		depthGenerator.GetMetaData(depthMD); 
		imageGenerator.GetMetaData(imageMD);

		//【7】OpenCV output
		memcpy(imgDepth16u->imageData,depthMD.Data(),640*480*2);//数据拷贝
		//将深度数据复制到imgDepth16U.imageData,图像大小为640*480,2通道（2个字节）  
		cvConvertScale(imgDepth16u,depthShow,255/4096.0,0);
		//使用线形变换转换数据，255/4096比例压缩，转换到0-255范围 

		memcpy(imgRGB8u->imageData,imageMD.Data(),640*480*3);
		cvCvtColor(imgRGB8u,imageShow,CV_RGB2BGR);
		//色彩空间转换，将RGB存储方式转换为BGR 
		cvShowImage("depth", depthShow);
		cvShowImage("image",imageShow);
		key=cvWaitKey(20);//不断刷新图像，频率时间为20ms，返回值为当前键盘按键值
	}

	//destroy
	cvDestroyWindow("depth");//关闭窗口
	cvDestroyWindow("image");
	cvReleaseImage(&imgDepth16u);//释放内存空间
	cvReleaseImage(&imgRGB8u);
	cvReleaseImage(&depthShow);
	cvReleaseImage(&imageShow);
	context.StopGeneratingAll();//关闭数据收集开关
	context.Shutdown();//关闭所有驱动并且正确地清除所有
	return 0;
}
