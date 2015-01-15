#include <stdlib.h>
#include <stdio.h>

#include <iostream>
using namespace std;

#include <vector>
#include <assert.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

#define max_layer 20
#define max_seg 50

int getLayer(string path, char *name, int *layer, int *seg)
{
	for (int i = 0; i< max_layer; i++)
	{
		char filename[100];
		sprintf(filename, "%s%s%s%d%s", path.c_str(), name, "_layer_", i, ".png");

		Mat tmp = imread(filename);
		if(tmp.empty())
		{
			*layer = i-1;
			break;
		}
	}

	for (int i = 0; i< max_seg; i++)
	{
		char filename[100];
		sprintf(filename, "%s%s%s%d%s", path.c_str(), name, "_texture_", i, ".png");

		Mat tmp = imread(filename);
		if(tmp.empty())
		{
			*seg = i-1;
			break;
		}
	}

	return 0;
}

int getFile(string path, char *name, int *layer, int *seg)
{
	//string path = "~/Desktop/patchmatch/";
	string cmd1 = "dir " + path + " > test.txt";
	system(cmd1.c_str());

	FILE *fp = fopen("test.txt", "r");
	assert(fp);

	char token[100];
	fscanf(fp, "%s", token);

	int count = 0;
	for (int i = 0; i < 100; i++)
		if (token[i] == '\0')
		{
			count = i;
			break;
		}

	char token2[10];
	for (int i = 0; i < 3; i++)
		token2[i] = token[count - 3 + i];
	token2[3] = '\0';

	//assert(!strcmp(token2, "jpg"));
	assert(token2[0] == 'j' &&token2[1] == 'p' &&token2[2] == 'g');

	for (int i = 0; i < count - 4; i++)
		name[i] = token[i];
	name[count - 4] = '\0';

	getLayer(path, name, layer, seg);

	system("rm test.txt");

	return 0;
}
