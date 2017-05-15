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

#include "IHomeHeaders.h"

using namespace bgslibrary::algorithms;
using namespace std;
using namespace cv;


class IHomeUtil
{
public:
	IHomeUtil();
	virtual ~IHomeUtil();

	// 跟踪
	void trackObj(Mat& img_input, Rect& rect);
	// 是否已经检测到了目标
	bool isDetected();

	// 将输入图片缩放至可以实时处理的足够小尺度
	void resizeInputImg(const Mat& img_input, Mat& img_output, int img_size);
	// 前景掩膜团块检测
	void blobDetection(Mat& imgInput, Mat& imgMask, Mat& imgBlob, CvBlob& blob);
	// 显示带有标记信息的输入图像
	void showInputImg(Mat& img, list<ClibrationData>& labels);

	// Draw a blob on the object
	void drawBlob(Mat& img, CvBlob* b, string objName, Scalar color);
	void drawBlob(Mat& img, CvBlob* b);
	void drawBlob(Mat& img, CvBlob* b, string objName);

private:
	CvBlobDetector* detector;
	CvBlobTracker* tracker;
	CompressiveTracker* ct;
	IBGS *bgs = new MultiLayer;;

	CvBlobSeq* pNewList;
	CvBlobSeq* pOldList;

	Scalar color_white = Scalar(255, 255, 255);
	Scalar color_green = Scalar(0, 255, 0);
	float threshold = 50.0;
	int resizeImgSize = 189;

	// 将Blob缩放回原图的尺度
	void resizeBlobToOrg(const CvBlob& blob_input, CvBlob& blob_output);
	// 检测阶段检测符合条件的运动目标
	void initCTTracker(Mat& img_input, Rect& rect);

	bool isObjDetected; // 是否已经检测到了目标
};

