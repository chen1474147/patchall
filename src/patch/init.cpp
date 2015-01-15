
#include "patch.h"

//该patch是否包含search
bool TestPatch(Mat Patch)
{
	for (int i = 0; i< Patch.rows; i++)
		for (int j = 0; j< Patch.cols; j++)
			if (Patch.at<uchar>(i, j) == back)
				return false;

	return true;
}

void patch::InitMask(Mat Mask, Mat &Mask2)
{
	//将边界处的search设为back
	Mask2 = Mask.clone();
	for (int i = 0; i< Mask.rows; i++)
	{
		for (int j = 0; j< Mask.cols; j++)
		{
			if (Mask.at<uchar>(i, j) == search)
			{
				Mat MaskPatch = GetPatch(Mask, i, j);
				if (MaskPatch.rows*MaskPatch.cols != PatchSize*PatchSize)
					Mask2.at<uchar>(i, j) = back;

			}
		}
	}
}

void patch::InitOff(Mat Mask, Mat &Off)
{
	//为方便起见，将所有的都附上，要求不能赋值到非搜索区域
	Off = Mat(Mask.size(), CV_32FC2, Scalar::all(0));

	for (int i=0; i< Mask.rows; i++)
	{
		for (int j=0; j< Mask.cols; j++)
		{
			//不考虑search区域
			if (Mask.at<uchar>(i, j) == search)
			{
				Off.at<Vec2f>(i, j)[0] = 0;
				Off.at<Vec2f>(i, j)[1] = 0;
			}
			else
			{
				//现在是全部赋值到search里
				int r_col = rand()%Mask.cols;
				int r_row = rand()%Mask.rows;

				r_col = r_col+j < Mask.cols? r_col:r_col-Mask.cols;
				r_row = r_row+i < Mask.rows? r_row:r_row-Mask.rows;

				while(Mask.at<uchar>(r_row+i,r_col+j) != search)
				{
					r_col = rand()%Mask.cols;
					r_row = rand()%Mask.rows;

					r_col = r_col+j < Mask.cols? r_col:r_col-Mask.cols;
					r_row = r_row+i < Mask.rows? r_row:r_row-Mask.rows;
				}

				Off.at<Vec2f>(i, j)[0] = r_row;
				Off.at<Vec2f>(i, j)[1] = r_col;
			}
		}
	}
}
