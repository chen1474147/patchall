#include <iostream>
using namespace std;

#include <stdlib.h>
#include <stdio.h>

#include <vector>

int getFolder(string path, vector<string> &name)
{
	//string path = "~/Desktop/patchmatch/input_50Input/";
	string cmd1 = "dir " + path + " > test.txt";
	system(cmd1.c_str());

	FILE *fp = fopen("test.txt", "r");

	if (!fp)
	{
		cout << "error!";
		system("pause");
	}

	char token[100];

	while (!feof(fp))
	{
		fscanf(fp, "%s", token);

		name.push_back(string(token));
	}

	name.pop_back();

	//for (int i = 0; i< name.size(); i++)
	//cout<<name[1]<<endl;

	system("rm test.txt");

	return 0;
}
