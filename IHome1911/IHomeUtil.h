#pragma once
#include "opencv2/video/background_segm.hpp"
#include "opencv2/legacy/blobtrack.hpp"
#include "opencv2/legacy/legacy.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc_c.h>

#include <iostream>
#include <cv.h>
#include <highgui.h>
#include "ClibrationData.h"
#include <list>   
#include <numeric>   
#include <algorithm>   
#include "CompressiveTracker.h"

using namespace std;

using namespace cv;


class IHomeUtil
{
public:
	IHomeUtil();
	virtual ~IHomeUtil();


	// ������ͼƬ����������ʵʱ������㹻С�߶�
	void resizeInputImg(const Mat& img_input, Mat& img_output, int img_size);
	// ǰ����Ĥ�ſ���
	void blobDetection(Mat& imgInput, Mat& imgMask, Mat& imgBlob, CvBlob& blob);
	// ��ʾ���б����Ϣ������ͼ��
	void showInputImg(Mat& img, list<ClibrationData>& labels);

	// Draw a blob on the object
	void drawBlob(Mat& img, CvBlob* b, string objName, Scalar color);
	void drawBlob(Mat& img, CvBlob* b);
	void drawBlob(Mat& img, CvBlob* b, string objName);

private:
	CvBlobDetector* detector;
	CvBlobTracker* tracker;
	CompressiveTracker* ct;

	CvBlobSeq* pNewList;
	CvBlobSeq* pOldList;

	Scalar color_white = Scalar(255, 255, 255);
	Scalar color_green = Scalar(0, 255, 0);
	float threshold = 50.0;
	int resizeImgSize = 189;

private:
	// ��Blob���Ż�ԭͼ�ĳ߶�
	void resizeBlobToOrg(const CvBlob& blob_input, CvBlob& blob_output);

};

