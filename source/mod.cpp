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
    static spm::seqdef::SeqFunc *seq_titleMainReal;
    static void seq_titleMainOverride(spm::seqdrv::SeqWork *wp)
    {
        wii::gx::GXColor notgreen = {255, 255, 180, 255};
        f32 scale = 0.8f;
        const char *msg = "SPM Blocks of Wisdom v1";
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
    static void titleScreenCustomTextPatch()
    {
        seq_titleMainReal = spm::seqdef::seq_data[spm::seqdrv::SEQ_TITLE].main;
        spm::seqdef::seq_data[spm::seqdrv::SEQ_TITLE].main = &seq_titleMainOverride;
    }

    s32 evtMarioChkKey(spm::evtmgr::EvtEntry *evtEntry, bool firstRun)
    {
        s32 marioKey = spm::mario::marioChkKey(); // "is mario allowed to move?" returns true if so
        spm::evtmgr_cmd::evtSetValue(evtEntry, evtEntry->pCurData[0], marioKey);
        return 2;
    }
    EVT_DECLARE_USER_FUNC(evtMarioChkKey, 1)

    const char* blockNames[] = {"BOW_01", "BOW_02", "BOW_03", "BOW_04", "BOW_05"};

    EVT_BEGIN(bow)
        SET(GSW(70), 0)
        SET_READ(PTR(blockNames))
        DO(0)
            IF_LARGE(GSW(69), 0)
                SUB(GSW(69), 1)
            ELSE()
                USER_FUNC(evtMarioChkKey, LW(0))
                IF_EQUAL(LW(0), 1)
                    USER_FUNC(spm::evt_sub::evt_key_get_button, 0, LW(3))
                    IF_SMALL(GSW(70), 5)
                        // Check for B-Up (Spawn Block)
                        IF_EQUAL(LW(3), 1026) // 0x400 is B, 0x2 is Right
                            SET(GSW(69), 60)
                            USER_FUNC(spm::evt_mario::evt_mario_get_pos, LW(0), LW(1), LW(2))
                            ADD(LW(1), 100)
                            READ(LW(4))
                            USER_FUNC(spm::evt_mobj::evt_mobj_blk, 22, LW(4), LW(0), LW(1), LW(2), 0, 0, 0)
                            ADD(GSW(70), 1)
                        END_IF()
                    END_IF()
                    // Check for B-Down (Clear All)
                    IF_EQUAL(LW(3), 1025) // 0x400 is B, 0x1 is Left
                        SET(GSW(69), 60)
                        // Check # of blocks, delete all that exist
                        IF_LARGE_EQUAL(GSW(70), 1)
                            USER_FUNC(spm::evt_mobj::evt_mobj_delete, PTR("BOW_01"))
                        END_IF()
                        IF_LARGE_EQUAL(GSW(70), 2)
                            USER_FUNC(spm::evt_mobj::evt_mobj_delete, PTR("BOW_02"))
                        END_IF()
                        IF_LARGE_EQUAL(GSW(70), 3)
                            USER_FUNC(spm::evt_mobj::evt_mobj_delete, PTR("BOW_03"))
                        END_IF()
                        IF_LARGE_EQUAL(GSW(70), 4)
                            USER_FUNC(spm::evt_mobj::evt_mobj_delete, PTR("BOW_04"))
                        END_IF()
                        IF_EQUAL(GSW(70), 5)
                            USER_FUNC(spm::evt_mobj::evt_mobj_delete, PTR("BOW_05"))
                        END_IF()
                        // Cleared all blocks. 5 remaining.
                        SET(GSW(70), 0)
                        SET_READ(PTR(blockNames))
                    END_IF()
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