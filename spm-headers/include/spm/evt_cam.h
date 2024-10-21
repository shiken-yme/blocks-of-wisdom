#pragma once

#include <common.h>
#include <evt_cmd.h>

CPP_WRAPPER(spm::evt_cam)

EVT_UNKNOWN_USER_FUNC(evt_cam_flag_onoff)
EVT_UNKNOWN_USER_FUNC(evt_cam_get_at)
EVT_DECLARE_USER_FUNC(evt_cam_get_pos, 4)
EVT_DECLARE_USER_FUNC(evt_cam_shake, 6)
EVT_DECLARE_USER_FUNC(evt_cam3d_evt_zoom_in, 9)
EVT_UNKNOWN_USER_FUNC(func_800e01f8)
EVT_UNKNOWN_USER_FUNC(func_800e02bc)
EVT_DECLARE_USER_FUNC(evt_cam_zoom_to_coords, 2)
EVT_DECLARE_USER_FUNC(evt_cam_look_at_door, 2)
EVT_UNKNOWN_USER_FUNC(func_800e0720)
EVT_UNKNOWN_USER_FUNC(func_800e07bc)
EVT_UNKNOWN_USER_FUNC(func_800e0890)
EVT_UNKNOWN_USER_FUNC(func_800e08f8)
EVT_UNKNOWN_USER_FUNC(func_800e092c)
EVT_UNKNOWN_USER_FUNC(evt_cam_check_dimension)
EVT_UNKNOWN_USER_FUNC(func_800e0a14)
EVT_UNKNOWN_USER_FUNC(func_800e0a84)
EVT_UNKNOWN_USER_FUNC(func_800e0b58)
EVT_UNKNOWN_USER_FUNC(func_800e0b8c)
EVT_UNKNOWN_USER_FUNC(func_800e0bbc)
EVT_UNKNOWN_USER_FUNC(func_800e0c40)

EVT_DECLARE(lbl_80439f10)


CPP_WRAPPER_END()
