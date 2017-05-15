#pragma once

#include <iostream>
#include <cv.h>

using namespace cv;
using namespace std;

class ClibrationData
{
public:
	ClibrationData();
	ClibrationData(Rect *r , string label);

	virtual ~ClibrationData();

private:
	Rect *r;
	string label;

public:
	Rect* getRect();
	string getLabel();
	void printInfo();
private:
	Scalar* color;
public:
	ClibrationData(Rect* r, string label, Scalar* color);
	// ·µ»ØÑÕÉ«
	Scalar* getColor();
};

