# myproject
- **Name:** Zhuoyun Qian
- **Email:** 3290862073@qq.com
- **GitCreateTime:** 8/12/2023
## project 1
- **Target:** finsh the project3 and upload the source code it to github.
- **Link:** [Project3](https://fangtian-zhong.github.io/project3.html)
- **Project Details:**
 1. I develop program1.cpp,the function of it is to print "hello program1".
 2. I develop program3.cpp,the function of it is to add a secion `.shell` at the last of program2.exe.The data of section `.shell` is the encrypted program1.exe.
 3. I develop program2.cpp,the function as follows:
    * print "hello program2",I think it is a symbol,giving the user a signal that the program2.exe runs correctly.
    * read and decrypt the last section of program2.exe to get program1.exe.`GetModuleHandle`
    * parse the program1.exe for its OEP and ImageBase.
    * stretch the program1.exe and load it in the buffer.
    * use API `CreateProcess` to create a suspended process.
    * get the context of suspended process,aiming to get the OEP and ImageBase stored in register.
    * uinistall the image of program2.exe in the suspended process.`NtUnmapViewOfSection`
    * use API `VirtualAllocEx`to allocate memory at the specified location which is "ImageBase" of program1.exe for program1.exe in the suspended process.Maybe need to repair the relocation table of the program1.exe
    * use API `WriteProcessMemory` to copy the program1.exe to the suspended process.
    * change the context of the program2.exe(the image of it is uninstalled).Change the base to "Imagebase" of program1.exe and change OEP to the "OEP" of program1.exe.
    * set the context and return to the main thread.'SetThreadContext' and `ResumeThread`
    * SUCCESS! 
   
