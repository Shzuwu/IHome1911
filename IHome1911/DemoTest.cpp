#include <iostream>
#include <cv.h>
#include <highgui.h>
#include "IHomeHeaders.h"



using namespace bgslibrary::algorithms;
using namespace SceneCalibration;

IBGS *bgs = new MultiLayer;
IHomeUtil *util = new IHomeUtil;
CvBlob bolb;
IplImage *frame;
cv::Size default_size;

int main(int argc, char **argv)
{
	LISTLABEL labels ;
	
	cvWaitKey(0);

	CvCapture *capture = 0;
	//capture = cvCaptureFromCAM(0);
	capture = cvCaptureFromFile("F:\\研二下学期\\方向\\Datasets\\ww3.wmv");

	if (!capture){
		std::cerr << "Cannot initialize video!" << std::endl;
		system("pause");
		return -1;
	}

	default_size.width = 235;
	default_size.height = 189;

	int cnt = 0;
	while (1)
	{
		cnt++;
		frame = cvQueryFrame(capture);

		if (!frame) break;

		if (cnt == 1){
			calibrateSence(frame, labels,"标定窗口");	// 标定环境
			cvWaitKey(0);
			cvDestroyWindow("标定窗口");
			printLabels(labels);			// 打印标记信息
		}

		cv::Mat img_frame(frame);
		util->showInputImg(img_frame, labels); // 显示带标记信息的输入图像

		cv::Mat img_input;
		cv::resize(img_frame, img_input, default_size);
		//imshow("输入图像", img_frame);

		cv::Mat img_mask;
		cv::Mat img_bkgmodel;
		// 默认情况下自动显示前景掩膜
		bgs->process(img_input, img_mask, img_bkgmodel);

		if (!img_mask.empty())
			imshow("前景", img_mask);

		Mat img_blob;
		// 全景掩膜的团块检测,
		util->blobDetection(img_input, img_mask, img_blob, bolb);

		if (!img_blob.empty()){
			imshow("团块信息", img_blob);
		}
			
		if (cvWaitKey(33) >= 0)
			break;
	}

	delete bgs;
	delete util;

	cvDestroyAllWindows();
	cvReleaseCapture(&capture);

	return 0;
}

