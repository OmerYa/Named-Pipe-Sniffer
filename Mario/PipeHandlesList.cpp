#include "stdafx.h"


PipeHandlesList::PipeHandlesList() : std::list<PipeHandle*>()
{
	//InitializeCriticalSection(&hLock);
	uiNetworkPipesCount = 0;
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



void PipeHandlesList::AddPipeHandle(PUNICODE_STRING lpHandleName, HANDLE hPipe, BOOL bNetworkPipe)
{
	PipeHandle *phPipe = NULL;

	phPipe = new (std::nothrow) PipeHandle(lpHandleName, hPipe, bNetworkPipe);
	if (NULL == phPipe)
	{
		OutputDebugString(TEXT("PipeHandlesList::AddPipeHandle::Failed to allocate PipeHandle object"));
		return;
	}

	//EnterCriticalSection(&hLock);
	this->push_back(phPipe);
	if (bNetworkPipe) uiNetworkPipesCount++;
	//LeaveCriticalSection(&hLock);
}


void PipeHandlesList::AddPipeHandle(LPCWSTR lpHandleName, HANDLE hPipe, BOOL bNetworkPipe)
{
	PipeHandle *phPipe = NULL;

	phPipe = new (std::nothrow) PipeHandle(lpHandleName, hPipe, bNetworkPipe);
	if (NULL == phPipe)
	{
		OutputDebugString(TEXT("PipeHandlesList::AddPipeHandle(W)::Failed to allocate PipeHandle object"));
		return;
	}

	//EnterCriticalSection(&hLock);
	this->push_back(phPipe);
	if (bNetworkPipe) uiNetworkPipesCount++;
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
		if ((*it)->bNetworkPipe) uiNetworkPipesCount--;
		this->erase(it);
		retVal = TRUE;
	}
	//LeaveCriticalSection(&hLock);

	return retVal;
}

PipeHandle *PipeHandlesList::GetPipeHandle(HANDLE hPipe)
{
	PipeHandle *pPipeHandle = NULL;

	std::list<PipeHandle*>::iterator it;
	//EnterCriticalSection(&hLock);
	if (TRUE == this->FindHandle(hPipe, &it))
	{
		pPipeHandle = (*it);
	}
	//LeaveCriticalSection(&hLock);

	return pPipeHandle;
}

BOOL PipeHandlesList::DuplicatePipeHandle(HANDLE hPipeSource, HANDLE hPipeDestination)
{
	//EnterCriticalSection(&hLock);
	PipeHandle *pPipeHandle = this->GetPipeHandle(hPipeSource);
	if (NULL != pPipeHandle)
	{
		this->AddPipeHandle(pPipeHandle->lpName, hPipeDestination, pPipeHandle->bNetworkPipe);
		if (pPipeHandle->bNetworkPipe) uiNetworkPipesCount++;
	}
	//LeaveCriticalSection(&hLock);

	return NULL != pPipeHandle;
}


BOOL PipeHandlesList::HasNetworkPipes()
{
	return 0 != uiNetworkPipesCount;
}