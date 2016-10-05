

#pragma once

#include "stdafx.h"

typedef _Return_type_success_(return >= 0) LONG NTSTATUS;
typedef NTSTATUS *PNTSTATUS;

#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L)



typedef _Null_terminated_ wchar_t* NTSTRSAFE_PWSTR;

typedef NTSTATUS(NTAPI *PFN_RTL_STRING_CB_COPY_UNICODE_STRING) (
	_Out_writes_bytes_(cbDest) NTSTRSAFE_PWSTR pszDest,
	_In_ size_t cbDest,
	_In_ PCUNICODE_STRING SourceString);




typedef NTSTATUS(NTAPI *PFN_NT_CREATEA_NAMED_PIPE)(
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



typedef NTSTATUS (NTAPI *PFN_NT_CREATE_FILE)(
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


typedef NTSTATUS(NTAPI *PFN_NT_CLOSE)(
	IN HANDLE               ObjectHandle);


typedef NTSTATUS (NTAPI *PFN_NT_DUPLICATE_OBJECT)(
	IN HANDLE               SourceProcessHandle,
	IN PHANDLE              SourceHandle,
	IN HANDLE               TargetProcessHandle,
	OUT PHANDLE             TargetHandle,
	IN ACCESS_MASK          DesiredAccess OPTIONAL,
	IN BOOLEAN              InheritHandle,
	IN ULONG                Options);

typedef NTSTATUS (NTAPI *PFN_NT_WRITE_FILE)(
	IN HANDLE               FileHandle,
	IN HANDLE               Event OPTIONAL,
	IN PIO_APC_ROUTINE      ApcRoutine OPTIONAL,
	IN PVOID                ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK    IoStatusBlock,
	IN PVOID                Buffer,
	IN ULONG                Length,
	IN PLARGE_INTEGER       ByteOffset OPTIONAL,
	IN PULONG               Key OPTIONAL);


typedef NTSTATUS (NTAPI *PFN_NT_FS_CONTROL_FILE)(
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



#define NTSTRSAFEDDI      __inline NTSTATUS __stdcall
#define NTSTRSAFEWORKERDDI    static NTSTRSAFEDDI

#define NTSTRSAFE_UNICODE_STRING_MAX_CCH    (0xffff / sizeof(wchar_t))  // max buffer size, in characters, for a UNICODE_STRING

#define STATUS_BUFFER_OVERFLOW           ((NTSTATUS)0x80000005L)

#pragma warning(push)
#pragma warning(disable : 4100) // Unused parameter (pszDest)
_Post_satisfies_(cchDest > 0 && cchDest <= cchMax)
NTSTRSAFEWORKERDDI
RtlStringValidateDestW(
_In_reads_opt_(cchDest) STRSAFE_PCNZWCH pszDest,
_In_ size_t cchDest,
_In_ const size_t cchMax)
{
	NTSTATUS status = STATUS_SUCCESS;

	if ((cchDest == 0) || (cchDest > cchMax))
	{
		status = STATUS_INVALID_PARAMETER;
	}

	return status;
}
#pragma warning(pop)


// Intentionally allow null deref in error cases.
#pragma warning(push)
#pragma warning(disable : __WARNING_DEREF_NULL_PTR)
#pragma warning(disable : __WARNING_INVALID_PARAM_VALUE_1)
#pragma warning(disable : __WARNING_RETURNING_BAD_RESULT)

NTSTRSAFEWORKERDDI
RtlUnicodeStringValidateWorker(
_In_opt_ PCUNICODE_STRING SourceString,
_In_ const size_t cchMax,
_In_ DWORD dwFlags)
{
	NTSTATUS status = STATUS_SUCCESS;

	if (SourceString || !(dwFlags & STRSAFE_IGNORE_NULLS))
	{
		if (((SourceString->Length % sizeof(wchar_t)) != 0) ||
			((SourceString->MaximumLength % sizeof(wchar_t)) != 0) ||
			(SourceString->Length > SourceString->MaximumLength) ||
			(SourceString->MaximumLength > (cchMax * sizeof(wchar_t))))
		{
			status = STATUS_INVALID_PARAMETER;
		}
		else if ((SourceString->Buffer == NULL) &&
			((SourceString->Length != 0) || (SourceString->MaximumLength != 0)))
		{
			status = STATUS_INVALID_PARAMETER;
		}
	}

	return status;
}

#pragma warning(push)
#pragma warning(disable : __WARNING_RANGE_POSTCONDITION_VIOLATION)

_Post_satisfies_(*pcchSrcLength*sizeof(wchar_t) == SourceString->MaximumLength)
NTSTRSAFEWORKERDDI
RtlUnicodeStringValidateSrcWorker(
_In_ PCUNICODE_STRING SourceString,
_Outptr_result_buffer_(*pcchSrcLength) wchar_t** ppszSrc,
_Out_ size_t* pcchSrcLength,
_In_ const size_t cchMax,
_In_ DWORD dwFlags)
{
	NTSTATUS status;

	*ppszSrc = NULL;
	*pcchSrcLength = 0;

	status = RtlUnicodeStringValidateWorker(SourceString, cchMax, dwFlags);

	if (NT_SUCCESS(status))
	{
		if (SourceString)
		{
			*ppszSrc = SourceString->Buffer;
			*pcchSrcLength = SourceString->Length / sizeof(wchar_t);
		}

		if ((*ppszSrc == NULL) && (dwFlags & STRSAFE_IGNORE_NULLS))
		{
			*ppszSrc = L"";
		}
	}

	return status;
}

#pragma warning(pop)
// End intentionally allow null deref.
#pragma warning(pop)


NTSTRSAFEWORKERDDI
RtlStringCopyWideCharArrayWorker(
_Out_writes_(cchDest) NTSTRSAFE_PWSTR pszDest,
_In_ size_t cchDest,
_Out_opt_ size_t* pcchNewDestLength,
_In_reads_(cchSrcLength) const wchar_t* pszSrc,
_In_ size_t cchSrcLength)
{
	NTSTATUS status = STATUS_SUCCESS;
	size_t cchNewDestLength = 0;

	// ASSERT(cchDest != 0);

	while (cchDest && cchSrcLength)
	{
		*pszDest++ = *pszSrc++;
		cchDest--;
		cchSrcLength--;

		cchNewDestLength++;
	}

	if (cchDest == 0)
	{
		// we are going to truncate pszDest
		pszDest--;
		cchNewDestLength--;

		status = STATUS_BUFFER_OVERFLOW;
	}

	*pszDest = L'\0';

	if (pcchNewDestLength)
	{
		*pcchNewDestLength = cchNewDestLength;
	}

	return status;
}


NTSTRSAFEDDI
RtlStringCbCopyUnicodeString(
_Out_writes_bytes_(cbDest) NTSTRSAFE_PWSTR pszDest,
_In_ size_t cbDest,
_In_ PCUNICODE_STRING SourceString)
{
	NTSTATUS status;
	size_t cchDest = cbDest / sizeof(wchar_t);

	status = RtlStringValidateDestW(pszDest,
		cchDest,
		NTSTRSAFE_UNICODE_STRING_MAX_CCH);
	if (NT_SUCCESS(status))
	{
		wchar_t* pszSrc;
		size_t cchSrcLength;

		status = RtlUnicodeStringValidateSrcWorker(SourceString,
			&pszSrc,
			&cchSrcLength,
			NTSTRSAFE_UNICODE_STRING_MAX_CCH,
			0);

		if (NT_SUCCESS(status))
		{
			status = RtlStringCopyWideCharArrayWorker(pszDest,
				cchDest,
				NULL,
				pszSrc,
				cchSrcLength);
		}
		else
		{
			*pszDest = L'\0';
		}
	}

	return status;
}






class FunctionDelegates
{
public:
	volatile PFN_NT_CREATEA_NAMED_PIPE pfnNtCreateNamedPipe;
	volatile PFN_NT_CREATE_FILE pfnNtCreateFile;
	volatile PFN_NT_CLOSE pfnNtClose;
	volatile PFN_NT_DUPLICATE_OBJECT pfnNtDuplicateObject;
	volatile PFN_NT_WRITE_FILE pfnNtWriteFile;
	volatile PFN_NT_FS_CONTROL_FILE pfnNtFsControlFile;

	static FunctionDelegates& GetInstance();

	FunctionDelegates();

	virtual ~FunctionDelegates();
};

