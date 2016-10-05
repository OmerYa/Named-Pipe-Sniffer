#include "stdafx.h"

PipeHooksManager::PipeHooksManager()
{

	pfnNtCreateNamedPipe = FunctionDelegates::GetInstance().pfnNtCreateNamedPipe;
	pfnNtCreateFile = FunctionDelegates::GetInstance().pfnNtCreateFile;
	pfnNtClose = FunctionDelegates::GetInstance().pfnNtClose;
	pfnNtDuplicateObject = FunctionDelegates::GetInstance().pfnNtDuplicateObject;
	pfnNtWriteFile = FunctionDelegates::GetInstance().pfnNtWriteFile;
	pfnNtFsControlFile = FunctionDelegates::GetInstance().pfnNtFsControlFile;
}



PipeHooksManager::~PipeHooksManager()
{
}

PipeHooksManager& PipeHooksManager::GetInstance()
{
	static PipeHooksManager instance;
	return instance;
}


NTSTATUS PipeHooksManager::NtCreateNamedPipeFileHook(
	OUT PHANDLE             NamedPipeFileHandle,
	IN ACCESS_MASK          DesiredAccess,
	IN POBJECT_ATTRIBUTES   ObjectAttributes,
	OUT PIO_STATUS_BLOCK    IoStatusBlock,
	IN ULONG                ShareAccess,
	IN ULONG                CreateDisposition,
	IN ULONG                CreateOptions,
	IN BOOLEAN              WriteModeMessage,
	IN BOOLEAN              ReadModeMessage,
	IN BOOLEAN              NonBlocking,
	IN ULONG                MaxInstances,
	IN ULONG                InBufferSize,
	IN ULONG                OutBufferSize,
	IN PLARGE_INTEGER       DefaultTimeOut)
{
	OutputDebugString(TEXT("PipeHooksManager::NtCreateNamedPipeFileHook :: enter\n"));
	
	NTSTATUS retVal = GetInstance().pfnNtCreateNamedPipe(
		NamedPipeFileHandle,
		DesiredAccess,
		ObjectAttributes,
		IoStatusBlock,
		ShareAccess,
		CreateDisposition,
		CreateOptions,
		WriteModeMessage,
		ReadModeMessage,
		NonBlocking,
		MaxInstances,
		InBufferSize,
		OutBufferSize,
		DefaultTimeOut);

	if (STATUS_SUCCESS == retVal)
	{
		if (INVALID_HANDLE_VALUE != *NamedPipeFileHandle)
		{
			OutputDebugString(ObjectAttributes->ObjectName->Buffer);
			OutputDebugString(TEXT("PipeHooksManager::NtCreateNamedPipeFileHook :: added NamedPipe\n"));
			GetInstance().lstPipes.AddPipeHandle(ObjectAttributes->ObjectName, *NamedPipeFileHandle);
		}
	}


	OutputDebugString(TEXT("PipeHooksManager::NtCreateNamedPipeFileHook :: end\n"));

	return retVal;
}

NTSTATUS PipeHooksManager::NtCreateFileHook(
	_Out_    PHANDLE            FileHandle,
	_In_     ACCESS_MASK        DesiredAccess,
	_In_     POBJECT_ATTRIBUTES ObjectAttributes,
	_Out_    PIO_STATUS_BLOCK   IoStatusBlock,
	_In_opt_ PLARGE_INTEGER     AllocationSize,
	_In_     ULONG              FileAttributes,
	_In_     ULONG              ShareAccess,
	_In_     ULONG              CreateDisposition,
	_In_     ULONG              CreateOptions,
	_In_opt_ PVOID              EaBuffer,
	_In_     ULONG              EaLength
	)
{
	OutputDebugString(TEXT("PipeHooksManager::NtCreateFile :: enter \n"));
	NTSTATUS retVal = GetInstance().pfnNtCreateFile(
		FileHandle,
		DesiredAccess,
		ObjectAttributes,
		IoStatusBlock,
		AllocationSize,
		FileAttributes,
		ShareAccess,
		CreateDisposition,
		CreateOptions,
		EaBuffer,
		EaLength);


	if (STATUS_SUCCESS == retVal)
	{
		if (INVALID_HANDLE_VALUE != *FileHandle &&
			NULL != ObjectAttributes->ObjectName->Buffer)
		{
			OutputDebugString(ObjectAttributes->ObjectName->Buffer);

			// Make sure its a Named Pipe
			if (ObjectAttributes->ObjectName->Buffer == StrStrIW(ObjectAttributes->ObjectName->Buffer, L"\\??\\pipe\\"))
			{
				// Whitelist garbage pipe
				if (ObjectAttributes->ObjectName->Buffer != StrStrIW(ObjectAttributes->ObjectName->Buffer, L"\\??\\pipe\\srvsvc"))
				{
					OutputDebugString(TEXT("PipeHooksManager::NtCreateFile :: added NamedPipe\n"));
					GetInstance().lstPipes.AddPipeHandle(ObjectAttributes->ObjectName, *FileHandle);
				}
			}
		}
		OutputDebugString(TEXT("PipeHooksManager::NtCreateFile :: end \n"));
	}

	return retVal;

}


NTSTATUS PipeHooksManager::NtCloseHook(
	IN HANDLE               ObjectHandle)
{
	NTSTATUS retVal = GetInstance().pfnNtClose(ObjectHandle);

	if (STATUS_SUCCESS == retVal)
	{
		if (TRUE == GetInstance().lstPipes.RemovePipeHandle(ObjectHandle))
		{
			OutputDebugString(TEXT("PipeHooksManager::NtCloseHook :: Removed handle\n"));
		}
	}

	return retVal;

}


NTSTATUS PipeHooksManager::NtDuplicateObjectHook(
	IN HANDLE               SourceProcessHandle,
	IN PHANDLE              SourceHandle,
	IN HANDLE               TargetProcessHandle,
	OUT PHANDLE             TargetHandle,
	IN ACCESS_MASK          DesiredAccess OPTIONAL,
	IN BOOLEAN              InheritHandle,
	IN ULONG                Options)
{
	NTSTATUS retVal = GetInstance().pfnNtDuplicateObject(
		SourceProcessHandle,
		SourceHandle,
		TargetProcessHandle,
		TargetHandle,
		DesiredAccess,
		InheritHandle,
		Options);

	if (STATUS_SUCCESS == retVal)
	{
		DWORD dwErrCode = GetLastError();

		DWORD dwSourceProcessId = GetProcessId(SourceProcessHandle);
		DWORD dwTargetProcessId = GetProcessId(TargetProcessHandle);
		DWORD dwCurrentProcessId = GetCurrentProcessId();
		// Monitor only if we duplicate on the same process
		// TODO : Add monitoring of handles between processes
		if ((dwSourceProcessId == dwTargetProcessId) && (dwCurrentProcessId == dwSourceProcessId))
		{
			if (TRUE == GetInstance().lstPipes.DuplicatePipeHandle(SourceHandle, *TargetHandle))
			{
				OutputDebugString(TEXT("PipeHooksManager::NtDuplicateObjectHook :: Handle duplicated\n"));
			}

			// TODO: Check if we need to monitor DUPLICATE_CLOSE_SOURCE flag or does it call CloseHandle internally
		}

		SetLastError(dwErrCode);
	}

	return retVal;
}




NTSTATUS PipeHooksManager::NtWriteFileHook(
	IN HANDLE               FileHandle,
	IN HANDLE               Event OPTIONAL,
	IN PIO_APC_ROUTINE      ApcRoutine OPTIONAL,
	IN PVOID                ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK    IoStatusBlock,
	IN PVOID                Buffer,
	IN ULONG                Length,
	IN PLARGE_INTEGER       ByteOffset OPTIONAL,
	IN PULONG               Key OPTIONAL)
{
	NTSTATUS retVal = GetInstance().pfnNtWriteFile(
		FileHandle,
		Event,
		ApcRoutine,
		ApcContext,
		IoStatusBlock,
		Buffer,
		Length,
		ByteOffset,
		Key);

	DWORD dwErrCode = GetLastError();

	LPWSTR lpPipeHandleName = GetInstance().lstPipes.GetHandleName(FileHandle);

	if (NULL != lpPipeHandleName)
	{
		OutputDebugString(TEXT("PipeHooksManager::NtWriteFileHook :: Writing buffer\n"));
		OutputDebugStringW(lpPipeHandleName);
		_LARGE_INTEGER liTimestamp = { 0 };
		QueryPerformanceCounter(&liTimestamp);
		GetInstance().WriteBuffer(lpPipeHandleName, Buffer, Length, liTimestamp.QuadPart);
	}

	SetLastError(dwErrCode);

	return retVal;
}


NTSTATUS PipeHooksManager::NtFsControlFileHook(
	IN HANDLE               FileHandle,
	IN HANDLE               Event OPTIONAL,
	IN PIO_APC_ROUTINE      ApcRoutine OPTIONAL,
	IN PVOID                ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK    IoStatusBlock,
	IN ULONG                FsControlCode,
	IN PVOID                InputBuffer OPTIONAL,
	IN ULONG                InputBufferLength,
	OUT PVOID               OutputBuffer OPTIONAL,
	IN ULONG                OutputBufferLength)
{
	NTSTATUS retVal = GetInstance().pfnNtFsControlFile(
		FileHandle,
		Event,
		ApcRoutine,
		ApcContext,
		IoStatusBlock,
		FsControlCode,
		InputBuffer,
		InputBufferLength,
		OutputBuffer,
		OutputBufferLength);

	DWORD dwErrCode = GetLastError();

	LPWSTR lpPipeHandleName = GetInstance().lstPipes.GetHandleName(FileHandle);

	if (NULL != lpPipeHandleName && NULL != InputBuffer)
	{
		OutputDebugString(TEXT("PipeHooksManager::NtFsControlFileHook :: Writing buffer\n"));
		OutputDebugStringW(lpPipeHandleName);
		_LARGE_INTEGER liTimestamp = { 0 };
		QueryPerformanceCounter(&liTimestamp);
		GetInstance().WriteBuffer(lpPipeHandleName, InputBuffer, InputBufferLength, liTimestamp.QuadPart);
	}

	SetLastError(dwErrCode);

	return retVal;
}




void PipeHooksManager::WriteBuffer(LPCWSTR lpPipeHandleName, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LONGLONG llTimestamp)
{
	TCHAR szTempPath[MAX_PATH + 1];
	TCHAR szTempFilePath[MAX_PATH + 1];

	DWORD dwRetVal = 0;
	UINT uRetVal = 0;

	HANDLE hTempFile = INVALID_HANDLE_VALUE;
	DWORD dwBytesWritten = 0;
	BOOL fSuccess = FALSE;

	dwRetVal = GetTempPath(MAX_PATH, szTempPath);
	if ((MAX_PATH < dwRetVal) || (0 == dwRetVal))
	{
		OutputDebugString(TEXT("PipeHooksManager::WriteBuffer :: Error getting temp path\n"));
		return;
	}

	uRetVal = GetTempFileName(szTempPath, TEXT("MARIO"), 0, szTempFilePath);
	if (uRetVal == 0)
	{
		OutputDebugString(TEXT("PipeHooksManager::WriteBuffer :: Error getting temp file name\n"));
		return;
	}


	hTempFile = CreateFile((LPTSTR)szTempFilePath, // file name 
		GENERIC_WRITE,        // open for write 
		0,                    // do not share 
		NULL,                 // default security 
		CREATE_ALWAYS,        // overwrite existing
		FILE_ATTRIBUTE_NORMAL,// normal file 
		NULL);                // no template 
	if (hTempFile == INVALID_HANDLE_VALUE)
	{
		OutputDebugString(TEXT("PipeHooksManager::WriteBuffer :: Error creating temp file\n"));
		return;
	}

	DWORD dwProcessId = GetCurrentProcessId();
	fSuccess = WriteFile(hTempFile, &dwProcessId, sizeof(DWORD), &dwBytesWritten, NULL);
	if (!fSuccess || (sizeof(DWORD) != dwBytesWritten))
	{
		OutputDebugString(TEXT("PipeHooksManager::WriteBuffer :: WriteFile failed to write PID\n"));
	}

	fSuccess = WriteFile(hTempFile, &llTimestamp, sizeof(LONGLONG), &dwBytesWritten, NULL);
	if (!fSuccess || (sizeof(LONGLONG) != dwBytesWritten))
	{
		OutputDebugString(TEXT("PipeHooksManager::WriteBuffer :: WriteFile failed to write time\n"));
	}

	size_t cbPipeHandleName = MAX_PATH * sizeof(WCHAR);
	if (S_OK == StringCbLengthW(lpPipeHandleName, cbPipeHandleName, &cbPipeHandleName))
	{
		cbPipeHandleName += sizeof(WCHAR); // Add terminating NULL char
		fSuccess = WriteFile(hTempFile, lpPipeHandleName, cbPipeHandleName, &dwBytesWritten, NULL);
		if (!fSuccess || (cbPipeHandleName != dwBytesWritten))
		{
			OutputDebugString(TEXT("PipeHooksManager::WriteBuffer :: WriteFile failed to write pipe name\n"));
		}
	}
	else
	{
		OutputDebugString(TEXT("PipeHooksManager::WriteBuffer :: WriteFile failed to get pipe name length\n"));
	}


	fSuccess = WriteFile(hTempFile, lpBuffer, nNumberOfBytesToWrite, &dwBytesWritten, NULL);
	if (!fSuccess || (nNumberOfBytesToWrite != dwBytesWritten))
	{
		OutputDebugString(TEXT("PipeHooksManager::WriteBuffer :: WriteFile failed to write buffer\n"));
	}

	if (!CloseHandle(hTempFile))
	{
		OutputDebugString(TEXT("PipeHooksManager::WriteBuffer :: Failed closing file\n"));
	}
}



VOID PipeHooksManager::MonitorPipes()
{
	OutputDebugString(TEXT("PipeHooksManager::MonitorPipes :: enter\n"));
	HOOK_TRACE_INFO hNtCreateNamedPipeHook = { NULL };
	// Install the hook
	NTSTATUS result = LhInstallHook(pfnNtCreateNamedPipe, NtCreateNamedPipeFileHook, NULL, &hNtCreateNamedPipeHook);
	if (FAILED(result))
		return;

	HOOK_TRACE_INFO hNtCreateFile = { NULL };
	result = LhInstallHook(pfnNtCreateFile, NtCreateFileHook, NULL, &hNtCreateFile);
	if (FAILED(result))
		return;

	HOOK_TRACE_INFO hNtClose = { NULL };
	result = LhInstallHook(pfnNtClose, NtCloseHook, NULL, &hNtClose);
	if (FAILED(result))
		return;

	HOOK_TRACE_INFO hNtDuplicateObject = { NULL };
	result = LhInstallHook(pfnNtDuplicateObject, NtDuplicateObjectHook, NULL, &hNtDuplicateObject);
	if (FAILED(result))
		return;

	HOOK_TRACE_INFO hNtWriteFileHook = { NULL };
	result = LhInstallHook(pfnNtWriteFile, NtWriteFileHook, NULL, &hNtWriteFileHook);
	if (FAILED(result))
		return;

	HOOK_TRACE_INFO hNtFsControlFileHook = { NULL };
	result = LhInstallHook(pfnNtFsControlFile, NtFsControlFileHook, NULL, &hNtFsControlFileHook);
	if (FAILED(result))
		return;


	ULONG ACLEntries[1] = { 0 };
	LhSetExclusiveACL(ACLEntries, 0, &hNtCreateNamedPipeHook);
	LhSetExclusiveACL(ACLEntries, 0, &hNtCreateFile);
	LhSetExclusiveACL(ACLEntries, 0, &hNtClose);
	LhSetExclusiveACL(ACLEntries, 0, &hNtDuplicateObject);
	LhSetExclusiveACL(ACLEntries, 0, &hNtWriteFileHook);
	LhSetExclusiveACL(ACLEntries, 0, &hNtFsControlFileHook);
	OutputDebugString(TEXT("PipeHooksManager::MonitorPipes :: end\n"));

}