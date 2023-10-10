/*
* This is a test for multithread
* This is a Win32 program, there are three threads uesd for grabing red envelop.
* there are four text Edits. The first text edit is the money, and others grab the money.
*/

#include "framework.h"
#include "WindowsProject2.h"
#include<stdio.h>
#define MAX_LOADSTRING 100


DWORD money;/*the money that will be grabed*/
CRITICAL_SECTION cs1;
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);/*used for handling the event provided by the main dialog*/
/*
* the first parameter is the handle to dialog box
* the second one is message produced by the dialog box
* the third and fourth one are used to specify additional information of the message. 
*/
DWORD WINAPI ThreadProc1(LPVOID lpParam);
DWORD WINAPI ThreadProc2(LPVOID lpParam);
DWORD WINAPI ThreadProc3(LPVOID lpParam);
/*used to define the function of each thread*/
HWND Edit;
HWND Edit1;
HWND Edit2;
HWND Edit3;
/*the handles to the four text*/
HANDLE hThread[3];
HANDLE hThread1;/*used to create the other threads*/
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    InitializeCriticalSection(&cs1);
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), NULL, About);
    /*
    * first:the handle to present module
    * second:the ID of a dialog
    * third:NULL means the present window owns the dialog box
    * fourth:The address of the function used to deal with messages produced by dialog box
    */
    return 0;
}
DWORD WINAPI ThreadProc1(
    LPVOID lpParam
)
{
    money = 0;
    DWORD money1 = 0;
    TCHAR buffer[10];
    memset(buffer, 0, 10);
    while (TRUE)
    {
        EnterCriticalSection(&cs1);/*occupy the source*/
        GetWindowText(Edit, buffer, 10);
        swscanf_s(buffer, L"%d", &money);/*get the money in the first text and change it to 'int'*/
        if (money < 50) break;
        memset(buffer, 0, 10);
        money -= 50;
        money1 += 50;/*grap 50 from the first text*/
        swprintf_s(buffer, 10, L"%d", money);
        SetWindowText(Edit, buffer);/*update the first text (The value of it decrements 50)*/
        memset(buffer, 0, 10);
        swprintf_s(buffer, 10, L"%d", money1);/*update  text (The value of it increase by 50)*/
        SetWindowText(Edit1, buffer);
        LeaveCriticalSection(&cs1);
        Sleep(200);
    }
    return 0;
}
/*The next ThreadProc2 and ThreadProc3 are the same*/
DWORD WINAPI ThreadProc2(
    LPVOID lpParam
)
{
    DWORD money2 = 0;
    TCHAR buffer[10];
    memset(buffer, 0, 10);
    while (TRUE)
    {
        EnterCriticalSection(&cs1);
        GetWindowText(Edit, buffer, 10);
        swscanf_s(buffer, L"%d", &money);
        if (money < 50) break;
        memset(buffer, 0, 10);
        money -= 50;
        money2 += 50;
        swprintf_s(buffer, 10, L"%d", money);
        SetWindowText(Edit, buffer);
        memset(buffer, 0, 10);
        swprintf_s(buffer, 10, L"%d", money2);
        SetWindowText(Edit2, buffer);
        LeaveCriticalSection(&cs1);
        Sleep(200);
    }

    return 0;
}
DWORD WINAPI ThreadProc3(
    LPVOID lpParam
)
{
    DWORD money3 = 0;
    TCHAR buffer[10];
    memset(buffer, 0, 10);
    while (TRUE)
    {
        EnterCriticalSection(&cs1);
        GetWindowText(Edit, buffer, 10);
        swscanf_s(buffer, L"%d", &money);
        if (money < 50) break;
        memset(buffer, 0, 10);
        money -= 50;
        money3 += 50;
        swprintf_s(buffer, 10, L"%d", money);
        SetWindowText(Edit, buffer);
        memset(buffer, 0, 10);
        swprintf_s(buffer, 10, L"%d", money3);
        SetWindowText(Edit3, buffer);
        LeaveCriticalSection(&cs1);
        Sleep(200);
    }

    return 0;
}

DWORD WINAPI ThreadProc(
        LPVOID lpParam
)
{
        /*The function is used to create three thread*/
  
        hThread[0] = CreateThread(NULL, 0, ThreadProc1,NULL ,NULL, NULL);
        hThread[1] = CreateThread(NULL, 0, ThreadProc2, NULL, NULL, NULL);
        hThread[2] = CreateThread(NULL, 0, ThreadProc3, NULL, NULL, NULL);
        WaitForMultipleObjects(3,hThread, TRUE, INFINITE);
        CloseHandle(hThread[0]);
        CloseHandle(hThread[1]);
        CloseHandle(hThread[2]);
    return 0;
}


INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:/*to initialize four texts set the value of them to zero*/
    {
        Edit = GetDlgItem(hDlg, IDC_EDIT1);
        Edit1 = GetDlgItem(hDlg, IDC_EDIT2);
        Edit2 = GetDlgItem(hDlg, IDC_EDIT3);
        Edit3 = GetDlgItem(hDlg, IDC_EDIT4);
        SetWindowText(Edit, L"0");
        SetWindowText(Edit1, L"0");
        SetWindowText(Edit2, L"0");
        SetWindowText(Edit3, L"0");
        return (INT_PTR)TRUE;
    }
    case WM_CLOSE:
    {
        EndDialog(hDlg, 0);
        break;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON1:/*after clicking the button "开始", update the value to zero, and create the new thread which is for controlling the other three threads*/
        {
            TCHAR szBuff[10];
            memset(szBuff, 0, 10);
            GetWindowText(Edit, szBuff, 10);
            SetWindowText(Edit, szBuff);
            SetWindowText(Edit1, L"0");
            SetWindowText(Edit2, L"0");
            SetWindowText(Edit3, L"0");
            hThread1 = CreateThread(NULL, 0, ThreadProc, NULL, NULL, NULL);
            CloseHandle(hThread1);
            return (INT_PTR)TRUE;
        }
        }
    }
        break;
    }
    return (INT_PTR)FALSE;
}
