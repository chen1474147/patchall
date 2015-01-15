
#ifndef PATCH_H_
#define PATCH_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <time.h>

using namespace cv;
using namespace std;

//¿¼ÂÇ3ÖÖÇé¿ö£¬back£¬ search£¬ hole
#define back 0
#define search 100
#define hole 255

class patch
{
public:
	patch(Mat Src, Mat Mask, int ps, int pyr, int ANN, string result);

	void InitMask(Mat Mask, Mat &Mask2);
	void InitOff(Mat Mask, Mat &Off);

	float Distance(Mat Dst, Mat Src); 
	Mat GetPatch(Mat Src, int row, int col);

	float GetDelta(float *sim, int length);
	Vec3b GetMeanshift(float *w, Vec3b *c, int now_length, int odd);

	Vec3b GeneratePixel(Mat Dst, Mat Src, Mat Mask, Mat Off, int row, int col, int odd);
	void GenerateImage(Mat &Dst, Mat Src, Mat Mask, Mat Off, int odd);

	Vec3b GeneratePixel2(Mat Dst, Mat Src, Mat Mask, Mat Off, int row, int col);
	void GenerateImage2(Mat &Dst, Mat Src, Mat Mask, Mat Off);

	int GetMinPatch2(Mat Dst, Mat Src1, Mat Src2);
	void RandomSearch(Mat Dst, Mat Src, Mat Mask, Mat &Off, int row, int col);

	int GetMinPatch1(Mat Dst, Mat Src1, Mat Src2, Mat Src3);
	void Propagation(Mat Dst, Mat Src, Mat Mask, Mat &Off, int row, int col, int odd);

	void PyrDownMask(Mat InM, Mat &OutM);
	void PyrUpOff(Mat InO, Mat InM, Mat &OutO);

	Mat Run();

	~patch(void);

public:
	Mat DstImg;
	Mat SrcImg;

	Mat MaskImg;
	Mat OffsetImg;

	Mat show;

	int PatchSize;
	int RandomWindow;
	int MinWindow;

	int PryIteration;
	int ANNIteration;

	float RandomAttenuation;

	string filename;
};

#endif
