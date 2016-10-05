// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		OutputDebugString(TEXT("PipeSniffer DllMain:DLL_PROCESS_ATTACH\n"));
		PipeHooksManager::GetInstance().MonitorPipes();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		//PipeHooksManager::GetInstance().StopMonitorPipes();
		break;
	}
	return TRUE;
}

