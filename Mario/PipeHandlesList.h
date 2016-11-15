
#pragma once

#include "stdafx.h"


class PipeHandlesList : private std::list<PipeHandle*>
{
private:
	CRITICAL_SECTION hLock;
protected:

	BOOL FindHandle(HANDLE hPipe, std::list<PipeHandle*>::iterator *pIterator);

	volatile UINT uiNetworkPipesCount;


public:

	PipeHandlesList();
	virtual ~PipeHandlesList();

	void AddPipeHandle(PUNICODE_STRING lpHandleName, HANDLE hPipe, BOOL bNetworkPipe);
	void AddPipeHandle(LPCWSTR lpHandleName, HANDLE hPipe, BOOL bNetworkPipe);

	BOOL RemovePipeHandle(HANDLE hPipe);

	PipeHandle *GetPipeHandle(HANDLE hPipe);

	BOOL DuplicatePipeHandle(HANDLE hPipeSource, HANDLE hPipeDestination);

	BOOL HasNetworkPipes();
};

