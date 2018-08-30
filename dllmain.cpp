#include "stdafx.h"
#include "stdio.h"
#include <windows.h>
#include "includes\injector\injector.hpp"
#include <cstdint>
#include "includes\IniReader.h"

float ObjX, ObjY;

void(__cdecl *FE_Object_SetCenter)(DWORD* FEObject, float _PositionX, float _PositionY) = (void(__cdecl*)(DWORD*, float, float))0x525050;
void(__cdecl *FE_Object_GetCenter)(DWORD* FEObject, float *PositionX, float *PositionY) = (void(__cdecl*)(DWORD*, float*, float*))0x524EE0;
void*(__cdecl *FEObject_FindObject)(const char *pkg_name, unsigned int obj_hash) = (void*(__cdecl*)(const char*, unsigned int))0x524850;
DWORD*(__cdecl *FEngGetScript)(char const * FNGName, unsigned int ObjectHash, unsigned int ScriptHash) = (DWORD*(__cdecl*)(char const*, unsigned int, unsigned int))0x5249D0;

int __stdcall cFEng_QueuePackageMessage_Hook(unsigned int MessageHash, char const *FEPackageName, DWORD* FEObject)
{
	int ResX, ResY;
	float Difference;

	ResX = *(int*)0x93CAD8;
	ResY = *(int*)0x93CADC;
	
	if (ResX != 0 && ResY != 0) Difference = (((float)ResX / (float)ResY) * 240) - 320; // Calculate position difference for current aspect ratio
	else Difference = 0; // 4:3 if we can't get any values

	float DifferenceMin = Difference * -1;

	// Minimap
	injector::WriteMemory<float*>(0x5678AA, &DifferenceMin, true);
	injector::WriteMemory<float>(0x5678B4, DifferenceMin, true);
	injector::WriteMemory<float*>(0x5678C6, &Difference, true);

	DWORD* LeftGroup = (DWORD*)FEObject_FindObject(FEPackageName, 0x1603009E); // "HUD_SingleRace.fng", leftgrouphash

	if (LeftGroup) // Move left group
	{
		FE_Object_GetCenter(LeftGroup, &ObjX, &ObjY);
		FE_Object_SetCenter(LeftGroup, ObjX - Difference, ObjY);
	}

	DWORD* RightGroup = (DWORD*)FEObject_FindObject(FEPackageName, 0x5D0101F1); // "HUD_SingleRace.fng", rightgrouphash

	if (RightGroup) // Move right group
	{
		FE_Object_GetCenter(RightGroup, &ObjX, &ObjY);
		FE_Object_SetCenter(RightGroup, ObjX + Difference, ObjY);
	}
    
    /* EA TRAX and WW sign are currently disabled, they don't work properly.
	DWORD* EATraxChyron = (DWORD*)FEObject_FindObject(FEPackageName, 0xA1341735); // "EA_Trax.fng", chyronhash

	if (EATraxChyron)
	{
		//cFEng_QueuePackageMessage(MessageHash, FEPackageName, FEObject);
		
		FE_Object_GetCenter(EATraxChyron, &ObjX, &ObjY);
		printf("EA Trax Chyron at address %d found at %f, %f.", EATraxChyron, ObjX, ObjY);
		FE_Object_SetCenter(EATraxChyron, ObjX - Difference, ObjY);
		printf("EA Trax Chyron position is changed to %f, %f.", ObjX - Difference, ObjY);
	}*/

	/*
	DWORD* WrongWaySign = (DWORD*)FEObject_FindObject(FEPackageName, 0xC82FA200); // "HUD_SingleRace.fng", WRONGWAYIMAGE

	if (WrongWaySign)
	{
		DWORD* WrongWayNormalScript = FEngGetScript(FEPackageName, 0xC82FA200, 0x47510B1E);
		DWORD* WrongWayWideScript = FEngGetScript(FEPackageName, 0xC82FA200, 0x908E787E);

		FE_Object_GetCenter(WrongWaySign, &ObjX, &ObjY);
		FE_Object_SetCenter(WrongWaySign, ObjX - Difference, ObjY);
	}
    */
    
	return 1;
}

void Init()
{
	// HUD Left and Right Groups
	injector::MakeCALL(0x5696F1, cFEng_QueuePackageMessage_Hook, true);
	injector::MakeCALL(0x569712, cFEng_QueuePackageMessage_Hook, true);

	/* EA TRAX and WW sign are currently disabled, they don't work properly.*/
	//injector::MakeCALL(0x58D8C3, cFEng_QueuePackageMessage_Hook, true);
	//injector::MakeCALL(0x58D86F, cFEng_QueuePackageMessage_Hook, true);
	//injector::MakeCALL(0x58D7F7, cFEng_QueuePackageMessage_Hook, true);
    
}

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		Init();
	}
	return TRUE;

}

