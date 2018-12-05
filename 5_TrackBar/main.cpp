#include <iostream>
#include <opencv2/opencv.hpp>
//#include <opencv2/highgui/highgui.hpp> 
#include "NIKinect.cpp"

using namespace cv;

const int bar1_max=100,bar2_max=100;
int bar1_slider,bar2_slider;
int bar1_value,bar2_value;
Rect g_rectangle;
bool g_DrawingBox=false;
RNG g_rng(12345);//随机数产生器
Mat g_images,g_imageM;

void on_MouseHandle(int event ,int x,int y,int flags ,void* param);
void DrawRectangle(cv::Mat& img,cv::Rect box);

int main(int argc, char **argv)
{
    bar1_slider=70;
    bar2_slider=60;
    g_rectangle=Rect(-1,-1,0,0);
    Mat image_gray,image_edge;
    cvNamedWindow("depth to canny",1);//定义CV图像显示窗口
    cvNamedWindow("images to canny",1);
    cvNamedWindow("images",1);
    
    createTrackbar("canny1","depth to canny",&bar1_slider,bar1_max);
    createTrackbar("canny2","images to canny",&bar2_slider,bar2_max);
//    setMouseCallback("images to canny",on_MouseHandle,NULL);
    setMouseCallback("images",on_MouseHandle,(void*)&g_images);
    char key=0;

    NIKinect nikinect;
    nikinect.Initial();
    nikinect.Start();
    
     while((key!=27)&&(nikinect.UpdateData()))
    {		
	/*对色彩图像进行canny边缘检测并显示*/
	 Mat color_image(nikinect.imageMD.YRes(), nikinect.imageMD.XRes(),CV_8UC3, (char *)nikinect.imageMD.Data());
	cvtColor(color_image,image_gray,CV_RGB2GRAY);
	blur(image_gray,image_gray,Size(7,7));
	bar2_value=getTrackbarPos("canny2","images to canny");
	Canny(image_gray,image_gray,5,bar2_value);
	imshow("images to canny",image_gray);   
	
	//鼠标选取ROI区域，并显示
	cvtColor(color_image,g_images,CV_RGB2BGR);
	if((g_rectangle.width>0)&&(g_rectangle.height>0))
	{
	     g_imageM=g_images(Rect(g_rectangle.x,g_rectangle.y,g_rectangle.width,g_rectangle.height));
	     namedWindow("Mouse select image ROI");
	     imshow("Mouse select image ROI", g_imageM);
	     DrawRectangle(g_images,g_rectangle);
	}
	imshow("images",g_images);//显示彩色图像
	
	///*计算深度图像的canny边缘并显示*/
	Mat depth_image(nikinect.depthMD.YRes(), nikinect.depthMD.XRes(),CV_16UC1, (char *)nikinect.depthMD.Data());
	//因为kinect获取到的深度图像实际上是无符号的16位数据
	depth_image.convertTo(image_edge, CV_8U, 255.0/8000);
	blur(image_edge,image_edge,Size(7,7));
	bar1_value=getTrackbarPos("canny1","depth to canny");
	Canny(image_edge,image_edge,5,bar1_value);
	imshow("depth to canny",image_edge);
	
	key=cvWaitKey(20);//不断刷新图像，频率时间为20ms，返回值为当前键盘按键值
	
    }
    //destroy
    cvDestroyWindow("depth");//关闭窗口
    cvDestroyWindow("image");
    nikinect.Stop();
    
    return 0;
}
void on_MouseHandle(int event, int x, int y, int flags, void* param)
{
    Mat& image=*(cv::Mat*) param;
    switch(event)
    {
	case EVENT_MOUSEMOVE:{
//	    printf("mouse light button move!\n");
	    if(g_DrawingBox)
	    {
		g_rectangle.width=x-g_rectangle.x;
		g_rectangle.height=y-g_rectangle.y;
		//防止超出原图像范围
//		g_rectangle &= Rect(0, 0, image.cols,image.rows);  
		if(g_rectangle.width>=image.cols) g_rectangle.width=image.cols;
		if(g_rectangle.height>=image.rows) g_rectangle.height=image.rows;
	    }
	    break;
	}
	case EVENT_LBUTTONDOWN:{
	   printf("mouse light button down!\n");
	   g_rectangle=Rect(x,y,0,0);
	   g_DrawingBox=true;
	   break;
	}
	case EVENT_LBUTTONUP:{
	    printf("mouse light button up!\n");
	    g_DrawingBox=false;
	    if(g_rectangle.width<0)
	    {
		g_rectangle.x+=g_rectangle.width;
		g_rectangle.width*=-1;
	    }
	     if(g_rectangle.height<0)
	    {
		g_rectangle.y+=g_rectangle.height;
		g_rectangle.height*=-1;
	    }
	    break;
	}
    }
}
void DrawRectangle(cv::Mat& img,cv::Rect box)
{
//    rectangle(img,box.br(),Scalar(g_rng.uniform(0,255),g_rng.uniform(0,255),g_rng.uniform(0,255)));
    cv::rectangle(img,box.tl(),box.br(),cv::Scalar(g_rng.uniform(0, 255), g_rng.uniform(0,255), g_rng.uniform(0,255)));//随机颜色
}
