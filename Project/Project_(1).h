#pragma once

#include "resource.h"

#include <ShlObj.h>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include <iostream>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <commdlg.h>

using namespace std;
using namespace cv;
using namespace std::chrono;

using std::min;
using std::max;
#pragma warning(push)
#pragma warning(disable : 4244)
#include <GdiPlus.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma warning(pop)

//create a struct of data contains Mat, name of image, directory of an image, and histogram
struct Database {
	Mat srcImg;
	string nameImg;
	string urlImg;
	vector<Mat> hists;
};


// calculate edge matrix for image
Mat calcEdgeMat(Mat);

// edge comparison functionality
void edgeMatching(vector<Database>&, Database);
// another version of edge comparison
void edgeMatching_2(vector<Database>&, Database);

//calculate histogram for each image
vector<Mat> calc_Img(Mat srcImg, bool gbin);

//calculate 2 histograms
int calc2Hists(vector<int>h1, vector<int>h2);

//reorder Bhattacharyya distance
void bhatta_distance(vector<Database>&objs, Database src, bool gbin);

//find all files in a folder
bool find_files(vector<Database>&tmp, string img);


// for color matching
void colorMatching(vector<Database>&, Database);

// kmean clluster method for color matching
Mat kMeanClusterMat(Mat);

//Convert image into icon
HICON convert_Icon(PTSTR imgPath);

// calculate one histogram matrix
Mat calcOneHist(Mat);

//Convert String to LPWSTR
LPWSTR Convert_LPWSTR(string& instr);

//show result into listview
void showResult(HWND hDialog, HWND hListview, vector<Database>list, int k);