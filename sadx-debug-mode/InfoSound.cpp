#include <SADXModLoader.h>
#include "Data.h"
#include "InfoSound.h"

int DisplaySoundIDMode = 0;

SoundBank_SE GetBankNumberAndID(int SoundID_HEX)
{
	SoundBank_SE result{ 127, 127, "ERROR" };
	if (SoundID_HEX == -1)
	{
		result.Bank_Name = "";
		result.Bank_ID = -1;
		result.SE_ID = -1;
		return result;
	}
	else
	{
		for (unsigned int i = 0; i < LengthOfArray(SoundBanks) - 1; i++)
		{
			if (SoundID_HEX < SoundBanks[i + 1].StartID)
			{
				result.Bank_Name = SoundBanks[i].Name;
				result.SE_ID = max(0, SoundID_HEX - SoundBanks[i].StartID - 1);
				result.Bank_ID = SoundBanks[i].Name[8] - 48;
				if (result.Bank_ID == 9)
					result.Bank_ID = 10; // Make 9 into A for ADX bank
				if (result.Bank_ID == 0)
					result.SE_ID = SoundID_HEX; // Exception for the first bank
				return result;
			}
		}
	}
	return result;
}

const char* SoundLookUp(int SoundID)
{
	for (int i = 0; i < 1519; i++)
	{
		if (SENameLookUp[i].SE_ID == SoundID) return SENameLookUp[i].SE_Name;
	}
	return "ERROR";
}

void SoundInfo()
{
	DrawDebugRectangle(0.25f, 0.75f, 63.75f, 45);
	ScaleDebugFont(16);
	SetDebugFontColor(0xFF88FFAA);
	if (EnableFontScaling || HorizontalResolution < 1024)
		DisplayDebugString(NJM_LOCATION(12, 1), "- SOUND QUEUE -");
	else
		DisplayDebugString(NJM_LOCATION(24, 1), "- SOUND QUEUE -");
	if (!EnableFontScaling && HorizontalResolution >= 1024)
		ScaleDebugFont(16);
	else
		ScaleDebugFont(10);
	SetDebugFontColor(0xFFBFFF00);
	DisplayDebugString(NJM_LOCATION(2, 4), "N   ID   PRI  TIME  FLAG   VOL MI/MX   PAN   PITCH    QNUM");
	SetDebugFontColor(0xFFBFBFBF);
	int ActiveSounds = 0;
	for (unsigned int i = 0; i < 36; i++)
	{
		// Add data to debug info array
		if (SoundQueue[i].PlayTime != 0)
		{
			SoundQueueDebug[i].Bank_ID = GetBankNumberAndID(SoundQueue[i].SoundID).Bank_ID;
			SoundQueueDebug[i].SE_ID = GetBankNumberAndID(SoundQueue[i].SoundID).SE_ID;
			SoundQueueDebug[i].EnumName = SoundLookUp(SoundQueue[i].SoundID);
			SoundQueueDebug[i].PlayTime = SoundQueue[i].PlayTime;
			SoundQueueDebug[i].Flags = SoundQueue[i].Flags;
			SoundQueueDebug[i].VolumeCur = SoundQueue[i].CurrentVolume;
			SoundQueueDebug[i].VolumeMax = SoundQueue[i].MaxVolume;
		}
		if (SoundQueue[i].PlayTime == 0)
			SetDebugFontColor(0xFFBF0000);
		else
		{
			if (SoundQueueDebug[i].Flags & 0x4000)
			{
				if (SoundQueueDebug[i].Flags & 0x1000)
					SetDebugFontColor(0xFF00FFFF);
				else
					SetDebugFontColor(0xFFBF00BF);
			}
			else if (SoundQueue[i].Panning != 0)
				SetDebugFontColor(0xFFFF7FB2);
			else if (SoundQueueDebug[i].Flags & 0x2000)
				SetDebugFontColor(0xFF7F4040);
			else if (SoundQueueDebug[i].Flags & 0x1000)
				SetDebugFontColor(0xFFBFBF00);
			else if (SoundQueueDebug[i].Flags & 0x100)
				SetDebugFontColor(0xFFFF7F00);
			else if (SoundQueueDebug[i].Flags & 0x200)
				SetDebugFontColor(0xFF00BF00);
			else
				SetDebugFontColor(0xFFBFBFBF);
			ActiveSounds++;
		}
		PrintDebugNumber(NJM_LOCATION(1, i + 1 + 5), i, 2);
		if (SoundQueue[i].SoundID != -1)
		{
			if (DisplaySoundIDMode == 1)
				DisplayDebugStringFormatted(NJM_LOCATION(5, i + 1 + 5), "%03X", SoundQueue[i].SoundID, 4);
			else if (DisplaySoundIDMode == 2)
				PrintDebugNumber(NJM_LOCATION(5, i + 1 + 5), SoundQueue[i].SoundID, 4);
			else
				DisplayDebugStringFormatted(NJM_LOCATION(5, i + 1 + 5), "%01X/%02i", SoundQueueDebug[i].Bank_ID, SoundQueueDebug[i].SE_ID, 4);
		}
		if (SoundQueue[i].SoundID != -1)
			PrintDebugNumber(NJM_LOCATION(11, i + 1 + 5), SoundQueue[i].Priority, 2);
		if (SoundQueue[i].PlayTime != 0)
			PrintDebugNumber(NJM_LOCATION(16, i + 1 + 5), SoundQueue[i].PlayTime, 4);
		if (SoundQueue[i].PlayTime != 0 && SoundQueue[i].Flags != 0)
			DisplayDebugStringFormatted(NJM_LOCATION(22, i + 1 + 5), "%04X", SoundQueue[i].Flags);
		if (SoundQueue[i].PlayTime != 0 && (SoundQueue[i].CurrentVolume != 0 || SoundQueue[i].MaxVolume != 0))
			DisplayDebugStringFormatted(NJM_LOCATION(29, i + 1 + 5), "%04i/%04i", SoundQueue[i].CurrentVolume, SoundQueue[i].MaxVolume);
		if (SoundQueue[i].PlayTime != 0 && SoundQueue[i].Panning != 0)
			DisplayDebugStringFormatted(NJM_LOCATION(41, i + 1 + 5), "%04i", SoundQueue[i].Panning);
		if (SoundQueue[i].PlayTime != 0 && SoundQueue[i].PitchShift != 0)
			DisplayDebugStringFormatted(NJM_LOCATION(47, i + 1 + 5), "%05i", SoundQueue[i].PitchShift);
		if (SoundQueue[i].PlayTime != 0 && SoundQueue[i].qnum != 0)
			DisplayDebugStringFormatted(NJM_LOCATION(57, i + 1 + 5), "%02i", SoundQueue[i].qnum);
	}
	SetDebugFontColor(0xFFBFBFBF);
	DisplayDebugStringFormatted(NJM_LOCATION(2, 43), "ACTIVE SOUNDS: %d", ActiveSounds);
}

void SoundBankInfo()
{
	DrawDebugRectangle(0.25f, 0.75f, 31.0f, 45);
	ScaleDebugFont(16);
	SetDebugFontColor(0xFF88FFAA);
	if (EnableFontScaling || HorizontalResolution < 1024)
		DisplayDebugString(NJM_LOCATION(1, 1), "- SOUNDBANK INFO -");
	else
		DisplayDebugString(NJM_LOCATION(6, 1), "- SOUNDBANK INFO -");
	if (!EnableFontScaling && HorizontalResolution >= 1024)
		ScaleDebugFont(16);
	else
		ScaleDebugFont(10);
	SetDebugFontColor(0xFFBFFF00);
	DisplayDebugString(NJM_LOCATION(2, 4), "N  BANK  ENUM NAME");
	SetDebugFontColor(0xFFBFBFBF);
	int ActiveSounds = 0;
	// Add data to debug info array
	for (unsigned int i = 0; i < 36; i++)
	{
		if (SoundQueue[i].PlayTime != 0)
		{
			SoundQueueDebug[i].Bank_ID = GetBankNumberAndID(SoundQueue[i].SoundID).Bank_ID;
			SoundQueueDebug[i].SE_ID = GetBankNumberAndID(SoundQueue[i].SoundID).SE_ID;
			SoundQueueDebug[i].EnumName = SoundLookUp(SoundQueue[i].SoundID);
			SoundQueueDebug[i].PlayTime = SoundQueue[i].PlayTime;
			SoundQueueDebug[i].Flags = SoundQueue[i].Flags;
			SoundQueueDebug[i].VolumeCur = SoundQueue[i].CurrentVolume;
			SoundQueueDebug[i].VolumeMax = SoundQueue[i].MaxVolume;
		}
	}
	for (unsigned int i = 0; i < 36; i++)
	{
		if (SoundQueue[i].PlayTime == 0) SetDebugFontColor(0xFFBF0000);
		else
		{
			if (SoundQueueDebug[i].Flags & 0x4000)
			{
				if (SoundQueueDebug[i].Flags & 0x1000)
					SetDebugFontColor(0xFF00FFFF);
				else
					SetDebugFontColor(0xFFBF00BF);
			}
			else if (SoundQueue[i].Panning != 0)
				SetDebugFontColor(0xFFFF7FB2);
			else if (SoundQueueDebug[i].Flags & 0x2000)
				SetDebugFontColor(0xFF7F4040);
			else if (SoundQueueDebug[i].Flags & 0x1000)
				SetDebugFontColor(0xFFBFBF00);
			else if (SoundQueueDebug[i].Flags & 0x100)
				SetDebugFontColor(0xFFFF7F00);
			else if (SoundQueueDebug[i].Flags & 0x200)
				SetDebugFontColor(0xFF00BF00);
			else
				SetDebugFontColor(0xFFBFBFBF);
		}
		PrintDebugNumber(NJM_LOCATION(1, i + 1 + 5), i, 2);
		if (SoundQueueDebug[i].Bank_ID != -1)
			DisplayDebugStringFormatted(NJM_LOCATION(5, i + 1 + 5), "%01X/%02i", SoundQueueDebug[i].Bank_ID, SoundQueueDebug[i].SE_ID, 4);
		if (SoundQueueDebug[i].EnumName != "")
			DisplayDebugStringFormatted(NJM_LOCATION(11, i + 1 + 5), SoundQueueDebug[i].EnumName, 4);
	}
	SetDebugFontColor(0xFFBF00BF);
	DisplayDebugStringFormatted(NJM_LOCATION(1, 43), "3D");
	SetDebugFontColor(0xFF00FFFF);
	DisplayDebugStringFormatted(NJM_LOCATION(4, 43), "3D+OLD");
	SetDebugFontColor(0xFFBFBF00);
	DisplayDebugStringFormatted(NJM_LOCATION(11, 43), "OLD");
	SetDebugFontColor(0xFFFF7F00);
	DisplayDebugStringFormatted(NJM_LOCATION(15, 43), "POS");
	SetDebugFontColor(0xFF00BF00);
	DisplayDebugStringFormatted(NJM_LOCATION(19, 43), "VOL");
	SetDebugFontColor(0xFFFF7FB2);
	DisplayDebugStringFormatted(NJM_LOCATION(23, 43), "PAN");
	SetDebugFontColor(0xFF7F4040);
	DisplayDebugStringFormatted(NJM_LOCATION(27, 43), "FRQ");
	SetDebugFontColor(0xFFBFBFBF);
}
