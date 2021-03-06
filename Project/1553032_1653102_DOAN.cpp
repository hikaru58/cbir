//

#include "stdafx.h"
#include "Project_(1).h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
OPENFILENAME ofn;
TCHAR urlImg[MAX_PATH], title[MAX_PATH], urlFolder[MAX_PATH];
vector<Database>arr;
bool isGbin = false;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Dialog(HWND, UINT, WPARAM, LPARAM);
BOOL				OpenFileDialog(HWND, PTSTR, PTSTR);
void				readFile(string fname, vector<Database> &hist, bool &gbin);
void                readEdgeDb(vector<Database>&);
void				writeFile(string fname1, string fname2, vector<Database>hist);
void				fileInitial(HWND, bool);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	Gdiplus::GdiplusStartupInput input;
	ULONG_PTR tokeninput;
	GdiplusStartup(&tokeninput, &input, NULL);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJECT1));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Dialog);
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//---------------------------------------------------------------------------------

Database src;
bool isImg = false;
bool isDatabase = false;
//the button Find only avaible if the program is loaded with database.txt and an image to compare
INT_PTR CALLBACK Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
	UNREFERENCED_PARAMETER(lParam);

	double elapsed;

	switch (message)
	{
	case WM_INITDIALOG: {
		//assign the amounts of pictures you want to get
		SendDlgItemMessage(hDlg, IDC_COMBO1, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"03");
		SendDlgItemMessage(hDlg, IDC_COMBO1, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"05");
		SendDlgItemMessage(hDlg, IDC_COMBO1, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"11");
		SendDlgItemMessage(hDlg, IDC_COMBO1, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"21");
		SendDlgItemMessage(hDlg, IDC_COMBO1, (UINT)CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
		EnableWindow(GetDlgItem(hDlg, IDC_HIST_CMP), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_EDGE_CMP), FALSE);
		return (INT_PTR)TRUE;
	}

	case WM_COMMAND:
		//if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		//{
		//	EndDialog(hDlg, LOWORD(wParam));
		//	return (INT_PTR)TRUE;
		//}
		//break;
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDC_BROWSE1:
		{
			fileInitial(hDlg, 1);//to open dialog with method 1 mean the tail is all
			//Open a dialog to get file
			if (!OpenFileDialog(hDlg, urlImg, title)) {
				break;
			}
			SetDlgItemText(hDlg, IDC_EDIT1, urlImg);
			Gdiplus::Image *srcImg = new Gdiplus::Image(urlImg);//Load imgage from image link
			if (!srcImg) 
			{
				isImg = 0;
				MessageBox(hDlg, L"Error in loading image", L"Notice", 0);
				EnableWindow(GetDlgItem(hDlg, IDC_HIST_CMP), FALSE);
			}
			isImg = 1;
			//starting drawing image using Gdiplus class
			HDC hdc = GetDC(hDlg);
			Gdiplus::Graphics graphics(hDlg);
			Gdiplus::Rect rc(580, 20, 245, 160);
			graphics.DrawImage(srcImg, rc);

			wstring w_url(urlImg);
			string url(w_url.begin(), w_url.end());
			src.urlImg = url;
			wstring w_name(title);
			string name(w_name.begin(), w_name.end());
			src.nameImg = name;
			//loading image into src image elements
			src.srcImg = imread(src.urlImg, IMREAD_COLOR);
			if (isImg == true && isDatabase == true) {
				EnableWindow(GetDlgItem(hDlg, IDC_HIST_CMP), TRUE);
			}
		}
		break;

		case IDC_BROWSE2:
		{
			fileInitial(hDlg, 0);//to open dialog with method 1 mean the tail is .txt
			if (!OpenFileDialog(hDlg, urlImg, title))
			{
				break;
			}
			arr.clear();
			SetDlgItemText(hDlg, IDC_EDIT2, urlImg);
			wstring w_fname(urlImg);
			string fname(w_fname.begin(), w_fname.end());
			readFile(fname, arr, isGbin);//read a file txt from fname is url link we get above line
			isDatabase = true;
			if (arr.empty()) 
			{//if readfile is failure
				MessageBox(hDlg, L"File is empty", L"Notice", 0);
				isDatabase = false;
			}
			if (isGbin) 
			{//case we have gBin
				SetDlgItemText(hDlg, IDC_INFORM, L"Image Database is loaded with gBin = 32");
			}
			else {//case we don't have gbin
				SetDlgItemText(hDlg, IDC_INFORM, L"Image Database is loaded with no gBin");
			}
			if (isImg == true && isDatabase == true) {
				EnableWindow(GetDlgItem(hDlg, IDC_HIST_CMP), TRUE);
			}
			SetDlgItemText(hDlg, IDC_EDIT3, L"");
			EnableWindow(GetDlgItem(hDlg, IDC_EDGE_CMP), FALSE);
		}
		break;

		case IDC_BROWSE3:
		{
			fileInitial(hDlg, 0);//to open dialog with method 1 mean the tail is .txt
			if (!OpenFileDialog(hDlg, urlImg, title))
			{
				break;
			}
			arr.clear();
			SetDlgItemText(hDlg, IDC_EDIT3, urlImg);
			wstring w_fname(urlImg);
			string fname(w_fname.begin(), w_fname.end());
			readEdgeDb(arr);//read a file txt from fname is url link we get above line
			isDatabase = true;

			if (arr.empty())
			{//if readfile is failure
				MessageBox(hDlg, L"File is empty", L"Notice", 0);
				isDatabase = false;
			}
			SetDlgItemText(hDlg, IDC_INFORM, L"Image Database is loaded with no gBin");
			SetDlgItemText(hDlg, IDC_EDIT2, L"");
			EnableWindow(GetDlgItem(hDlg, IDC_HIST_CMP), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_EDGE_CMP), TRUE);

		}
		break;

		case IDC_CALC_HISTS:
		{
		
			arr.clear();
			BROWSEINFO infos;
			infos.hwndOwner = NULL;
			infos.pidlRoot = NULL;
			infos.pszDisplayName = urlImg;
			infos.lpszTitle = TEXT("Select an Image Folder");
			infos.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
			infos.lpfn = NULL;
			infos.lParam = 0;
			LPITEMIDLIST pidl;
			if (pidl = SHBrowseForFolder(&infos))
			{
				// Copy the path directory to the buffer
				if (SHGetPathFromIDList(pidl, urlFolder))
				{
					wstring w_url_folder(urlFolder);
					string url_folder(w_url_folder.begin(), w_url_folder.end());
					find_files(arr, url_folder);
				}
			}
			else
				break;

			if (arr.empty())
			{
				MessageBox(hDlg, L"Folder doesn't contain any image", L"Notice", 0);
				break;
			}
			writeFile("co_luong_hoa.txt", "khong_luong_hoa.txt", arr);//when execute loaded image folder, we do it with 2 case
			MessageBox(hDlg, L"Succcess writing file", L"Notice", 0);
			arr.clear();
		}
		break;

		case IDC_EDGE_CMP:
		{
			if (!src.srcImg.empty() && arr.size() > 0) 
			{
				//edgeMatching(arr, src);
				high_resolution_clock::time_point t1 = high_resolution_clock::now();
				edgeMatching_2(arr, src);
				HWND hListView = GetDlgItem(hDlg, IDC_LIST2);
				ListView_DeleteAllItems(hListView);
				//get the amount of image we want to get
				int ItemIndex = SendDlgItemMessage(hDlg, IDC_COMBO1, (UINT)CB_GETCURSEL,(WPARAM)0, (LPARAM)0);
				TCHAR tmp[3];
				SendDlgItemMessage(hDlg, IDC_COMBO1, (UINT)CB_GETLBTEXT,(WPARAM)ItemIndex, (LPARAM)tmp);
				wstring m(tmp);
				string c(m.begin(), m.end());
				// show result in list view
				int amount = stoi(c);
				showResult(hDlg, hListView, arr, amount);
				high_resolution_clock::time_point t2 = high_resolution_clock::now();
				elapsed = duration_cast<milliseconds>(t2 - t1).count();
				string res = to_string(elapsed);
				LPWSTR timeElapsed = Convert_LPWSTR(res);
				SetDlgItemText(hDlg, IDC_TIME, timeElapsed);
			}
		}
		break;

		case IDC_HIST_CMP:
		{
			if (!src.srcImg.empty() && arr.size() > 0) 
			{
				high_resolution_clock::time_point t1 = high_resolution_clock::now();
				bhatta_distance(arr, src, isGbin);//use bHattan Method 4 to compare histogram
				HWND hListView = GetDlgItem(hDlg, IDC_LIST2);
				ListView_DeleteAllItems(hListView);
				//get the amount of image we want to get
				int ItemIndex = SendDlgItemMessage(hDlg, IDC_COMBO1, (UINT)CB_GETCURSEL,(WPARAM)0, (LPARAM)0);
				TCHAR tmp[3];
				SendDlgItemMessage(hDlg, IDC_COMBO1, (UINT)CB_GETLBTEXT,(WPARAM)ItemIndex, (LPARAM)tmp);
				wstring m(tmp);
				string c(m.begin(), m.end());
				// show result in list view
				int amount = stoi(c);
				showResult(hDlg, hListView, arr, amount);
				high_resolution_clock::time_point t2 = high_resolution_clock::now();
				elapsed = duration_cast<milliseconds>(t2 - t1).count();
				string res = to_string(elapsed);
				LPWSTR timeElapsed = Convert_LPWSTR(res);
				SetDlgItemText(hDlg, IDC_TIME, timeElapsed);
			}
		}
		break;

		case IDOK:
		{
			EndDialog(hDlg, LOWORD(wParam));
			DestroyWindow(GetParent(hDlg));
			return (INT_PTR)TRUE;
		}
		break;

		case IDCANCEL:
		{
			EndDialog(hDlg, LOWORD(wParam));
			DestroyWindow(GetParent(hDlg));
			return (INT_PTR)TRUE;
		}
		break;
		}
	}
	return (INT_PTR)FALSE;
}


// for edge matching functionality
void readEdgeDb(vector<Database>& db)
{
	ifstream file("so_sanh_canh.txt", ios::in);

	if (file.is_open())
	{
		int fsize = 0;
		file >> fsize;
		if (fsize == 0)
		{
			return;
		}
		int bin = 0;
		file >> bin;
		
		for (int i = 0; i < fsize; i++)
		{
			Database tmp;
			file >> tmp.nameImg;
			file >> tmp.urlImg;
			int tmp_cur;

			
			Mat tmp_channel = Mat::zeros(bin, 1, CV_32FC1);
			for (int i = 0; i < bin; i++) {
				file >> tmp_cur;
				tmp_channel.at<float>(i, 0) = tmp_cur;
			}
			tmp.hists.push_back(tmp_channel);
		
			db.push_back(tmp);
			tmp.hists.clear();
		}
		file.close();
	}
	else
	{
		return;
	}
}

BOOL OpenFileDialog(HWND hWnd, PTSTR _name, PTSTR _title) {
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = _name;
	ofn.lpstrFileTitle = _title;
	ofn.Flags = 0;
	return GetOpenFileName(&ofn);
}

void readFile(string fname, vector<Database> &hist, bool &gbin) {
	ifstream file;
	file.open(fname);
	if (file.is_open()) 
	{
		int fsize = 0;
		file >> fsize;
		if (fsize == 0) 
		{
			return;
		}
		int bin = 0;
		file >> bin;
		if (bin == 256) {
			gbin = 0;;
		}
		else {
			gbin = 1;
		}
		for (int i = 0; i < fsize; i++) 
		{
			Database tmp;
			file >> tmp.nameImg;
			file >> tmp.urlImg;
			int tmp_cur;

			for (int ch = 0; ch < 3; ch++) 
			{
				Mat tmp_channel = Mat::zeros(bin, 1, CV_32FC1);
				for (int b = 0; b < bin; b++) {
					file >> tmp_cur;
					tmp_channel.at<float>(b, 0) = tmp_cur;
				}
				tmp.hists.push_back(tmp_channel);
			}
			hist.push_back(tmp);
			tmp.hists.clear();
		}
		file.close();
	}
	else 
	{
		return;
	}
}

void writeFile(string fname1, string fname2, vector<Database>hist) 
{
	//case 1 for co_luong_hoa.txt
	ofstream file1;
	file1.open(fname1);
	if (file1.is_open()) 
	{
		file1 << hist.size() << endl;
		file1 << 32 << endl;

		for (int i = 0; i < hist.size(); i++) {
			file1 << hist[i].nameImg << endl;
			file1 << hist[i].urlImg << endl;

			vector<Mat> hist_arr = calc_Img(hist[i].srcImg, true);
			for (int ch = 0; ch < 3; ch++) {
				for (int gbin = 0; gbin < 32; gbin++) 
				{
					file1 << hist_arr[ch].at<float>(gbin) << " ";
				}
				file1 << endl;
			}
		}
		file1.close();
	}

	//case 2 for khong_co_luong_hoa.txt
	ofstream file2;
	file2.open(fname2);
	if (file2.is_open())
	{
		file2 << hist.size() << endl;
		file2 << 256 << endl;

		for (int i = 0; i < hist.size(); i++) 
		{
			file2 << hist[i].nameImg << endl;
			file2 << hist[i].urlImg << endl;

			vector<Mat> hist_arr = calc_Img(hist[i].srcImg, false);
			for (int ch = 0; ch < 3; ch++)
			{
				for (int k = 0; k < 256; k++)
				{
					file2 << hist_arr[ch].at<float>(k) << " ";
				}
				file2 << endl;
			}
		}

		file2.close();
	}	
	

	// for edge matching method
	ofstream file3;
	file3.open("so_sanh_canh.txt");
	if (file3.is_open())
	{
		file3 << hist.size() << endl;
		file3 << 256 << endl;

		for (int i = 0; i < hist.size(); i++)
		{
			file3 << hist[i].nameImg << endl;
			file3 << hist[i].urlImg << endl;

			Mat edge_Mat = calcEdgeMat(hist[i].srcImg);
			Mat edge_Hist = calcOneHist(edge_Mat);
			for (int k = 0; k < 256; k++)
			{
				file3 << edge_Hist.at<float>(k) << " ";
			}
			file3 << endl;
		}

		file3.close();
	}
}

	

void fileInitial(HWND hWnd, bool choose) {
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.hInstance = NULL;
	if (choose)
		ofn.lpstrFilter = TEXT("All file\0 * .*\0");
	else
		ofn.lpstrFilter = TEXT("Txt file\0 * .txt*\0");
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = NULL; // Set in Open and Close functions
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = NULL; // Set in Open and Close functions
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = NULL;
	ofn.Flags = 0; // Set in Open and Close functions
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = TEXT("child");
	ofn.lCustData = 0;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;
}