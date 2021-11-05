# SADX Debug Mode by PkR

Debug Mode is a mod for Sonic Adventure DX PC (2004) that adds various debug features to the game.

**Using the mod**

1) Press the Z button on the controller or the B key on the keyboard to toggle between different debug information screens. Alternatively, press 1-9 on the keyboard to show a specific screen.
2) If you press the Z button on the controller (or the B key on the keyboard) while also holding the A button (or its keyboard counterpart - X key by default), vanilla debug mode will be enabled. To disable it, press the same button/key combination again.
3) Press P on the keyboard to enable Crash Log. This will log each texture list and object code change to the SADX Mod Loader log/console.
4) Press Pause/Break on the keyboard the freeze and unfreeze the game. Press Insert to advance one frame forward.
5) In the Player and Camera screens, press H on the keyboard to display angles as hexadecimal values. In the Sound Queue screen, press H on the keyboard to toggle between decimal, hexadecimal and soundbank representations of sound IDs. In the LS Palette and Stage Lights screens, press H on the keyboard to toggle between multiple LS Palette Data and Stage Light Data entries for the current stage.
6) Press the C button on the controller or the C key on the keyboard to enable or disable collision data display.
7) Press T to toggle textures.
8) Press F to toggle fog.
9) Press Y on the keyboard to toggle free camera (see below for details).
10) Press END to toggle the speed hack. When the speed hack is enabled, press Page Up and Page Down to control how fast the game runs. Note: the game cannot run slower than its original speed.

For better legibility enable the "Hide HUD" code in SADX Mod Manager before running the game with this mod.

**Using the free camera mode**

When the free camera mode is enabled, you can use the mouse to move the camera around. The control scheme is similar to the one used in 3D editors in [SA Tools](https://github.com/sonicretro/sa_tools).
- Move the mouse to rotate the camera.
- Hold Left Shift and move the mouse to move the camera.
- Hold Control and move the mouse up and down to zoom in and out.
- Press both Left Shift and Control to lock the camera in place. Press them again to unlock it.
- Press Numpad + and Numpad - to change camera speed.
- Press Y on the keyboard to re-enable vanilla camera. It will remain locked in place until the character touches a camera trigger.

**Using the vanilla debug mode**

This is SADX' built-in debug mode (what's left of it). Collision objects are visible in this mode.
When this mode is enabled, you can move the character freely through any obstacles. Use the analog stick to position the character on the X and Z plane. Hold the X button to move the character up and down.

**Notes and known issues**
1) The information in LS Palette and Stage Lights screens is irrelevant when using Lantern Engine.
2) Some transparent objects may flicker when using frame advance.
3) Some debug text may not display properly when using frame advance.
4) Pressing (as opposed to holding) buttons may not work correctly when using freeze frame/frame advance.
5) The speed hack may not always work reliably, especially in cutscenes.

**Credits**

[Speeps](https://twitter.com/SPEEPSHighway) for providing the original free camera code that was adapted for this mod

[Sora](https://github.com/Sora-yx) for reimplementing vanilla Debug Mode for Tails, Big and Gamma

**Related mods/tools**

SADX Mod Installer: https://sadxmodinstaller.unreliable.network

SADX Mod Loader (direct link): http://mm.reimuhakurei.net/sadxmods/SADXModLoader.7z
