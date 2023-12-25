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

## Task1: analyze the malware1.c


* please pay attention! I find the node of the graph I set can not make the malicious sequences clear. One of the reason is that some malicious code is not a whole block of function,  but just a few lines of code. So the node will not only represent the function name but also a line of code, serval lines of code, etc.

_The graph:_



_The result that displays on the console:_



_From malware1.c to malware5 I will summarize the commonly used apis and structures._
