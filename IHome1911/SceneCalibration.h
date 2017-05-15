#pragma once

#include <iostream>
#include <cv.h>
#include <highgui.h>
#include "ClibrationData.h"
#include <list>   
#include <numeric>   
#include <algorithm>   
using namespace std;

using namespace cv;

//����һ��list������ʵ��  
typedef list<ClibrationData> LISTLABEL;

namespace SceneCalibration
{
	Mat org, result, img, tmp, mdl;
	string windowName ;
	LISTLABEL* labels;
	string label = "";
	Scalar scalar;
	vector<ClibrationData> cDatas;

	void on_mouse(int event, int x, int y, int flags, void *ustc);
	void setColor(int Red, int Green, int Blue);
	void printLabels(LISTLABEL &labels);
	void printLabels(vector<ClibrationData> &labels);

	void showMenu(){
		cout << "*************Welcome to Calibration***************" << endl;
		cout << "** Now, choose the rect using your mouse, and   **" << endl;
		cout << "** then give the label                          **" << endl;
		cout << "**                                              **" << endl;
		cout << "** c  - > Chair       |  d - >  Desk            **" << endl;
		cout << "** s  - > Sofa        |  r - >  Water dispenser **" << endl;
		cout << "** w  - > Window      |  m - >  Show menu       **" << endl;
		cout << "** q  - > exit        |  space - >  confirm     **" << endl;
		cout << "**                                              **" << endl;
		cout << "**************************************************" << endl;
	}

	void calibrateSence(const Mat& initialImg, LISTLABEL& theLabels,const string name){
		initialImg.copyTo(org);
		initialImg.copyTo(img);
		initialImg.copyTo(tmp);
		initialImg.copyTo(mdl);
		labels = &theLabels;		// ����ָ��Ա������в���
		windowName = name;

		namedWindow(windowName);	// ����һ��img����  
		imshow(windowName, img);
		setMouseCallback(windowName, on_mouse, 0);// ���ûص�����

		showMenu();
	}

	void setLabel(Rect &rect){

		label = "";
		char cmd = cvWaitKey(0);
		switch (cmd){

		case 'c':
			label = "Chair";
			setColor(0, 0, 255);//Blue
			break;

		case 'r':
			label = "Water dispenser";
			setColor(255, 255, 0);//Gold
			break;

		case 'w':
			label = "Window";
			setColor(255, 0, 0);//Red
			break;

		case 's':
			label = "Sofa";
			setColor(255, 69, 0);//Orange
			break;

		case 'd':
			label = "Desk";
			setColor(0, 250, 154);//SpringGreen
			break;

		case 'q':
			cout << "push Q�� exiting ������" << endl;

			return;
			break;

		case 'm':
			showMenu();

		default:
			break;
		}

		if (label != ""){
			ClibrationData *data = new ClibrationData(&rect, label, &scalar);

			(*labels).push_back(*data);
			data->printInfo();

			// �����Ϣ
			putText(img, label, Point(rect.x, rect.y), FONT_HERSHEY_SIMPLEX, 0.5, scalar);
			rectangle(img, Point(rect.x, rect.y), Point(rect.x + rect.width, rect.y + rect.height), scalar, 1, 8, 0);
		}		
	}

	void on_mouse(int event, int x, int y, int flags, void *ustc)//event����¼����ţ�x,y������꣬flags��ק�ͼ��̲����Ĵ���  
	{
		static Point pre_pt = (-1, -1);//��ʼ����  
		static Point cur_pt = (-1, -1);//ʵʱ����  
		char temp[16];
		if (event == CV_EVENT_LBUTTONDOWN)//������£���ȡ��ʼ���꣬����ͼ���ϸõ㴦��Բ  
		{
			mdl.copyTo(img);//��ǩ��ͼƬ�ϱ�����ʾ  
			sprintf(temp, "(%d,%d)", x, y);
			pre_pt = Point(x, y);
			putText(img, temp, pre_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255), 1, 8);//�ڴ�������ʾ����  
			circle(img, pre_pt, 2, Scalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);//��Բ  
			imshow(windowName, img);
		}
		else if (event == CV_EVENT_MOUSEMOVE && !(flags & CV_EVENT_FLAG_LBUTTON))//���û�а��µ����������ƶ��Ĵ�����  
		{
			img.copyTo(tmp);//��img���Ƶ���ʱͼ��tmp�ϣ�������ʾʵʱ����  
			sprintf(temp, "(%d,%d)", x, y);
			cur_pt = Point(x, y);
			putText(tmp, temp, cur_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));//ֻ��ʵʱ��ʾ����ƶ�������  
			imshow(windowName, tmp);
		}
		else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//�������ʱ������ƶ�������ͼ���ϻ�����  
		{
			img.copyTo(tmp);
			sprintf(temp, "(%d,%d)", x, y);
			cur_pt = Point(x, y);
			putText(tmp, temp, cur_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));
			rectangle(tmp, pre_pt, cur_pt, Scalar(0, 255, 0, 0), 1, 8, 0);//����ʱͼ����ʵʱ��ʾ����϶�ʱ�γɵľ���  
			imshow(windowName, tmp);
		}
		else if (event == CV_EVENT_LBUTTONUP)//����ɿ�������ͼ���ϻ�����  
		{
			mdl.copyTo(img);
			sprintf(temp, "(%d,%d)", x, y);
			cur_pt = Point(x, y);
			//putText(img, temp, cur_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));
			circle(img, pre_pt, 2, Scalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);
			rectangle(img, pre_pt, cur_pt, Scalar(0, 255, 0, 0), 1, 8, 0);//���ݳ�ʼ��ͽ����㣬�����λ���img��  
			imshow(windowName, img);
			
			
			//��ȡ���ΰ�Χ��ͼ�� 
			int width = abs(pre_pt.x - cur_pt.x);
			int height = abs(pre_pt.y - cur_pt.y);
			if (width == 0 || height == 0)
			{
				// �����ɾ���ѡ��
				return;
			}

			Rect temp(min(cur_pt.x, pre_pt.x), min(cur_pt.y, pre_pt.y), width, height);

			setLabel(temp); // Ϊ�������ñ�ǩ
			if (label != ""){img.copyTo(mdl);}//����б�ǩ�򱣴���ο򣬷�����ʧ
		}
	}

	void setColor(int Red, int Green, int Blue){
		scalar.val[0] = Red;
		scalar.val[1] = Green;
		scalar.val[2] = Blue;
	}

	// ��ӡ�궨��Ϣ
	void printLabels(LISTLABEL &labels){
		cout << "----------------------------------------------------"<<endl;
		cout << "The calibrated information is as follow:" << endl;
		cout << "Calibrated item size:[ " << labels.size() << " ]" << endl;
		list<ClibrationData>::iterator v;
		for (v = labels.begin(); v != labels.end(); ++v){
			v->printInfo();
		}
	}
};