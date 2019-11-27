#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;

Point anchor1, anchor2;
Mat source;


void saveROI(cv::Mat &src, cv::Point p1, cv::Point p2, std::string path){
	cv::Mat _tempMat;
	cv::Rect roi(p1, p2);
	_tempMat = src(roi);
	if(path != ""){
		cv::imwrite(path,_tempMat);
	}else{
		std::cout << "Empty Path!" << std::endl;
	}
}

// function which will be called on mouse input
void drawRectangle(int action, int x, int y, int flags, void *userdata)
{
	std::string path = "../data/images/face.png";
  // Action to be taken when left mouse button is pressed
  if( action == EVENT_LBUTTONDOWN )
  {
    anchor1 = Point(x,y);
    // Mark the first corner
	circle(source, anchor1, 1, Scalar(255,255,0), 1, LINE_8 );
  }
  // Action to be taken when left mouse button is released
  else if( action == EVENT_LBUTTONUP)
  {
    anchor2 = Point(x,y);
    if(anchor1 == anchor2){
    }else{
	   	rectangle(source, anchor1, anchor2, Scalar(255,255,0), 3, LINE_8);
	    imshow("Window", source);
	    saveROI(source, anchor1, anchor2, path);
	    std::cout << "Image saved in: " << path << std::endl;
    }
  }
}

int main(){
	source = imread("../data/images/sample.jpg",1);
	if(!source.data)
	{
	    cout <<  "Could not open or find the image" << std::endl ;
	    return -1;
	}
	// Make a dummy image, will be useful to clear the drawing
	Mat dummy = source.clone();
	namedWindow("Window");
	// highgui function called when mouse events occur
	setMouseCallback("Window", drawRectangle);
	int k=0;
	// loop until escape character is pressed
	while(k!=27){
	imshow("Window", source );
	putText(source,"Choose top left corner, and drag.?" ,Point(10,30), FONT_HERSHEY_SIMPLEX, 0.7,Scalar(255,255,255), 2 );
	k= waitKey(20) & 0xFF;
	if(k== 99)
	        // Another way of cloning
	        dummy.copyTo(source);
	}
	return 0;
}
