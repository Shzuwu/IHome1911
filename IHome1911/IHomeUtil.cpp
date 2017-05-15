#include "IHomeUtil.h"


IHomeUtil::IHomeUtil()
	: isObjDetected(false)
{
	cout << "IHomeUtil()" << endl;

	detector = cvCreateBlobDetectorSimple();
	tracker = cvCreateBlobTrackerMSFG(); // 测试最佳跟踪方法
	ct = new CompressiveTracker;

	pNewList = new CvBlobSeq;
	pOldList = new CvBlobSeq;
	isObjDetected = false;
}

// 析构函数
IHomeUtil::~IHomeUtil()
{
	cout << "~IHomeUtil()" << endl;

	delete bgs;
	delete pOldList;

	tracker->Release();
	detector->Release();
	//delete pNewList;
}

// 前景掩膜团块检测
void IHomeUtil::blobDetection(Mat& imgInput, Mat& imgMask, Mat& imgBlob, CvBlob& blob)
{
	IplImage *pImg = &IplImage(imgInput);
	IplImage *pImgFG = &IplImage(imgMask);
	imgBlob = Mat(imgInput);//显示到前景图像上

	if (tracker->GetBlobNum() < 1){

		// 没有跟踪结果，则进行新团块检测
		detector->DetectNewBlob(pImg, pImgFG, pNewList, pOldList);

		if (pNewList->GetBlobNum() > 0){

			int i = pNewList->GetBlobNum();
			for (; i > 0; i--)// 遍历当前帧中的新团块
			{
				CvBlob* pB = pNewList->GetBlob(i - 1);
				drawBlob(imgBlob, pB, "", color_white);

				// 满足阈值则添加至跟踪器
				if (pB->w > threshold && pB->h > threshold)
				{
					cout << "新目标  --->  W： " << pB->w << ", H: " << pB->h << endl;
					tracker->AddBlob(pB, pImg, pImgFG); // 添加到跟踪器
					pNewList->DelBlobByID(i - 1);			// 从检测器中删除

					// 压缩跟踪器
					Point start(pB->x - 0.5*pB->w, pB->y - 0.5*pB->h);
					Point end(pB->x + 0.5*pB->w, pB->y + 0.5*pB->h);
					Rect r_blob(start, end);
					ct->init(imgInput, r_blob);
				}
			}
		}
		pOldList = pNewList;

	}

	// 团块跟踪
	if (tracker->GetBlobNum() > 0){

		tracker->Process(pImg, pImgFG);

		int i = tracker->GetBlobNum();
		for (; i > 0; i--){
			CvBlob* bTrack = tracker->GetBlob(i - 1);
			tracker->ProcessBlob(i - 1, bTrack, pImg, pImgFG);

			drawBlob(imgBlob, bTrack);

			// 缩放回原图尺度，供外部使用
			resizeBlobToOrg(*bTrack, blob);
		}
	}

}


// 显示带有标记信息的输入图像
void IHomeUtil::showInputImg(Mat& img, list<ClibrationData>& labels)
{
	list<ClibrationData>::iterator v;
	for (v = labels.begin(); v != labels.end(); ++v){
		// 标记信息
		Rect rect(*(v->getRect()));
		Point pointStart(rect.x, rect.y);
		Point pointEnd(rect.x + rect.width, rect.y + rect.height);
		Scalar color = *(v->getColor());

		CvSize textSize = getTextSize(v->getLabel(), FONT_HERSHEY_SIMPLEX, 0.5, 1, 0);
		rectangle(img, pointStart, Point(rect.x + textSize.width, rect.y - textSize.height), color, CV_FILLED, 8, 0);

		putText(img, v->getLabel(), pointStart, FONT_HERSHEY_SIMPLEX, 0.5, color_white, 1);
		rectangle(img, pointStart, pointEnd, color, 1, 8, 0);
	}
	imshow("标定后的输入", img);
}

// Draw a blob on the object
void IHomeUtil::drawBlob(Mat& img, CvBlob* b)
{
	drawBlob(img, b, "", color_green);
}

void IHomeUtil::drawBlob(Mat& img, CvBlob* b, string objName)
{
	drawBlob(img, b, objName, color_green);
}

// Draw a blob on the object
void IHomeUtil::drawBlob(Mat& img, CvBlob* b, string objName, Scalar color)
{
	// 标记信息
	Point pointStart(b->x - 0.5*b->w, b->y - 0.5*b->h);
	Point pointEnd(b->x + 0.5*b->w, b->y + 0.5*b->h);
	if (!objName.empty())
	{
		CvSize textSize = getTextSize(objName, FONT_HERSHEY_SIMPLEX, 0.5, 1, 0);
		rectangle(
			img, pointStart,
			Point(b->x - 0.5*b->w + textSize.width, b->y - 0.5*b->h - textSize.height),
			color, CV_FILLED, 8, 0);
		putText(img, objName, pointStart, FONT_HERSHEY_SIMPLEX, 0.5, color_white, 1);
	}
	if (!img.empty())
	{
		rectangle(img, pointStart, pointEnd, color, 1, 8, 0);
	}
}

// 将Blob缩放回原图的尺度
void IHomeUtil::resizeBlobToOrg(const CvBlob& blob_input, CvBlob& blob_output)
{
	// TODO：尺度还原


}


// 将输入图片缩放至可以实时处理的足够小尺度,img_size为缩放是的最大尺度
void IHomeUtil::resizeInputImg(const Mat& img_input, Mat& img_output, int img_size)
{
	// TODO：尺度缩小
	Size size;



	resize(img_input, img_output, size);
}


// 是否已经检测到了目标
bool IHomeUtil::isDetected()
{
	return isObjDetected;
}



// 检测阶段检测符合条件的运动目标
void IHomeUtil::initCTTracker(Mat& img_input, Rect& rect)
{
	cv::Mat img_mask;
	cv::Mat img_bkgmodel;
	// 默认情况下自动显示前景掩膜
	bgs->process(img_input, img_mask, img_bkgmodel);

	IplImage *pImg = &IplImage(img_input);
	IplImage *pImgFG = &IplImage(img_mask);
	Mat imgBlob = Mat(img_input);//显示到前景图像上

	// 没有跟踪结果，则进行新团块检测
	detector->DetectNewBlob(pImg, pImgFG, pNewList, pOldList);

	if (pNewList->GetBlobNum() > 0){

		int i = pNewList->GetBlobNum();
		for (; i > 0; i--)// 遍历当前帧中的新团块
		{
			CvBlob* pB = pNewList->GetBlob(i - 1);
			drawBlob(imgBlob, pB, "", color_white);

			// 满足阈值则添加至跟踪器
			if (pB->w > threshold && pB->h > threshold)
			{
				cout << "新目标  --->  W： " << pB->w << ", H: " << pB->h << endl;
				tracker->AddBlob(pB, pImg, pImgFG); // 添加到跟踪器
				pNewList->DelBlobByID(i - 1);			// 从检测器中删除

				// 压缩跟踪器
				Point start(pB->x - 0.5*pB->w, pB->y - 0.5*pB->h);
				Point end(pB->x + 0.5*pB->w, pB->y + 0.5*pB->h);
				Rect r_blob(start, end);
				rect = r_blob;
				
				ct->init(img_input, r_blob);
				isObjDetected = true;		
			}
		}
	}
	pOldList = pNewList;
}



// CT Tracking
void IHomeUtil::trackObj(Mat& img_input, Rect& rect)
{
	// 如果没有检测到就检测
	if (!isDetected()){
		initCTTracker(img_input, rect);
		return;
	}

	// 检测到了就做跟踪，采用压缩跟踪器
	ct->processFrame(img_input, rect);
}
