#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char **argv) 
{
    int sz[3]={2,2,2};
    Mat M3;
    Mat M1(2,2,CV_8UC3,Scalar(0,0,255));
    Mat M2(3,sz,CV_8UC3,Scalar::all(0));
    M3.create(2,2,CV_8UC(2));
    
    cout << "M1" <<endl<<M1<< endl;
 //    cout << "M2" <<endl<<M2<< endl;
    cout << "M3" <<endl<<M3<< endl;
    
    return 0;
}
