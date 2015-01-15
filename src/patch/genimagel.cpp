
#include "patch.h"

Vec3b patch::GeneratePixel(Mat Dst, Mat Src, Mat Mask, Mat Off, int row, int col, int odd)
{

	Mat DstSize = GetPatch(Dst, row, col);

	int window_length = DstSize.rows*DstSize.cols;

	float *a = new float[window_length];
	float *sim = new float[window_length];
	float *w = new float[window_length];
	Vec3b *c = new Vec3b[window_length];
	int nowlength = 0;

	float ratio = sqrt(Dst.rows*Dst.rows+Dst.cols*Dst.cols*1.0f) > 50? sqrt(Dst.rows*Dst.rows+Dst.cols*Dst.cols*1.0f)/50:1;

	if (odd%2 == 0)
	{
		for (int i = 0; i< PatchSize; i++)
		{
			for (int j = 0; j< PatchSize; j++)
			{
				int now_row = row-(PatchSize/2)+i;
				int now_col = col-(PatchSize/2)+j;

				if (now_row<PatchSize/2
					||now_col<PatchSize/2
					||now_row>Dst.rows-1-PatchSize/2
					||now_col>Dst.cols-1-PatchSize/2
					||Mask.at<uchar>(now_row, now_col) == back
					)
				{
					continue;
				}

				Propagation(Dst, Src, Mask, Off, now_row, now_col, odd);
				RandomSearch(Dst, Src, Mask, Off, now_row, now_col);

				int patch_row = now_row+Off.at<Vec2f>(now_row, now_col)[0];
				int patch_col = now_col+Off.at<Vec2f>(now_row, now_col)[1];

				Mat DstPatch = GetPatch(Dst, now_row, now_col);
				Mat SrcPatch = GetPatch(Src, patch_row, patch_col);

				c[nowlength] = SrcPatch.at<Vec3b>(PatchSize-1-i, PatchSize-1-j);
				//按照距离该点的距离算
				a[nowlength] = sqrt((now_row - patch_row)*(now_row - patch_row)*1.0+
						(now_col - patch_col)*(now_col-patch_col)*1.0)/ratio;
				sim[nowlength] = Distance(SrcPatch, DstPatch);
				nowlength++;
			}
		}

		float delta = GetDelta(sim, nowlength);

		if( delta == 0)
		for (int i = 0; i< nowlength; i++)
		{
			sim[i] = exp(-sim[i]);
			a[i] = pow(1.3f, -a[i]);
			w[i] = a[i]*sim[i];
		}
		else
			for (int i = 0; i< nowlength; i++)
			{
				sim[i] = exp(-sim[i]/(2*delta));
				a[i] = pow(1.3f, -a[i]);
				w[i] = a[i]*sim[i];
			}
	}
	else
	{
		for (int i = PatchSize-1; i>= 0; i--)
		{
			for (int j = PatchSize-1; j>= 0; j--)
			{
				int now_row = row-(PatchSize/2)+i;
				int now_col = col-(PatchSize/2)+j;

				if (now_row<PatchSize/2
					||now_col<PatchSize/2
					||now_row>Dst.rows-1-PatchSize/2
					||now_col>Dst.cols-1-PatchSize/2
					||Mask.at<uchar>(now_row, now_col) == back
					)
				{
					continue;
				}

				Propagation(Dst, Src, Mask, Off, now_row, now_col, odd);
				RandomSearch(Dst, Src, Mask, Off, now_row, now_col);

				int patch_row = now_row+Off.at<Vec2f>(now_row, now_col)[0];
				int patch_col = now_col+Off.at<Vec2f>(now_row, now_col)[1];

				Mat DstPatch = GetPatch(Dst, now_row, now_col);
				Mat SrcPatch = GetPatch(Src, patch_row, patch_col);

				c[nowlength] = SrcPatch.at<Vec3b>(PatchSize-1-i, PatchSize-1-j);
				//按照距离该点的距离算
				a[nowlength] = sqrt((now_row - patch_row)*(now_row - patch_row)*1.0+
						(now_col - patch_col)*(now_col-patch_col)*1.0)/ratio;
				sim[nowlength] = Distance(SrcPatch, DstPatch);
				nowlength++;
			}
		}

		float delta = GetDelta(sim, nowlength);

		if( delta == 0)
		for (int i = 0; i< nowlength; i++)
		{
			sim[i] = exp(-sim[i]);
			a[i] = pow(1.3f, -a[i]);
			w[i] = a[i]*sim[i];
		}
		else
			for (int i = 0; i< nowlength; i++)
			{
				sim[i] = exp(-sim[i]/(2*delta));
				a[i] = pow(1.3f, -a[i]);
				w[i] = a[i]*sim[i];
			}
	}

	return  GetMeanshift(w, c, nowlength, odd);
}

void patch::GenerateImage(Mat &Dst, Mat Src, Mat Mask, Mat Off, int odd)
{
	if (odd%2 == 0)
		for (int i = 0; i< Dst.rows; i++)
			for (int j = 0; j< Dst.cols; j++)
			{
				if (Mask.at<uchar>(i, j) == hole)
					Dst.at<Vec3b>(i, j) = GeneratePixel(Dst, Src, Mask, Off, i, j, odd);
			}
	else
		for (int i = Dst.rows-1; i>= 0; i--)
			for (int j = Dst.cols-1; j>= 0; j--)
			{
				if (Mask.at<uchar>(i, j) == hole)
					Dst.at<Vec3b>(i, j) = GeneratePixel(Dst, Src, Mask, Off, i, j, odd);
			}
}
