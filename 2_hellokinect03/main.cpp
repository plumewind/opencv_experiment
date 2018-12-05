#include <iostream>
#include <opencv2/opencv.hpp>
#include "NIKinect.cpp"

using namespace cv;

int main(int argc, char **argv) 
{
    //【1】OpenCV定义存储相关图像数据的参数
    Mat image_gray,image_edge;
    IplImage*  imgDepth16u=cvCreateImage(cvSize(640,480),IPL_DEPTH_16U,1);
    IplImage* imgRGB8u=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);
    IplImage*  depthShow=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,1);
    IplImage* imageShow=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);
    cvNamedWindow("depth",1);//定义CV图像显示窗口
    cvNamedWindow("image",1);
    char key=0;

    NIKinect nikinect;
    nikinect.Initial();
    nikinect.Start();

    while((key!=27)&&(nikinect.UpdateData()))
    {
    /*获取并显示深度图像*/
    memcpy(imgDepth16u->imageData,nikinect.depthMD.Data(),640*480*2);//数据拷贝
    //将深度数据复制到imgDepth16U.imageData,图像大小为640*480,2通道（2个字节）  
    cvConvertScale(imgDepth16u,depthShow,255/4096.0,0);
    //使用线形变换转换数据，255/4096比例压缩，转换到0-255范围 
    cvShowImage("depth", depthShow);

    /*计算深度图像的canny边缘并显示*/
    Mat depth_image(nikinect.depthMD.YRes(), nikinect.depthMD.XRes(),
                            CV_16UC1, (char *)nikinect.depthMD.Data());//因为kinect获取到的深度图像实际上是无符号的16位数据
    depth_image.convertTo(image_edge, CV_8U, 255.0/8000);
    blur(image_edge,image_edge,Size(7,7));
    Canny(image_edge,image_edge,5,100);
    imshow("depth to canny",image_edge);

    /*获取并显示色彩图像*/
    memcpy(imgRGB8u->imageData,nikinect.imageMD.Data(),640*480*3);
    cvCvtColor(imgRGB8u,imageShow,CV_RGB2BGR);
    //色彩空间转换，将RGB存储方式转换为BGR 
    cvShowImage("image",imageShow);

    /*对色彩图像进行canny边缘检测并显示*/
    Mat color_image(nikinect.imageMD.YRes(), nikinect.imageMD.XRes(),
                            CV_8UC3, (char *)nikinect.imageMD.Data());
    cvtColor(color_image,image_gray,CV_RGB2GRAY);
    blur(image_gray,image_gray,Size(7,7));
    Canny(image_gray,image_gray,5,100);
    imshow("images to canny",image_gray);

    key=cvWaitKey(20);//不断刷新图像，频率时间为20ms，返回值为当前键盘按键值

    }
    //destroy
    cvDestroyWindow("depth");//关闭窗口
    cvDestroyWindow("image");
    cvReleaseImage(&imgDepth16u);//释放内存空间
    cvReleaseImage(&imgRGB8u);
    cvReleaseImage(&depthShow);
    cvReleaseImage(&imageShow);
    nikinect.Stop();
}