#pragma once

#pragma warning(push)
#pragma warning(disable: 4267 4838)

extern bool EnableFontScaling;
extern int CurrentPalette;
extern int LSPaletteArray[];
extern int DisplaySoundIDMode;
extern int CurrentStageLight;
extern int CurrentLights[];
extern char DebugSetting;
extern int DebugMessageTimer;
extern const char* DebugMessage;

void BackupDebugFontSettings();
void RestoreDebugFontSettings();
void ScaleDebugFont(int scale);
void DrawDebugRectangle(float leftchars, float topchars, float numchars_horz, float numchars_vert);
void SendDebugMessage(const char* msg);

#pragma warning(pop)