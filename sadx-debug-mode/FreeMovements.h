#pragma once

enum FreeMovAction {
	FreeMovements = 110
};


// void __usercall Debug_FreeMovements(EntityData1* a1@<esi>)
static const void* const FreeMovePtr = (void*)0x492170;
static inline void Debug_FreeMovements(EntityData1* a1)
{
	__asm
	{
		mov esi, [a1]
		call FreeMovePtr
	}
}

void init_FreeMovements();