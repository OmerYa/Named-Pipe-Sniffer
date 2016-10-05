
#pragma once

#include "stdafx.h"


class PipeHandlesList : private std::list<PipeHandle*>
{
private:
	CRITICAL_SECTION hLock;
protected:

	BOOL FindHandle(HANDLE hPipe, std::list<PipeHandle*>::iterator *pIterator);


public:

	PipeHandlesList();
	virtual ~PipeHandlesList();

	void AddPipeHandle(PUNICODE_STRING lpHandleName, HANDLE hPipe);
	void AddPipeHandle(LPCWSTR lpHandleName, HANDLE hPipe);

	BOOL RemovePipeHandle(HANDLE hPipe);

	LPWSTR GetHandleName(HANDLE hPipe);

	BOOL DuplicatePipeHandle(HANDLE hPipeSource, HANDLE hPipeDestination);
};

