// Import libraries
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <cmath>
using namespace std;
using namespace cv;

const std::string data_path = "/home/prismadynamics/githome/opencv_projects/opencv_courses/C++/ComputerVision_I";
double var_abs_laplacian(Mat image){
    ///
    /// YOUR CODE HERE
    ///
    cv::Mat ret, _temp;
    cv::cvtColor(image,_temp,COLOR_BGR2GRAY);
    double sum, L_mean;
    cv::Mat k1 = (Mat_<double>(3,3) << 0,-1,0,-1,4,1,0,-1,0);
    k1 = (1.0/6.0)*k1;
    cv::filter2D(_temp, ret, -1 ,  k1, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
    L_mean = (1.0/(ret.rows*ret.cols))*cv::sum(ret)[0];
	ret = cv::abs(ret) - L_mean;
    ret.mul(ret);
    sum = cv::sum(ret)[0];
    return sum;
}
double sum_modified_laplacian(Mat image){
    ///
    /// YOUR CODE HERE
    ///
    Mat kernel, _temp;
    double ret;

    int borderType = BORDER_CONSTANT;
    int img_row = image.rows;
    int img_col = image.cols;

    int step = 1;
    cv::Mat ML = cv::Mat::zeros(img_row-2*step,img_col-2*step, CV_64F);
    cv::cvtColor(image,_temp,COLOR_BGR2GRAY);
    for(int i = step; i < img_row-2*step; i++){
    	for(int j = step; j < img_col-2*step; j++){
    		double x1 = 2*_temp.at<double>(i,j) - _temp.at<double>(i-step,j) - _temp.at<double>(i+step,j); 
    		double x2 = 2*_temp.at<double>(i,j) - _temp.at<double>(i,j-step) - _temp.at<double>(i,j+step);
    		ML.at<double>(i-step, j-step) = cv::abs(x1) - cv::abs(x2);
    		cout << _temp.at<uchar>(i,j) << endl;

    	}
    }
    ret = cv::sum(ML)[0];
    return ret;
}
void CallBackFunc(int event, int x, int y, int flags, void* userdata){
	     if  ( event == EVENT_LBUTTONDOWN )
     {
          cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
     }
}
int main() 
{
	// Read input video filename
	string filename = "/home/prismadynamics/githome/opencv_projects/opencv_courses/C++/ComputerVision_I/week4_autofocus//data/focus-test.mp4";

	// Create a VideoCapture object
	VideoCapture cap(filename);

	// Read first frame from the video
	Mat frame;
	cap >> frame;
	// Display total number of frames in the video
	cout << "Total number of frames : " << (int)cap.get(CAP_PROP_FRAME_COUNT);

	double maxV1 = 0;
	double maxV2 = 0;

	// Frame with maximum measure of focus
	// Obtained using methods 1 and 2
	Mat bestFrame1;
	Mat bestFrame2;

	// Frame ID of frame with maximum measure
	// of focus
	// Obtained using methods 1 and 2
	int bestFrameId1 = 0;
	int bestFrameId2 = 0;

	// Get measures of focus from both methods
	double val1 = var_abs_laplacian(frame);
	double val2 = sum_modified_laplacian(frame);

	// Specify the ROI for flower in the frame
	// UPDATE THE VALUES BELOW
	int topCorner = 57;
	int leftCorner = 470;
	int bottomCorner = 599;
	int rightCorner = 994;

	Mat flower;
	flower = frame(Range(topCorner,bottomCorner),Range(leftCorner,rightCorner));
	// Iterate over all the frames present in the video
	while (1){
	    // Crop the flower region out of the frame
	    flower = frame(Range(topCorner,bottomCorner),Range(leftCorner,rightCorner));
	    // Get measures of focus from both methods
	    val1 = var_abs_laplacian(flower);
	    val2 = sum_modified_laplacian(flower);
	    // If the current measure of focus is greater 
	    // than the current maximum
	    cv::imshow("frame",flower);
	    if (val1 > maxV1){
	        // Revise the current maximum
	        maxV1 = val1;
	        // Get frame ID of the new best frame
	        bestFrameId1 = (int)cap.get(CAP_PROP_POS_FRAMES);
	        // Revise the new best frame
	        bestFrame1 = frame.clone();
	        cout << "Frame ID of the best frame [Method 1]: " << bestFrameId1 << endl;
	    }
	    // If the current measure of focus is greater 
	    // than the current maximum
	    if (val2 > maxV2){
	        // Revise the current maximum
	        maxV2 = val2;
	        // Get frame ID of the new best frame
	        bestFrameId2 = (int)cap.get(CAP_PROP_POS_FRAMES);
	        // Revise the new best frame
	        bestFrame2 = frame.clone();
	        cout << "Frame ID of the best frame [Method 2]: " << bestFrameId2 << endl;
	    }
	    cap >> frame;
	    if (frame.empty())
	        break;
	}

	cout << "================================================" << endl;

	// Print the Frame ID of the best frame
	cout << "Frame ID of the best frame [Method 1]: " << bestFrameId1 << endl;
	cout << "Frame ID of the best frame [Method 2]: " << bestFrameId2 << endl;

	cap.release();

	Mat out;
	//hconcat(bestFrame1, bestFrame2, out);
	return 0;
}
