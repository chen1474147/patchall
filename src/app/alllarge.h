

//�������Ŀ����������չ���������

#ifndef ALLLARGE_H_
#define ALLLARGE_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

#include <iostream>
using namespace std;


void getleft_b_e(Mat &Src, Mat &Mask, int **m_c, int *m_c_c) {
	int max_seg = 20;
	//ÿһ����������
	int min_seg = 0;

	//m_c_c���ÿ���м��Σ�Ĭ�������10��
	//m_c�����εĿ�ͷ�ͽ�β
	for (int i = 0; i < Mask.rows; i++)
		m_c[i] = new int[2 * max_seg];

	for (int i = 0; i < Mask.rows; i++) {
		int count = 0;

		for (int j = 0; j < Mask.cols; j++) {
			if (Mask.at<Vec3b>(i, j) != Vec3b(0, 0, 0)) {
				m_c[i][count * 2] = j;

				while (j < Mask.cols && Mask.at<Vec3b>(i, j) != Vec3b(0, 0, 0))
					j++;

				if (j-1-m_c[i][count * 2] >= min_seg - 1 )
				{
				m_c[i][count * 2 + 1] = j - 1;

				count++;
				}
			}
		}

		assert(count <= max_seg);

		m_c_c[i] = count;
	}
}

//������
void copy(Mat &Src, Mat &h, Mat &Mask) {

	h.setTo(0);

	//���ٱ����������չ
	double kk = 0.5;
	//��չ����
	double p = 0.3;
	//erode����
	double erode = 0.1;

	int **m_c = new int *[Mask.rows];
	int *m_c_c = new int[Mask.rows];

	getleft_b_e(Src, Mask, m_c, m_c_c);

	for (int i = 0; i < Mask.rows; i++)
	{

		if (m_c_c[i] == 0)
			continue;

		for (int k = 0; k < m_c_c[i]; k++)
		{
			//����Ӳ�Ա߽�
			int border_b, border_e;

			if (k == 0)
				border_b = 0;
			else
				border_b = (m_c[i][2 * (k - 1) + 1] + m_c[i][2 * k]) / 2 + 1;

			if (k == m_c_c[i] - 1)
				border_e = Mask.cols - 1;
			else
				border_e = (m_c[i][2 * k + 1] + m_c[i][2 * (k + 1)]) / 2;

			//��ɫ�Ŀ�ͷ��β
			int b = m_c[i][2 * k];
			int e = m_c[i][2 * k + 1];

			//erode�Ĵ�С
			int s = erode * (e - b);

			//Ҫ���ӵĴ�С
			int l = p * (e - b + 1) + 1;
			int r = l;

			//���ı߽�
			int m_e_l = b - l >= border_b ? b - l : border_b;
			int m_e_r = e + r <= border_e ? e + r : border_e;

			//erode֮��Ŀ�ͷ��β
			int tmp_b = b + s;
			int tmp_e = e - s;

			//��ʾerode������
			for (int j = b; j<= b+s-1; j++)
				h.at<Vec3b>(i, j) = 0.1*Src.at<Vec3b>(i, j);
			for (int j = e-s+1; j<= e; j++)
				h.at<Vec3b>(i, j) = 0.1*Src.at<Vec3b>(i, j);

			//��ʾ���������
			for(int j = tmp_b + (tmp_e - tmp_b + 1) * kk; j< tmp_e - (tmp_e - tmp_b + 1) * kk; j++)
				h.at<Vec3b>(i, j) = 1.0*Src.at<Vec3b>(i, j);

			//��ʾrepete������

			//erode֮��ѡ���Ŀ�ͷ��β
			int m_c_b, m_c_e;

			//��
			m_c_b = tmp_b;

			if ((tmp_e - tmp_b + 1) * kk >= 1)
				m_c_e = tmp_b - 1 + (tmp_e - tmp_b + 1) * kk;
			else
				m_c_e = tmp_b;

			for (int j = m_c_b; j<= m_c_e; j++)
				h.at<Vec3b>(i, j) = 0.5*Src.at<Vec3b>(i, j);

			while (m_c_e - m_c_b <= m_c_b - 1 - m_e_l) {
				for (int j = m_c_b; j <= m_c_e; j++) {
					if (Mask.at<Vec3b>(i, j) != Vec3b(0, 0, 0)) {
						Src.at<Vec3b>(i, 2 * m_c_b - j - 1) = Src.at<Vec3b>(i,
								j);
						Mask.at<Vec3b>(i, 2 * m_c_b - j - 1) = Mask.at<Vec3b>(i,
								j);
					}

				}

				m_c_b = 2 * m_c_b - m_c_e - 1;
			}

			for (int j = m_c_b; j <= m_c_e && 2 * m_c_b - j - 1 >= m_e_l; j++) {
				if (Mask.at<Vec3b>(i, j) != Vec3b(0, 0, 0)) {
					Src.at<Vec3b>(i, 2 * m_c_b - j - 1) = Src.at<Vec3b>(i, j);
					Mask.at<Vec3b>(i, 2 * m_c_b - j - 1) = Mask.at<Vec3b>(i, j);
				}

			}

			//��
			m_c_e = tmp_e;

			if ((tmp_e - tmp_b + 1) * kk >= 1)
				m_c_b = tmp_e + 1 - (tmp_e - tmp_b + 1) * kk;
			else
				m_c_b = tmp_e;

			for (int j = m_c_b; j<= m_c_e; j++)
				h.at<Vec3b>(i, j) = 0.5*Src.at<Vec3b>(i, j);

			while (m_c_e - m_c_b <= m_e_r - m_c_e - 1) {
				for (int j = m_c_e; j >= m_c_b; j--) {
					if (Mask.at<Vec3b>(i, j) != Vec3b(0, 0, 0)) {
						Src.at<Vec3b>(i, 2 * m_c_e - j + 1) = Src.at<Vec3b>(i,
								j);
						Mask.at<Vec3b>(i, 2 * m_c_e - j + 1) = Mask.at<Vec3b>(i,
								j);
					}

				}

				m_c_e = 2 * m_c_e - m_c_b + 1;
			}

			for (int j = m_c_e; j >= m_c_b && 2 * m_c_e - j + 1 <= m_e_r; j--) {
				if (Mask.at<Vec3b>(i, j) != Vec3b(0, 0, 0)) {
					Src.at<Vec3b>(i, 2 * m_c_e - j + 1) = Src.at<Vec3b>(i, j);
					Mask.at<Vec3b>(i, 2 * m_c_e - j + 1) = Mask.at<Vec3b>(i, j);
				}

			}
		}

	}
}

//������
void copy2(Mat &Src, Mat &h, Mat &Mask) {

	h.setTo(0);

	//���ٱ����������չ
	double kk = 1.0;
	//erode��������
	int erode = 5;
	//��չ��������(�����erode)
	int p = 10;

	int **m_c = new int *[Mask.rows];
	int *m_c_c = new int[Mask.rows];

	getleft_b_e(Src, Mask, m_c, m_c_c);

	for (int i = 0; i < Mask.rows; i++)
	{

		if (m_c_c[i] == 0)
			continue;

		for (int k = 0; k < m_c_c[i]; k++)
		{
			//����Ӳ�Ա߽�
			int border_b, border_e;

			if (k == 0)
				border_b = 0;
			else
				border_b = (m_c[i][2 * (k - 1) + 1] + m_c[i][2 * k]) / 2 + 1;

			if (k == m_c_c[i] - 1)
				border_e = Mask.cols - 1;
			else
				border_e = (m_c[i][2 * k + 1] + m_c[i][2 * (k + 1)]) / 2;

			//��ɫ�Ŀ�ͷ��β
			int b = m_c[i][2 * k];
			int e = m_c[i][2 * k + 1];

			//erode�Ĵ�С
			int erode_tmp = erode;
			while(b +2*erode_tmp >e)
				erode_tmp = erode_tmp/2;
			int s = erode_tmp;

			//Ҫ���ӵĴ�С
			int l = erode_tmp+p;
			int r = l;

			//���ı߽�
			int m_e_l = b - l >= border_b ? b - l : border_b;
			int m_e_r = e + r <= border_e ? e + r : border_e;

			//erode֮��Ŀ�ͷ��β
			int tmp_b = b + s;
			int tmp_e = e - s;

			//��ʾerode������
			for (int j = b; j<= b+s-1; j++)
				h.at<Vec3b>(i, j) = 0.1*Src.at<Vec3b>(i, j);
			for (int j = e-s+1; j<= e; j++)
				h.at<Vec3b>(i, j) = 0.1*Src.at<Vec3b>(i, j);

			//��ʾ���������
			for(int j = tmp_b + (tmp_e - tmp_b + 1) * kk; j< tmp_e - (tmp_e - tmp_b + 1) * kk; j++)
				h.at<Vec3b>(i, j) = 1.0*Src.at<Vec3b>(i, j);

			//��ʾrepete������

			//erode֮��ѡ���Ŀ�ͷ��β
			int m_c_b, m_c_e;

			//��
			m_c_b = tmp_b;

			if ((tmp_e - tmp_b + 1) * kk >= 1)
				m_c_e = tmp_b - 1 + (tmp_e - tmp_b + 1) * kk;
			else
				m_c_e = tmp_b;

			for (int j = m_c_b; j<= m_c_e; j++)
				h.at<Vec3b>(i, j) = 0.5*Src.at<Vec3b>(i, j);

			while (m_c_e - m_c_b <= m_c_b - 1 - m_e_l) {
				for (int j = m_c_b; j <= m_c_e; j++) {
					if (Mask.at<Vec3b>(i, j) != Vec3b(0, 0, 0)) {
						Src.at<Vec3b>(i, 2 * m_c_b - j - 1) = Src.at<Vec3b>(i,
								j);
						Mask.at<Vec3b>(i, 2 * m_c_b - j - 1) = Mask.at<Vec3b>(i,
								j);
					}

				}

				m_c_b = 2 * m_c_b - m_c_e - 1;
			}

			for (int j = m_c_b; j <= m_c_e && 2 * m_c_b - j - 1 >= m_e_l; j++) {
				if (Mask.at<Vec3b>(i, j) != Vec3b(0, 0, 0)) {
					Src.at<Vec3b>(i, 2 * m_c_b - j - 1) = Src.at<Vec3b>(i, j);
					Mask.at<Vec3b>(i, 2 * m_c_b - j - 1) = Mask.at<Vec3b>(i, j);
				}

			}

			//��
			m_c_e = tmp_e;

			if ((tmp_e - tmp_b + 1) * kk >= 1)
				m_c_b = tmp_e + 1 - (tmp_e - tmp_b + 1) * kk;
			else
				m_c_b = tmp_e;

			for (int j = m_c_b; j<= m_c_e; j++)
				h.at<Vec3b>(i, j) = 0.5*Src.at<Vec3b>(i, j);

			while (m_c_e - m_c_b <= m_e_r - m_c_e - 1) {
				for (int j = m_c_e; j >= m_c_b; j--) {
					if (Mask.at<Vec3b>(i, j) != Vec3b(0, 0, 0)) {
						Src.at<Vec3b>(i, 2 * m_c_e - j + 1) = Src.at<Vec3b>(i,
								j);
						Mask.at<Vec3b>(i, 2 * m_c_e - j + 1) = Mask.at<Vec3b>(i,
								j);
					}

				}

				m_c_e = 2 * m_c_e - m_c_b + 1;
			}

			for (int j = m_c_e; j >= m_c_b && 2 * m_c_e - j + 1 <= m_e_r; j--) {
				if (Mask.at<Vec3b>(i, j) != Vec3b(0, 0, 0)) {
					Src.at<Vec3b>(i, 2 * m_c_e - j + 1) = Src.at<Vec3b>(i, j);
					Mask.at<Vec3b>(i, 2 * m_c_e - j + 1) = Mask.at<Vec3b>(i, j);
				}

			}
		}

	}
}


void alllarge(string n)
{
	//��ȫ
	int allseg = 30;

	for (int seg = 0; seg< allseg; seg++)
	{
		string input = "enlarge/";
		string output = "expand2/";

		string name = n;

		string texturename = "_texture_";
		string maskname = "_segment_";


		string mtexturename = "_mtexture_";

		string pngname = ".png";
		string jpgname = ".jpg";

		stringstream ss;
		ss<<seg;

		string srcname = input+name+texturename+ss.str()+pngname;
		string maskinfoname = input+name+maskname+ss.str()+pngname;

		string srcresultname = output+name+texturename+ss.str()+pngname;
		string maskresultname = output+name+"_mtexture_"+ss.str()+pngname;
		//string highsrcresultname = output+name+"_htexture_"+ss.str()+pngname;

		Mat Src = imread(srcname);
		Mat Maskinfo = imread(maskinfoname);

		if (!Src.empty() && !Maskinfo.empty())
		{
			//imshow("src", Src);
			//imshow("mask", Maskinfo);
			//waitKey(33);

			Mat h = Src.clone();
			copy2(Src, h, Maskinfo);

			//imshow("src", Src);
			//imshow("mask", Maskinfo);
			//imshow("h", h);
			//waitKey(33);

			imwrite(srcresultname, Src);
			imwrite(maskresultname, Maskinfo);
			//imwrite(highsrcresultname, h);
		}
	}
}

#endif
