# README

- **Name :** Zhuoyun Qian

- **Email :** qianzhuoyun@nenu.edu.cn

  

## Task1: analyze the malware1.c

 _The Depth-First-Search has problems!!!!!!!!!!!!! I will solve it, now I am tired. 2023-12-22 17:22_

_The malicious invocation relationship I had found._

```c++
//#1 : main -> CopyFile
//#2 : main -> infzip -> WinExec
```

## Task2: analyze the malware3.c

* please pay attention! I find the node of the graph I set can not make the malicious sequences clear. One of the reason is that some malicious code is not a whole block of function,  but just a few lines of code. So the node will not only represent the function name but also a line of code, serval lines of code, etc.

_The graph:_

![imageg](https://github.com/QXinHan/csci-591-fall2023-private/blob/main/Assignment4/images/malware3_graph.bmp)

_The result that displays on the console:_

![imagec](https://github.com/QXinHan/csci-591-fall2023-private/blob/main/Assignment4/images/malware3_console.bmp)

_From malware1.c to malware5 I will summarize the commonly used apis and structures._

## Task3: analyze the malware4.c

_The graph:_

![imageg2](https://github.com/QXinHan/csci-591-fall2023-private/blob/main/Assignment4/images/malware3_graph.bmp)

_The result that displays on the console:_

![imagec2](https://github.com/QXinHan/csci-591-fall2023-private/blob/main/Assignment4/images/malware4_console1.bmp)

![imagec3](https://github.com/QXinHan/csci-591-fall2023-private/blob/main/Assignment4/images/malware3_console2.bmp)

## Task4: analyze the malware2.c

_**it is `Rootkit` **_

_**Its purpose:**_ this program targets to hide a file named `WindowsSecurityTask` and a executable file named `csrss.exe`, so it hooks some kernel APIs.
