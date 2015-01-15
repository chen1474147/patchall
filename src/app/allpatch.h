
//�������Ŀ���ǽ��в�ȫ

#ifndef ALLPATCH_H_
#define ALLPATCH_H_

#include "../patch/patch.h"
#include "../core.h"

Mat RunImplement(Mat src, Mat mask)
{
	assert(src.channels() == 3
		&&mask.channels() == 1);

	//search��search����Ϊ0�� ����Ϊ255
	//hole����������Ϊ0�� holeΪ255

	Mat mask_s = Mat(mask.size(), CV_8UC1, Scalar::all(255));
	Mat mask_h = Mat(mask.size(), CV_8UC1, Scalar::all(0));

	for (int i = 0; i< mask.rows; i++)
	{
		for (int j = 0; j< mask.cols; j++)
		{
			if (mask.at<uchar>(i, j) == search)
				mask_s.at<uchar>(i, j) = 0;

			if (mask.at<uchar>(i, j) == hole)
				mask_h.at<uchar>(i, j) = 255;
		}
	}

	return run(src, mask_h, mask_s);
}

int GetMaxContour(vector<vector<Point> > contours)
{
	assert(contours.size() > 0);

	int i = contours.size();

	if (i == 1)
	{
		return 0;
	}
	else
	{
		int result = 0;

		for (int j = 1; j < i; j++)
		{
			if (contours[j].size() > contours[result].size())
				result = j;
		}

		return result;
	}
}

//����Ƿ���ڰ�ɫ���أ� �Ƿ���true
bool TestWhite2(Mat mask)
{
	assert(mask.type() == CV_8UC3);
	
	for (int i = 0; i < mask.rows; i++)
		for (int j = 0; j < mask.cols; j++)
			if (mask.at<Vec3b>(i, j) == Vec3b(255, 255, 255))
				return true;

	return false;
}


void allpatch(string n, int e)
{
	//��ȫ�� ��һ���ܴ��ֵ
	int allsegnum = 30;
	//������ ���һ��
	int endsegnum = e;

	string inputfoldername = "input/";
	string outputfldername = "patch/";

	string name = n;

	string texturename = "_texture_";//���search����
	string holename = "_segment_";//��?ȫ����
	string searchname = "_realmask_";//���search����

	string pngname = ".png";
	string jpgname = ".jpg";

	for (int segnum = 0; segnum< allsegnum; segnum++)
	{
		stringstream ss;//���
		ss<<segnum;

		string srcname = inputfoldername+name+texturename+ss.str()+pngname;
		string holeinfoname = inputfoldername+name+holename+ss.str()+pngname;
		string searchinfoname = inputfoldername+name+searchname+ss.str()+pngname;

		string srcresultname = outputfldername+name+texturename+ss.str()+pngname;
		string maskresultname = outputfldername+name+holename+ss.str()+pngname;

		Mat Src = imread(srcname);
		Mat Holeinfo = imread(holeinfoname);
		Mat Searchinfo = imread(searchinfoname);
		Mat Searcharea = Mat(Src.size(), CV_8UC3, Scalar::all(0));
		
		Mat result_src, result_mask;

		if (!Src.empty()
				&&!Holeinfo.empty()
				&&!Searchinfo.empty())
		{
			//holeΪ��ɫ���ɫ�� ����Ҫ��
			if (!TestWhite2(Holeinfo))
			{
				result_src = Src;
				result_mask = Searchinfo;

				imwrite(srcresultname, result_src);
				imwrite(maskresultname, result_mask);
				
				continue;
			}
			
			//�������
			Mat Allmask = Mat(Src.size(), CV_8UC1, Scalar::all(0));
			for (int i = 0; i < Src.rows; i++)
				for (int j = 0; j < Src.cols; j++)
					if (Searchinfo.at<Vec3b>(i, j) != Vec3b(0, 0, 0)
							||Holeinfo.at<Vec3b>(i, j) != Vec3b(0, 0, 0))
					{
						Allmask.at<uchar>(i, j) = 255;
					}
			
			vector<vector<Point> > contours;
			vector<Vec4i> hierarchy;

			findContours(Allmask.clone(), contours, hierarchy, CV_RETR_TREE,
					CV_CHAIN_APPROX_NONE, Point(0, 0));

			Mat Allmask2 = Mat(Src.size(), CV_8UC1, Scalar::all(0));

			drawContours(Allmask2, contours, GetMaxContour(contours),
					Scalar::all(255), CV_FILLED, 8, hierarchy, 0);
			
			//��������ʹ�߽�ȱʧһ�����أ��ڴ˲���
			for(int i = 0; i<Src.rows; i++)
			{
				if (Allmask.at<uchar>(i, 0) == 255)
					Allmask2.at<uchar>(i, 0) = 255;
				if (Allmask.at<uchar>(i, Src.cols-1) == 255)
					Allmask2.at<uchar>(i, Src.cols-1) = 255;
			}

			for(int j = 0; j<Src.cols; j++)
			{
				if (Allmask.at<uchar>(0, j) == 255)
					Allmask2.at<uchar>(0, j) = 255;
				if (Allmask.at<uchar>(Src.rows-1, j) == 255)
					Allmask2.at<uchar>(Src.rows-1, j) = 255;
			}

			//Allmask2����������� Searchinfo���search���� ����search���� ������hole����
			Mat Programmask = Mat(Src.size(), CV_8UC1, Scalar::all(back));

			for (int i = 0; i < Src.rows; i++)
				for (int j = 0; j < Src.cols; j++)
					if (
							//��һ�����������⣬������û��Ȧ�Ĳ�ȫ
							//�ڶ�����������Ȧ�Ĳ�ȫ
							//Allmask2.at<uchar>(i, j) == 255
							 Holeinfo.at<Vec3b>(i, j) != Vec3b(0, 0, 0)
							)
					{
						Programmask.at<uchar>(i, j) = hole;
					}

			for (int i = 0; i < Src.rows; i++)
				for (int j = 0; j < Src.cols; j++)
					if (Searchinfo.at<Vec3b>(i, j) != Vec3b(0, 0, 0))
					{
						Programmask.at<uchar>(i, j) = search;
						Searcharea.at<Vec3b>(i, j) = Src.at<Vec3b>(i, j);
					}

			//imshow("src", Src);
			//imshow("search", Searcharea);
			//imshow("mask", Programmask);
			//waitKey(33);

			int ps = 5;
			int py = 3;
			int an = 3;

			if (segnum == 10000)
				py = 0;
				//continue;

			//erode
			Mat Programmask2 = Programmask.clone();
			for (int i = 0; i < Programmask.rows; i++)
			{
				for (int j = 0; j < Programmask.cols; j++)
				{
					if (Programmask.at<uchar>(i, j) == hole)
					{
						//15����̫���ˣ�����ɲ����ͼƬ��һ����
						int m_s = 7;
						if (segnum == e)
							m_s = 9;

						int e_s = 3 * ps > m_s ? m_s : 3 * ps;

						int r_b = i - e_s / 2 > 0 ? i - e_s / 2 : 0;
						int r_e =
								i + e_s / 2 < Programmask.rows ?
										i + e_s / 2 : Programmask.rows - 1;

						int c_b = j - e_s / 2 > 0 ? j - e_s / 2 : 0;
						int c_e =
								j + e_s / 2 < Programmask.cols - 1 ?
										j + e_s / 2 : Programmask.cols - 1;

						for (int k = r_b; k < r_e; k++)
							for (int p = c_b; p < c_e; p++)
								Programmask2.at<uchar>(k, p) = hole;

					}
				}
			}

			Searcharea.setTo(0);
			for (int i = 0; i < Src.rows; i++)
				for (int j = 0; j < Src.cols; j++)
					if (Programmask2.at<uchar>(i, j) == search)
						Searcharea.at<Vec3b>(i, j) = Src.at<Vec3b>(i, j);

			//imshow("src", Src);
			//imshow("search", Searcharea);
			//imshow("mask", Programmask2);
			//waitKey(33);

			if (segnum != endsegnum)
			{
				patch *test = new patch(Src, Programmask2, ps, py, an,
						"output/result.png");

				result_src = test->Run();
				result_mask = Mat(Src.size(), CV_8UC1, Scalar::all(255));
			}
			else
			{
				result_src = RunImplement(Src, Programmask2);
				result_mask = Mat(Src.size(), CV_8UC1, Scalar::all(255));
			}


			//���ಹ������
			for (int i = 0; i < Programmask.rows; i++) {
				for (int j = 0; j < Programmask.cols; j++) {
					{
						if (Programmask.at<uchar>(i, j) == back)
						{
							result_src.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
							result_mask.at<uchar>(i, j) = 0;
						}
					}
				}
			}

			imwrite(srcresultname, result_src);
			imwrite(maskresultname, result_mask);
		}
	}
}

#endif
