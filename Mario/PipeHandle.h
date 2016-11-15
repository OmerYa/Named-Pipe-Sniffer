
#pragma once
#include "stdafx.h"


class PipeHandle
{
public:
	LPWSTR              lpName;
	HANDLE				hPipe;
	BOOL				bNetworkPipe;

	PipeHandle(PUNICODE_STRING _lpName, HANDLE _hPipe, BOOL _bNetworkPipe);
	PipeHandle(LPCWSTR _lpName, HANDLE _hPipe, BOOL _bNetworkPipe);

	virtual ~PipeHandle();
};
