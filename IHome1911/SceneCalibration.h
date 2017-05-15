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

//创建一个list容器的实例  
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
		labels = &theLabels;		// 借助指针对变量进行操作
		windowName = name;

		namedWindow(windowName);	// 定义一个img窗口  
		imshow(windowName, img);
		setMouseCallback(windowName, on_mouse, 0);// 调用回调函数

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
			cout << "push Q， exiting ···" << endl;

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

			// 标记信息
			putText(img, label, Point(rect.x, rect.y), FONT_HERSHEY_SIMPLEX, 0.5, scalar);
			rectangle(img, Point(rect.x, rect.y), Point(rect.x + rect.width, rect.y + rect.height), scalar, 1, 8, 0);
		}		
	}

	void on_mouse(int event, int x, int y, int flags, void *ustc)//event鼠标事件代号，x,y鼠标坐标，flags拖拽和键盘操作的代号  
	{
		static Point pre_pt = (-1, -1);//初始坐标  
		static Point cur_pt = (-1, -1);//实时坐标  
		char temp[16];
		if (event == CV_EVENT_LBUTTONDOWN)//左键按下，读取初始坐标，并在图像上该点处划圆  
		{
			mdl.copyTo(img);//标签在图片上保持显示  
			sprintf(temp, "(%d,%d)", x, y);
			pre_pt = Point(x, y);
			putText(img, temp, pre_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255), 1, 8);//在窗口上显示坐标  
			circle(img, pre_pt, 2, Scalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);//划圆  
			imshow(windowName, img);
		}
		else if (event == CV_EVENT_MOUSEMOVE && !(flags & CV_EVENT_FLAG_LBUTTON))//左键没有按下的情况下鼠标移动的处理函数  
		{
			img.copyTo(tmp);//将img复制到临时图像tmp上，用于显示实时坐标  
			sprintf(temp, "(%d,%d)", x, y);
			cur_pt = Point(x, y);
			putText(tmp, temp, cur_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));//只是实时显示鼠标移动的坐标  
			imshow(windowName, tmp);
		}
		else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//左键按下时，鼠标移动，则在图像上划矩形  
		{
			img.copyTo(tmp);
			sprintf(temp, "(%d,%d)", x, y);
			cur_pt = Point(x, y);
			putText(tmp, temp, cur_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));
			rectangle(tmp, pre_pt, cur_pt, Scalar(0, 255, 0, 0), 1, 8, 0);//在临时图像上实时显示鼠标拖动时形成的矩形  
			imshow(windowName, tmp);
		}
		else if (event == CV_EVENT_LBUTTONUP)//左键松开，将在图像上划矩形  
		{
			mdl.copyTo(img);
			sprintf(temp, "(%d,%d)", x, y);
			cur_pt = Point(x, y);
			//putText(img, temp, cur_pt, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));
			circle(img, pre_pt, 2, Scalar(255, 0, 0, 0), CV_FILLED, CV_AA, 0);
			rectangle(img, pre_pt, cur_pt, Scalar(0, 255, 0, 0), 1, 8, 0);//根据初始点和结束点，将矩形画到img上  
			imshow(windowName, img);
			
			
			//截取矩形包围的图像 
			int width = abs(pre_pt.x - cur_pt.x);
			int height = abs(pre_pt.y - cur_pt.y);
			if (width == 0 || height == 0)
			{
				// 不构成矩形选区
				return;
			}

			Rect temp(min(cur_pt.x, pre_pt.x), min(cur_pt.y, pre_pt.y), width, height);

			setLabel(temp); // 为矩形设置标签
			if (label != ""){img.copyTo(mdl);}//如果有标签则保存矩形框，否则消失
		}
	}

	void setColor(int Red, int Green, int Blue){
		scalar.val[0] = Red;
		scalar.val[1] = Green;
		scalar.val[2] = Blue;
	}

	// 打印标定信息
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