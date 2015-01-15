
//�������Ŀ������չlayer
#ifndef ALLLAYER_H_
#define ALLLAYER_H_

#include "../core.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

#include <iostream>
using namespace std;

static int addone = 1;
static int addpixels = 50;

Mat RunImplement2(Mat src, Mat mask)
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

bool TestBorder(Mat mask)
{
	for (int i = 0; i< mask.rows; i++)
	{
		if (mask.at<Vec3b>(i, 0+addone) != Vec3b(0, 0, 0)
			||mask.at<Vec3b>(i, mask.cols-1-addone) != Vec3b(0, 0, 0))
		{
			return true;
		}
	}

	return false;
}

void CopyBorder(Mat &src, Mat &mask)
{
	for (int i = 0; i< mask.rows; i++)
	{
		if (mask.at<Vec3b>(i, addpixels+addone) != Vec3b(0, 0, 0))
		{
			for (int j = addpixels+addone; j< 2*addpixels && mask.at<Vec3b>(i, j) != Vec3b(0, 0, 0); j++)
			{
				int newj = addpixels+addone-1 - (j - addpixels -addone);

				mask.at<Vec3b>(i, newj) = mask.at<Vec3b>(i, j);
				src.at<Vec3b>(i, newj) = src.at<Vec3b>(i, j);
			}

		}
		if (mask.at<Vec3b>(i, mask.cols-addpixels-1-addone) != Vec3b(0, 0, 0))
		{
			for (int j = mask.cols - addpixels - 1 - addone; j> mask.cols - 2*addpixels-1 && mask.at<Vec3b>(i, j) != Vec3b(0, 0, 0); j--)
			{
				int newj = mask.cols - addpixels -addone+ mask.cols - addpixels -1 -addone -j;

				mask.at<Vec3b>(i, newj) = mask.at<Vec3b>(i, j);
				src.at<Vec3b>(i, newj) = src.at<Vec3b>(i, j);
			}
		}
	}
}

//���Բ�����ͼ
bool TestBorder2(Mat mask)
{
	for (int i = 0; i< mask.rows; i++)
	{
		if (mask.at<Vec3b>(i, 0+addone+addpixels) != Vec3b(0, 0, 0)
			||mask.at<Vec3b>(i, mask.cols-1-addone-addpixels) != Vec3b(0, 0, 0))
		{
			return true;
		}
	}

	return false;
}

void CopyBorder2(Mat &src, Mat &mask)
{
	for (int i = 0; i< mask.rows; i++)
	{
		if (mask.at<Vec3b>(i, addpixels+addone) != Vec3b(0, 0, 0))
		{
			for (int j = addpixels+addone; j< 2*addpixels && mask.at<Vec3b>(i, j) != Vec3b(0, 0, 0); j++)
			{
				int newj = addpixels+addone-1 - (j - addpixels -addone);

				mask.at<Vec3b>(i, newj) = mask.at<Vec3b>(i, j);
				src.at<Vec3b>(i, newj) = src.at<Vec3b>(i, j);
			}

		}
		if (mask.at<Vec3b>(i, mask.cols-addpixels-1-addone) != Vec3b(0, 0, 0))
		{
			for (int j = mask.cols - addpixels - 1 - addone; j> mask.cols - 2*addpixels-1 && mask.at<Vec3b>(i, j) != Vec3b(0, 0, 0); j--)
			{
				int newj = mask.cols - addpixels -addone+ mask.cols - addpixels -1 -addone -j;

				mask.at<Vec3b>(i, newj) = mask.at<Vec3b>(i, j);
				src.at<Vec3b>(i, newj) = src.at<Vec3b>(i, j);
			}
		}
	}
}

void alllayer(string n, int le, int se)
{
	//��ȫ
	int allsegnum = 30;

	//����
	int endsegnum = se;

	//layer
	int allayernum = le;


	//��¼layer�����ļ���seg
	int **layerobjrecord = new int *[allayernum+1];
	for (int i = 0; i< allayernum+1; i++)
		layerobjrecord[i] = new int [allsegnum];

	//��¼layer���seg
	int *layerobjcount = new int [allayernum+1];
	for (int i = 0; i< allayernum+1; i++)
		layerobjcount[i] = 0;

	//��¼��ɫ
	Vec3b *color = new Vec3b[endsegnum+1];
	for (int i = 0; i< endsegnum+1; i++)
		color[i] = Vec3b(0, 0, 0);

	string inputlayerfoldername = "input/";
	string inputsrcfildername = "patch/";
	string outputfolername = "enlarge/";

	string name = n;

	string realmaskname = "_realmask_";
	string texturename = "_texture_";
	string maskname = "_segment_";
	string layername = "_layer_";

	string pngname = ".png";
	string jpgname = ".jpg";

	for (int layernum = 0; layernum<= allayernum; layernum++)
	{
		stringstream ll;
		ll<<layernum;

		string layer_name = inputlayerfoldername+name+layername+ll.str()+pngname;

		Mat Layer = imread(layer_name);
		if (Layer.empty())
			break;
		//imshow("l", Layer);
		//waitKey();

		for (int segnum = 0; segnum< allsegnum; segnum++)
		{
			stringstream ss;
			ss<<segnum;

			string srcname = inputlayerfoldername+name+texturename+ss.str()+pngname;
			string maskinfoname = inputlayerfoldername+name+maskname+ss.str()+pngname;

			Mat Src = imread(srcname);
			Mat Maskinfo = imread(maskinfoname);

			if (!Src.empty()
					&&!Maskinfo.empty())
			{
				//imshow("s", Src);
				//imshow("m", Maskinfo);
				//waitKey();

				int pixelcount = 0;
				int allpixelcount = 0;

				for (int i = 0; i < Src.rows; i++)
					for (int j = 0; j < Src.cols; j++)
					{
						if (Src.at<Vec3b>(i, j) != Vec3b(0, 0, 0))
						{
							allpixelcount++;

							if (Layer.at<Vec3b>(i, j) != Vec3b(0, 0,0))
								pixelcount++;
						}
					}

				double ratio = pixelcount*1.0/allpixelcount;
				if (ratio > 0.9)
				{
					layerobjrecord[layernum][layerobjcount[layernum]] = segnum;
					layerobjcount[layernum]++;
				}
			}
		}
	}

	//��ʾ���
	for (int i = 0; i <= allayernum; i++)
	{
		for (int j = 0; j < layerobjcount[i]; j++)
		{
			cout<<layerobjrecord[i][j]<<"\t";
		}

		cout<<"\n";
	}

	{
	//��ɫ
	Mat AllMask = imread(inputlayerfoldername+name+pngname);

	for (int segnum = 0; segnum<= endsegnum; segnum++)
	{
		if(AllMask.empty())
			break;

		stringstream ss;
		ss<<segnum;

		string maskinfoname = inputlayerfoldername+name+realmaskname+ss.str()+pngname;

		Mat Maskinfo = imread(maskinfoname);

		if (!Maskinfo.empty())
		{
			//imshow("m", Maskinfo);
			//waitKey();

			int signal = 0;

			for (int i = 0; i < Maskinfo.rows; i++)
			{
				for (int j = 0; j < Maskinfo.cols; j++)
				{
					if (Maskinfo.at<Vec3b>(i, j) != Vec3b(0, 0, 0))
					{
						color[segnum] = AllMask.at<Vec3b>(i, j);

						signal = 1;

						break;
					}
				}

				if (signal)
					break;
			}
		}
	}

	//for (int segnum = 0; segnum< endsegnum; segnum++)
		//assert(color[segnum] != Vec3b(0, 0, 0));
	}

	//����
	for (int segnum = 0; segnum < allsegnum; segnum++)
	{
		stringstream ss;
		ss << segnum;

		string srcname = inputsrcfildername + name + texturename + ss.str() + pngname;
		string maskinfoname = inputsrcfildername + name + maskname + ss.str() + pngname;

		string srcresultname = outputfolername + name + texturename + ss.str()
				+ pngname;
		string maskresultname = outputfolername + name + maskname + ss.str()
				+ pngname;

		Mat Src = imread(srcname);
		Mat Maskinfo = imread(maskinfoname);

		if (!Src.empty() && !Maskinfo.empty()) {

			int oldrow = Src.rows;
			int ollcol = Src.cols;

			Mat newSrc = Mat(oldrow, ollcol + 2 * addpixels, CV_8UC3,
					Scalar::all(0));
			Mat newMask = Mat(oldrow, ollcol + 2 * addpixels, CV_8UC3,
					Scalar::all(0));

			for (int i = 0; i < oldrow; i++) {
				for (int j = 0; j < ollcol; j++) {
					newSrc.at<Vec3b>(i, j + addpixels) = Src.at<Vec3b>(i, j);
					newMask.at<Vec3b>(i, j + addpixels) = Maskinfo.at<Vec3b>(i,
							j);
				}
			}

			if (segnum != endsegnum+1)
			{
				//obj
				if (TestBorder2(Maskinfo))
					CopyBorder(newSrc, newMask);

				imwrite(srcresultname, newSrc);
				imwrite(maskresultname, newMask);
			}
			else
			{
				//����
				Mat newMask1 = Mat(oldrow, ollcol + 2 * addpixels, CV_8UC1,
						Scalar::all(search));

				/*
				for (int i = 0; i < oldrow; i++) {
					for (int j = 0; j < ollcol + 2 * addpixels; j++) {
						if (newMask.at<Vec3b>(i, j) != Vec3b(0, 0, 0))
							newMask1.at<uchar>(i, j) = search;
						else
							newMask1.at<uchar>(i, j) = hole;
					}
				}
				*/

				for (int i = 0; i < oldrow; i++) {
					for (int j = 0; j < addpixels; j++) {
						newMask1.at<uchar>(i, j) = hole;
					}
				}

				for (int i = 0; i < oldrow; i++) {
					for (int j = ollcol; j < ollcol+addpixels; j++) {
						newMask1.at<uchar>(i, j) = hole;
					}
				}


				Mat result_src = RunImplement2(newSrc, newMask1);
				Mat result_mask = Mat(result_src.size(), CV_8UC1, Scalar::all(255));

				imwrite(srcresultname, result_src);
				imwrite(maskresultname, result_mask);
			}
		}
	}

	/*
	//���±���
	{
		Mat oldSrc = imread((inputlayerfoldername+name+jpgname));
		Mat oldMask = imread(inputlayerfoldername+name+pngname);

		Mat newSrc, newMask;

		for (int seg = allsegnum; seg >= 0; seg--) {
			stringstream ss;
			ss << seg;

			string srcname = outputfolername + name + texturename + ss.str()
					+ pngname;
			string maskinfoname = outputfolername + name + maskname + ss.str()
					+ pngname;

			Mat Src = imread(srcname);
			Mat Maskinfo = imread(maskinfoname);

			Vec3b r_color = Vec3b((uchar)rand(), (uchar)rand(), (uchar)rand());

			if (!Src.empty() && !Maskinfo.empty()) {
				if (seg == endsegnum) {
					newSrc = Src.clone();
					newMask = Mat(newSrc.size(), CV_8UC3, Scalar::all(0));
				} else {
					for (int i = 0; i < newSrc.rows; i++) {
						for (int j = 0; j < newSrc.cols; j++) {
							if (Maskinfo.at<Vec3b>(i, j) != Vec3b(0, 0, 0)) {
								newMask.at<Vec3b>(i, j) = r_color;
								newSrc.at<Vec3b>(i, j) = Src.at<Vec3b>(i, j);
							}
						}
					}
				}
			}
		}

		string srcresultname = outputfolername + name + jpgname;
		string maskresultname = outputfolername + name + pngname;

		imwrite(srcresultname, newSrc);
		imwrite(maskresultname, newMask);
	}
	*/

	//���±���2
	{
		Mat oldSrc = imread((inputlayerfoldername+name+jpgname));
		//imshow("sdas", oldSrc);
		Mat oldMask = imread(inputlayerfoldername+name+pngname);
		//imshow("saddd", oldMask);
		//waitKey();

		int oldrow = oldSrc.rows;
		int ollcol = oldSrc.cols;

		Mat newSrc = Mat(oldrow, ollcol + 2 * addpixels, CV_8UC3,
				Scalar::all(0));
		Mat newMask = Mat(oldrow, ollcol + 2 * addpixels, CV_8UC3,
				Scalar::all(0));

		for (int i = 0; i < oldrow; i++)
		{
			for (int j = 0; j < ollcol; j++)
			{
				newSrc.at<Vec3b>(i, j + addpixels) = oldSrc.at < Vec3b > (i, j);
				newMask.at<Vec3b>(i, j + addpixels) = oldMask.at < Vec3b > (i, j);
			}
		}


		for (int seg = allsegnum; seg >= 0; seg--)
		{
			stringstream ss;
			ss << seg;

			string srcname = outputfolername + name + texturename + ss.str()
					+ pngname;
			string maskinfoname = outputfolername + name + maskname + ss.str()
					+ pngname;

			Mat Src = imread(srcname);
			Mat Maskinfo = imread(maskinfoname);

			if (!Src.empty() && !Maskinfo.empty())
			{
				if (seg == endsegnum)
				{
					for (int i = 0; i< newSrc.rows; i++)
						for (int j = 0; j< addpixels+addone; j++)
							if (Maskinfo.at<Vec3b>(i, j) != Vec3b(0, 0, 0))
							{
								newSrc.at<Vec3b>(i, j) = Src.at<Vec3b>(i, j);
								newMask.at<Vec3b>(i, j) = color[seg];
							}

					for (int i = 0; i< newSrc.rows; i++)
						for (int j = newSrc.cols-1; j> newSrc.cols-1-addone-addpixels; j--)
							if (Maskinfo.at<Vec3b>(i, j) != Vec3b(0, 0, 0))
							{
								newSrc.at<Vec3b>(i, j) = Src.at<Vec3b>(i, j);
								newMask.at<Vec3b>(i, j) = color[seg];
							}
				}
				else
				{
					//obj
					if (TestBorder2(Maskinfo))
					{
						for (int i = 0; i< newSrc.rows; i++)
							for (int j = 0; j< addpixels+addone; j++)
								if (Maskinfo.at<Vec3b>(i, j) != Vec3b(0, 0, 0))
								{
									newSrc.at<Vec3b>(i, j) = Src.at<Vec3b>(i, j);
									newMask.at<Vec3b>(i, j) = color[seg];
								}

						for (int i = 0; i< newSrc.rows; i++)
							for (int j = newSrc.cols-1; j> newSrc.cols-1-addone-addpixels; j--)
								if (Maskinfo.at<Vec3b>(i, j) != Vec3b(0, 0, 0))
								{
									newSrc.at<Vec3b>(i, j) = Src.at<Vec3b>(i, j);
									newMask.at<Vec3b>(i, j) = color[seg];
								}
					}
				}
			}
		}

		string srcresultname = outputfolername + name + jpgname;
		string maskresultname = outputfolername + name + pngname;

		imwrite(srcresultname, newSrc);
		imwrite(maskresultname, newMask);
	}

	//����layer
	for (int ll = 0; ll<= allayernum; ll++)
	{
		stringstream llss;
		llss<<ll;

		string layer_name = inputlayerfoldername+name+layername+llss.str()+pngname;

		Mat Layer = imread(layer_name);
		//imshow("ol", Layer);
		//waitKey();

		Mat newLayer = Mat(Layer.rows, Layer.cols+2*addpixels, CV_8UC3, Scalar::all(0));
		for (int i = 0; i < Layer.rows; i++)
		{
			for (int j = 0; j < Layer.cols; j++)
			{
				newLayer.at<Vec3b>(i, j + addpixels) = Layer.at<Vec3b>(i, j);
			}
		}

		for (int i = 0; i < newLayer.rows; i++)
		{
			for (int j = 0; j < newLayer.cols; j++)
			{
				if (newLayer.at<Vec3b>(i, j ) != Vec3b(0, 0, 0))
					newLayer.at<Vec3b>(i, j ) = Vec3b(255, 255, 255);

			}
		}

		//imshow("nl1", newLayer);
		//waitKey();

		for (int j = 0; j < layerobjcount[ll] ; j++)
		{
			int seg = layerobjrecord[ll][j];
			stringstream ss;
			ss << seg;

			string srcname = outputfolername + name + texturename + ss.str()
					+ pngname;
			string maskinfoname = outputfolername + name + maskname + ss.str()
					+ pngname;

			Mat Src = imread(srcname);
			Mat Maskinfo = imread(maskinfoname);


			if (!Src.empty() && !Maskinfo.empty())
			{
				//imshow("obj", Src);
					//	waitKey();

					for (int i = 0; i < Maskinfo.rows; i++)
					{
						for (int jj = 0; jj < addpixels+addone; jj++)
						{
							if (Maskinfo.at<Vec3b>(i, jj) != Vec3b(0, 0, 0))
							{
								newLayer.at<Vec3b>(i, jj) = Vec3b(255, 255, 255);
							}
						}
					}

					for (int i = 0; i < Maskinfo.rows; i++)
					{
						for (int jj = Maskinfo.cols - addpixels -1-addone; jj < Maskinfo.cols; jj++)
						{
							if (Maskinfo.at<Vec3b>(i, jj) != Vec3b(0, 0, 0))
							{
								newLayer.at<Vec3b>(i, jj) = Vec3b(255, 255, 255);
							}
						}
					}
			}
		}

		if (ll == allayernum) {
			for (int j = 0; j < ll; j++) {
				stringstream sslll;
				sslll << j;

				string tmp_name = outputfolername+name+layername+sslll.str()+pngname;

				Mat temp = imread(tmp_name);

				for (int i = 0; i < newLayer.rows; i++)
				{
					for (int j = 0; j < newLayer.cols; j++)
					{
						if (temp.at<Vec3b>(i, j ) != Vec3b(0, 0, 0))
							newLayer.at<Vec3b>(i, j ) = Vec3b(0, 0, 0);

					}
				}
			}
		}



		string layerresultname = outputfolername +name+layername+llss.str()+pngname;

		imwrite(layerresultname, newLayer);
	}
}

#endif
