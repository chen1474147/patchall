#include <stdlib.h>
#include <stdio.h>

#include <iostream>
using namespace std;

#include "getFolder.h"
#include "getFile.h"

#include "app/allshow.h"
#include "app/allpatch.h"
#include "app/allshow2.h"
#include "app/alllarge.h"
#include "app/alllarge0.h"
#include "app/alllarge2.h"
#include "app/alllarge3.h"
#include "app/alllayer.h"

#include <time.h>

int main()
{
	string path = "/home/administrator/Desktop/patchmatch/";
	vector<string> name;

	getFolder(path, name);
	cout << "size : " << name.size() << endl;

	for (int i = 0; i < name.size(); i++)
	{
		string path2 = path + name[i] + "/";
		char jpgname[50];
		int l = -1, s = -1;

		getFile(path2, jpgname, &l, &s);
		cout << jpgname << "\t" << l << "\t" << s << endl;

		string cmd2 = "cp " + path2 + "* " + "/home/administrator/workspace/patchall/input/";
		system(cmd2.c_str());

		system("rm -rf show");
		system("mkdir show");

		system("rm -rf patch");
		system("mkdir patch");

		system("rm -rf vis");
		system("mkdir vis");

		system("rm -rf expand");
		system("mkdir expand");

		system("rm -rf expand2");
		system("mkdir expand2");

		system("rm -rf expand3");
		system("mkdir expand3");

		system("rm -rf enlarge");
		system("mkdir enlarge");

		clock_t start, finish;
		double duration;

		string name = string(jpgname);
		int le = l;
		int se = s;

		start = clock();

		allshow(name);
		allpatch(name, se);
		allshow2(name, se);
		alllayer(name, le, se);
		alllarge2(name);
		alllarge3(name);

		alllarge(name);
		alllarge0(name);

		finish = clock();

		duration = (double) (finish - start) / CLOCKS_PER_SEC;
		printf("%f seconds\n", duration);

		string zip = "tar -zvcf /home/administrator/workspace/patchall/" + name
				+ ".tar.gz show/ vis/ enlarge/ expand3/";
		system(zip.data());
	}

	system("rm -rf input");
	system("mkdir input");

	return 0;
}
