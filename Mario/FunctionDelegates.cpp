
#include "stdafx.h"



FunctionDelegates::FunctionDelegates()
{
	HMODULE hNtdll = GetModuleHandle(TEXT("ntdll"));
	if (NULL == hNtdll)
	{
		OutputDebugString(TEXT("FunctionDelegates::FunctionDelegates :: error fetching handle for NTDLL.dll\n"));
		return;
	}
	pfnNtCreateNamedPipe = (PFN_NT_CREATEA_NAMED_PIPE)GetProcAddress(hNtdll, "NtCreateNamedPipeFile");
	pfnNtCreateFile = (PFN_NT_CREATE_FILE)GetProcAddress(hNtdll, "NtCreateFile");
	pfnNtClose = (PFN_NT_CLOSE)GetProcAddress(hNtdll, "NtClose");
	pfnNtDuplicateObject = (PFN_NT_DUPLICATE_OBJECT)GetProcAddress(hNtdll, "NtDuplicateObject");
	pfnNtWriteFile = (PFN_NT_WRITE_FILE)GetProcAddress(hNtdll, "NtWriteFile");
	pfnNtReadFile = (PFN_NT_READ_FILE)GetProcAddress(hNtdll, "NtReadFile");
	pfnNtFsControlFile = (PFN_NT_FS_CONTROL_FILE)GetProcAddress(hNtdll, "NtFsControlFile");

}



FunctionDelegates::~FunctionDelegates()
{
}

FunctionDelegates& FunctionDelegates::GetInstance()
{
	static FunctionDelegates instance;
	
	return instance;
}
