
#include "patch.h"

patch::patch(Mat Src, Mat Mask, int ps, int pyr, int ANN, string result)
{
	srand((unsigned)time(NULL));

	assert(Src.type() == CV_8UC3);
	assert(Mask.type() == CV_8UC1);

	SrcImg = Src.clone();
	DstImg = Src.clone();
	MaskImg = Mask.clone();
	OffsetImg = Mat(MaskImg.size(), CV_32FC2, Scalar::all(0));
	show = Mat::zeros(DstImg.size(), CV_8UC3);

	PatchSize = ps;
	//��randomsearch����������������
	RandomWindow = -1;
	MinWindow = 7*PatchSize;

	//����������
	PryIteration = pyr;
	ANNIteration = ANN;

	RandomAttenuation = 0.5f;

	filename = result;
}

Mat patch::Run()
{	
	Mat Srcs[20];
	Mat Dsts[20];
	Mat Masks[20];
	Mat Masks2[20];
	Mat Offs[20];

	Srcs[0] = SrcImg;
	Dsts[0] = DstImg;
	Masks[0] = MaskImg;
	Offs[0] = OffsetImg;

	InitMask(Masks[0], Masks2[0]);

	int i = 0;//����������

	cout<<"PyrDown Step "<<i<<endl;

	while (i < PryIteration
		&&Dsts[i].rows > MinWindow
		&&Dsts[i].cols > MinWindow)
	{
		cout<<"PyrDown Step "<<i+1<<endl;

		pyrDown(Srcs[i], Srcs[i+1]);
		//Srcs[i+1].setTo(0);
		//resize(Srcs[i], Srcs[i+1], Srcs[i+1].size());

		pyrDown(Dsts[i], Dsts[i+1]);
		//Dsts[i+1].setTo(0);
		//resize(Dsts[i], Dsts[i+1], Dsts[i+1].size());

		PyrDownMask(Masks[i], Masks[i+1]);
		InitMask(Masks[i+1], Masks2[i+1]);

		i++;
	}

	cout<<endl;

	InitOff(Masks2[i], Offs[i]);

	while (i>=0)
	{
		cout<<"PyrUp Step "<<i<<endl;

		RandomWindow = max(Srcs[i].rows, Srcs[i].cols);

		GenerateImage2(Dsts[i], Srcs[i], Masks2[i], Offs[i]);

		show.setTo(0);
		resize(Dsts[i], show, DstImg.size());
		//imshow("show", show);
		//waitKey(33);

		for (int j = 0; j< ANNIteration+i* 3; j++)
		{
			int odd = 0;

			cout<<"ANN "<<j<<" start"<<endl;
			GenerateImage(Dsts[i], Srcs[i], Masks2[i], Offs[i], odd);
			cout<<"ANN "<<j<<" end"<<endl;

			show.setTo(0);
			resize(Dsts[i], show, DstImg.size());
			//imshow("show", show);
			//waitKey(33);
		}

		if (i > 0)
			PyrUpOff(Offs[i], Masks2[i-1], Offs[i-1]);

		cout<<endl;

		i--;
	}

	waitKey(33);
	//imwrite(filename.data(), DstImg);
	return DstImg;
}

patch::~patch(void)
{
}
