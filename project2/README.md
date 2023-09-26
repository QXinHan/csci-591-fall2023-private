# 	Project2: Tables

- **Name :** Zhuoyun Qian
- **Email :** 3290862073@qq.com
- **Due :** 9/26/2023

**Link to assignment:** [project2](https://github.com/QXinHan/csci-591-fall2023-private/tree/main/project2)

## Task 2: A Demo of Tasks with SubTasks

### Task 2(a): problem about zero filled structure in IAT

_As we know, we can use `pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddres` to find the structure `PIMAGE_THUNK_DATA64` (`PIMAGE_THUNK_DATA32` in x86)of each data. The data is continuous but there is some zero filled structure. So we can not decide whether we parse all of the data just by judging if the next structure is filled with zero. The code as follows:_

```c++
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
    PIMAGE_NT_HEADERS64 pNt = (PIMAGE_NT_HEADERS64)((DWORD64)file + pDos->e_lfanew);
    if (!pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress && !pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].Size)
    {
        printf("There is no IAT\n");
        return;
    }
    DWORD64 Foa = RVATOFOA(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress);
    PIMAGE_THUNK_DATA64 pIatTable = (PIMAGE_THUNK_DATA64)((DWORD64)file + Foa);
    DWORD numOfTables = pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].Size / sizeof(IMAGE_THUNK_DATA64);

    for (DWORD i = 0; i < numOfTables; i++)
        {
        if (!pIatTable->u1.AddressOfData) continue;
            if (pIatTable->u1.AddressOfData & IMAGE_ORDINAL_FLAG64)
            {
                printf("Ordinal:%x\n", (pIatTable->u1.Ordinal) & 0x7fffffffffffffff);
            }
            else
            {
                Foa = RVATOFOA(file, pIatTable->u1.AddressOfData);
                PIMAGE_IMPORT_BY_NAME pImpourByName = (PIMAGE_IMPORT_BY_NAME)((DWORD64)file + Foa);
                printf("%s\n", pImpourByName->Name);
            }
            pIatTable++;
        }
    
}
```

_I store the number of thunks of the IAT, and go through the thunks._

### Task 2(b): Bound Import Table

_Notice! the two executable file doesn't have bound import table so I do not whether my code can parse the bound import table successfully. The code as follows:_

```c++
void parseBoundImportTable(FILE* file)
{
    if (Jude32or64(file)) {
        PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
        PIMAGE_NT_HEADERS64 pNt = (PIMAGE_NT_HEADERS64)((DWORD64)file + pDos->e_lfanew);
        if (!pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].Size && !pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress)
        {
            printf("There is no bound import table\n");
            return;
        }
        DWORD64 Foa = RVATOFOA(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress);
        PIMAGE_BOUND_IMPORT_DESCRIPTOR pboundImportDes = (PIMAGE_BOUND_IMPORT_DESCRIPTOR)((DWORD64)file + Foa);
        while (true)
        {
            if (pboundImportDes == 0) break;
            char* name = (char*)((DWORD64)file + RVATOFOA(file,pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress + pboundImportDes->OffsetModuleName));
            printf("BoundDLLName:%s\n", name);
            DWORD numOfDll = pboundImportDes->NumberOfModuleForwarderRefs;
            PIMAGE_BOUND_FORWARDER_REF pBoundRef = (PIMAGE_BOUND_FORWARDER_REF)((DWORD64)pboundImportDes + sizeof(PIMAGE_BOUND_IMPORT_DESCRIPTOR));
            for (DWORD i = 0; i < numOfDll; i++)
            {
                //The dll is used by the bounded dll.
                name = (char*)((DWORD64)file + RVATOFOA(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress + pBoundRef->OffsetModuleName));
                printf("boundDllName:%s\n", name);
            }
            pboundImportDes++;
        }
    }
    else {
        PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
        PIMAGE_NT_HEADERS32 pNt = (PIMAGE_NT_HEADERS32)((DWORD64)file + pDos->e_lfanew);
        if (!pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].Size && !pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress)
        {
            printf("There is no bound import table\n");
            return;
        }
        DWORD64 Foa = RVATOFOA32(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress);
        PIMAGE_BOUND_IMPORT_DESCRIPTOR pboundImportDes = (PIMAGE_BOUND_IMPORT_DESCRIPTOR)((DWORD64)file + Foa);
        while (true)
        {
            if (pboundImportDes == 0) break;
            char* name = (char*)((DWORD64)file + RVATOFOA32(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress + pboundImportDes->OffsetModuleName));
            printf("BoundDLLName:%s\n", name);
            DWORD numOfDll = pboundImportDes->NumberOfModuleForwarderRefs;
            PIMAGE_BOUND_FORWARDER_REF pBoundRef = (PIMAGE_BOUND_FORWARDER_REF)((DWORD64)pboundImportDes + sizeof(PIMAGE_BOUND_IMPORT_DESCRIPTOR));
            for (DWORD i = 0; i < numOfDll; i++)
            {
                //The dll is used by the bounded dll.
                name = (char*)((DWORD64)file + RVATOFOA32(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress + pBoundRef->OffsetModuleName));
                printf("boundDllName:%s\n", name);
            }
            pboundImportDes++;
        }
    }
}
```

### Task 2(c): A bug of parsing Resource Table

_While parsing the Resource Table of the 32-bit executable file there is a bug in my code but now I am too tired to solve it, I need to have a break._ The code as follows:

```c++
else
{
    PIMAGE_NT_HEADERS32 pNt32 = (PIMAGE_NT_HEADERS32)((DWORD64)file + pDos->e_lfanew);
    DWORD64 a = RVATOFOA32(file, pNt32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress);
    pRescDir1 = (PIMAGE_RESOURCE_DIRECTORY)((DWORD64)file + RVATOFOA32(file, pNt32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress));

    numOfNameEntries1 = pRescDir1->NumberOfNamedEntries;
    numOfIdEntries1 = pRescDir1->NumberOfIdEntries;
    for (WORD i = 0; i < numOfIdEntries1 + numOfNameEntries1; i++)
    {
        PIMAGE_RESOURCE_DIRECTORY_ENTRY pRescDirEntry1 = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD64)pRescDir1 + sizeof(IMAGE_RESOURCE_DIRECTORY) + i * 8);
        if (pRescDirEntry1->NameIsString)
        {
            PIMAGE_RESOURCE_DIR_STRING_U pNameStr = (PIMAGE_RESOURCE_DIR_STRING_U)(pRescDirEntry1->NameOffset);
            wprintf(L"%s\n", pNameStr->NameString);
        }
        else
        {
            printf("Res1 ID: %x\n", pRescDirEntry1->Id);
        }
        if (pRescDirEntry1->DataIsDirectory)
        {
            PIMAGE_RESOURCE_DIRECTORY pRescDir2 = (PIMAGE_RESOURCE_DIRECTORY)((DWORD64)pRescDir1 + (pRescDirEntry1->OffsetToData & 0x7fffffff));

            WORD numOfNameEntries2 = pRescDir2->NumberOfNamedEntries;
            WORD numOfIdEntries2 = pRescDir2->NumberOfIdEntries;

            for (WORD j = 0; j < numOfIdEntries2 + numOfNameEntries2; j++)
            {
                PIMAGE_RESOURCE_DIRECTORY_ENTRY pRescDirEntry2 = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD64)pRescDir2 + sizeof(IMAGE_RESOURCE_DIRECTORY) + j * 8);
                if (pRescDirEntry2->NameIsString)
                {
                    PIMAGE_RESOURCE_DIR_STRING_U pNameStr = (PIMAGE_RESOURCE_DIR_STRING_U)(pRescDirEntry2->NameOffset);
                    wprintf(L"%s\n", pNameStr->NameString);
                }
                else
                {
                    printf("Res2 ID: %x\n", pRescDirEntry2->Id);
                }
                if (pRescDirEntry2->DataIsDirectory)
                {
                    PIMAGE_RESOURCE_DIRECTORY pRescDir3 = (PIMAGE_RESOURCE_DIRECTORY)((DWORD64)pRescDir1 + (pRescDirEntry2->OffsetToData & 0x7fffffff));

                    WORD numOfNameEntries3 = pRescDir3->NumberOfNamedEntries;
                    WORD numOfIdEntries3 = pRescDir3->NumberOfIdEntries;

                    for (WORD k = 0; k < numOfIdEntries3 + numOfNameEntries3; k++)
                    {
                        PIMAGE_RESOURCE_DIRECTORY_ENTRY pRescDirEntry3 = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD64)pRescDir3 + sizeof(IMAGE_RESOURCE_DIRECTORY) + k * 8);
                        if (pRescDirEntry2->NameIsString)
                        {
                            PIMAGE_RESOURCE_DIR_STRING_U pNameStr = (PIMAGE_RESOURCE_DIR_STRING_U)(pRescDirEntry3->NameOffset);
                            wprintf(L"%s\n", pNameStr->NameString);
                        }
                        else
                        {
                            printf("Res3 ID: %x\n", pRescDirEntry3->Id);
                        }
                        PIMAGE_DATA_DIRECTORY pDataDir = (PIMAGE_DATA_DIRECTORY)((DWORD64)pRescDir1 + (pRescDirEntry3->OffsetToData & 0x7fffffff));
                        printf("RVA:%x\n", pDataDir->VirtualAddress);
                        printf("SIZE:%x\n", pDataDir->Size);
                    }
                }
            }
        }
    }
}
```

_It is too long, when I am energetic I will solve the problem._

## What I learned from this project

_I learned how a executable use the function in other DLL, and have written a program for DLL injection._