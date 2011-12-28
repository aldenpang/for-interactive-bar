#ifndef _PS_VISUAL_ALGORITHM_H_
#define _PS_VISUAL_ALGORITHM_H_
#include "cv.h"
#include "highgui.h"

class PsVisualAlgorithm
{
public:
	~PsVisualAlgorithm(){};

	static PsVisualAlgorithm GetSingleton()
	{
		static PsVisualAlgorithm singleton;
		return singleton;
	}

	IplImage* Canny(IplImage* _input);

protected:
private:
	PsVisualAlgorithm(){};
};


#endif