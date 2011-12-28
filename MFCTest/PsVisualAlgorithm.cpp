#include "stdafx.h"
#include "PsVisualAlgorithm.h"


IplImage* PsVisualAlgorithm::Canny( IplImage* _input )
{
	// 要先申请和输入一模一样大的空间
	IplImage* output = cvCreateImage(cvGetSize(_input),IPL_DEPTH_8U,1);

	cvCanny(_input, output, 50, 150, 3);

	return output;
}