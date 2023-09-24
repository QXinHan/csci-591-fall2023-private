# Project2: PE Headers

- **Name:** Zhuoyun Qian (QXinHan)
- **Class:** CSCI 591/594
- **Email:** 3290862073@qq.com _I forgot password of my NENU email and it is troublesome to find back my password._

**Link to assignment:** [My Project1](https://github.com/QXinHan/csci-591-fall2023-private/tree/main/project1)

## Task 2: A Demo of Tasks with SubTasks

### Task 2(a): The difference of ImageBase between 32-bit app and 64-bit app

**summary of problem**

_This subtask is to print the values in Section Headers, and we will use two structures, `PIMAGE_NT_HEADERS32` and `PIMAGE_NT_HEADERS64`. While parsing a 32-bit application, if we use `PIMAGE_NT_HEADERS64` or `PIMAGE_NT_HEADERS` to define a pointer `pNt`, it will cause some problem. The wrong code as follows:_

```c++
 PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
    PIMAGE_NT_HEADERS64 pNt = (PIMAGE_NT_HEADERS64)((char*)file + pDos->e_lfanew);
    PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((char*)pNt + 4);
    PIMAGE_SECTION_HEADER pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
```

_Please look at this code, if we are parsing 32-bit application, we can't get the result we want. Because the size of `ImageBase` is 4 bytes in 32-bit exe but 8 bytes in 64-bit exe._

**solution of problem**

_Firstly I need to judge that the exe is 32-bit of 64-bit. The code as follows:_

```c++
bool Jude32or64(FILE* file)
{
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
    PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)((char*)file + pDos->e_lfanew);
    PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((char*)pNt + 4);
    PIMAGE_OPTIONAL_HEADER pOptionalHeader = PIMAGE_OPTIONAL_HEADER((char*)pFileHeader + sizeof(IMAGE_FILE_HEADER));
    if (pOptionalHeader->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
    {
        return 1;
    }
    return 0;
}
```

_Actually this code was written two weeks ago, so there are some codes can be optimized._

_Then I parse the section header separately. The code as follows:_

```C++
+void parseSecHeader64(FILE* file)
{
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
    PIMAGE_NT_HEADERS64 pNt = (PIMAGE_NT_HEADERS64)((char*)file + pDos->e_lfanew);
    PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((char*)pNt + 4);
    PIMAGE_SECTION_HEADER pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
    size_t numberOfSec = pFileHeader->NumberOfSections;
    printf("---------------------------------------------\n");
    printf("SECTION_HEADER\n");
    for (size_t i = 0; i < numberOfSec; i++)
    {
        PIMAGE_SECTION_HEADER pSecHeader = pSec + i;
        char name[9] = {};
        memcpy(name, pSecHeader->Name, 8);
        printf("Name:%s\n", name);
        printf("VirtualSize:%x\n", pSecHeader->Misc.VirtualSize);
        printf("VirtualAddress:%x\n", pSecHeader->VirtualAddress);
        printf("SizeOfRawData:%x\n", pSecHeader->SizeOfRawData);
        printf("PointerToRawData:%x\n", pSecHeader->PointerToRawData);
        printf("PointerToRelocations:%x\n", pSecHeader->PointerToRelocations);
        printf("PointerToLinenumbers:%x\n", pSecHeader->PointerToLinenumbers);
        printf("NumberOfRelocations:%x\n", pSecHeader->NumberOfRelocations);
        printf("NumberOfLinenumbers:%x\n", pSecHeader->NumberOfLinenumbers);
        printf("Characteristics:%x\n", pSecHeader->Characteristics);
    }
}


void parseSecHeader32(FILE* file)
{
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
    PIMAGE_NT_HEADERS32 pNt = (PIMAGE_NT_HEADERS32)((char*)file + pDos->e_lfanew);
    PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((char*)pNt + 4);
    PIMAGE_SECTION_HEADER pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
    size_t numberOfSec = pFileHeader->NumberOfSections;
    printf("---------------------------------------------\n");
    printf("SECTION_HEADER\n");
    for (size_t i = 0; i < numberOfSec; i++)
    {
        PIMAGE_SECTION_HEADER pSecHeader = pSec + i;
        char name[9] = {};
        memcpy(name, pSecHeader->Name, 8);
        printf("Name:%s\n", name);
        printf("VirtualSize:%x\n", pSecHeader->Misc.VirtualSize);
        printf("VirtualAddress:%x\n", pSecHeader->VirtualAddress);
        printf("SizeOfRawData:%x\n", pSecHeader->SizeOfRawData);
        printf("PointerToRawData:%x\n", pSecHeader->PointerToRawData);
        printf("PointerToRelocations:%x\n", pSecHeader->PointerToRelocations);
        printf("PointerToLinenumbers:%x\n", pSecHeader->PointerToLinenumbers);
        printf("NumberOfRelocations:%x\n", pSecHeader->NumberOfRelocations);
        printf("NumberOfLinenumbers:%x\n", pSecHeader->NumberOfLinenumbers);
        printf("Characteristics:%x\n", pSecHeader->Characteristics);
    }
}

```

_I need to optimize this part of the code, The repeatability of this part is too high. By 24/9/2023 I have no idea to do that._



## Task3: What I learned

_Know how to optimize the code above will be the biggest gain, I am going to do that!_