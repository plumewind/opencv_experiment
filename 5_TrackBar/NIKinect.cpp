#include<XnCppWrapper.h>
#include<iostream>
#include <boost/concept_check.hpp>

using namespace xn;//openni命名空间
using namespace std;//C++命名空间

class NIKinect 
{
private:
    XnStatus status;//状态标志
    Context context;//kinect物件
    DepthGenerator depth_generator;//深度数据生成器
    ImageGenerator image_generator;//彩色图像生成器
    
    //检测openni运行状态
    bool CheckError(XnStatus result,string re_str)
    {
	if(result!=XN_STATUS_OK)
	{
	    cerr<<re_str<<"Error:"<<xnGetStatusString(result)<<endl;
	    return false;
	}
	return true;
    }
    
public:
    DepthMetaData depthMD;
    ImageMetaData imageMD;
    ~NIKinect()
    {
	context.Release();//释放空间
    }
    bool Initial()
    {
	//设备初始化
	status=context.Init();
	if(!CheckError(status,"Initial context failed !"))
	    return false;
	
	//彩色图像生成器初始化
	status=image_generator.Create(context);
	if(!CheckError(status,"Create image generator error!"))
	    return false;
	
	//深度图像生成器初始化
	status=depth_generator.Create(context);
	if(!CheckError(status,"Create depth generator error!"))
	    return false;
	
	//调整视角
	status=depth_generator.GetAlternativeViewPointCap().SetViewPoint(image_generator);
	if(!CheckError(status,"Correcting error!"))
	    return false;
	return true;
    }
    bool Start()
    {
	XnMapOutputMode mapMode; 
	mapMode.nXRes = 640;//设定生成器的参数，分辨率为640*480（标准），30fps采样 
	mapMode.nYRes = 480; 
	mapMode.nFPS = 30; 
	status = depth_generator.SetMapOutputMode( mapMode );//设置参数  
	status = image_generator.SetMapOutputMode( mapMode );  
	status=context.StartGeneratingAll();
	if(!CheckError(status,"Start generating error !"))
	    return false;
	return true;
    }
    bool UpdateData()
    {
	status=context.WaitNoneUpdateAll();
	if(!CheckError(status,"Update data error !"))
	    return false;
	//获取数据
	image_generator.GetMetaData(imageMD);
	depth_generator.GetMetaData(depthMD);
	
	return true;
    }
    bool Stop()
    {
	context.StopGeneratingAll();//关闭数据收集开关
	context.Shutdown();//关闭所有驱动并且正确地清除所有
	
	return true;
    }
};
