#ifndef NIKINECT_H
#define NIKINECT_H

#include<XnCppWrapper.h>
#include<iostream>
#include <unistd.h>
#include <stdlib.h>
#include <vector>

using namespace xn;//openni命名空间
using namespace std;//C++命名空间

class NIKinect 
{
public:
    ~NIKinect();
    
    enum NIMode{Mode_I,Mode_D,Mode_I_D,Mode_G,Mode_I_D_G};//状态选择器
    DepthMetaData depthMD;//共享的深度图像数据
    ImageMetaData imageMD;//共享的彩色图像数据
    
    static int Kinect_XRes;
    static int Kinect_YRes;//kinect分辨率-一般为640*480
    static int Kinect_FPs;//kinect图像帧率-一般为30
    static bool GestureFlag;//手势跟新标志位，true表示已更新
    static int GestureStartX;
    static int GestureStartY;
    static int GestureEndX;
    static int GestureEndY;
    static string HandGesture;
    
    bool Initial(NIMode nimode);//kinect物件初始化
    bool Start(NIMode nimode);
    bool UpdateData(NIMode nimode);
    bool Stop();
    bool SetGestureResult();
    
private:
    XnStatus status;//状态标志
    Context context;//kinect物件
    DepthGenerator depth_generator;//深度数据生成器
    ImageGenerator image_generator;//彩色图像生成器
    GestureGenerator gestureGenerator;//手势生成器
    
    XnCallbackHandle handle;
    
    //检测openni运行状态
    bool CheckError(XnStatus result,string re_str);
    
    // callback function for gesture recognized-XN_CALLBACK_TYPE
    static void  gestureRecog( GestureGenerator &generator,
								   const XnChar *strGesture,
								   const XnPoint3D *pIDPosition,
								   const XnPoint3D *pEndPosition,
								   void *pCookie );
    // callback function for gesture progress-XN_CALLBACK_TYPE
     static void gestureProgress( GestureGenerator &generator,
									  const XnChar *strGesture,
									  const XnPoint3D *pPosition,
									  XnFloat fProgress,
									  void *pCookie );
};
//静态变量初始化
int NIKinect::Kinect_XRes=640;
int NIKinect::Kinect_YRes=480;//kinect分辨率-一般为640*480
int NIKinect::Kinect_FPs=30;//kinect图像帧率-一般为30
bool NIKinect::GestureFlag=false;//手势跟新标志位，true表示已更新
int NIKinect:: GestureStartX=0;
int NIKinect::GestureStartY=0;
int NIKinect::GestureEndX=0;
int NIKinect::GestureEndY=0;
string NIKinect::HandGesture=" ";

#endif