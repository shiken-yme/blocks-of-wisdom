#include "mod.h"
#include "patch.h"

#include <common.h>
#include <util.h>
#include <cutscene_helpers.h>
#include <evtpatch.h>
#include <evt_cmd.h>
#include <spm/rel/aa1_01.h>
#include <spm/temp_unk.h>
#include <spm/animdrv.h>
#include <spm/bgdrv.h>
#include <spm/camdrv.h>
#include <spm/dispdrv.h>
#include <spm/eff_small_star.h>
#include <spm/eff_spm_confetti.h>
#include <spm/eff_zunbaba.h>
#include <spm/evt_cam.h>
#include <spm/evt_dimen.h>
#include <spm/evt_eff.h>
#include <spm/evt_fade.h>
#include <spm/evt_fairy.h>
#include <spm/evt_frame.h>
#include <spm/evt_guide.h>
#include <spm/evt_hit.h>
#include <spm/evt_img.h>
#include <spm/evt_item.h>
#include <spm/evt_map.h>
#include <spm/evt_mario.h>
#include <spm/evt_msg.h>
#include <spm/evt_mobj.h>
#include <spm/evt_npc.h>
#include <spm/evt_offscreen.h>
#include <spm/evt_paper.h>
#include <spm/evt_pouch.h>
#include <spm/evt_shop.h>
#include <spm/evt_snd.h>
#include <spm/evt_seq.h>
#include <spm/evt_sub.h>
#include <spm/evtmgr.h>
#include <spm/evtmgr_cmd.h>
#include <spm/evt_door.h>
#include <spm/fontmgr.h>
#include <spm/hitdrv.h>
#include <spm/hud.h>
#include <spm/itemdrv.h>
#include <spm/item_data.h>
#include <spm/lz_embedded.h>
#include <spm/map_data.h>
#include <spm/mapdrv.h>
#include <spm/mario.h>
#include <spm/mario_pouch.h>
#include <spm/mobjdrv.h>
#include <spm/memory.h>
#include <spm/msgdrv.h>
#include <spm/npcdrv.h>
#include <spm/parse.h>
#include <spm/seqdef.h>
#include <spm/seqdrv.h>
#include <spm/seq_title.h>
#include <spm/spmario.h>
#include <spm/swdrv.h>
#include <spm/system.h>
#include <spm/rel/dan.h>
#include <spm/rel/machi.h>
#include <wii/os/OSError.h>
#include <wii/cx.h>
#include <wii/gx.h>
#include <wii/wpad.h>
#include <msl/math.h>
#include <msl/stdio.h>
#include <msl/string.h>
#include <cstdio>

namespace mod
{

    bool display_placed_clear = false;
    bool display_type = false;
    bool display_orient = false;

    static spm::seqdef::SeqFunc *seq_titleMainReal;
    static void seq_titleMainOverride(spm::seqdrv::SeqWork *wp)
    {
        wii::gx::GXColor notgreen = {255, 255, 180, 255};
        f32 scale = 0.8f;
        const char *msg = "SPM Blocks of Wisdom v2";
        spm::fontmgr::FontDrawStart();
        spm::fontmgr::FontDrawEdge();
        spm::fontmgr::FontDrawColor(&notgreen);
        spm::fontmgr::FontDrawScale(scale);
        spm::fontmgr::FontDrawNoiseOff();
        spm::fontmgr::FontDrawRainbowColorOff();
        f32 x = -((spm::fontmgr::FontGetMessageWidth(msg) * scale) / 2);
        spm::fontmgr::FontDrawString(x, 200.0f, msg);
        seq_titleMainReal(wp);
    }

    static spm::seqdef::SeqFunc *seq_gameMainReal;
    void displayBlkRem(spm::seqdrv::SeqWork *wp)
    {
        if (display_placed_clear)
        {
            wii::gx::GXColor funnyColor = {255, 255, 180, 225};
            f32 scale = 0.5f;
            char buffer[50];
            const char *displayBlkRemTextOptions[] = {"Blocks cleared. 5/5 remaining.", "Block spawned. 4/5 remaining.", "Block spawned. 3/5 remaining.", "Block spawned. 2/5 remaining.", "Block spawned. 1/5 remaining.", "Block spawned. 0 remaining."};
            int blocksSpawned = spm::swdrv::swByteGet(70);
            const char *displayBlkRemText = displayBlkRemTextOptions[blocksSpawned];
            msl::stdio::sprintf(buffer, "%s", displayBlkRemText);
            const char *msg = buffer;
            spm::fontmgr::FontDrawStart();
            spm::fontmgr::FontDrawEdge();
            spm::fontmgr::FontDrawColor(&funnyColor);
            spm::fontmgr::FontDrawScale(scale);
            spm::fontmgr::FontDrawNoiseOff();
            spm::fontmgr::FontDrawRainbowColorOff();
            spm::fontmgr::FontDrawString(-360, -200.0f, msg);
        }
        seq_gameMainReal(wp);
    }

    void displayBlkType(spm::seqdrv::SeqWork *wp)
    {
        if (display_type)
        {
            wii::gx::GXColor funnyColor = {255, 180, 255, 225};
            f32 scale = 0.5f;
            char buffer[50];
            const char *displayBlkTypeTextOptions[] = {"Block type: Throwable", "Block type: Coin"};
            int currentBlockType = spm::swdrv::swByteGet(71);
            const char *displayBlkTypeText = displayBlkTypeTextOptions[currentBlockType];
            msl::stdio::sprintf(buffer, "%s", displayBlkTypeText);
            const char *msg = buffer;
            spm::fontmgr::FontDrawStart();
            spm::fontmgr::FontDrawEdge();
            spm::fontmgr::FontDrawColor(&funnyColor);
            spm::fontmgr::FontDrawScale(scale);
            spm::fontmgr::FontDrawNoiseOff();
            spm::fontmgr::FontDrawRainbowColorOff();
            spm::fontmgr::FontDrawString(-360, -185.0f, msg);
        }
        seq_gameMainReal(wp);
    }

    void displayBlkOrient(spm::seqdrv::SeqWork *wp)
    {
        if (display_orient)
        {
            wii::gx::GXColor funnyColor = {180, 255, 255, 225};
            f32 scale = 0.5f;
            char buffer[50];
            const char *displayBlkOrientTextOptions[] = {"Orientation: Directly Above", "Orientation: Facing Direction"};
            int currentBlockOrient = spm::swdrv::swByteGet(72);
            int orientTxt = 0;
            if (currentBlockOrient != 0)
            {
                orientTxt = 1;
            }
            const char *displayBlkOrientText = displayBlkOrientTextOptions[orientTxt];
            msl::stdio::sprintf(buffer, "%s", displayBlkOrientText);
            const char *msg = buffer;
            spm::fontmgr::FontDrawStart();
            spm::fontmgr::FontDrawEdge();
            spm::fontmgr::FontDrawColor(&funnyColor);
            spm::fontmgr::FontDrawScale(scale);
            spm::fontmgr::FontDrawNoiseOff();
            spm::fontmgr::FontDrawRainbowColorOff();
            spm::fontmgr::FontDrawString(-360, -170.0f, msg);
        }
        seq_gameMainReal(wp);
    }

    void textDisplay(spm::seqdrv::SeqWork *wp)
    {
        displayBlkRem(wp);
        displayBlkOrient(wp);
        displayBlkType(wp);
    }

    static void titleScreenCustomTextPatch()
    {
        seq_gameMainReal = spm::seqdef::seq_data[spm::seqdrv::SEQ_GAME].main;
        spm::seqdef::seq_data[spm::seqdrv::SEQ_GAME].main = &textDisplay;
        seq_titleMainReal = spm::seqdef::seq_data[spm::seqdrv::SEQ_TITLE].main;
        spm::seqdef::seq_data[spm::seqdrv::SEQ_TITLE].main = &seq_titleMainOverride;
    }

    s32 evtMarioChkKey(spm::evtmgr::EvtEntry *evtEntry, bool firstRun)
    {
        s32 marioKey = spm::mario::marioChkKey(); // "is mario allowed to move?" returns true if so
        s32 mario3d = spm::mario::marioCheck3d();
        spm::mario::MarioWork* marioWp = spm::mario::marioGetPtr();
        f32 marioAngle = marioWp->dispDirectionTarget;
        spm::evtmgr_cmd::evtSetValue(evtEntry, evtEntry->pCurData[0], marioKey);
        spm::evtmgr_cmd::evtSetValue(evtEntry, evtEntry->pCurData[1], mario3d);
        spm::evtmgr_cmd::evtSetValue(evtEntry, evtEntry->pCurData[2], marioAngle);
        return 2;
    }
    EVT_DECLARE_USER_FUNC(evtMarioChkKey, 3)

    s32 setBlkStr(spm::evtmgr::EvtEntry *evtEntry, bool firstRun)
    {
        spm::evtmgr::EvtScriptCode *args = evtEntry->pCurData;
        s32 strType = spm::evtmgr_cmd::evtGetValue(evtEntry, args[0]);
        switch (strType)
        {
        case 0: // Block placed or cleared
            display_placed_clear = true;
            break;
        case 1: // Block type changed
            display_type = true;
            break;
        case 2: // Block orientation changed
            display_orient = true;
            break;
        }
        return 2;
    }
    EVT_DECLARE_USER_FUNC(setBlkStr, 1)

    s32 clearText(spm::evtmgr::EvtEntry *evtEntry, bool firstRun)
    {
        display_placed_clear = false;
        display_type = false;
        display_orient = false;
        return 2;
    }
    EVT_DECLARE_USER_FUNC(clearText, 0)

    const char *blockNames[] = {"BOW_01", "BOW_02", "BOW_03", "BOW_04", "BOW_05"};

    EVT_BEGIN(bow)
        SET(GSW(70), 0)
        SET_READ(PTR(blockNames))
        IF_EQUAL(GSWF(238), 0) // Check if block spawn pos values have been initialized before; if not, set them.
            SET(GSWF(238), 1)
            SET(GSW(73), 70)
            SET(GSW(74), 70)
        END_IF()
        DO(0)
            IF_LARGE(GSW(69), 0)
                SUB(GSW(69), 1)
            ELSE()
                USER_FUNC(evtMarioChkKey, LW(0), LW(5), LW(6))
                IF_EQUAL(LW(0), 1)
                    USER_FUNC(spm::evt_sub::evt_key_get_button, 0, LW(3))
                    IF_SMALL(GSW(70), 5)
                        // Check for B-Up (Spawn Block)
                        IF_EQUAL(LW(3), 1026) // 0x400 is B, 0x2 is Right
                            SET(GSW(69), 60)
                            USER_FUNC(spm::evt_mario::evt_mario_get_pos, LW(0), LW(1), LW(2))
                            ADD(LW(1), GSW(73))
                            // Modify X and Z coords based on facing direction & 3D state
                            IF_EQUAL(LW(5), 0) // In 2D
                                IF_EQUAL(LW(6), FLOAT(180.0)) // Facing right
                                    ADD(LW(0), GSW(72)) // Add to X pos
                                ELSE() // Facing left
                                    SUB(LW(0), GSW(72)) // Sub from X pos
                                END_IF()
                            ELSE() // In 3D
                                IF_EQUAL(LW(6), FLOAT(180.0)) // Facing right
                                    ADD(LW(2), GSW(72)) // Add to Z pos
                                ELSE() // Facing left
                                    SUB(LW(2), GSW(72)) // Sub from Z pos
                                END_IF()
                            END_IF()
                            READ(LW(4))
                            // Check for block type
                            SWITCH(GSW(71))
                                CASE_EQUAL(0)
                                    USER_FUNC(spm::evt_mobj::evt_mobj_blk, 22, LW(4), LW(0), LW(1), LW(2), 0, 0, 0)
                                CASE_EQUAL(1)
                                    USER_FUNC(spm::evt_mobj::evt_mobj_blk, 1, LW(4), LW(0), LW(1), LW(2), 1, 0, 0)
                            END_SWITCH()
                            ADD(LW(1), 15)
                            USER_FUNC(spm::evt_eff::evt_eff, 0, PTR("kemuri_test"), 0, LW(0), LW(1), LW(2), FLOAT(1.5), 0, 0, 0, 0, 0, 0, 0)
                            USER_FUNC(spm::evt_snd::evt_snd_sfxon_3d, PTR("SFX_E_FIREBROS_ATTACK1"), LW(0), LW(1), LW(2))
                            ADD(GSW(70), 1)
                            USER_FUNC(setBlkStr, 0)
                        END_IF()
                    END_IF()
                    // Check for B-Down (Clear All)
                    IF_EQUAL(LW(3), 1025) // 0x400 is B, 0x1 is Left
                        SET(GSW(69), 60)
                        // Check # of blocks, delete all that exist
                        IF_LARGE_EQUAL(GSW(70), 1)
                            USER_FUNC(spm::evt_mobj::evt_mobj_get_position, PTR("BOW_01"), LW(0), LW(1), LW(2))
                            USER_FUNC(spm::evt_eff::evt_eff, 0, PTR("spm_explosion"), 0, LW(0), LW(1), LW(2), FLOAT(0.4), FLOAT(0.4), 0, 0, 0, 0, 0, 0)
                            USER_FUNC(spm::evt_snd::evt_snd_sfxon_3d, PTR("SFX_E_CAMEREBOM2_APPEAR1"), LW(0), LW(1), LW(2))
                            ADD(LW(1), 15)
                            USER_FUNC(spm::evt_eff::evt_eff, 0, PTR("spm_confetti"), 0, LW(0), LW(1), LW(2), FLOAT(1.0), 0, 0, 0, 0, 0, 0, 0)
                            USER_FUNC(spm::evt_mobj::evt_mobj_delete, PTR("BOW_01"))
                        END_IF()
                        IF_LARGE_EQUAL(GSW(70), 2)
                            USER_FUNC(spm::evt_mobj::evt_mobj_get_position, PTR("BOW_02"), LW(0), LW(1), LW(2))
                            USER_FUNC(spm::evt_eff::evt_eff, 0, PTR("spm_explosion"), 0, LW(0), LW(1), LW(2), FLOAT(0.4), FLOAT(0.4), 0, 0, 0, 0, 0, 0)
                            USER_FUNC(spm::evt_snd::evt_snd_sfxon_3d, PTR("SFX_E_CAMEREBOM2_APPEAR1"), LW(0), LW(1), LW(2))
                            ADD(LW(1), 15)
                            USER_FUNC(spm::evt_eff::evt_eff, 0, PTR("spm_confetti"), 0, LW(0), LW(1), LW(2), FLOAT(1.0), 0, 0, 0, 0, 0, 0, 0)
                            USER_FUNC(spm::evt_mobj::evt_mobj_delete, PTR("BOW_02"))
                        END_IF()
                        IF_LARGE_EQUAL(GSW(70), 3)
                            USER_FUNC(spm::evt_mobj::evt_mobj_get_position, PTR("BOW_03"), LW(0), LW(1), LW(2))
                            USER_FUNC(spm::evt_eff::evt_eff, 0, PTR("spm_explosion"), 0, LW(0), LW(1), LW(2), FLOAT(0.4), FLOAT(0.4), 0, 0, 0, 0, 0, 0)
                            USER_FUNC(spm::evt_snd::evt_snd_sfxon_3d, PTR("SFX_E_CAMEREBOM2_APPEAR1"), LW(0), LW(1), LW(2))
                            ADD(LW(1), 15)
                            USER_FUNC(spm::evt_eff::evt_eff, 0, PTR("spm_confetti"), 0, LW(0), LW(1), LW(2), FLOAT(1.0), 0, 0, 0, 0, 0, 0, 0)
                            USER_FUNC(spm::evt_mobj::evt_mobj_delete, PTR("BOW_03"))
                        END_IF()
                        IF_LARGE_EQUAL(GSW(70), 4)
                            USER_FUNC(spm::evt_mobj::evt_mobj_get_position, PTR("BOW_04"), LW(0), LW(1), LW(2))
                            USER_FUNC(spm::evt_eff::evt_eff, 0, PTR("spm_explosion"), 0, LW(0), LW(1), LW(2), FLOAT(0.4), FLOAT(0.4), 0, 0, 0, 0, 0, 0)
                            USER_FUNC(spm::evt_snd::evt_snd_sfxon_3d, PTR("SFX_E_CAMEREBOM2_APPEAR1"), LW(0), LW(1), LW(2))
                            ADD(LW(1), 15)
                            USER_FUNC(spm::evt_eff::evt_eff, 0, PTR("spm_confetti"), 0, LW(0), LW(1), LW(2), FLOAT(1.0), 0, 0, 0, 0, 0, 0, 0)
                            USER_FUNC(spm::evt_mobj::evt_mobj_delete, PTR("BOW_04"))
                        END_IF()
                        IF_EQUAL(GSW(70), 5)
                            USER_FUNC(spm::evt_mobj::evt_mobj_get_position, PTR("BOW_05"), LW(0), LW(1), LW(2))
                            USER_FUNC(spm::evt_eff::evt_eff, 0, PTR("spm_explosion"), 0, LW(0), LW(1), LW(2), FLOAT(0.4), FLOAT(0.4), 0, 0, 0, 0, 0, 0)
                            USER_FUNC(spm::evt_snd::evt_snd_sfxon_3d, PTR("SFX_E_CAMEREBOM2_APPEAR1"), LW(0), LW(1), LW(2))
                            ADD(LW(1), 15)
                            USER_FUNC(spm::evt_eff::evt_eff, 0, PTR("spm_confetti"), 0, LW(0), LW(1), LW(2), FLOAT(1.0), 0, 0, 0, 0, 0, 0, 0)
                            USER_FUNC(spm::evt_mobj::evt_mobj_delete, PTR("BOW_05"))
                        END_IF()
                        SET(GSW(70), 0)
                        USER_FUNC(setBlkStr, 0)
                        SET_READ(PTR(blockNames))
                    END_IF()
                    // Check for B-Right: Change Block Type
                    IF_EQUAL(LW(3), 1028) // 0x400 is B, 0x4 is Down
                        SET(GSW(69), 30)
                        IF_SMALL(GSW(71), 1)
                            ADD(GSW(71), 1)
                        ELSE()
                            SET(GSW(71), 0)
                        END_IF()
                        USER_FUNC(setBlkStr, 1)
                        USER_FUNC(spm::evt_mario::evt_mario_get_pos, LW(0), LW(1), LW(2))
                        USER_FUNC(spm::evt_snd::evt_snd_sfxon_3d, PTR("SFX_MOBJ_BLOCK_STRIKE1"), LW(0), LW(1), LW(2))
                    //    USER_FUNC(spm::evt_eff::evt_eff, 0, PTR("spm_spindash"), 0, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
                    END_IF()
                    // Check for B-Left: Change Block Orientation
                    IF_EQUAL(LW(3), 1032) // 0x400 is B, 0x8 is Up
                        SET(GSW(69), 30)
                        IF_EQUAL(GSW(72), 0)
                            SET(GSW(72), GSW(74))
                        ELSE()
                            SET(GSW(72), 0)
                        END_IF()
                        USER_FUNC(setBlkStr, 2)
                        USER_FUNC(spm::evt_mario::evt_mario_get_pos, LW(0), LW(1), LW(2))
                        USER_FUNC(evtMarioChkKey, LW(15), LW(5), LW(6)) // Get 3D and angle state
                        USER_FUNC(spm::evt_snd::evt_snd_sfxon_3d, PTR("SFX_EVT_TAMARA_BUNRETU1"), LW(0), LW(1), LW(2))
                        ADD(LW(1), GSW(73))
                        // Modify X and Z coords based on facing direction & 3D state
                        IF_EQUAL(LW(5), 0) // In 2D
                            IF_EQUAL(LW(6), FLOAT(180.0)) // Facing right
                                ADD(LW(0), GSW(72)) // Add to X pos
                            ELSE() // Facing left
                                SUB(LW(0), GSW(72)) // Sub from X pos
                            END_IF()
                        ELSE() // In 3D
                            IF_EQUAL(LW(6), FLOAT(180.0)) // Facing right
                                ADD(LW(2), GSW(72)) // Add to Z pos
                            ELSE() // Facing left
                                SUB(LW(2), GSW(72)) // Sub from Z pos
                            END_IF()
                        END_IF()
                        INLINE_EVT()
                            USER_FUNC(spm::evt_mobj::evt_mobj_blk, 4, PTR("BOW_DUMMY"), LW(0), LW(1), LW(2), 0, 0, 0)
                            WAIT_FRM(10)
                            USER_FUNC(spm::evt_mobj::evt_mobj_delete, PTR("BOW_DUMMY"))
                        END_INLINE()
                    END_IF()
                    // Check for Z-Up: Increase Block Spawn Height (Max 100, Min -100, GSW(73))
                    IF_EQUAL(LW(3), 8194) // 0x2000 is Z, 0x2 is Right
                        IF_SMALL(GSW(73), 100)
                            SET(GSW(69), 15)
                            ADD(GSW(73), 10)
                            USER_FUNC(spm::evt_mario::evt_mario_get_pos, LW(0), LW(1), LW(2))
                            USER_FUNC(evtMarioChkKey, LW(15), LW(5), LW(6)) // Get 3D and angle state
                            USER_FUNC(spm::evt_snd::evt_snd_sfxon_3d, PTR("SFX_MOBJ_BLUE_SWITCH1"), LW(0), LW(1), LW(2))
                            SET(GSW(72), GSW(74))
                            USER_FUNC(setBlkStr, 2)
                            ADD(LW(1), GSW(73))
                            // Modify X and Z coords based on facing direction & 3D state
                            IF_EQUAL(LW(5), 0) // In 2D
                                IF_EQUAL(LW(6), FLOAT(180.0)) // Facing right
                                    ADD(LW(0), GSW(72)) // Add to X pos
                                ELSE() // Facing left
                                    SUB(LW(0), GSW(72)) // Sub from X pos
                                END_IF()
                            ELSE() // In 3D
                                IF_EQUAL(LW(6), FLOAT(180.0)) // Facing right
                                    ADD(LW(2), GSW(72)) // Add to Z pos
                                ELSE() // Facing left
                                    SUB(LW(2), GSW(72)) // Sub from Z pos
                                END_IF()
                            END_IF()
                            INLINE_EVT()
                                USER_FUNC(spm::evt_mobj::evt_mobj_blk, 4, PTR("BOW_DUMMY"), LW(0), LW(1), LW(2), 0, 0, 0)
                                WAIT_FRM(10)
                                USER_FUNC(spm::evt_mobj::evt_mobj_delete, PTR("BOW_DUMMY"))
                            END_INLINE()
                        END_IF()
                    END_IF()
                    // Check for Z-Down: Decrease Block Spawn Height (Max 100, Min 0, GSW(73))
                    IF_EQUAL(LW(3), 8193) // 0x2000 is Z, 0x1 is Left
                        IF_LARGE(GSW(73), 0)
                            SET(GSW(69), 15)
                            ADD(GSW(73), -10)
                            USER_FUNC(spm::evt_mario::evt_mario_get_pos, LW(0), LW(1), LW(2))
                            USER_FUNC(evtMarioChkKey, LW(15), LW(5), LW(6)) // Get 3D and angle state
                            USER_FUNC(spm::evt_snd::evt_snd_sfxon_3d, PTR("SFX_MOBJ_BLUE_SWITCH1"), LW(0), LW(1), LW(2))
                            SET(GSW(72), GSW(74))
                            USER_FUNC(setBlkStr, 2)
                            ADD(LW(1), GSW(73))
                            // Modify X and Z coords based on facing direction & 3D state
                            IF_EQUAL(LW(5), 0) // In 2D
                                IF_EQUAL(LW(6), FLOAT(180.0)) // Facing right
                                    ADD(LW(0), GSW(72)) // Add to X pos
                                ELSE() // Facing left
                                    SUB(LW(0), GSW(72)) // Sub from X pos
                                END_IF()
                            ELSE() // In 3D
                                IF_EQUAL(LW(6), FLOAT(180.0)) // Facing right
                                    ADD(LW(2), GSW(72)) // Add to Z pos
                                ELSE() // Facing left
                                    SUB(LW(2), GSW(72)) // Sub from Z pos
                                END_IF()
                            END_IF()
                            INLINE_EVT()
                                USER_FUNC(spm::evt_mobj::evt_mobj_blk, 4, PTR("BOW_DUMMY"), LW(0), LW(1), LW(2), 0, 0, 0)
                                WAIT_FRM(10)
                                USER_FUNC(spm::evt_mobj::evt_mobj_delete, PTR("BOW_DUMMY"))
                            END_INLINE()
                        END_IF()
                    END_IF()
                    // Check for C-Up: Increase Block Spawn X (Max 100, Min 30, GSW(74))
                    IF_EQUAL(LW(3), 16386) // 0x4000 is C, 0x2 is Right
                        IF_SMALL(GSW(74), 100)
                            SET(GSW(69), 15)
                            ADD(GSW(74), 10)
                            USER_FUNC(spm::evt_mario::evt_mario_get_pos, LW(0), LW(1), LW(2))
                            USER_FUNC(evtMarioChkKey, LW(15), LW(5), LW(6)) // Get 3D and angle state
                            USER_FUNC(spm::evt_snd::evt_snd_sfxon_3d, PTR("SFX_MAP_NUMBERBLOCK1"), LW(0), LW(1), LW(2))
                            SET(GSW(72), GSW(74))
                            USER_FUNC(setBlkStr, 2)
                            ADD(LW(1), GSW(73))
                            // Modify X and Z coords based on facing direction & 3D state
                            IF_EQUAL(LW(5), 0) // In 2D
                                IF_EQUAL(LW(6), FLOAT(180.0)) // Facing right
                                    ADD(LW(0), GSW(72)) // Add to X pos
                                ELSE() // Facing left
                                    SUB(LW(0), GSW(72)) // Sub from X pos
                                END_IF()
                            ELSE() // In 3D
                                IF_EQUAL(LW(6), FLOAT(180.0)) // Facing right
                                    ADD(LW(2), GSW(72)) // Add to Z pos
                                ELSE() // Facing left
                                    SUB(LW(2), GSW(72)) // Sub from Z pos
                                END_IF()
                            END_IF()
                            INLINE_EVT()
                                USER_FUNC(spm::evt_mobj::evt_mobj_blk, 4, PTR("BOW_DUMMY"), LW(0), LW(1), LW(2), 0, 0, 0)
                                WAIT_FRM(10)
                                USER_FUNC(spm::evt_mobj::evt_mobj_delete, PTR("BOW_DUMMY"))
                            END_INLINE()
                        END_IF()
                    END_IF()
                    // Check for C-Down: Decrease Block Spawn X (Max 100, Min 30, GSW(74))
                    IF_EQUAL(LW(3), 16385) // 0x4000 is C, 0x1 is Left
                        IF_LARGE(GSW(74), 30)
                            SET(GSW(69), 15)
                            SUB(GSW(74), 10)
                            USER_FUNC(spm::evt_mario::evt_mario_get_pos, LW(0), LW(1), LW(2))
                            USER_FUNC(evtMarioChkKey, LW(15), LW(5), LW(6)) // Get 3D and angle state
                            USER_FUNC(spm::evt_snd::evt_snd_sfxon_3d, PTR("SFX_MAP_NUMBERBLOCK1"), LW(0), LW(1), LW(2))
                            SET(GSW(72), GSW(74))
                            USER_FUNC(setBlkStr, 2)
                            ADD(LW(1), GSW(73))
                            // Modify X and Z coords based on facing direction & 3D state
                            IF_EQUAL(LW(5), 0) // In 2D
                                IF_EQUAL(LW(6), FLOAT(180.0)) // Facing right
                                    ADD(LW(0), GSW(72)) // Add to X pos
                                ELSE() // Facing left
                                    SUB(LW(0), GSW(72)) // Sub from X pos
                                END_IF()
                            ELSE() // In 3D
                                IF_EQUAL(LW(6), FLOAT(180.0)) // Facing right
                                    ADD(LW(2), GSW(72)) // Add to Z pos
                                ELSE() // Facing left
                                    SUB(LW(2), GSW(72)) // Sub from Z pos
                                END_IF()
                            END_IF()
                            INLINE_EVT()
                                USER_FUNC(spm::evt_mobj::evt_mobj_blk, 4, PTR("BOW_DUMMY"), LW(0), LW(1), LW(2), 0, 0, 0)
                                WAIT_FRM(10)
                                USER_FUNC(spm::evt_mobj::evt_mobj_delete, PTR("BOW_DUMMY"))
                            END_INLINE()
                        END_IF()
                    END_IF()
                ELSE()
                    USER_FUNC(clearText)
                END_IF()
            END_IF()
            WAIT_FRM(1)
        WHILE()
        RETURN()
    EVT_END()

    EVT_BEGIN(run_bow)
        RUN_EVT(bow)
    RETURN_FROM_CALL()

    static void evtPatches()
    {
        // Initialize the patches to the EVT interpreter to add custom opcodes
        evtpatch::evtmgrExtensionInit();

        evtpatch::hookEvt(spm::evt_door::door_init_evt, 1, run_bow);
    }

    void main()
    {
        wii::os::OSReport("SPM Rel Loader: the mod has ran!\n");
        titleScreenCustomTextPatch();
        evtPatches();
    }
}