# Project4

- **Name :** Zhuoyun Qian
- **Email :** 3290862073@qq.com
- **Due :** 5/3/2024

**Link to assignment :**[csci-591-fall2023-private/project4 at main · QXinHan/csci-591-fall2023-private (github.com)](https://github.com/QXinHan/csci-591-fall2023-private/tree/main/project4)



## Task1: Parent Process

**Target: ** Launch the Process Explorer as Administrator. Find the name of the parent process, covered by a green box in the image below.  

![task1_1](.\task1_1.bmp)

**Solution:**

_Launch the WinDbg.exe as Administrator. In WinDbg.exe click **File**, `Launch executable`._

![task1_2](.\task1_2.bmp)

_Navigate to `C:\Windows\System32\notepad.exe`, and open it._

![task1_3](.\task1_3.bmp)

_Launch the Process Explorer, scroll down to find flag._

![task1_4](.\task1_4.bmp)

**Finally, the flag is `EngHost`.**

## Task2: Crash Message

**Target: **Find the flag covered by a green box in the image below. 

![task2_1](.\task2_1.bmp)

**Solution:**

_Use `x64 Native Tools Command Prompt ` to create a C++ program._

_In the `x64 Native Tools Command Prompt` window, execute these commands:_

```
mkdir c:\MyApp
cd c:\MyApp
notepad MyApp.cpp
```

_Click **Yes** to create a new file._

_Paste in this code, as shown below._

```c++
void MyFunction(long p1, long p2, long p3)
{
    long x = p1 + p2 + p3;
    long y = 0;
    y = x / p2;
}
void main ()
{
    long a = 2;
    long b = 0;
    MyFunction(a, b, 5);
}
In Notepad, save the file.
```

_Then execute `cl/Zi MyApp.cpp` . We get some files._

![task2_2](.\task2_2.bmp)

_Launch the WInDbg.exe, load in the MyApp.exe, directly execute the command `g` in the command line._

![task2_3](.\task2_3.bmp)

**Finally, we can find the flag is `exception`.**