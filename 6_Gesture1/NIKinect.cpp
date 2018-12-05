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
    
    if(nimode==Mode_I||nimode==Mode_I_D||nimode==Mode_I_D_G)
    { //彩色图像生成器初始化
	status=image_generator.Create(context);
	if(!CheckError(status,"Create image generator error!"))
	    return false;
    }
    if(nimode==Mode_D||nimode==Mode_I_D||nimode==Mode_I_D_G)
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
    return true;
}
bool NIKinect::Start(NIMode nimode)
{
    XnMapOutputMode mapMode; 
    mapMode.nXRes =Kinect_XRes;//设定生成器的参数，分辨率为640*480（标准），30fps采样 
    mapMode.nYRes = Kinect_YRes; 
    mapMode.nFPS = Kinect_FPs; 
    if(nimode==Mode_D||nimode==Mode_I_D||nimode==Mode_I_D_G)
	status = depth_generator.SetMapOutputMode( mapMode );//设置参数  
    if(nimode==Mode_I||nimode==Mode_I_D||nimode==Mode_I_D_G)
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
    if(nimode==Mode_I||nimode==Mode_I_D||nimode==Mode_I_D_G)
	image_generator.GetMetaData(imageMD);
    if(nimode==Mode_D||nimode==Mode_I_D||nimode==Mode_I_D_G)
	depth_generator.GetMetaData(depthMD);
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




