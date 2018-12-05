#include <iostream>
#include <malloc.h>
#include  "NIKinect.h"
#include <opencv2/opencv.hpp>

using namespace std;//C++命名空间
using namespace cv;
// colors
const Scalar COLOR_BLUE        = Scalar(240,40,0);
const Scalar COLOR_DARK_GREEN  = Scalar(0, 128, 0);
const Scalar COLOR_LIGHT_GREEN = Scalar(0,255,0);
const Scalar COLOR_YELLOW      = Scalar(0,128,200);
const Scalar COLOR_RED         = Scalar(0,0,255);
// defines the value about witch the region of interest is extracted
const unsigned int ROI_OFFSET = 70;
const float DEPTH_SCALE_FACTOR = 255./4096.;
// defines the value about which thresholding occurs
const unsigned int BIN_THRESH_OFFSET = 5;
// median blur factor
const unsigned int MEDIAN_BLUR_K = 5;
// grasping threshold
const double GRASPING_THRESH = 0.9;
// returns true if the hand is near the sensor area
bool handApproachingDisplayPerimeter(float x, float y)
{
    return (x > (640 - ROI_OFFSET)) || (x < (ROI_OFFSET)) ||
           (y > (480 - ROI_OFFSET)) || (y < (ROI_OFFSET));
}
// conversion from cvConvexityDefect
struct ConvexityDefect
{
    Point start;
    Point end;
    Point depth_point;
    float depth;
};
// Thanks to Jose Manuel Cabrera for part of this C++ wrapper function
void findConvexityDefects(vector<Point>& contour, vector<int>& hull, vector<ConvexityDefect>& convexDefects)
{
    if (hull.size() > 0 && contour.size() > 0)
    {
        CvSeq* contourPoints;
        CvSeq* defects;
        CvMemStorage* storage;
        CvMemStorage* strDefects;
        CvMemStorage* contourStr;
        CvConvexityDefect *defectArray = 0;

        strDefects = cvCreateMemStorage();
        defects = cvCreateSeq( CV_SEQ_KIND_GENERIC|CV_32SC2, sizeof(CvSeq),sizeof(CvPoint), strDefects );

        //We transform our vector<Point> into a CvSeq* object of CvPoint.
        contourStr = cvCreateMemStorage();
        contourPoints = cvCreateSeq(CV_SEQ_KIND_GENERIC|CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), contourStr);
        for(int i = 0; i < (int)contour.size(); i++) 
	{
            CvPoint cp;
	    cp = {contour[i].x,  contour[i].y};
            cvSeqPush(contourPoints, &cp);
        }

        //Now, we do the same thing with the hull index
        int count = (int) hull.size();
        //int hullK[count];
        int* hullK = (int*) malloc(count*sizeof(int));
        for(int i = 0; i < count; i++) { hullK[i] = hull.at(i); }
        CvMat hullMat = cvMat(1, count, CV_32SC1, hullK);

        // calculate convexity defects
        storage = cvCreateMemStorage(0);
        defects = cvConvexityDefects(contourPoints, &hullMat, storage);
        defectArray = (CvConvexityDefect*)malloc(sizeof(CvConvexityDefect)*defects->total);
        cvCvtSeqToArray(defects, defectArray, CV_WHOLE_SEQ);
        //printf("DefectArray %i %i\n",defectArray->end->x, defectArray->end->y);

        //We store defects points in the convexDefects parameter.
        for(int i = 0; i<defects->total; i++){
            ConvexityDefect def;
            def.start       = Point(defectArray[i].start->x, defectArray[i].start->y);
            def.end         = Point(defectArray[i].end->x, defectArray[i].end->y);
            def.depth_point = Point(defectArray[i].depth_point->x, defectArray[i].depth_point->y);
            def.depth       = defectArray[i].depth;
            convexDefects.push_back(def);
        }

    // release memory
    cvReleaseMemStorage(&contourStr);
    cvReleaseMemStorage(&strDefects);
    cvReleaseMemStorage(&storage);

    }
}
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
		
	// initial context
	NIKinect nikinect;
	
	Mat depthRaw(nikinect.Kinect_YRes,nikinect.Kinect_XRes, CV_16UC1);
	Mat depthShow(nikinect.Kinect_YRes,nikinect.Kinect_XRes, CV_8UC1);
	Mat handDebug;
	    // this vector holds the displayed images of the hands
	vector<Mat> debugFrames;
	// rectangle used to extract hand regions from depth map
	Rect roi;
	roi.width  = ROI_OFFSET*2;
	roi.height = ROI_OFFSET*2;
	cvNamedWindow("Camera",1);
	
	nikinect.Initial(nikinect.Mode_U);
	nikinect.Start(nikinect.Mode_U);
	clearImg(cameraImg);//清空手势绘制屏幕
	
	 while((key!=27)&&(nikinect.UpdateData(nikinect.Mode_U)))
	{
		memcpy(cameraImg->imageData,nikinect.imageMD.Data(),640*480*3);
		cvCvtColor(cameraImg,cameraImg,CV_RGB2BGR);//转换彩色图像数据
//		Mat color_image(nikinect.imageMD.YRes(), nikinect.imageMD.XRes(),CV_8UC3, (char *)nikinect.imageMD.Data());
		  // update 16 bit depth matrix
		Mat depth_image(nikinect.depthMD.YRes(), nikinect.depthMD.XRes(),
                            CV_16UC1, (char *)nikinect.depthMD.Data());//因为kinect获取到的深度图像实际上是无符号的16位数据
//		memcpy(depthRaw.data, nikinect.depthMD, nikinect.Kinect_XRes*nikinect.Kinect_YRes*2);
		depth_image.convertTo(depthShow, CV_8U, DEPTH_SCALE_FACTOR);
//		color_image.convertTo(depthShow, CV_8U, DEPTH_SCALE_FACTOR);
		// get users
		nikinect.updateTrackedUsers();
		XnUInt16 userCounts = nikinect.userGenerator.GetNumberOfUsers();//获得使用者数量
		if(nikinect.getNumTrackedUsers()> 0 )
		{
		    Skeleton skel = nikinect.getSkeleton(nikinect.getUID(0));
		    for(int handI = 0; handI < 2; handI++)
		    {
			SkeletonPoint hand;
			int handDepth;
			if(handI==0) hand=skel.leftHand;
			else hand=skel.rightHand;
			if(hand.confidence==1.0)
			{
			    handDepth=hand.z*(DEPTH_SCALE_FACTOR);
			     if(!handApproachingDisplayPerimeter(hand.x, hand.y))
			    {
				roi.x = hand.x - ROI_OFFSET;
				roi.y = hand.y - ROI_OFFSET;
			    }
			}else handDepth = -1;
			 // extract hand from image
			Mat handCpy(depthShow, roi);
//			cvCvtColor(handCpy,handCpy,COLOR_BGR2GRAY);//转换
			Mat handMat = handCpy.clone();//复制得到手型区域

			// binary threshold二值化
			if(handDepth != -1)
			{
//			    blur(handMat,handMat,Size(3,3));
			    handMat = (handMat > (handDepth - BIN_THRESH_OFFSET)) & (handMat < (handDepth + BIN_THRESH_OFFSET));
//			    threshold(handMat,handMat,handDepth - BIN_THRESH_OFFSET,255,CV_THRESH_BINARY);
			}

			// last pre-filtering step, apply median blur中值滤波
			medianBlur(handMat, handMat, MEDIAN_BLUR_K);

			// create debug image of thresholded hand and cvt to RGB so hints show in color
			handDebug = handMat.clone();
			debugFrames.push_back(handDebug);
			cvtColor(debugFrames[handI], debugFrames[handI], CV_GRAY2RGB);
			
			std::vector< std::vector<Point> > contours;
			findContours(handMat, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
			
			 if(contours.size()) 
			 {
			    for (int i = 0; i < contours.size(); i++) 
			    {
				vector<Point> contour = contours[i];
				Mat contourMat = Mat(contour);
				double cArea = contourArea(contourMat);

				if(cArea > 2000) // likely the hand
				{
				    Scalar center = mean(contourMat);
				    Point centerPoint = Point(center.val[0], center.val[1]);

				    // approximate the contour by a simple curve
				    vector<Point> approxCurve;
				    approxPolyDP(contourMat, approxCurve, 10, true);

				    vector< vector<Point> > debugContourV;
				    debugContourV.push_back(approxCurve);
				    drawContours(debugFrames[handI], debugContourV, 0, COLOR_DARK_GREEN, 3);

				    vector<int> hull;
				    convexHull(Mat(approxCurve), hull, false, false);

				    // draw the hull points
				    for(int j = 0; j < hull.size(); j++)
				    {
					int index = hull[j];
					circle(debugFrames[handI], approxCurve[index], 3, COLOR_YELLOW, 2);
				    }

				    // find convexity defects
				    vector<ConvexityDefect> convexDefects;
				    findConvexityDefects(approxCurve, hull, convexDefects);
				    printf("Number of defects: %d.\n", (int) convexDefects.size());

				    for(int j = 0; j < convexDefects.size(); j++)
				    {
					circle(debugFrames[handI], convexDefects[j].depth_point, 3, COLOR_BLUE, 2);
				    }
				    
				    // assemble point set of convex hull
				    vector<Point> hullPoints;
				    for(int k = 0; k < hull.size(); k++)
				    {
					int curveIndex = hull[k];
					Point p = approxCurve[];
					hullPoints.push_back(p);
				    }

				    // area of hull and curve
				    double hullArea  = contourArea(Mat(hullPoints));
				    double curveArea = contourArea(Mat(approxCurve));
				    double handRatio = curveArea/hullArea;

				    // hand is grasping
				    if(handRatio > GRASPING_THRESH)
					circle(debugFrames[handI], centerPoint, 5, COLOR_LIGHT_GREEN, 5);
				    else
					circle(debugFrames[handI], centerPoint, 5, COLOR_RED, 5);
				}
			    } // contour conditional
			 }	
		    }
		    cvCircle(cameraImg,cvPoint(skel.rightHand.x,skel.rightHand.y),3,CV_RGB(0,0,255),12);//画关节圆点7
		    cvCircle(cameraImg,cvPoint(skel.leftHand.x,skel.leftHand.y),3,CV_RGB(0,0,255),12);//画关节圆点8
		    

		}
		cvShowImage("Camera",cameraImg);//显示彩色图像
		 if(debugFrames.size() >= 2 )
		{
		    resize(debugFrames[0], debugFrames[0], Size(), 3, 3);
		    resize(debugFrames[1], debugFrames[1], Size(), 3, 3);
		    imshow("leftHandFrame",  debugFrames[0]);
		    imshow("rightHandFrame",  debugFrames[1]);
		    debugFrames.clear();
		}
		key=cvWaitKey(20);
	}
	// stop and shutdown
	cvDestroyWindow("Camera");
	cvReleaseImage(&cameraImg);
	nikinect.Stop();
	
	
    return 0;
}
