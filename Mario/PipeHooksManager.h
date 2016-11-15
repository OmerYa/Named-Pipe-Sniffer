#pragma once

#include "stdafx.h"

typedef enum _HANDLE_TYPE {
	UNKNOWN_HANDLE = 0,
	LOCAL_PIPE,
	NETWORK_PIPE
} HANDLE_TYPE;

class PipeHooksManager
{

private:
	volatile PFN_NT_CREATEA_NAMED_PIPE pfnNtCreateNamedPipe;
	volatile PFN_NT_CREATE_FILE		   pfnNtCreateFile;
	volatile PFN_NT_CLOSE			   pfnNtClose;
	volatile PFN_NT_DUPLICATE_OBJECT   pfnNtDuplicateObject;
	volatile PFN_NT_WRITE_FILE		   pfnNtWriteFile;
	volatile PFN_NT_READ_FILE		   pfnNtReadFile;
	volatile PFN_NT_FS_CONTROL_FILE	   pfnNtFsControlFile;

	void WriteBuffer(LPCWSTR lpPipeHandleName, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LONGLONG llTimestamp);

	HANDLE_TYPE GetHandleType(UNICODE_STRING *szHandleName);

protected:
	PipeHandlesList				lstPipes;

	static NTSTATUS NTAPI NtCreateNamedPipeFileHook(
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
		IN PLARGE_INTEGER       DefaultTimeOut);

	static NTSTATUS NTAPI NtCreateFileHook(
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
		);

	static NTSTATUS NTAPI NtCloseHook(
		IN HANDLE               ObjectHandle);

	static NTSTATUS NTAPI NtDuplicateObjectHook(
		IN HANDLE               SourceProcessHandle,
		IN PHANDLE              SourceHandle,
		IN HANDLE               TargetProcessHandle,
		OUT PHANDLE             TargetHandle,
		IN ACCESS_MASK          DesiredAccess OPTIONAL,
		IN BOOLEAN              InheritHandle,
		IN ULONG                Options);


	static NTSTATUS NTAPI NtWriteFileHook(
		IN HANDLE               FileHandle,
		IN HANDLE               Event OPTIONAL,
		IN PIO_APC_ROUTINE      ApcRoutine OPTIONAL,
		IN PVOID                ApcContext OPTIONAL,
		OUT PIO_STATUS_BLOCK    IoStatusBlock,
		IN PVOID                Buffer,
		IN ULONG                Length,
		IN PLARGE_INTEGER       ByteOffset OPTIONAL,
		IN PULONG               Key OPTIONAL);

	static NTSTATUS NTAPI NtReadFileHook(
		_In_     HANDLE           FileHandle,
		_In_opt_ HANDLE           Event,
		_In_opt_ PIO_APC_ROUTINE  ApcRoutine,
		_In_opt_ PVOID            ApcContext,
		_Out_    PIO_STATUS_BLOCK IoStatusBlock,
		_Out_    PVOID            Buffer,
		_In_     ULONG            Length,
		_In_opt_ PLARGE_INTEGER   ByteOffset,
		_In_opt_ PULONG           Key);

	static NTSTATUS NTAPI NtFsControlFileHook(
		IN HANDLE               FileHandle,
		IN HANDLE               Event OPTIONAL,
		IN PIO_APC_ROUTINE      ApcRoutine OPTIONAL,
		IN PVOID                ApcContext OPTIONAL,
		OUT PIO_STATUS_BLOCK    IoStatusBlock,
		IN ULONG                FsControlCode,
		IN PVOID                InputBuffer OPTIONAL,
		IN ULONG                InputBufferLength,
		OUT PVOID               OutputBuffer OPTIONAL,
		IN ULONG                OutputBufferLength);


public:
	static PipeHooksManager& GetInstance();

	PipeHooksManager();

	VOID MonitorPipes();
	VOID StopMonitorPipes();

	virtual ~PipeHooksManager();
};

