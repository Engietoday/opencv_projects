#include <iostream>
#include <string>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

int maxScaleUp = 100;
int scaleFactor = 0;
int scaleType = 0;
int maxType = 1;

string windowName = "Resize Image";
string trackbarValue = "Scale";
string trackbarType = "Type: \n 0: Scale Up \n 1: Scale Down";
Mat im;
void scaleImage(int, void*);

// Callback functions
void scaleImage(int, void*){

    // Get the Scale factor from the trackbar
    double scaleFactorDouble = 1;
    if(scaleType){
   		if(scaleFactor == 0){
   			scaleFactorDouble = 1;
   		}else if(scaleFactor == 100){
   			scaleFactorDouble = 0.01;
   		}else{
    		scaleFactorDouble = (100.0 - scaleFactor)/100.0;
   		}
    }else{
    	scaleFactorDouble = 1 + scaleFactor/100.0;
    }
    
    if (scaleFactorDouble == 0){
        scaleFactorDouble = 1;
    }
    
    Mat scaledImage;
    
    // Resize the image
    resize(im, scaledImage, Size(), scaleFactorDouble, scaleFactorDouble, INTER_LINEAR);
    if(scaleFactorDouble < 0.6){
    	resizeWindow(windowName, 0.6*im.rows, 0.6*im.cols); // Resizing window because window size becomes unstable when minimizing image beyond 50%.
    }else{
    	resizeWindow(windowName, scaledImage.rows, scaledImage.cols);
    }
    imshow(windowName, scaledImage);
}
int main(){
// load an image
im = imread("../data/images/truth.png");

// Create a window to display results
namedWindow(windowName, WINDOW_AUTOSIZE);

// Create Trackbars and associate a callback function
createTrackbar(trackbarValue, windowName, &scaleFactor, maxScaleUp, scaleImage);
createTrackbar(trackbarType, windowName, &scaleType, maxType, scaleImage);

scaleImage(25,0);

while (true){
    int c;
    c = waitKey(20);
    if (static_cast<char>(c) == 27)
        break;
}

return 0;
}
