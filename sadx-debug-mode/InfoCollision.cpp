#include <SADXModLoader.h>
#include "Lantern.h"

// Sets color for the dynamic collision's material
void SetDynColor(int c)
{
	float r, b, g;
	// Set color
	if (MobileEntry[c].slAttribute & ColFlags_Solid)
		r = g = b = 0.8f;
	if (MobileEntry[c].slAttribute & ColFlags_Hurt)
	{
		r = 1.0f;
		g = 0.0f;
		b = 0.0f;
	}
	else if (MobileEntry[c].slAttribute & 0x20000000) // Unknown
	{
		r = 1.0f;
		g = 0.5f;
		b = 0.0f;
	}
	else if (MobileEntry[c].slAttribute & ColFlags_Water || MobileEntry[c].slAttribute & 0x400000) // Water
	{
		r = 0.0f;
		g = 0.4f;
		b = 0.9f;
	}
	else if (MobileEntry[c].slAttribute & 0x8000000) // Rebuffer every frame
	{
		r = 0.0f;
		g = 0.0f;
		b = 1.0f;
	}
	else if (MobileEntry[c].slAttribute & 0x40000000) // Use rotation?
	{
		r = 0.5f;
		g = 0.0f;
		b = 1.0f;
	}
	SetMaterial(0.8f, r, g, b);
}

// Displays dynamic collision
void CollisionDebug_OnFrame()
{
	for (int c = 0; c < numMobileEntry; c++)
	{
		auto& entry = MobileEntry[c];
		auto tp = entry.pTask;
		auto obj = entry.pObject;
		SaveConstantAttr();
		SaveControl3D();
		njControl3D_Add(NJD_CONTROL_3D_CONSTANT_ATTR | NJD_CONTROL_3D_CONSTANT_MATERIAL);
		RemoveConstantAttr(NJD_FLAG_USE_TEXTURE, 0);
		AddConstantAttr(0, NJD_FLAG_USE_ALPHA);
		njPushMatrixEx();
		njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
		njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
		SetDynColor(c);
		njTranslateV(0, (NJS_VECTOR*)obj->pos);
		if (obj->ang[2])
			njRotateZ(0, obj->ang[2] & 0xFFFF);
		if (obj->ang[1])
			njRotateY(0, obj->ang[1] & 0xFFFF);
		if (obj->ang[0])
			njRotateX(0, obj->ang[0] & 0xFFFF);
		njScaleV(0, (NJS_VECTOR*)obj->scl);
		late_DrawModelClip(obj->basicdxmodel, LATE_MAT, MobileEntry[c].pTask->twp->scl.x);
		njPopMatrixEx();
		ResetMaterial();
		LoadControl3D();
		LoadConstantAttr();
	}
}