
#include "patch.h"

float patch::GetDelta(float *sim, int length)
{
	//assert(length > 0);

	float *dist = new float[length];
	for (int i = 0; i< length; i++)
		dist[i] = sim[i];

	for (int i = 0; i< length-1; i++)
	{
		for (int j = 0; j< length-1-i; j++)
		{
			if (dist[j] > dist[j+1])
			{
				float tmp = dist[j+1];
				dist [j+1] =  dist[j];
				dist[j] = tmp;
			}
		}
	}

	return dist[(int)(length*0.75)];
}

float GetWindowSize(Vec3b * c, int length)
{
	//assert(length > 0);

	float allb = 0, allg = 0, allr = 0;
	for(int i = 0; i< length; i++)
	{
		allb += c[i][0];
		allg += c[i][1];
		allr += c[i][2];
	}
	allb = allb/length;
	allg = allg/length;
	allr = allr/length;

	float delta = 0;

	for(int i = 0; i< length; i++)
	{
		delta +=
		(allb - c[i][0])*(allb - c[i][0])+
		(allg - c[i][1])*(allg - c[i][1])+
		(allr - c[i][2])*(allr - c[i][2]);
	}

	return delta;
}

Vec3b patch::GetMeanshift(float *w, Vec3b * c, int now_length, int odd)
{
	/*
	 * float allp = 0, allb = 0, allg = 0, allr = 0;

		for (int i = 0; i< now_length; i++)
		{
			allp += w[i];
			allb += ((int)c[i][0])*w[i];
			allg += ((int)c[i][1])*w[i];
			allr += ((int)c[i][2])*w[i];
		}

		return Vec3b((uchar)(allb/allp), (uchar)(allg/allp), (uchar)(allr/allp));
	 */

	//assert(now_length > 0);

	float delta = GetWindowSize(c, now_length);
	float iter = 1;

	Vec3b centre;
	if (odd%2 == 0)
		centre = c[0];
	else
		centre = c[now_length-1];

	float *w1 = w;
	Vec3b * c1 = c;
	int length1 = now_length;

	while(true)
	{
		float *w2 = new float[length1];
		Vec3b * c2 = new Vec3b[length1];
		int length2 = 0;

		for(int i = 0; i< length1; i++)
		{
			if ((c1[i][0] - centre[0])*(c1[i][0] - centre[0])
					+(c1[i][1] - centre[1])*(c1[i][1] - centre[1])
					+(c1[i][2] - centre[2])*(c1[i][2] - centre[2])
					< iter*iter*delta
					)
			{
				w2[length2] = w1[i];
				c2[length2] = c1[i];
				length2++;
			}
		}

		if(length2 == 0)
			break;

		float allb = 0, allg = 0, allr = 0;

		for (int i = 0; i< length2; i++)
		{
			allb += c2[i][0]-centre[0];
			allg += c2[i][1]-centre[1];
			allr += c2[i][2]-centre[2];
		}

		allb = allb/length2;
		allg = allg/length2;
		allr = allr/length2;

		Vec3b newcentre = Vec3b(allb+centre[0], allg+centre[1], allr+centre[2]);

		c1 = c2;
		w1 = w2;
		length1 = length2;

		if ((newcentre[0]-centre[0])*(newcentre[0]-centre[0])
				+(newcentre[0]-centre[0])*(newcentre[0]-centre[0])
				+(newcentre[0]-centre[0])*(newcentre[0]-centre[0])
				<1
				)
			break;
		else
			centre = newcentre;
	}

	float allp = 0, allb = 0, allg = 0, allr = 0;

	for (int i = 0; i< length1; i++)
	{
		allp += w1[i];
		allb += ((int)c1[i][0])*w1[i];
		allg += ((int)c1[i][1])*w1[i];
		allr += ((int)c1[i][2])*w1[i];
	}

	return Vec3b((uchar)(allb/allp), (uchar)(allg/allp), (uchar)(allr/allp));
}
