

#include "stdafx.h"


PipeHandle::PipeHandle(PUNICODE_STRING _lpName, HANDLE _hPipe)
{
	size_t cbName = 0;
	HRESULT hr = S_OK;

	lpName = NULL;
	hPipe = INVALID_HANDLE_VALUE;

	if (NULL == _lpName)
	{
		OutputDebugString(TEXT("PipeHandle::PipeHandle::Invalid input, pipe's name is NULL\n"));
		return;
	}

	if (INVALID_HANDLE_VALUE == _hPipe)
	{
		OutputDebugString(TEXT("PipeHandle::PipeHandle::Invalid input, handle value is invalid\n"));
		return;
	}

	// Get pipe's name length
	if (0 == _lpName->Length)
	{
		OutputDebugString(TEXT("PipeHandle::PipeHandle::Zero length handle name\n"));
		return;
	}
	cbName = _lpName->Length + sizeof(WCHAR); // Add space for null char

	// Allocate space for pipe's name copy
	lpName = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cbName);
	if (NULL == lpName)
	{
		OutputDebugString(TEXT("PipeHandle::PipeHandle::Error allocating string\n"));
		return;
	}

	// Copy pipe's name
	hr = RtlStringCbCopyUnicodeString(lpName, cbName, _lpName);
	if (FAILED(hr))
	{
		HeapFree(GetProcessHeap(), NULL, lpName);
		lpName = NULL;
		OutputDebugString(TEXT("PipeHandle::PipeHandle::Error copying string\n"));
		return;
	}


	hPipe = _hPipe;
}



PipeHandle::PipeHandle(LPCWSTR _lpName, HANDLE _hPipe)
{
	size_t cbName = 0;
	HRESULT hr = S_OK;

	lpName = NULL;
	hPipe = INVALID_HANDLE_VALUE;

	if (NULL == _lpName)
	{
		OutputDebugString(TEXT("PipeHandle::PipeHandle(W)::Invalid input, pipe's name is NULL\n"));
		return;
	}

	if (INVALID_HANDLE_VALUE == _hPipe)
	{
		OutputDebugString(TEXT("PipeHandle::PipeHandle(W)::Invalid input, handle value is invalid\n"));
		return;
	}

	// Get pipe's name length
	hr = StringCbLengthW(_lpName, STRSAFE_MAX_CCH * sizeof(WCHAR), &cbName);
	if (FAILED(hr))
	{
		OutputDebugString(TEXT("PipeHandle::PipeHandle(W)::Error getting string length\n"));
		return;
	}
	cbName += sizeof(WCHAR); // Add space for null char

	// Allocate space for pipe's name copy
	lpName = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cbName);
	if (NULL == lpName)
	{
		OutputDebugString(TEXT("PipeHandle::PipeHandle(W)::Error allocating string\n"));
		return;
	}

	// Copy pipe's name
	hr = StringCbCopyW(lpName, cbName, _lpName);
	if (FAILED(hr))
	{
		HeapFree(GetProcessHeap(), NULL, lpName);
		lpName = NULL;
		OutputDebugString(TEXT("PipeHandle::PipeHandle(W)::Error copying string\n"));
		return;
	}

	hPipe = _hPipe;
}




PipeHandle::~PipeHandle()
{
	hPipe = INVALID_HANDLE_VALUE;

	if (NULL != lpName)
	{
		HeapFree(GetProcessHeap(), NULL, lpName);
		lpName = NULL;
	}
}
