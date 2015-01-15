
//�������Ŀ����չʾҪ����ͼƬ

#ifndef ALLSHOW_H_
#define ALLSHOW_H_

#include <iostream>
using namespace std;

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

//����Ƿ���ڰ�ɫ���أ� �Ƿ���true
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
	//��ȫ�� ��һ���ܴ��ֵ
	int allsegnum = 30;

	string inputfoldername = "input/";
	string outputfldername = "show/";

	string name = n;

	string texturename = "_texture_";//����src����
	string holename = "_segment_";//����ȫ����
	string searchname = "_realmask_";//����search����

	string pngname = ".png";
	string jpgname = ".jpg";

	Mat Background = imread("Background.png");

	for (int segnum = 0; segnum< allsegnum; segnum++)
	{
		stringstream ss;//���
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
				//����ȫ�ǰ�ɫ��
				result_src = Mat(Src.size(), CV_8UC3, Scalar::all(255));

				//ǰ��
				for (int i = 0; i < Src.rows; i++)
					for (int j = 0; j < Src.cols; j++)
						if (Searchinfo.at<Vec3b>(i, j) != Vec3b(0, 0, 0))
							result_src.at<Vec3b>(i, j) = Src.at<Vec3b>(i, j);

				imwrite(srcresultname, result_src);

				continue;
			}

			//����ȫ�ǰ�ɫ��
			result_src = Mat(Src.size(), CV_8UC3, Scalar::all(255));

			//ǰ��
			for (int i = 0; i< Src.rows; i++)
				for (int j = 0; j< Src.cols; j++)
					if (Searchinfo.at<Vec3b>(i, j) != Vec3b(0, 0, 0))
						result_src.at<Vec3b>(i, j) = Src.at<Vec3b>(i, j);

			//��ȫ����
			for (int i = 0; i< Src.rows; i++)
				for (int j = 0; j< Src.cols; j++)
					if (Holeinfo.at<Vec3b>(i, j) != Vec3b(0, 0, 0))
						result_src.at<Vec3b>(i, j) = Background.at<Vec3b>(i, j);

			imwrite(srcresultname, result_src);
		}
	}
}

#endif

