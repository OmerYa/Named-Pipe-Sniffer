#include "stdafx.h"


PipeHandlesList::PipeHandlesList() : std::list<PipeHandle*>()
{
	//InitializeCriticalSection(&hLock);
}


PipeHandlesList::~PipeHandlesList()
{
	PipeHandle *phPipe = NULL;

	//EnterCriticalSection(&hLock);
	std::list<PipeHandle*>::iterator phIterator = this->begin();
	while (phIterator != this->end())
	{
		phPipe = *phIterator;
		phIterator = this->erase(phIterator);

		delete phPipe;
	}
	//LeaveCriticalSection(&hLock);
}



void PipeHandlesList::AddPipeHandle(PUNICODE_STRING lpHandleName, HANDLE hPipe)
{
	PipeHandle *phPipe = NULL;

	phPipe = new (std::nothrow) PipeHandle(lpHandleName, hPipe);
	if (NULL == phPipe)
	{
		OutputDebugString(TEXT("PipeHandlesList::AddPipeHandle::Failed to allocate PipeHandle object"));
		return;
	}

	//EnterCriticalSection(&hLock);
	this->push_back(phPipe);
	//LeaveCriticalSection(&hLock);
}


void PipeHandlesList::AddPipeHandle(LPCWSTR lpHandleName, HANDLE hPipe)
{
	PipeHandle *phPipe = NULL;

	phPipe = new (std::nothrow) PipeHandle(lpHandleName, hPipe);
	if (NULL == phPipe)
	{
		OutputDebugString(TEXT("PipeHandlesList::AddPipeHandle(W)::Failed to allocate PipeHandle object"));
		return;
	}

	//EnterCriticalSection(&hLock);
	this->push_back(phPipe);
	//LeaveCriticalSection(&hLock);
}

BOOL PipeHandlesList::FindHandle(HANDLE hPipe, std::list<PipeHandle*>::iterator *pIterator)
{
	std::list<PipeHandle*>::iterator it;
	PipeHandle *phPipe;
	//EnterCriticalSection(&hLock);
	for (it = this->begin(); it != this->end(); ++it)
	{
		phPipe = *it;
		if (phPipe->hPipe == hPipe)
		{
			*pIterator = it;
			return TRUE;
		}
	}
	//LeaveCriticalSection(&hLock);

	return FALSE;
}

BOOL PipeHandlesList::RemovePipeHandle(HANDLE hPipe)
{
	BOOL retVal = FALSE;
	
	std::list<PipeHandle*>::iterator it;
	//EnterCriticalSection(&hLock);
	if (TRUE == this->FindHandle(hPipe, &it))
	{
		this->erase(it);
		retVal = TRUE;
	}
	//LeaveCriticalSection(&hLock);

	return retVal;
}

LPWSTR PipeHandlesList::GetHandleName(HANDLE hPipe)
{
	LPWSTR lpPipeName = NULL;

	std::list<PipeHandle*>::iterator it;
	//EnterCriticalSection(&hLock);
	if (TRUE == this->FindHandle(hPipe, &it))
	{
		lpPipeName = (*it)->lpName;
	}
	//LeaveCriticalSection(&hLock);

	return lpPipeName;
}

BOOL PipeHandlesList::DuplicatePipeHandle(HANDLE hPipeSource, HANDLE hPipeDestination)
{
	//EnterCriticalSection(&hLock);
	LPWSTR lpName = this->GetHandleName(hPipeSource);
	if (NULL != lpName)
	{
		this->AddPipeHandle(lpName, hPipeDestination);
	}
	//LeaveCriticalSection(&hLock);

	return NULL != lpName;
}
