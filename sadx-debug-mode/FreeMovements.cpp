#include "stdafx.h"
#include "Trampoline.h"
#include <SADXModLoader.h>
#include "FreeMovements.h"

Trampoline* Tails_Main_t;
Trampoline* Gamma_Main_t;
Trampoline* Big_Main_t;


void SetFreeMovements(EntityData1* data) {

	if (EV_MainThread_ptr || !data || !IsIngame())
		return;

	if (DebugMode && data->Action != FreeMovements)
	{
		data->Status &= ~0x30u;
		data->Action = FreeMovements;
	}
}

void __cdecl Tails_Main_r(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	if (data->Action == FreeMovements)
	{
		Debug_FreeMovements(data);
	}

	ObjectFunc(original, Tails_Main_t->Target());
	original(obj);

	SetFreeMovements(data);
}

void Gamma_Main_r(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	if (data->Action == FreeMovements)
	{
		Debug_FreeMovements(data);
	}

	ObjectFunc(original, Gamma_Main_t->Target());
	original(obj);

	SetFreeMovements(data);
}

void __cdecl Big_Main_r(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	if (data->Action == FreeMovements)
	{
		Debug_FreeMovements(data);
	}

	ObjectFunc(original, Big_Main_t->Target());
	original(obj);

	SetFreeMovements(data);
}

void init_FreeMovements() {
	Tails_Main_t = new Trampoline((int)Tails_Main, (int)Tails_Main + 0x7, Tails_Main_r);
	Gamma_Main_t = new Trampoline((int)Gamma_Main, (int)Gamma_Main + 0x7, Gamma_Main_r);
	Big_Main_t = new Trampoline((int)Big_Main, (int)Big_Main + 0x5, Big_Main_r);
	return;
}