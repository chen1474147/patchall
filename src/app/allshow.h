
//本程序的目的是展示要补的图片

#ifndef ALLSHOW_H_
#define ALLSHOW_H_

#include <iostream>
using namespace std;

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

//检测是否存在白色像素， 是返回true
bool TestWhite(Mat mask)
{
	assert(mask.type() == CV_8UC3);

	for (int i = 0; i < mask.rows; i++)
		for (int j = 0; j < mask.cols; j++)
			if (mask.at<Vec3b>(i, j) == Vec3b(255, 255, 255))
				return true;

	return false;
}

void allshow(string n)
{
	//补全， 设一个很大的值
	int allsegnum = 30;

	string inputfoldername = "input/";
	string outputfldername = "show/";

	string name = n;

	string texturename = "_texture_";//代表src区域
	string holename = "_segment_";//代表补全区域
	string searchname = "_realmask_";//代表search区域

	string pngname = ".png";
	string jpgname = ".jpg";

	Mat Background = imread("Background.png");

	for (int segnum = 0; segnum< allsegnum; segnum++)
	{
		stringstream ss;//编号
		ss<<segnum;

		string srcname = inputfoldername+name+texturename+ss.str()+pngname;
		string holeinfoname = inputfoldername+name+holename+ss.str()+pngname;
		string searchinfoname = inputfoldername+name+searchname+ss.str()+pngname;

		string srcresultname = outputfldername+name+texturename+ss.str()+pngname;

		Mat Src = imread(srcname);
		Mat Holeinfo = imread(holeinfoname);
		Mat Searchinfo = imread(searchinfoname);

		Mat result_src;

		if (!Src.empty()
				&&!Holeinfo.empty()
				&&!Searchinfo.empty())
		{
			if (!TestWhite(Holeinfo))
			{
				//背景全是白色？
				result_src = Mat(Src.size(), CV_8UC3, Scalar::all(255));

				//前景
				for (int i = 0; i < Src.rows; i++)
					for (int j = 0; j < Src.cols; j++)
						if (Searchinfo.at<Vec3b>(i, j) != Vec3b(0, 0, 0))
							result_src.at<Vec3b>(i, j) = Src.at<Vec3b>(i, j);

				imwrite(srcresultname, result_src);

				continue;
			}

			//背景全是白色？
			result_src = Mat(Src.size(), CV_8UC3, Scalar::all(255));

			//前景
			for (int i = 0; i< Src.rows; i++)
				for (int j = 0; j< Src.cols; j++)
					if (Searchinfo.at<Vec3b>(i, j) != Vec3b(0, 0, 0))
						result_src.at<Vec3b>(i, j) = Src.at<Vec3b>(i, j);

			//补全区域
			for (int i = 0; i< Src.rows; i++)
				for (int j = 0; j< Src.cols; j++)
					if (Holeinfo.at<Vec3b>(i, j) != Vec3b(0, 0, 0))
						result_src.at<Vec3b>(i, j) = Background.at<Vec3b>(i, j);

			imwrite(srcresultname, result_src);
		}
	}
}

#endif

