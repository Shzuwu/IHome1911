#include "IHomeUtil.h"


IHomeUtil::IHomeUtil()
	: isObjDetected(false)
{
	cout << "IHomeUtil()" << endl;

	detector = cvCreateBlobDetectorSimple();
	tracker = cvCreateBlobTrackerMSFG(); // ������Ѹ��ٷ���
	ct = new CompressiveTracker;

	pNewList = new CvBlobSeq;
	pOldList = new CvBlobSeq;
	isObjDetected = false;
}

// ��������
IHomeUtil::~IHomeUtil()
{
	cout << "~IHomeUtil()" << endl;

	delete bgs;
	delete pOldList;

	tracker->Release();
	detector->Release();
	//delete pNewList;
}

// ǰ����Ĥ�ſ���
void IHomeUtil::blobDetection(Mat& imgInput, Mat& imgMask, Mat& imgBlob, CvBlob& blob)
{
	IplImage *pImg = &IplImage(imgInput);
	IplImage *pImgFG = &IplImage(imgMask);
	imgBlob = Mat(imgInput);//��ʾ��ǰ��ͼ����

	if (tracker->GetBlobNum() < 1){

		// û�и��ٽ������������ſ���
		detector->DetectNewBlob(pImg, pImgFG, pNewList, pOldList);

		if (pNewList->GetBlobNum() > 0){

			int i = pNewList->GetBlobNum();
			for (; i > 0; i--)// ������ǰ֡�е����ſ�
			{
				CvBlob* pB = pNewList->GetBlob(i - 1);
				drawBlob(imgBlob, pB, "", color_white);

				// ������ֵ�������������
				if (pB->w > threshold && pB->h > threshold)
				{
					cout << "��Ŀ��  --->  W�� " << pB->w << ", H: " << pB->h << endl;
					tracker->AddBlob(pB, pImg, pImgFG); // ��ӵ�������
					pNewList->DelBlobByID(i - 1);			// �Ӽ������ɾ��

					// ѹ��������
					Point start(pB->x - 0.5*pB->w, pB->y - 0.5*pB->h);
					Point end(pB->x + 0.5*pB->w, pB->y + 0.5*pB->h);
					Rect r_blob(start, end);
					ct->init(imgInput, r_blob);
				}
			}
		}
		pOldList = pNewList;

	}

	// �ſ����
	if (tracker->GetBlobNum() > 0){

		tracker->Process(pImg, pImgFG);

		int i = tracker->GetBlobNum();
		for (; i > 0; i--){
			CvBlob* bTrack = tracker->GetBlob(i - 1);
			tracker->ProcessBlob(i - 1, bTrack, pImg, pImgFG);

			drawBlob(imgBlob, bTrack);

			// ���Ż�ԭͼ�߶ȣ����ⲿʹ��
			resizeBlobToOrg(*bTrack, blob);
		}
	}

}


// ��ʾ���б����Ϣ������ͼ��
void IHomeUtil::showInputImg(Mat& img, list<ClibrationData>& labels)
{
	list<ClibrationData>::iterator v;
	for (v = labels.begin(); v != labels.end(); ++v){
		// �����Ϣ
		Rect rect(*(v->getRect()));
		Point pointStart(rect.x, rect.y);
		Point pointEnd(rect.x + rect.width, rect.y + rect.height);
		Scalar color = *(v->getColor());

		CvSize textSize = getTextSize(v->getLabel(), FONT_HERSHEY_SIMPLEX, 0.5, 1, 0);
		rectangle(img, pointStart, Point(rect.x + textSize.width, rect.y - textSize.height), color, CV_FILLED, 8, 0);

		putText(img, v->getLabel(), pointStart, FONT_HERSHEY_SIMPLEX, 0.5, color_white, 1);
		rectangle(img, pointStart, pointEnd, color, 1, 8, 0);
	}
	imshow("�궨�������", img);
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
	// �����Ϣ
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

// ��Blob���Ż�ԭͼ�ĳ߶�
void IHomeUtil::resizeBlobToOrg(const CvBlob& blob_input, CvBlob& blob_output)
{
	// TODO���߶Ȼ�ԭ


}


// ������ͼƬ����������ʵʱ������㹻С�߶�,img_sizeΪ�����ǵ����߶�
void IHomeUtil::resizeInputImg(const Mat& img_input, Mat& img_output, int img_size)
{
	// TODO���߶���С
	Size size;



	resize(img_input, img_output, size);
}


// �Ƿ��Ѿ���⵽��Ŀ��
bool IHomeUtil::isDetected()
{
	return isObjDetected;
}



// ���׶μ������������˶�Ŀ��
void IHomeUtil::initCTTracker(Mat& img_input, Rect& rect)
{
	cv::Mat img_mask;
	cv::Mat img_bkgmodel;
	// Ĭ��������Զ���ʾǰ����Ĥ
	bgs->process(img_input, img_mask, img_bkgmodel);

	IplImage *pImg = &IplImage(img_input);
	IplImage *pImgFG = &IplImage(img_mask);
	Mat imgBlob = Mat(img_input);//��ʾ��ǰ��ͼ����

	// û�и��ٽ������������ſ���
	detector->DetectNewBlob(pImg, pImgFG, pNewList, pOldList);

	if (pNewList->GetBlobNum() > 0){

		int i = pNewList->GetBlobNum();
		for (; i > 0; i--)// ������ǰ֡�е����ſ�
		{
			CvBlob* pB = pNewList->GetBlob(i - 1);
			drawBlob(imgBlob, pB, "", color_white);

			// ������ֵ�������������
			if (pB->w > threshold && pB->h > threshold)
			{
				cout << "��Ŀ��  --->  W�� " << pB->w << ", H: " << pB->h << endl;
				tracker->AddBlob(pB, pImg, pImgFG); // ��ӵ�������
				pNewList->DelBlobByID(i - 1);			// �Ӽ������ɾ��

				// ѹ��������
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
	// ���û�м�⵽�ͼ��
	if (!isDetected()){
		initCTTracker(img_input, rect);
		return;
	}

	// ��⵽�˾������٣�����ѹ��������
	ct->processFrame(img_input, rect);
}
