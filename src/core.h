
#ifndef CORE_H_
#define CORE_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

#include <iostream>
using namespace std;

#include <time.h>

//�������Ŀ�����޸�gimp���
//֤ʵgimp����������ӣ�0��0����ʼ
//֤ʵgimp�����طֲ���rgb

typedef unsigned char Pixelel;

const int max_neighbours = 1000;
const int max_trys_per_pixel = 10000;

/***********************/
/***********************/
/*** x,y Coordinates ***/
/***********************/
/***********************/

struct Coordinates
{
	int x, y;
	Coordinates(int _x, int _y) : x(_x), y(_y)
	{
	}

	Coordinates()
	{
	}

	bool operator < (const Coordinates other) const
	{ 
		return y*y+x*x < (other.y*other.y+other.x*other.x); 
	}

	Coordinates operator + (const Coordinates a) const
	{ 
		return Coordinates(x+a.x, y+a.y); 
	}

	Coordinates operator - (const Coordinates a) const
	{ 
		return Coordinates(x-a.x, y-a.y); 
	}
};

/**************/
/**************/
/*** Bitmap ***/
/**************/
/**************/

//Bitmap class with three dimensions (width, height, number of channels)
template<class t>
struct Bitmap
{
	int width, height, depth;
	t *data;

	Bitmap()
	{ 
		data = 0;
	}

	~Bitmap()
	{
		delete data;
	}

	void size(int w, int h, int d)
	{
		width = w; 
		height = h; 
		depth = d;

		delete data;
		data = new t[w*h*d];
	}

	t *at(int x, int y) const
	{
		return data + (y*width+x)*depth;
	}

	t *at(const Coordinates position) const
	{
		return at(position.x, position.y);
	}

	//ֻ��д��Dst
	void to_drawable(Mat drawable)
	{
		for(int i = 0; i< height; i++)
			for(int j = 0; j< width; j++)
				for(int k = 0; k< depth; k++)
					drawable.at<Vec3b>(i, j)[2-k] = data[(i*width+j)*depth+k];
	}

	//Src��Mask
	void from_drawable(Mat drawable)
	{
		if (drawable.channels() == 1)
		{
			for(int i = 0; i< height; i++)
				for(int j = 0; j< width; j++)
					data[i*width+j] = drawable.at<uchar>(i, j);
		}
		else
		{
			for(int i = 0; i< height; i++)
				for(int j = 0; j< width; j++)
					for(int k = 0; k< depth; k++)
						data[(i*width+j)*depth+k] = drawable.at<Vec3b>(i, j)[2-k];
		}
	}
};

/*******************************/
/*******************************/
/*** Status holding type *******/
/*******************************/
/*******************************/

struct Status
{
	bool has_value, has_source;
	Coordinates source;
};

/*******************************/
/*******************************/
/*** Get stuff from GIMP *******/
/*******************************/
/*******************************/

//Get a drawable and possibly its selection mask from the GIMP
void fetch_image_and_mask
	(
	Mat drawable,
	Mat drawable_mask,
	Bitmap<Pixelel> &image,
	Bitmap<Pixelel> &mask
	)
{
	assert(drawable.channels() == 3
		&&drawable_mask.channels() == 1);

	image.size(drawable.cols, drawable.rows, 3);
	mask.size(drawable.cols, drawable.rows, 1);

	image.from_drawable(drawable);
	mask.from_drawable(drawable_mask);
}

/******************/
/******************/
/*** Parameters ***/
/******************/
/******************/

struct Parameters
{
	bool h_tile, v_tile;
	bool use_border;

	double map_weight;
	double autism;
	int neighbours, trys;
};

/* Restore the last parameters used from deep in the bowels of
the main GIMP app */
void get_last_parameters(Parameters *param)
{
	/* Defaults in case this is our first run */
	param->v_tile = true;
	param->h_tile = true;
	param->use_border = true;
	param->map_weight = 0.5;
	param->autism = 0.117; /* 30/256 */
	param->neighbours = 30;
	param->trys = 200;
}

inline bool wrap_or_clip(Parameters &parameters, Bitmap<Pixelel> &image, Coordinates &point)
{ 
	while(point.x < 0)
		if (parameters.h_tile)
			point.x += image.width;
		else
			return false;

	while(point.x >= image.width)
		if (parameters.h_tile)
			point.x -= image.width;
		else
			return false;

	while(point.y < 0)
		if (parameters.v_tile)
			point.y += image.height;
		else
			return false;

	while(point.y >= image.height)
		if (parameters.v_tile)
			point.y -= image.height;
		else
			return false;

	return true;
}

/* Helpers for the main function */

int diff_table[512], map_diff_table[512];

int input_bytes, map_bytes, map_pos, bytes;
Bitmap<Pixelel> data, data_mask, corpus, corpus_mask;
Bitmap<Status> status;
vector<Coordinates> data_points, corpus_points, sorted_offsets;

Coordinates neighbours[max_neighbours];
Pixelel neighbour_values[max_neighbours][8];
Status *neighbour_statuses[max_neighbours];
int n_neighbours;

int best;
Coordinates best_point;

double neglog_cauchy(double x)
{
	return log(x*x+1.0);
}

void make_offset_list(void)
{
	int width = (corpus.width<data.width?corpus.width:data.width);
	int height = (corpus.height<data.height?corpus.height:data.height);
	sorted_offsets.resize(0);
	for(int y=-height+1;y<height;y++)
		for(int x=-width+1;x<width;x++)
			sorted_offsets.push_back(Coordinates(x,y));

	sort(sorted_offsets.begin(), sorted_offsets.end());
}

inline void try_point(const Coordinates &point)
{
	int sum = 0;

	for(int i=0;i<n_neighbours;i++)
	{
		Coordinates off_point = point + neighbours[i];
		if (off_point.x < 0 || off_point.y < 0 || 
			off_point.x >= corpus.width || off_point.y >= corpus.height ||
			!corpus_mask.at(off_point)[0])
		{
			sum += diff_table[0]*input_bytes + map_diff_table[0]*map_bytes;
		}
		else
		{
			const Pixelel *corpus_pixel = corpus.at(off_point),
				*data_pixel = neighbour_values[i];
			if (i)
				for(int j=0;j<input_bytes;j++)
					sum += diff_table[256u + data_pixel[j] - corpus_pixel[j]];
			if (input_bytes != bytes)
				for(int j=input_bytes;j<bytes;j++)
					sum += map_diff_table[256u + data_pixel[j] - corpus_pixel[j]];
		}

		if (sum >= best) return;
	}

	best = sum;
	best_point = point;
}

/* This is the main function. */

Mat run(Mat src, Mat mask_search, Mat mask_hole)
{
	assert(src.channels() == 3
		&&mask_search.channels() == 1
		&&mask_hole.channels() == 1);

	Parameters parameters;

	Mat drawable = src.clone(),
		corpus_drawable = src.clone(),
		drawable_mask = mask_hole.clone(),
		corpus_drawable_mask = mask_search.clone();

	srand(time(0));

	get_last_parameters(&parameters); 

	input_bytes  = 3;
	map_bytes    = 0;
	map_pos      = input_bytes;
	bytes        = map_pos + map_bytes;

	fetch_image_and_mask(drawable, drawable_mask, data, data_mask);

	status.size(data.width,data.height,1);

	data_points.resize(0);

	for(int y=0;y<status.height;y++)
	{
		for(int x=0;x<status.width;x++)
		{
			status.at(x,y)[0].has_source = false;
			status.at(x,y)[0].has_value = false;

			if (parameters.use_border && data_mask.at(x,y)[0] == 0)
				status.at(x,y)->has_value  = true;

			if (data_mask.at(x,y)[0] != 0)
				data_points.push_back(Coordinates(x,y));
		}
	}

	/* Fetch the corpus */

	fetch_image_and_mask(corpus_drawable, corpus_drawable_mask, corpus, corpus_mask);

	corpus_points.resize(0);

	for(int y=0;y<corpus.height;y++)
	{
		for(int x=0;x<corpus.width;x++)
		{
			corpus_mask.at(x,y)[0] = 255 - corpus_mask.at(x,y)[0];
			if (corpus_mask.at(x,y)[0])
				corpus_points.push_back(Coordinates(x,y));
		}
	}

	/* Sanity check */

	if (!corpus_points.size() || !data_points.size())
	{
		if (!corpus_points.size())
			cout<<"The input texture is too small.";
		else
			cout<<"The output image is too small.";

		system("pause");
	}

	/* Setup */

	make_offset_list();

	for(int i=-256;i<256;i++) {
		double value = neglog_cauchy(i/256.0/parameters.autism) 
			/ neglog_cauchy(1.0/parameters.autism) * 65536.0;
		diff_table[256+i] = int(value);
		map_diff_table[256+i] = int(i*i*parameters.map_weight*4.0);
	}

	for(int i=0;i<int(data_points.size());i++) {
		int j = rand() % data_points.size();
		Coordinates temp = data_points[i];
		data_points[i] = data_points[j];
		data_points[j] = temp;
	}

	for(int n=data_points.size();n>0;) {
		n = n*3/4; // <- note magic number... the more repetition, the higher the quality, maybe
		for(int i=0;i<n;i++)
			data_points.push_back(data_points[i]);
	}

	/* Do it */

	Bitmap<int> tried;
	tried.size(corpus.width,corpus.height,1);
	for(int i=0;i<corpus.width*corpus.height;i++)
		tried.data[i] = -1;

	for(int i=data_points.size()-1;i>=0;i--) {
		if ((i&1023) == 0)
			cout<<(1.0-float(i)/data_points.size())<<endl;

		Coordinates position = data_points[i];

		status.at(position)->has_value = true;

		n_neighbours = 0;
		const int sorted_offsets_size = sorted_offsets.size();
		for(int j=0;j<sorted_offsets_size;j++) {
			Coordinates point = position + sorted_offsets[j];

			if (wrap_or_clip(parameters, data, point) && 
				status.at(point)->has_value) {
					neighbours[n_neighbours] = sorted_offsets[j];
					neighbour_statuses[n_neighbours] = status.at(point);
					for(int k=0;k<bytes;k++)
						neighbour_values[n_neighbours][k] = data.at(point)[k];
					n_neighbours++;
					if (n_neighbours >= parameters.neighbours) break;
			}
		}

		best = 1<<30;

		for(int j=0;j<n_neighbours && best != 0;j++)
		{
			if (neighbour_statuses[j]->has_source) {
				Coordinates point = neighbour_statuses[j]->source - neighbours[j];
				if (point.x < 0 || point.y < 0 || point.x >= corpus.width || point.y >= corpus.height) continue;
				if (!corpus_mask.at(point)[0] || tried.at(point)[0] == i) continue;
				try_point(point);
				tried.at(point)[0] = i;
			}
		}

		for(int j=0;j<parameters.trys && best != 0;j++)
			try_point(corpus_points[rand()%corpus_points.size()]);

		for(int j=0;j<input_bytes;j++)
			data.at(position)[j] = corpus.at(best_point)[j];

		status.at(position)->has_source = true;
		status.at(position)->source = best_point;
	}

	/* Write result to region */
	drawable.setTo(0);
	data.to_drawable(drawable);

	//imshow("show", drawable);
	//waitKey(33);

	return drawable;
}

#endif

