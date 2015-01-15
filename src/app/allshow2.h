
//本程序的目的是展示图片的遮挡关系

#ifndef ALLSHOW2_H_
#define ALLSHOW2_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

#include <iostream>
using namespace std;

//最小重叠
static int minpixelcount = 20;

void allshow2(string n, int e)
{
		//背景
		int endsegnum = e;

		string inputfoldername = "patch/";
		string outputfoldername = "vis/";

		string name = n;

		string texturename = "_texture_";
		string maskname = "_segment_";

		string pngname = ".png";
		string jpgname = ".jpg";

		Mat Background = imread("Background.png");

		for (int segnum = 0; segnum< endsegnum; segnum++)
		{
			stringstream ss;
			ss<<segnum;

			string srcname = inputfoldername+name+texturename+ss.str()+pngname;
			string maskinfoname = inputfoldername+name+maskname+ss.str()+pngname;

			string srcresultname = outputfoldername+name+"_viscompletion_"+ss.str()+pngname;

			Mat Src = imread(srcname);
			Mat Maskinfo = imread(maskinfoname);

			Mat result_src, result_mask;

			if (!Src.empty()
					&&!Maskinfo.empty())
			{
				result_src = Src;
				result_mask = Maskinfo;

				for (int seg2 = 0; seg2< segnum; seg2++)
				{
					stringstream ss2;
					ss2<<seg2;

					string srccomparename = inputfoldername+name+texturename+ss2.str()+pngname;
					string maskcomparename = inputfoldername+name+maskname+ss2.str()+pngname;

					Mat Srccompare = imread(srccomparename);
					Mat Maskinfocompare = imread(maskcomparename);

					if (!Srccompare.empty()
							&&!Maskinfocompare.empty())
					{
					int pixelcount = 0;

					for (int i = 0; i < Src.rows; i++)
						for (int j = 0; j < Src.cols; j++)
							if (Maskinfo.at<Vec3b>(i, j) != Vec3b(0, 0, 0)
									&& Maskinfocompare.at<Vec3b>(i, j)
											!= Vec3b(0, 0, 0))
								pixelcount++;

					if (pixelcount >= minpixelcount)
					{
						result_src += Srccompare*0.4;
						result_mask += Maskinfocompare;
					}
					}
				}

				for (int i = 0; i< result_src.rows; i++)
					for (int j = 0; j< result_src.cols; j++)
						if (result_mask.at<Vec3b>(i, j) == Vec3b(0, 0, 0))
							result_src.at<Vec3b>(i, j) = Background.at<Vec3b>(i, j);

				imwrite(srcresultname, result_src);
			}
		}
}

#endif
