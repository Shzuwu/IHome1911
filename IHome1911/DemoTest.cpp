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
	capture = cvCaptureFromFile("F:\\�ж���ѧ��\\����\\Datasets\\ww3.wmv");

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
			calibrateSence(frame, labels,"�궨����");	// �궨����
			cvWaitKey(0);
			cvDestroyWindow("�궨����");
			printLabels(labels);			// ��ӡ�����Ϣ
		}

		cv::Mat img_frame(frame);
		util->showInputImg(img_frame, labels); // ��ʾ�������Ϣ������ͼ��

		cv::Mat img_input;
		cv::resize(img_frame, img_input, default_size);
		//imshow("����ͼ��", img_frame);

		cv::Mat img_mask;
		cv::Mat img_bkgmodel;
		// Ĭ��������Զ���ʾǰ����Ĥ
		bgs->process(img_input, img_mask, img_bkgmodel);

		if (!img_mask.empty())
			imshow("ǰ��", img_mask);

		Mat img_blob;
		// ȫ����Ĥ���ſ���,
		util->blobDetection(img_input, img_mask, img_blob, bolb);

		if (!img_blob.empty()){
			imshow("�ſ���Ϣ", img_blob);
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

