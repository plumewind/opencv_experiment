#include <iostream>
#include <malloc.h>
#include  "NIKinect.h"
#include <opencv2/opencv.hpp>

using namespace std;//C++命名空间
using namespace cv;

int main(int argc, char **argv) 
{
    Mat image;
    string imageFileName;
    stringstream StrStm;
    int number=20;
//    IplImage* imgRGB8u=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);
    cvNamedWindow("Images",1);
    char key=0;

    // initial context
    NIKinect nikinect;
    nikinect.Initial(nikinect.Mode_I);
    nikinect.Start(nikinect.Mode_I);
    
    while((key!=27)&&(nikinect.UpdateData(nikinect.Mode_I)))//ESC键退出
    {		
	/*对色彩图像显示*/
	Mat color_image(nikinect.imageMD.YRes(), nikinect.imageMD.XRes(),CV_8UC3, (char *)nikinect.imageMD.Data());
	cvtColor(color_image,image,CV_RGB2BGR);
	imshow("Images",image);   
	
	 key=cvWaitKey(20);//不断刷新图像，频率时间为20ms，返回值为当前键盘按键值
	 if(key==112)//按下字母p键
	 {
	   StrStm.clear();
 	   imageFileName.clear();
	   StrStm<<++number;
	   StrStm>>imageFileName;
	   Mat newimage = image.clone();
	   imageFileName= "chess"+imageFileName+".bmp";
	   imwrite(imageFileName,newimage);
	   cout<<"保存第"<<number<<"张图片"<<endl;	
	 }
    }
  //destroy
    cvDestroyWindow("image");
 //   cvReleaseImage(&imgRGB8u);
    nikinect.Stop();

}
