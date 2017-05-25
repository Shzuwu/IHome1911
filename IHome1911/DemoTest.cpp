#include <iostream>
#include <cv.h>
#include <highgui.h>

#include "SceneCalibration.h"
#include "IHomeUtil.h"

using namespace SceneCalibration;

IHomeUtil *util = new IHomeUtil;

CvBlob bolb;
IplImage *frame;
Size default_size;
Rect objRect;

int main(int argc, char **argv)
{
	LISTLABEL labels ;
	string areaName = "initial area";
	
	cvWaitKey(0);

	CvCapture *capture = 0;
	// capture = cvCaptureFromCAM(0);
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

		Mat img_frame(frame);
		Mat img_input;
		Mat img_blob;

		//resize(img_frame, img_input, default_size);

		util->resizeInputImg(img_frame, img_input, 160);

		util->showInputImg(img_frame, labels); // 显示带标记信息的输入图像
		
		util->trackObj(img_input, img_blob, objRect);// BGS Tracking

		imshow("输入图像", img_input);

		////检测到目标，rect才有值
		if (util->isDetected())
		{
			// Draw Points
			rectangle(img_frame, objRect, Scalar(0, 0, 255));

			util->anomalyDetection(img_frame,objRect, labels, areaName);

			// Display
			imshow("跟踪窗口", img_frame);			
		}
			
		if (cvWaitKey(1) >= 0)
			break;
	}

	delete util;

	system("pause");

	cvDestroyAllWindows();
	cvReleaseCapture(&capture);

	return 0;
}