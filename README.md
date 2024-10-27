# SPM Blocks of Wisdom Mod
A mod I made in a week that ended up being a hell of a lot cooler than I originally thought it'd be. Throwable blocks are quite broken - with the correct uses, you can gain extra height, clip through any wall, deal "damage" to NPCs without HP, and a lot more. Being able to spawn solid ground anywhere in the form of Coin Blocks is also quite broken... (smirk)

Please report any bugs that you experience OR suggestions you might have to me on Discord. I am active in the Star Haven and SPM Speedrunning communities & prefer to talk about modding in those spaces, but if you would like to reach me directly you're free to DM me - @tartt

## Installing
This mod is currently PAL (EU0 or EU1) only, meant to be installed with Riivolution. For more information, check out the tutorial in my [other mod!](https://github.com/shiken-yme/spm-pit-randomizer/blob/main/README.md)

## Controls
Blocks may only be spawned while you're able to move (or more specifically, "keyed on" in game). You can have up to 5 blocks in a room at once. The number of active blocks on screen, as well as your current orientation/type settings, are all located at the bottom-left of the screen upon updating. This will be cleared upon "key off," or when you talk to an NPC / activate a cutscene / enter a loading zone.

Every time a BoW action is performed, there will be a cooldown before the next one may occur. BoW actions may be buffered, but no other buttons may be held in order for these to activate. (This means that, for example, holding 2, B, and Up will not do anything, but letting go of 2 will cause a block to spawn instantly as long as the cooldown is finished.)

* **B-Up** spawns a throwable block directly over the player. This hotkey will do nothing when you have 5 in a room.

* **B-Down** clears all active blocks.

* **B-Left** changes block orientation, switching between Directly Above the player and their Facing Direction. A dummy block is temporarily spawned in the relative position where a block may be spawned with B-Up next.

* **B-Right** changes block type, switching between Throwable Block and Coin Block. *(These currently share the same 5-block limit.)*

You want even more customization??? **Get out your Nunchuk.** *(D-Pad directions for Z/C controls assume you are still holding the Wiimote horizontally.)*

* **Z-Up/Z-Down** increases/decreases block Y position (vertical offset). Minimum 0 units (directly in front of the player), maximum 100 units above the player.

* **C-Up/C-Down** increases/decreases block X/Z position (horizontal offset), depending on whether you're in 2D or 3D. Minimum 30 units, maximum 100 units. *(Changing the horizontal offset for blocks will automatically set your Block Orientation to Facing Direction.)*

## Upcoming Features

* Separate the 5-block cap between different types of blocks - maybe also add new types of blocks...?

* Portal blocks - spawn a blue switch block in one room and a red switch block anywhere else, then hit either block to teleport to the other one. These will operate on their own block cap.

* Possible B-Down overhaul, or at least a separate option to remove the last block spawned instead of every single one.

* Overhaul the messages at the bottom-left if possible, maybe turn them into icons?
