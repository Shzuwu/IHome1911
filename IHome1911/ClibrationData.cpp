#include "ClibrationData.h"


ClibrationData::ClibrationData()
	: color(NULL)
{
}

ClibrationData::ClibrationData(Rect *r, string label)
{
	this->r = (Rect *)malloc(sizeof(*r));
	this->r->x = r->x;
	this->r->y = r->y;
	this->r->height = r->height;
	this->r->width = r->width;

	this->label = label;
}

ClibrationData::~ClibrationData()
{
}


Rect* ClibrationData::getRect()
{
	return r;
}


string ClibrationData::getLabel()
{
	return label;
}

//打印标签信息，标签名&矩形框信息
void ClibrationData::printInfo()
{
	cout << "( x = " << r->x << "\ty = " << r->y ;
	cout << "\twidth = " << r->width << "\theight = " << r->height << " )" ;
	cout << "\t[ " << label << " ]" << endl;
}


ClibrationData::ClibrationData(Rect* r, string label, Scalar* color)
{
	this->r = (Rect *)malloc(sizeof(*r));
	this->r->x = r->x;
	this->r->y = r->y;
	this->r->height = r->height;
	this->r->width = r->width;

	this->color = (Scalar*)malloc(sizeof(*color));
	this->color->val[0] = color->val[0];
	this->color->val[1] = color->val[1];
	this->color->val[2] = color->val[2];

	this->label = label;
}


// 返回颜色
Scalar* ClibrationData::getColor()
{
	return color;
}
