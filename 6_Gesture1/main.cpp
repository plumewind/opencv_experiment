#include <iostream>
#include  "NIKinect.h"
#include <opencv2/opencv.hpp>

using namespace std;//C++命名空间
using namespace cv;

void clearImg(IplImage* inputimg)
{
	CvFont font;
	cvInitFont( &font, CV_FONT_VECTOR0,1, 1, 0, 3, 5);//初始化字体结构体
	memset(inputimg->imageData,255,640*480*3);//图像RGB值初始化
	cvPutText(inputimg, "Hand Raise!" ,cvPoint(20, 20), &font, CV_RGB(255,0,0));//红色
	cvPutText(inputimg, "Hand Wave!" , cvPoint(20, 50), &font, CV_RGB(255,255,0));//黄色
	cvPutText(inputimg, "Hand Push!" , cvPoint(20, 80), &font, CV_RGB(0,0,255));//蓝色
}
int main(int argc, char **argv) 
{
    Mat image,depth;
    IplImage* drawPadImg=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);
    cvNamedWindow("Depth",1);//定义CV图像显示窗口
    cvNamedWindow("Images",1);
    cvNamedWindow("Gestures",1);
    char key=0;
    char locationinfo[100];
    
    NIKinect nikinect
//    cout << "nikinect.Initial-->>" <<endl<<nikinect.Initial(nikinect.Mode_I_D_G)<< endl;
    nikinect.Initial(nikinect.Mode_I_D_G);
//    cout << " nikinect.Start-->>" <<endl<<nikinect.Start(nikinect.Mode_I_D_G)<< endl;
    nikinect.Start(nikinect.Mode_I_D_G);
    clearImg(drawPadImg);//清空手势绘制屏幕
    
    while((key!=27)&&(nikinect.UpdateData(nikinect.Mode_I_D_G)))
    {		
	/*对色彩图像进行canny边缘检测并显示*/
	Mat color_image(nikinect.imageMD.YRes(), nikinect.imageMD.XRes(),CV_8UC3, (char *)nikinect.imageMD.Data());
	cvtColor(color_image,image,CV_RGB2BGR);
	imshow("Images",image);   
	
	/*计算深度图像的canny边缘并显示*/
	Mat depth_image(nikinect.depthMD.YRes(), nikinect.depthMD.XRes(),CV_16UC1, (char *)nikinect.depthMD.Data());
	depth_image.convertTo(depth, CV_8U, 255.0/8000);
	imshow("Depth",depth);
	
	//c键按下就清除画布
	if(key=='c') clearImg(drawPadImg);
	if(nikinect.GestureFlag)
	{
	    cout << "GestureResult：" <<endl<<nikinect.HandGesture<<endl;
	    if(nikinect.HandGesture=="RaiseHand")//发现举起手势就绘制小红点
		cvCircle(drawPadImg,cvPoint(nikinect.GestureStartX,nikinect.GestureStartY),1,CV_RGB(255,0,0),2);
	    if(nikinect.HandGesture=="Wave") //发现挥动手势就绘制小黄点
		cvLine(drawPadImg,cvPoint(nikinect.GestureStartX,nikinect.GestureStartY),cvPoint(nikinect.GestureEndX,nikinect.GestureEndY),CV_RGB(255,255,0),6);
	    if(nikinect.HandGesture=="Click")//发现前推手势就绘制大蓝点
		cvCircle(drawPadImg,cvPoint(nikinect.GestureStartX,nikinect.GestureStartY),6,CV_RGB(0,0,255),12);
	    
	    cvSetImageROI(drawPadImg,cvRect(40,450,640,30));//设置图像的感兴趣区域（矩形），即设置图像操作区域
	    CvFont font;
	    cvInitFont( &font, CV_FONT_VECTOR0,1, 1, 0, 3, 5);//字体结构初始化
	    cvSet(drawPadImg, cvScalar(255,255,255));//设置画布为白色
	    sprintf(locationinfo,"From: %d,%d to %d,%d",nikinect.GestureStartX,nikinect.GestureStartY,nikinect.GestureEndX,nikinect.GestureEndY);
	    cvPutText(drawPadImg, locationinfo ,cvPoint(30, 30), &font, CV_RGB(0,0,0));//在画布上写字符串
	    cvResetImageROI(drawPadImg);//取消感兴趣区域的设置
	    nikinect.GestureFlag=false;
	}else cout << "SetGestureResult Error!" << endl;
	cvShowImage("Gestures",drawPadImg);//显示画布
	key=cvWaitKey(5);//不断刷新图像，频率时间为20ms，返回值为当前键盘按键值
	
    }
    //destroy
    cvDestroyWindow("Depth");//关闭窗口
    cvDestroyWindow("Image");
    cvDestroyWindow("Gestures");
    nikinect.Stop();
    
    return 0;
}
