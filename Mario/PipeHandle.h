
#pragma once
#include "stdafx.h"


class PipeHandle
{
public:
	LPWSTR               lpName;
	HANDLE				 hPipe;

	PipeHandle(PUNICODE_STRING _lpName, HANDLE _hPipe);
	PipeHandle(LPCWSTR _lpName, HANDLE _hPipe);

	virtual ~PipeHandle();
};
