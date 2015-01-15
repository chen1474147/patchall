
#include "patch.h"

int patch::GetMinPatch2(Mat Dst, Mat Src1, Mat Src2)
{
	float dist1 = Distance(Dst, Src1);
	float dist2 = Distance(Dst, Src2);

	if (dist1 < dist2)
		return 1;
	else
		return 2;
}

void patch::RandomSearch(Mat Dst, Mat Src, Mat Mask, Mat &Off, int row, int col)
{
	Mat DstPatch = GetPatch(Dst, row, col);

	int attenuate = 0;

	while(true)
	{
		//random value between [-1;1]
		float divcol = rand()%2000/1000.0f - 1.0f;
		float divrow = rand()%2000/1000.0f - 1.0f;

		float veccol = RandomWindow*pow(RandomAttenuation, attenuate)*divcol;
		float vecrow = RandomWindow*pow(RandomAttenuation, attenuate)*divrow;

		float length = sqrt(veccol*veccol+vecrow*vecrow);
		if (length < 1)//if below than one pixel break the loop
			break;

		int nowcol = Off.at<Vec2f>(row, col)[1]+col+veccol;
		int nowrow = Off.at<Vec2f>(row, col)[0]+row+vecrow;
		if (nowcol >= 0
			&&nowcol <= Off.cols-1
			&&nowrow >=0
			&&nowrow <= Off.rows-1
			&&Mask.at<uchar>(nowrow, nowcol) == search)
		{
			Mat SrcPatch1 = GetPatch(Src, Off.at<Vec2f>(row, col)[0]+row, Off.at<Vec2f>(row, col)[1]+col);
			Mat SrcPatch2 = GetPatch(Src, nowrow, nowcol);

			//calculate Distance
			int location = GetMinPatch2(DstPatch, SrcPatch1, SrcPatch2);
			switch (location)
			{
			case 2:
				Off.at<Vec2f>(row,col)[1] = nowcol - col;
				Off.at<Vec2f>(row,col)[0] = nowrow - row;
				break;
			}
		}

		attenuate++;
	}
}

int patch::GetMinPatch1(Mat Dst, Mat Src1, Mat Src2, Mat Src3)
{
	float dist1 = Distance(Dst, Src1);
	float dist2 = Distance(Dst, Src2);
	float dist3 = Distance(Dst, Src3);

	if (dist1 < dist2)
	{
		if (dist1 < dist3)
			return 1;
		else
			return 3;
	}
	else if (dist2 < dist3)
		return 2;
	else
		return 3;
}

void patch::Propagation(Mat Dst, Mat Src, Mat Mask, Mat &Off, int row, int col, int odd)
{
	Mat DstPatch = GetPatch(Dst, row, col);

	if (odd%2 == 0)
	{
		Mat SrcPatch = GetPatch(Src, row+Off.at<Vec2f>(row, col)[0], col+Off.at<Vec2f>(row, col)[1]);
		Mat LSrcPatch = GetPatch(Src, row+Off.at<Vec2f>(row, col-1)[0], col-1+Off.at<Vec2f>(row, col-1)[1]);
		Mat USrcPatch = GetPatch(Src, row-1+Off.at<Vec2f>(row-1, col)[0], col+Off.at<Vec2f>(row-1, col)[1]);

		int location = GetMinPatch1(DstPatch, SrcPatch, LSrcPatch, USrcPatch);
		switch(location)
		{
		case 2:
			Off.at<Vec2f>(row, col)[0] = Off.at<Vec2f>(row, col-1)[0];
			Off.at<Vec2f>(row, col)[1] = Off.at<Vec2f>(row, col-1)[1]-1;
			break;
		case 3:
			Off.at<Vec2f>(row, col)[0] = Off.at<Vec2f>(row-1, col)[0]-1;
			Off.at<Vec2f>(row, col)[1] = Off.at<Vec2f>(row-1, col)[1];
			break;
		}
	}
	else
	{
		Mat SrcPatch = GetPatch(Src, row+Off.at<Vec2f>(row, col)[0], col+Off.at<Vec2f>(row, col)[1]);
		Mat RSrcPatch = GetPatch(Src, row+Off.at<Vec2f>(row, col+1)[0], col+1+Off.at<Vec2f>(row, col+1)[1]);
		Mat DSrcPatch = GetPatch(Src, row+1+Off.at<Vec2f>(row+1, col)[0], col+Off.at<Vec2f>(row+1, col)[1]);

		int location = GetMinPatch1(DstPatch, SrcPatch, RSrcPatch, DSrcPatch);
		switch(location)
		{
		case 2:
			Off.at<Vec2f>(row, col)[0] = Off.at<Vec2f>(row, col+1)[0];
			Off.at<Vec2f>(row, col)[1] = Off.at<Vec2f>(row, col+1)[1]+1;
			break;
		case 3:
			Off.at<Vec2f>(row, col)[0] = Off.at<Vec2f>(row+1, col)[0]+1;
			Off.at<Vec2f>(row, col)[1] = Off.at<Vec2f>(row+1, col)[1];
			break;
		}
	}

}
