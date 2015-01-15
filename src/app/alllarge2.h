
//�������Ŀ����������չ1������

#ifndef ALLLARGE2_H_
#define ALLLARGE2_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

#include <iostream>
using namespace std;

int getsegnum(Mat mask, int i, int j, int oppo)
{
	//oppo��?��
	int count = 0;
	for (int k = j; k< mask.rows && k>= 0; k = k+oppo)
	{
		if (mask.at<Vec3b>(i, k) != Vec3b (0, 0, 0))
			count++;
	}
	return count;
}

void delete2(Mat &Src, Mat &Mask)
{
	for (int i = 0; i< Mask.rows; i++)
	{
		for (int j = 0; j< Mask.cols-1; j++)
		{
			if (Mask.at<Vec3b>(i, j) != Vec3b(0, 0, 0) && Mask.at<Vec3b>(i, j+1) == Vec3b(0, 0, 0))
			{
				//int segnum = getsegnum(Mask, i, j, -1);

				//if (segnum > 2)
				{
				Src.at<Vec3b>(i, j) = Src.at<Vec3b>(i, j+1);
				Mask.at<Vec3b>(i, j) = Mask.at<Vec3b>(i, j+1);
				}
			}
		}
	}

	for (int i = 0; i< Mask.rows; i++)
	{
		for (int j = Mask.cols-1; j> 0; j--)
		{
			if (Mask.at<Vec3b>(i, j) != Vec3b(0, 0, 0) && Mask.at<Vec3b>(i, j-1) == Vec3b(0, 0, 0))
			{
				//int segnum = getsegnum(Mask, i, j, 1);

				//if (segnum > 2)
				{
				Src.at<Vec3b>(i, j) = Src.at<Vec3b>(i, j-1);
				Mask.at<Vec3b>(i, j) = Mask.at<Vec3b>(i, j-1);
				}
			}
		}
	}
}

void copy3(Mat &Src, Mat &Mask, int addpixels)
{
	for (int i = 0; i< Mask.rows; i++)
	{
		for (int j = 0; j< Mask.cols-1; j++)
		{
			if (Mask.at<Vec3b>(i, j) == Vec3b(0, 0, 0) && Mask.at<Vec3b>(i, j+1) != Vec3b(0, 0, 0))
			{
				for (int k = j+1; k<= j+addpixels && k<= Mask.cols-1 && Mask.at<Vec3b>(i, k) != Vec3b(0, 0, 0); k++)
				{
					int p = j-(k-j-1);
					if (p>=0 && Mask.at<Vec3b>(i, p) == Vec3b(0, 0, 0))
					{
						Src.at<Vec3b>(i, p) = Src.at<Vec3b>(i, k);
						Mask.at<Vec3b>(i, p) = Mask.at<Vec3b>(i, k);
					}
				}
			}
		}
	}

	for (int i = 0; i< Mask.rows; i++)
	{
		for (int j = Mask.cols-1; j> 0; j--)
		{
			if (Mask.at<Vec3b>(i, j) == Vec3b(0, 0, 0) && Mask.at<Vec3b>(i, j-1) != Vec3b(0, 0, 0))
			{
				for (int k = j-1; k>= j-addpixels && k>= 0 && Mask.at<Vec3b>(i, k) != Vec3b(0, 0, 0); k--)
				{
					int p = j+(j-1-k);
					if (p<= Mask.cols-1 && Mask.at<Vec3b>(i, p) == Vec3b(0, 0, 0))
					{
						Src.at<Vec3b>(i, p) = Src.at<Vec3b>(i, k);
						Mask.at<Vec3b>(i, p) = Mask.at<Vec3b>(i, k);
					}
				}
			}
		}
	}
}


void alllarge2(string n)
{
	int allseg = 30;

	for (int seg = 0; seg< allseg; seg++)
	{
		string input = "enlarge/";
		string output = "expand/";

		string name = n;

		string texturename = "_texture_";
		string maskname = "_segment_";

		string mtexturename = "_texture_";

		string pngname = ".png";
		string jpgname = ".jpg";

		stringstream ss;
		ss<<seg;

		string srcname = input+name+texturename+ss.str()+pngname;
		string maskinfoname = input+name+maskname+ss.str()+pngname;

		string srcresultname = output+name+texturename+ss.str()+pngname;
		string maskresultname = output+name+maskname+ss.str()+pngname;

		Mat Src = imread(srcname);
		Mat Maskinfo = imread(maskinfoname);

		Mat Src2 = Src.clone();
		Mat Maskinfo2 = Maskinfo.clone();

		if (!Src.empty() && !Maskinfo.empty())
		{
			//imshow("src", Src);
			//imshow("mask", Maskinfo);
			//waitKey(33);

			int deletep = 2;

			for(int i = 0; i< deletep; i++)
				delete2(Src, Maskinfo);

			copy3(Src, Maskinfo, deletep+2);

			//imshow("src", Src);
			//imshow("mask", Maskinfo);
			//waitKey(33);

			for (int i = 0; i< Src.rows; i++)
				for (int j = 0; j< Src.cols; j++)
					if (Maskinfo2.at<Vec3b>(i, j) != Vec3b(0, 0, 0)
							&& Maskinfo.at<Vec3b> (i, j) == Vec3b(0, 0, 0))
					{
						Src.at<Vec3b>(i, j) = Src2.at<Vec3b>(i, j);
						Maskinfo.at<Vec3b>(i, j) = Maskinfo2.at<Vec3b>(i, j);

					}

			//imshow("src", Src);
			//imshow("mask", Maskinfo);
			//waitKey(33);

			imwrite(srcresultname, Src);
			imwrite(maskresultname, Maskinfo);
		}
	}
}

#endif

