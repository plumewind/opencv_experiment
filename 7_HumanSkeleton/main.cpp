#include <iostream>
#include  "NIKinect.h"
#include <opencv2/opencv.hpp>

using namespace std;//C++命名空间
using namespace cv;
void clearImg(IplImage* inputimg)
{
	CvFont font;
	cvInitFont( &font, CV_FONT_VECTOR0,1, 1, 0, 3, 5);
	memset(inputimg->imageData,255,640*480*3);
}
int main(int argc, char **argv) 
{
	char key=0;
	IplImage* cameraImg=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);
	cvNamedWindow("Camera",1);
		
	// initial context
	NIKinect nikinect;
	nikinect.Initial(nikinect.Mode_U);
	nikinect.Start(nikinect.Mode_U);
	clearImg(cameraImg);//清空手势绘制屏幕
	
    while((key!=27)&&(nikinect.UpdateData(nikinect.Mode_U)))
	{
		memcpy(cameraImg->imageData,nikinect.imageMD.Data(),640*480*3);
		cvCvtColor(cameraImg,cameraImg,CV_RGB2BGR);//转换彩色图像数据
		// get users
		nikinect.updateTrackedUsers();
		XnUInt16 userCounts = nikinect.userGenerator.GetNumberOfUsers();//获得使用者数量
		if(nikinect.getNumTrackedUsers()> 0 )
		{
		    Skeleton skel = nikinect.getSkeleton(nikinect.getUID(0));
//		    SkeletonPoint hand;
		    cvCircle(cameraImg,cvPoint(skel.head.x,skel.head.y),3,CV_RGB(0,0,255),12);//画关节圆点1
		    cvCircle(cameraImg,cvPoint(skel.neck.x,skel.neck.y),3,CV_RGB(0,0,255),12);//画关节圆点2
		    cvCircle(cameraImg,cvPoint(skel.rightShoulder.x,skel.rightShoulder.y),3,CV_RGB(0,0,255),12);//画关节圆点3
		    cvCircle(cameraImg,cvPoint(skel.leftShoulder.x,skel.leftShoulder.y),3,CV_RGB(0,0,255),12);//画关节圆点4
		    cvCircle(cameraImg,cvPoint(skel.rightElbow.x,skel.rightElbow.y),3,CV_RGB(0,0,255),12);//画关节圆点5
		    cvCircle(cameraImg,cvPoint(skel.leftElbow.x,skel.leftElbow.y),3,CV_RGB(0,0,255),12);//画关节圆点6
		    cvCircle(cameraImg,cvPoint(skel.rightHand.x,skel.rightHand.y),3,CV_RGB(0,0,255),12);//画关节圆点7
		    cvCircle(cameraImg,cvPoint(skel.leftHand.x,skel.leftHand.y),3,CV_RGB(0,0,255),12);//画关节圆点8
		    
		    cvCircle(cameraImg,cvPoint(skel.rightHip.x,skel.rightHip.y),3,CV_RGB(0,0,255),12);//画关节圆点9
		    cvCircle(cameraImg,cvPoint(skel.leftHip.x,skel.leftHip.y),3,CV_RGB(0,0,255),12);//画关节圆点10
		    cvCircle(cameraImg,cvPoint(skel.rightKnee.x,skel.rightKnee.y),3,CV_RGB(0,0,255),12);//画关节圆点11
		    cvCircle(cameraImg,cvPoint(skel.leftKnee.x,skel.leftKnee.y),3,CV_RGB(0,0,255),12);//画关节圆点12
		    cvCircle(cameraImg,cvPoint(skel.rightFoot.x,skel.rightFoot.y),3,CV_RGB(0,0,255),12);//画关节圆点13
		    cvCircle(cameraImg,cvPoint(skel.leftFoot.x,skel.leftFoot.y),3,CV_RGB(0,0,255),12);//画关节圆点14
		    cvCircle(cameraImg,cvPoint(skel.torso.x,skel.torso.y),3,CV_RGB(0,0,255),12);//画关节圆点15

		}
		cvShowImage("Camera",cameraImg);//显示彩色图像
		key=cvWaitKey(20);
	}
	// stop and shutdown
	cvDestroyWindow("Camera");
	cvReleaseImage(&cameraImg);
	nikinect.Stop();
	
	
    return 0;
}
