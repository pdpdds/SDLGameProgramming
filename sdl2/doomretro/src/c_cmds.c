/*
========================================================================

                           D O O M  R e t r o
         The classic, refined DOOM source port. For Windows PC.

========================================================================

  Copyright © 1993-2012 id Software LLC, a ZeniMax Media company.
  Copyright © 2013-2018 Brad Harding.

  DOOM Retro is a fork of Chocolate DOOM. For a list of credits, see
  <https://github.com/bradharding/doomretro/wiki/CREDITS>.

  This file is part of DOOM Retro.

  DOOM Retro is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation, either version 3 of the License, or (at your
  option) any later version.

  DOOM Retro is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with DOOM Retro. If not, see <https://www.gnu.org/licenses/>.

  DOOM is a registered trademark of id Software LLC, a ZeniMax Media
  company, in the US and/or other countries and is used without
  permission. All other trademarks are the property of their respective
  holders. DOOM Retro is in no way affiliated with nor endorsed by
  id Software.

========================================================================
*/

#include <ctype.h>
#include <float.h>
#include <stdio.h>

#if defined(_WIN32)
#include <Windows.h>
#include <ShellAPI.h>
#endif

#include "am_map.h"
#include "c_cmds.h"
#include "c_console.h"
#include "d_deh.h"
#include "doomstat.h"
#include "g_game.h"
#include "hu_stuff.h"
#include "i_colors.h"
#include "i_gamepad.h"
#include "i_system.h"
#include "m_cheat.h"
#include "m_config.h"
#include "m_menu.h"
#include "m_misc.h"
#include "m_random.h"
#include "p_inter.h"
#include "p_local.h"
#include "p_setup.h"
#include "p_tick.h"
#include "r_sky.h"
#include "s_sound.h"
#include "sc_man.h"
#include "sounds.h"
#include "st_stuff.h"
#include "v_video.h"
#include "version.h"
#include "w_wad.h"
#include "z_zone.h"

#define ALIASCMDFORMAT      "<i>alias</i> [[<b>\"</b>]<i>command</i>[<b>;</b> <i>command</i> ...<b>\"</b>]]"
#define BINDCMDFORMAT       "<i>control</i> [<b>+</b><i>action</i>|[<b>\"</b>]<i>command</i>[<b>;</b> <i>command</i> ...<b>\"</b>]]"
#define EXECCMDFORMAT       "<i>filename</i>"
#define GIVECMDFORMAT       "<b>ammo</b>|<b>armor</b>|<b>health</b>|<b>keys</b>|<b>weapons</b>|<b>all</b>|<i>item</i>"
#define IFCMDFORMAT         "<i>CVAR</i> <i>value</i> <b>then</b> [<b>\"</b>]<i>command</i>[<b>;</b> <i>command</i> ...<b>\"</b>]"
#define KILLCMDFORMAT       "<b>player</b>|<b>all</b>|<i>monster</i>|<b>barrels</b>|<b>missiles</b>"
#define LOADCMDFORMAT       "<i>filename</i><b>.save</b>"
#define MAPCMDFORMAT        "<b>E</b><i>x</i><b>M</b><i>y</i>|<b>MAP</b><i>xy</i>|<b>first</b>|<b>previous</b>|<b>next</b>|<b>last</b>|<b>random</b>"
#define PLAYCMDFORMAT       "<i>sound</i>|<i>music</i>"
#define PRINTCMDFORMAT      "<b>\"</b><i>message</i><b>\"</b>"
#define RESETCMDFORMAT      "<i>CVAR</i>"
#define SAVECMDFORMAT       "<i>filename</i><b>.save</b>"
#define SPAWNCMDFORMAT      "<i>monster</i>|<i>item</i>"
#define TELEPORTCMDFORMAT   "<i>x</i> <i>y</i>"
#define TIMERCMDFORMAT      "<i>minutes</i>"
#define UNBINDCMDFORMAT     "<i>control</i>|<b>+</b><i>action</i>"

#define PENDINGCHANGE       "This change won't be effective until the next map."

#define UNITSPERFOOT        16
#define FEETPERMETER        3.28084f
#define METERSPERKILOMETER  1000
#define FEETPERMILE         5280

alias_t         aliases[MAXALIASES];

static int      ammo;
static int      armor;
static int      health;

static int      mapcmdepisode;
static int      mapcmdmap;
static char     mapcmdlump[7];


dboolean        executingalias = false;
dboolean        vanilla = false;
dboolean        resettingcvar = false;
dboolean        togglingvanilla = false;

char            *version = version_default;

extern dboolean setsizeneeded;
extern dboolean usemouselook;
extern char     *packageconfig;
extern int      st_palette;
extern menu_t   EpiDef;
extern menu_t   ExpDef;
extern menu_t   LoadDef;
extern menu_t   NewDef;
extern menu_t   SaveDef;

control_t controls[] =
{
    { "1",             keyboardcontrol, '1'                   }, { "2",             keyboardcontrol, '2'                    },
    { "3",             keyboardcontrol, '3'                   }, { "4",             keyboardcontrol, '4'                    },
    { "5",             keyboardcontrol, '5'                   }, { "6",             keyboardcontrol, '6'                    },
    { "7",             keyboardcontrol, '7'                   }, { "8",             keyboardcontrol, '8'                    },
    { "9",             keyboardcontrol, '9'                   }, { "0",             keyboardcontrol, '0'                    },
    { "-",             keyboardcontrol, KEY_MINUS             }, { "=",             keyboardcontrol, KEY_EQUALS             },
    { "+",             keyboardcontrol, KEY_EQUALS            }, { "backspace",     keyboardcontrol, KEY_BACKSPACE          },
    { "tab",           keyboardcontrol, KEY_TAB               }, { "q",             keyboardcontrol, 'q'                    },
    { "w",             keyboardcontrol, 'w'                   }, { "e",             keyboardcontrol, 'e'                    },
    { "r",             keyboardcontrol, 'r'                   }, { "t",             keyboardcontrol, 't'                    },
    { "y",             keyboardcontrol, 'y'                   }, { "u",             keyboardcontrol, 'u'                    },
    { "i",             keyboardcontrol, 'i'                   }, { "o",             keyboardcontrol, 'o'                    },
    { "p",             keyboardcontrol, 'p'                   }, { "[",             keyboardcontrol, '['                    },
    { "]",             keyboardcontrol, ']'                   }, { "enter",         keyboardcontrol, KEY_ENTER              },
    { "ctrl",          keyboardcontrol, KEY_CTRL              }, { "a",             keyboardcontrol, 'a'                    },
    { "s",             keyboardcontrol, 's'                   }, { "d",             keyboardcontrol, 'd'                    },
    { "f",             keyboardcontrol, 'f'                   }, { "g",             keyboardcontrol, 'g'                    },
    { "h",             keyboardcontrol, 'h'                   }, { "j",             keyboardcontrol, 'j'                    },
    { "k",             keyboardcontrol, 'k'                   }, { "l",             keyboardcontrol, 'l'                    },
    { ";",             keyboardcontrol, ';'                   }, { "'",             keyboardcontrol, '\''                   },
    { "shift",         keyboardcontrol, KEY_SHIFT             }, { "\\",            keyboardcontrol, '\\'                   },
    { "z",             keyboardcontrol, 'z'                   }, { "x",             keyboardcontrol, 'x'                    },
    { "c",             keyboardcontrol, 'c'                   }, { "v",             keyboardcontrol, 'v'                    },
    { "b",             keyboardcontrol, 'b'                   }, { "n",             keyboardcontrol, 'n'                    },
    { "m",             keyboardcontrol, 'm'                   }, { ",",             keyboardcontrol, ','                    },
    { ".",             keyboardcontrol, '.'                   }, { "/",             keyboardcontrol, '/'                    },
    { "tilde",         keyboardcontrol, '`'                   }, { "alt",           keyboardcontrol, KEY_ALT                },
    { "space",         keyboardcontrol, ' '                   }, { "numlock",       keyboardcontrol, KEY_NUMLOCK            },
    { "capslock",      keyboardcontrol, KEY_CAPSLOCK          }, { "scrolllock",    keyboardcontrol, KEY_SCROLLLOCK         },
    { "home",          keyboardcontrol, KEY_HOME              }, { "up",            keyboardcontrol, KEY_UPARROW            },
    { "pageup",        keyboardcontrol, KEY_PAGEUP            }, { "left",          keyboardcontrol, KEY_LEFTARROW          },
    { "right",         keyboardcontrol, KEY_RIGHTARROW        }, { "end",           keyboardcontrol, KEY_END                },
    { "down",          keyboardcontrol, KEY_DOWNARROW         }, { "pagedown",      keyboardcontrol, KEY_PAGEDOWN           },
    { "insert",        keyboardcontrol, KEY_INSERT            }, { "printscreen",   keyboardcontrol, KEY_PRINTSCREEN        },
    { "delete",        keyboardcontrol, KEY_DELETE            }, { "escape",        keyboardcontrol, KEY_ESCAPE             },
    { "mouse1",        mousecontrol,    0                     }, { "mouse2",        mousecontrol,    1                      },
    { "mouse3",        mousecontrol,    2                     }, { "mouse4",        mousecontrol,    3                      },
    { "mouse5",        mousecontrol,    4                     }, { "mouse6",        mousecontrol,    5                      },
    { "mouse7",        mousecontrol,    6                     }, { "mouse8",        mousecontrol,    7                      },
    { "wheelup",       mousecontrol,    MOUSE_WHEELUP         }, { "wheeldown",     mousecontrol,    MOUSE_WHEELDOWN        },
    { "dpadup",        gamepadcontrol,  GAMEPAD_DPAD_UP       }, { "dpaddown",      gamepadcontrol,  GAMEPAD_DPAD_DOWN      },
    { "dpadleft",      gamepadcontrol,  GAMEPAD_DPAD_LEFT     }, { "dpadright",     gamepadcontrol,  GAMEPAD_DPAD_RIGHT     },
    { "start",         gamepadcontrol,  GAMEPAD_START         }, { "back",          gamepadcontrol,  GAMEPAD_BACK           },
    { "leftthumb",     gamepadcontrol,  GAMEPAD_LEFT_THUMB    }, { "rightthumb",    gamepadcontrol,  GAMEPAD_RIGHT_THUMB    },
    { "leftshoulder",  gamepadcontrol,  GAMEPAD_LEFT_SHOULDER }, { "rightshoulder", gamepadcontrol,  GAMEPAD_RIGHT_SHOULDER },
    { "lefttrigger",   gamepadcontrol,  GAMEPAD_LEFT_TRIGGER  }, { "righttrigger",  gamepadcontrol,  GAMEPAD_RIGHT_TRIGGER  },
    { "gamepad1",      gamepadcontrol,  GAMEPAD_A             }, { "gamepad2",      gamepadcontrol,  GAMEPAD_B              },
    { "gamepad3",      gamepadcontrol,  GAMEPAD_X             }, { "gamepad4",      gamepadcontrol,  GAMEPAD_Y              },
    { "",              0,               0                     }
};

static void alwaysrun_action_func(void);
static void automap_action_func(void);
static void back_action_func(void);
static void clearmark_action_func(void);
static void console_action_func(void);
static void fire_action_func(void);
static void followmode_action_func(void);
static void forward_action_func(void);
static void grid_action_func(void);
static void left_action_func(void);
static void mark_action_func(void);
static void maxzoom_action_func(void);
static void menu_action_func(void);
static void nextweapon_action_func(void);
static void prevweapon_action_func(void);
static void right_action_func(void);
static void rotatemode_action_func(void);
static void screenshot_action_func(void);
static void strafeleft_action_func(void);
static void straferight_action_func(void);
static void use_action_func(void);
static void weapon1_action_func(void);
static void weapon2_action_func(void);
static void weapon3_action_func(void);
static void weapon4_action_func(void);
static void weapon5_action_func(void);
static void weapon6_action_func(void);
static void weapon7_action_func(void);

action_t actions[] =
{
    { "+alwaysrun",    alwaysrun_action_func,   &keyboardalwaysrun,         NULL,                  NULL,             &gamepadalwaysrun,         NULL         },
    { "+automap",      automap_action_func,     &keyboardautomap,           NULL,                  NULL,             &gamepadautomap,           NULL         },
    { "+back",         back_action_func,        &keyboardback,              &keyboardback2,        NULL,             &gamepadback,              NULL         },
    { "+clearmark",    clearmark_action_func,   &keyboardautomapclearmark,  NULL,                  NULL,             &gamepadautomapclearmark,  NULL         },
    { "+console",      console_action_func,     &keyboardconsole,           NULL,                  NULL,             NULL,                      NULL         },
    { "+fire",         fire_action_func,        &keyboardfire,              NULL,                  &mousefire,       &gamepadfire,              NULL         },
    { "+followmode",   followmode_action_func,  &keyboardautomapfollowmode, NULL,                  NULL,             &gamepadautomapfollowmode, NULL         },
    { "+forward",      forward_action_func,     &keyboardforward,           &keyboardforward2,     &mouseforward,    &gamepadforward,           NULL         },
    { "+grid",         grid_action_func,        &keyboardautomapgrid,       NULL,                  NULL,             &gamepadautomapgrid,       NULL         },
    { "+left",         left_action_func,        &keyboardleft,              NULL,                  NULL,             &gamepadleft,              NULL         },
    { "+mark",         mark_action_func,        &keyboardautomapmark,       NULL,                  NULL,             &gamepadautomapmark,       NULL         },
    { "+maxzoom",      maxzoom_action_func,     &keyboardautomapmaxzoom,    NULL,                  NULL,             &gamepadautomapmaxzoom,    NULL         },
    { "+menu",         menu_action_func,        &keyboardmenu,              NULL,                  NULL,             &gamepadmenu,              NULL         },
    { "+mouselook",    NULL,                    &keyboardmouselook,         NULL,                  &mousemouselook,  &gamepadmouselook,         NULL         },
    { "+nextweapon",   nextweapon_action_func,  &keyboardnextweapon,        NULL,                  &mousenextweapon, &gamepadnextweapon,        NULL         },
    { "+prevweapon",   prevweapon_action_func,  &keyboardprevweapon,        NULL,                  &mouseprevweapon, &gamepadprevweapon,        NULL         },
    { "+right",        right_action_func,       &keyboardright,             NULL,                  NULL,             &gamepadright,             NULL         },
    { "+rotatemode",   rotatemode_action_func,  &keyboardautomaprotatemode, NULL,                  NULL,             &gamepadautomaprotatemode, NULL         },
    { "+run",          NULL,                    &keyboardrun,               NULL,                  &mouserun,        &gamepadrun,               NULL         },
    { "+screenshot",   screenshot_action_func,  &keyboardscreenshot,        NULL,                  NULL,             NULL,                      NULL         },
    { "+strafe",       NULL,                    &keyboardstrafe,            NULL,                  &mousestrafe,     &gamepadstrafe,            NULL         },
    { "+strafeleft",   strafeleft_action_func,  &keyboardstrafeleft,        &keyboardstrafeleft2,  NULL,             &gamepadstrafeleft,        NULL         },
    { "+straferight",  straferight_action_func, &keyboardstraferight,       &keyboardstraferight2, NULL,             &gamepadstraferight,       NULL         },
    { "+use",          use_action_func,         &keyboarduse,               &keyboarduse2,         &mouseuse,        &gamepaduse,               &gamepaduse2 },
    { "+weapon1",      weapon1_action_func,     &keyboardweapon1,           NULL,                  NULL,             &gamepadweapon1,           NULL         },
    { "+weapon2",      weapon2_action_func,     &keyboardweapon2,           NULL,                  NULL,             &gamepadweapon2,           NULL         },
    { "+weapon3",      weapon3_action_func,     &keyboardweapon3,           NULL,                  NULL,             &gamepadweapon3,           NULL         },
    { "+weapon4",      weapon4_action_func,     &keyboardweapon4,           NULL,                  NULL,             &gamepadweapon4,           NULL         },
    { "+weapon5",      weapon5_action_func,     &keyboardweapon5,           NULL,                  NULL,             &gamepadweapon5,           NULL         },
    { "+weapon6",      weapon6_action_func,     &keyboardweapon6,           NULL,                  NULL,             &gamepadweapon6,           NULL         },
    { "+weapon7",      weapon7_action_func,     &keyboardweapon7,           NULL,                  NULL,             &gamepadweapon7,           NULL         },
    { "+zoomin",       NULL,                    NULL,                       NULL,                  NULL,             &gamepadautomapzoomin,     NULL         },
    { "+zoomout",      NULL,                    NULL,                       NULL,                  NULL,             &gamepadautomapzoomout,    NULL         },
    { "",              NULL,                    NULL,                       NULL,                  NULL,             NULL,                      NULL         }
};

static dboolean alive_func1(char *cmd, char *parms);
static dboolean cheat_func1(char *cmd, char *parms);
static dboolean game_func1(char *cmd, char *parms);
static dboolean null_func1(char *cmd, char *parms);

void alias_cmd_func2(char *cmd, char *parms);
void bind_cmd_func2(char *cmd, char *parms);
static void bindlist_cmd_func2(char *cmd, char *parms);
static void clear_cmd_func2(char *cmd, char *parms);
static void cmdlist_cmd_func2(char *cmd, char *parms);
static void condump_cmd_func2(char *cmd, char *parms);
static void cvarlist_cmd_func2(char *cmd, char *parms);
static void endgame_cmd_func2(char *cmd, char *parms);
static void exec_cmd_func2(char *cmd, char *parms);
static void exitmap_cmd_func2(char *cmd, char *parms);
static dboolean fastmonsters_cmd_func1(char *cmd, char *parms);
static void fastmonsters_cmd_func2(char *cmd, char *parms);
static void freeze_cmd_func2(char *cmd, char *parms);
static dboolean give_cmd_func1(char *cmd, char *parms);
static void give_cmd_func2(char *cmd, char *parms);
static dboolean god_cmd_func1(char *cmd, char *parms);
static void god_cmd_func2(char *cmd, char *parms);
static void help_cmd_func2(char *cmd, char *parms);
static void if_cmd_func2(char *cmd, char *parms);
static dboolean kill_cmd_func1(char *cmd, char *parms);
static void kill_cmd_func2(char *cmd, char *parms);
static void load_cmd_func2(char *cmd, char *parms);
static dboolean map_cmd_func1(char *cmd, char *parms);
static void map_cmd_func2(char *cmd, char *parms);
static void maplist_cmd_func2(char *cmd, char *parms);
static void mapstats_cmd_func2(char *cmd, char *parms);
static void newgame_cmd_func2(char *cmd, char *parms);
static void noclip_cmd_func2(char *cmd, char *parms);
static void nomonsters_cmd_func2(char *cmd, char *parms);
static void notarget_cmd_func2(char *cmd, char *parms);
static void pistolstart_cmd_func2(char *cmd, char *parms);
static dboolean play_cmd_func1(char *cmd, char *parms);
static void play_cmd_func2(char *cmd, char *parms);
static void playerstats_cmd_func2(char *cmd, char *parms);
static void print_cmd_func2(char *cmd, char *parms);
static void quit_cmd_func2(char *cmd, char *parms);
static void regenhealth_cmd_func2(char *cmd, char *parms);
static void reset_cmd_func2(char *cmd, char *parms);
static void resetall_cmd_func2(char *cmd, char *parms);
static void respawnitems_cmd_func2(char *cmd, char *parms);
static dboolean respawnmonsters_cmd_func1(char *cmd, char *parms);
static void respawnmonsters_cmd_func2(char *cmd, char *parms);
static void restartmap_cmd_func2(char *cmd, char *parms);
static dboolean resurrect_cmd_func1(char *cmd, char *parms);
static void resurrect_cmd_func2(char *cmd, char *parms);
static dboolean save_cmd_func1(char *cmd, char *parms);
static void save_cmd_func2(char *cmd, char *parms);
static dboolean spawn_cmd_func1(char *cmd, char *parms);
static void spawn_cmd_func2(char *cmd, char *parms);
static void teleport_cmd_func2(char *cmd, char *parms);
static void thinglist_cmd_func2(char *cmd, char *parms);
static void timer_cmd_func2(char *cmd, char *parms);
static void unbind_cmd_func2(char *cmd, char *parms);
static void vanilla_cmd_func2(char *cmd, char *parms);

static dboolean bool_cvars_func1(char *cmd, char *parms);
static void bool_cvars_func2(char *cmd, char *parms);
static dboolean color_cvars_func1(char *cmd, char *parms);
static void color_cvars_func2(char *cmd, char *parms);
static dboolean float_cvars_func1(char *cmd, char *parms);
static dboolean int_cvars_func1(char *cmd, char *parms);
static void int_cvars_func2(char *cmd, char *parms);
static void str_cvars_func2(char *cmd, char *parms);
static void time_cvars_func2(char *cmd, char *parms);

static void alwaysrun_cvar_func2(char *cmd, char *parms);
static void am_external_cvar_func2(char *cmd, char *parms);
static dboolean am_followmode_cvar_func1(char *cmd, char *parms);
static void am_gridsize_cvar_func2(char *cmd, char *parms);
static void am_path_cvar_func2(char *cmd, char *parms);
static void episode_cvar_func2(char *cmd, char *parms);
static void expansion_cvar_func2(char *cmd, char *parms);
static dboolean gp_deadzone_cvars_func1(char *cmd, char *parms);
static void gp_deadzone_cvars_func2(char *cmd, char *parms);
static void gp_sensitivity_cvar_func2(char *cmd, char *parms);
static void gp_swapthumbsticks_cvar_func2(char *cmd, char *parms);
static void mouselook_cvar_func2(char *cmd, char *parms);
static dboolean player_cvars_func1(char *cmd, char *parms);
static void player_cvars_func2(char *cmd, char *parms);
static void playername_cvar_func2(char *cmd, char *parms);
static dboolean r_blood_cvar_func1(char *cmd, char *parms);
static void r_blood_cvar_func2(char *cmd, char *parms);
static void r_bloodsplats_translucency_cvar_func2(char *cmd, char *parms);
static dboolean r_detail_cvar_func1(char *cmd, char *parms);
static void r_detail_cvar_func2(char *cmd, char *parms);
static void r_dither_cvar_func2(char *cmd, char *parms);
static void r_fixmaperrors_cvar_func2(char *cmd, char *parms);
static void r_fov_cvar_func2(char *cmd, char *parms);
static dboolean r_gamma_cvar_func1(char *cmd, char *parms);
static void r_gamma_cvar_func2(char *cmd, char *parms);
static void r_hud_cvar_func2(char *cmd, char *parms);
static void r_hud_translucency_cvar_func2(char *cmd, char *parms);
static void r_lowpixelsize_cvar_func2(char *cmd, char *parms);
static void r_messagepos_cvar_func2(char *cmd, char *parms);
static dboolean r_messagescale_cvar_func1(char *cmd, char *parms);
static void r_messagescale_cvar_func2(char *cmd, char *parms);
static void r_screensize_cvar_func2(char *cmd, char *parms);
static void r_shadows_translucency_cvar_func2(char *cmd, char *parms);
static dboolean r_skycolor_cvar_func1(char *cmd, char *parms);
static void r_skycolor_cvar_func2(char *cmd, char *parms);
static void r_textures_cvar_func2(char *cmd, char *parms);
static void r_translucency_cvar_func2(char *cmd, char *parms);
static dboolean s_volume_cvars_func1(char *cmd, char *parms);
static void s_volume_cvars_func2(char *cmd, char *parms);
static void savegame_cvar_func2(char *cmd, char *parms);
static void skilllevel_cvar_func2(char *cmd, char *parms);
static dboolean turbo_cvar_func1(char *cmd, char *parms);
static void turbo_cvar_func2(char *cmd, char *parms);
static dboolean units_cvar_func1(char *cmd, char *parms);
static void units_cvar_func2(char *cmd, char *parms);
static dboolean vid_capfps_cvar_func1(char *cmd, char *parms);
static void vid_capfps_cvar_func2(char *cmd, char *parms);
static void vid_display_cvar_func2(char *cmd, char *parms);
static void vid_fullscreen_cvar_func2(char *cmd, char *parms);
static dboolean vid_scaleapi_cvar_func1(char *cmd, char *parms);
static void vid_scaleapi_cvar_func2(char *cmd, char *parms);
static dboolean vid_scalefilter_cvar_func1(char *cmd, char *parms);
static void vid_scalefilter_cvar_func2(char *cmd, char *parms);
static void vid_screenresolution_cvar_func2(char *cmd, char *parms);
static void vid_showfps_cvar_func2(char *cmd, char *parms);
static void vid_vsync_cvar_func2(char *cmd, char *parms);
static void vid_widescreen_cvar_func2(char *cmd, char *parms);
static void vid_windowpos_cvar_func2(char *cmd, char *parms);
static void vid_windowsize_cvar_func2(char *cmd, char *parms);

static int C_LookupValueFromAlias(const char *text, const valuealias_type_t valuealiastype)
{
    for (int i = 0; *valuealiases[i].text; i++)
        if (valuealiastype == valuealiases[i].type && M_StringCompare(text, valuealiases[i].text))
            return valuealiases[i].value;

    return INT_MIN;
}

static char *C_LookupAliasFromValue(const int value, const valuealias_type_t valuealiastype)
{
    for (int i = 0; *valuealiases[i].text; i++)
        if (valuealiastype == valuealiases[i].type && value == valuealiases[i].value)
            return valuealiases[i].text;

    return commify(value);
}

#define CMD(name, alt, cond, func, parms, form, desc) \
    { #name, #alt, cond, func, parms, CT_CMD, CF_NONE, NULL, 0, 0, 0, form, desc, 0, 0 }
#define CMD_CHEAT(name, parms) \
    { #name, "", cheat_func1, NULL, parms, CT_CHEAT, CF_NONE, NULL, 0, 0, 0, "", "", 0, 0 }
#define CVAR_BOOL(name, alt, cond, func, valuealiases, desc) \
    { #name, #alt, cond, func, 1, CT_CVAR, CF_BOOLEAN, &name, valuealiases, false, true, "", desc, name##_default, 0 }
#define CVAR_INT(name, alt, cond, func, flags, valuealiases, desc) \
    { #name, #alt, cond, func, 1, CT_CVAR, (CF_INTEGER | flags), &name, valuealiases, name##_min, name##_max, "", desc, name##_default, 0 }
#define CVAR_FLOAT(name, alt, cond, func, flags, desc) \
    { #name, #alt, cond, func, 1, CT_CVAR, (CF_FLOAT | flags), &name, 0, 0, 0, "", desc, name##_default, 0 }
#define CVAR_POS(name, alt, cond, func, desc) \
    { #name, #alt, cond, func, 1, CT_CVAR, CF_POSITION, &name, 0, 0, 0, "", desc, 0, name##_default }
#define CVAR_SIZE(name, alt, cond, func, desc) \
    { #name, #alt, cond, func, 1, CT_CVAR, CF_SIZE, &name, 0, 0, 0, "", desc, 0, name##_default }
#define CVAR_STR(name, alt, cond, func, flags, desc) \
    { #name, #alt, cond, func, 1, CT_CVAR, (CF_STRING | flags), &name, 0, 0, 0, "", desc, 0, name##_default }
#define CVAR_TIME(name, alt, cond, func, desc) \
    { #name, #alt, cond, func, 1, CT_CVAR, (CF_TIME | CF_READONLY), &name, 0, 0, 0, "", desc, 0, "" }

consolecmd_t consolecmds[] =
{
    CMD(alias, "", null_func1, alias_cmd_func2, true, ALIASCMDFORMAT,
        "Creates an <i>alias</i> that executes a string of\n<i>commands</i>."),
    CVAR_BOOL(alwaysrun, "", bool_cvars_func1, alwaysrun_cvar_func2, BOOLVALUEALIAS,
        "Toggles the player always running when they move."),
    CVAR_INT(am_allmapcdwallcolor, am_allmapcdwallcolour, color_cvars_func1, color_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The color of lines with a change in ceiling height in\nthe automap when the player has the computer\narea map power-up (<b>0</b> to <b>255</b>, or <b>#</b><i>rrggbb</i>)."),
    CVAR_INT(am_allmapfdwallcolor, am_allmapfdwallcolour, color_cvars_func1, color_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The color of lines with a change in floor height in\nthe automap when the player has the computer\narea map power-up (<b>0</b> to <b>255</b>, or <b>#</b><i>rrggbb</i>)."),
    CVAR_INT(am_allmapwallcolor, am_allmapwallcolour, color_cvars_func1, color_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The color of solid walls in the automap when the\nplayer has the computer area map power-up (<b>0</b> to\n<b>255</b>, or <b>#</b><i>rrggbb</i>)."),
    CVAR_INT(am_backcolor, am_backcolour, color_cvars_func1, color_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The color of the automap's background (<b>0</b> to <b>255</b>,\nor <b>#</b><i>rrggbb</i>)."),
    CVAR_INT(am_cdwallcolor, am_cdwallcolour, color_cvars_func1, color_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The color of lines with a change in ceiling height in\nthe automap (<b>0</b> to <b>255</b>, or <b>#</b><i>rrggbb</i>)."),
    CVAR_INT(am_crosshaircolor, am_crosshaircolour, color_cvars_func1, color_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The color of the crosshair in the automap (<b>0</b> to <b>255</b>,\nor <b>#</b><i>rrggbb</i>)."),
    CVAR_BOOL(am_external, "", bool_cvars_func1, am_external_cvar_func2, BOOLVALUEALIAS,
        "Toggles showing the automap on an external\ndisplay."),
    CVAR_INT(am_fdwallcolor, am_fdwallcolour, color_cvars_func1, color_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The color of lines with a change in floor height in\nthe automap (<b>0</b> to <b>255</b>, or <b>#</b><i>rrggbb</i>)."),
    CVAR_BOOL(am_followmode, "", am_followmode_cvar_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles follow mode in the automap."),
    CVAR_BOOL(am_grid, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles the grid in the automap."),
    CVAR_INT(am_gridcolor, am_gridcolour, color_cvars_func1, color_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The color of the grid in the automap (<b>0</b> to <b>255</b>, or\n<b>#</b><i>rrggbb</i>)."),
    CVAR_SIZE(am_gridsize, "", null_func1, am_gridsize_cvar_func2,
        "The size of the grid in the automap (<i>width</i><b>\xD7</b><i>height</i>)."),
    CVAR_INT(am_markcolor, am_markcolour, color_cvars_func1, color_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The color of marks in the automap (<b>0</b> to <b>255</b>, or\n<b>#</b><i>rrggbb</i>)."),
    CVAR_BOOL(am_path, "", bool_cvars_func1, am_path_cvar_func2, BOOLVALUEALIAS,
        "Toggles the player's path in the automap."),
    CVAR_INT(am_pathcolor, am_pathcolour, color_cvars_func1, color_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The color of the player's path in the automap (<b>0</b> to\n<b>255</b>, or <b>#</b><i>rrggbb</i>)."),
    CVAR_INT(am_playercolor, am_playercolour, color_cvars_func1, color_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The color of the player in the automap (<b>0</b> to <b>255</b>, or\n<b>#</b><i>rrggbb</i>)."),
    CVAR_BOOL(am_rotatemode, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles rotate mode in the automap."),
    CVAR_INT(am_teleportercolor, am_teleportercolour, color_cvars_func1, color_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The color of teleporters in the automap (<b>0</b> to <b>255</b>,\nor <b>#</b><i>rrggbb</i>)."),
    CVAR_INT(am_thingcolor, am_thingcolour, color_cvars_func1, color_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The color of things in the automap (<b>0</b> to <b>255</b>, or\n<b>#</b><i>rrggbb</i>)."),
    CVAR_INT(am_tswallcolor, am_tswallcolour, color_cvars_func1, color_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The color of lines with no change in height in the\nautomap (<b>0</b> to <b>255</b>, or <b>#</b><i>rrggbb</i>)."),
    CVAR_INT(am_wallcolor, am_wallcolour, color_cvars_func1, color_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The color of solid walls in the automap (<b>0</b> to <b>255</b>, or\n<b>#</b><i>rrggbb</i>)."),
    CVAR_INT(ammo, "", player_cvars_func1, player_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The amount of ammo for the player's currently\nequipped weapon."),
    CVAR_INT(armor, armour, player_cvars_func1, player_cvars_func2, CF_PERCENT, NOVALUEALIAS,
        "The player's armor."),
    CVAR_BOOL(autoaim, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles vertical autoaiming as the player fires\ntheir weapon while using mouselook."),
    CVAR_BOOL(autoload, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles automatically loading the last savegame\nafter the player dies."),
    CVAR_BOOL(autouse, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles automatically using a door or switch when\nthe player is near."),
    CMD(bind, "", null_func1, bind_cmd_func2, true, BINDCMDFORMAT,
        "Binds an <i>action</i> or string of <i>commands</i> to a\n<i>control</i>."),
    CMD(bindlist, "", null_func1, bindlist_cmd_func2, false, "",
        "Shows a list of all bound controls."),
    CVAR_BOOL(centerweapon, centreweapon, bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles centering the player's weapon when firing."),
    CMD(clear, "", null_func1, clear_cmd_func2, false, "",
        "Clears the console."),
    CMD(cmdlist, ccmdlist, null_func1, cmdlist_cmd_func2, true, "[<i>searchstring</i>]",
        "Shows a list of console commands."),
    CVAR_BOOL(con_obituaries, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles obituaries in the console when monsters\nare killed or resurrected."),
    CVAR_BOOL(con_timestamps, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles timestamps next to player messages and\nobituaries in the console."),
    CMD(condump, "", null_func1, condump_cmd_func2, true, "[<i>filename</i><b>.txt</b>]",
        "Dumps the console to a file."),
    CMD(cvarlist, "", null_func1, cvarlist_cmd_func2, true, "[<i>searchstring</i>]",
        "Shows a list of console variables."),
    CMD(endgame, "", game_func1, endgame_cmd_func2, false, "",
        "Ends a game."),
    CVAR_INT(episode, "", int_cvars_func1, episode_cvar_func2, CF_NONE, NOVALUEALIAS,
        "The currently selected <i><b>DOOM</b></i> episode in the menu\n(<b>1</b> to <b>4</b>)."),
    CMD(exec, "", null_func1, exec_cmd_func2, true, EXECCMDFORMAT,
        "Executes a series of commands stored in a file."),
    CMD(exitmap, "", game_func1, exitmap_cmd_func2, false, "",
        "Exits the current map."),
    CVAR_INT(expansion, "", int_cvars_func1, expansion_cvar_func2, CF_NONE, NOVALUEALIAS,
        "The currently selected <i><b>DOOM II</b></i> expansion in the\nmenu (<b>1</b> or <b>2</b>)."),
    CVAR_INT(facebackcolor, facebackcolour, color_cvars_func1, color_cvars_func2, CF_NONE, FACEBACKVALUEALIAS,
        "The color behind the player's face in the status bar\n(<b>none</b>, <b>0</b> to <b>255</b>, or <b>#</b><i>rrggbb</i>)."),
    CMD(fastmonsters, "", fastmonsters_cmd_func1, fastmonsters_cmd_func2, true, "[<b>on</b>|<b>off</b>]",
        "Toggles fast monsters."),
    CMD(freeze, "", alive_func1, freeze_cmd_func2, true, "[<b>on</b>|<b>off</b>]",
        "Toggles freeze mode."),
    CVAR_TIME(gametime, "", null_func1, time_cvars_func2,
        "The amount of time <i><b>"PACKAGE_NAME"</b></i> has been running."),
    CMD(give, "", give_cmd_func1, give_cmd_func2, true, GIVECMDFORMAT,
        "Gives <b>ammo</b>, <b>armor</b>, <b>backpack</b>, <b>health</b>, <b>keys</b>,\n<b>weapons</b>, or <b>all</b> or certain <i>items</i> to the player."),
    CMD(god, "", god_cmd_func1, god_cmd_func2, true, "[<b>on</b>|<b>off</b>]",
        "Toggles god mode."),
    CVAR_FLOAT(gp_deadzone_left, "", gp_deadzone_cvars_func1, gp_deadzone_cvars_func2, CF_PERCENT,
        "The dead zone of the gamepad's left thumbstick."),
    CVAR_FLOAT(gp_deadzone_right, "", gp_deadzone_cvars_func1, gp_deadzone_cvars_func2, CF_PERCENT,
        "The dead zone of the gamepad's right thumbstick."),
    CVAR_BOOL(gp_invertyaxis, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles inverting the vertical axis of the\ngamepad's right thumbstick when looking up and\ndown."),
    CVAR_INT(gp_sensitivity, "", int_cvars_func1, gp_sensitivity_cvar_func2, CF_NONE, NOVALUEALIAS,
        "The gamepad's sensitivity (<b>0</b> to <b>128</b>)."),
    CVAR_BOOL(gp_swapthumbsticks, "", bool_cvars_func1, gp_swapthumbsticks_cvar_func2, BOOLVALUEALIAS,
        "Toggles swapping the gamepad's left and right\nthumbsticks."),
    CVAR_INT(gp_vibrate_barrels, "", int_cvars_func1, int_cvars_func2, CF_PERCENT, NOVALUEALIAS,
        "The amount <i><b>XInput</b></i> gamepads vibrate when the\nplayer is near an exploding barrel (<b>0%</b> to <b>200%</b>)."),
    CVAR_INT(gp_vibrate_damage, "", int_cvars_func1, int_cvars_func2, CF_PERCENT, NOVALUEALIAS,
        "The amount <i><b>XInput</b></i> gamepads vibrate when the\nplayer receives damage (<b>0%</b> to <b>200%</b>)."),
    CVAR_INT(gp_vibrate_weapons, "", int_cvars_func1, int_cvars_func2, CF_PERCENT, NOVALUEALIAS,
        "The amount <i><b>XInput</b></i> gamepads vibrate when the\nplayer fires their weapon (<b>0%</b> to <b>200%</b>)."),
    CVAR_INT(health, "", player_cvars_func1, player_cvars_func2, CF_PERCENT, NOVALUEALIAS,
        "The player's health."),
    CMD(help, "", null_func1, help_cmd_func2, false, "",
        "Opens the <i><b>"PACKAGE_NAME" Wiki</b></i>."),
    CMD_CHEAT(idbeholda, false),
    CMD_CHEAT(idbeholdl, false),
    CMD_CHEAT(idbeholdi, false),
    CMD_CHEAT(idbeholdr, false),
    CMD_CHEAT(idbeholds, false),
    CMD_CHEAT(idbeholdv, false),
    CMD_CHEAT(idchoppers, false),
    CMD_CHEAT(idclev, true),
    CMD_CHEAT(idclip, false),
    CMD_CHEAT(iddqd, false),
    CMD_CHEAT(iddt, false),
    CMD_CHEAT(idfa, false),
    CMD_CHEAT(idkfa, false),
    CMD_CHEAT(idmus, true),
    CMD_CHEAT(idmypos, false),
    CMD_CHEAT(idspispopd, false),
    CMD(if, "", null_func1, if_cmd_func2, true, IFCMDFORMAT,
        "If a <i>CVAR</i> equals a <i>value</i> then execute a string of\n<i>commands</i>."),
    CVAR_BOOL(infighting, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles infighting among monsters once the player\ndies."),
    CVAR_STR(iwadfolder, "", null_func1, str_cvars_func2, CF_NONE,
        "The folder where an IWAD was last opened."),
    CMD(kill, explode, kill_cmd_func1, kill_cmd_func2, true, KILLCMDFORMAT,
        "Kills the <b>player</b>, <b>all</b> monsters, a type of <i>monster</i>,\nall <b>barrels</b> or all <b>missiles</b>."),
    CMD(load, "", null_func1, load_cmd_func2, true, LOADCMDFORMAT,
        "Loads a game from a file."),
    CVAR_BOOL(m_doubleclick_use, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles double-clicking a mouse button for the\n<b>+use</b> action."),
    CVAR_BOOL(m_invertyaxis, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles inverting the mouse's vertical axis when\nusing mouselook."),
    CVAR_BOOL(m_novertical, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles no vertical movement of the mouse."),
    CVAR_INT(m_sensitivity, "", int_cvars_func1, int_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The mouse's sensitivity (<b>0</b> to <b>128</b>)."),
    CMD(map, warp, map_cmd_func1, map_cmd_func2, true, MAPCMDFORMAT,
        "Warps to a map."),
    CMD(maplist, "", null_func1, maplist_cmd_func2, false, "",
        "Shows a list of the available maps."),
    CMD(mapstats, "", game_func1, mapstats_cmd_func2, false, "",
        "Shows statistics about the current map."),
    CVAR_BOOL(messages, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles player messages."),
    CVAR_BOOL(mouselook, "", bool_cvars_func1, mouselook_cvar_func2, BOOLVALUEALIAS,
        "Toggles mouselook."),
    CVAR_INT(movebob, "", int_cvars_func1, int_cvars_func2, CF_PERCENT, NOVALUEALIAS,
        "The amount the player's view bobs up and down\nwhen they move."),
    CMD_CHEAT(mumu, 0),
    CMD(newgame, "", null_func1, newgame_cmd_func2, true, "",
        "Starts a new game."),
    CMD(noclip, "", game_func1, noclip_cmd_func2, true, "[<b>on</b>|<b>off</b>]",
        "Toggles no clipping mode."),
    CMD(nomonsters, "", null_func1, nomonsters_cmd_func2, true, "[<b>on</b>|<b>off</b>]",
        "Toggles the presence of monsters in maps."),
    CMD(notarget, "", game_func1, notarget_cmd_func2, true, "[<b>on</b>|<b>off</b>]",
        "Toggles monsters not seeing the player as a\ntarget."),
    CMD(pistolstart, "", null_func1, pistolstart_cmd_func2, true, "[<b>on</b>|<b>off</b>]",
        "Toggles the player starting each map with 100%\nhealth, no armor, and only a pistol and 50 bullets."),
    CMD(play, "", play_cmd_func1, play_cmd_func2, true, PLAYCMDFORMAT,
        "Plays a <i>sound</i> or <i>music</i> lump."),
    CVAR_STR(playername, "", null_func1, playername_cvar_func2, CF_NONE,
        "The name of the player used in player messages."),
    CMD(playerstats, "", null_func1, playerstats_cmd_func2, false, "",
        "Shows statistics about the player."),
    CMD(print, "", null_func1, print_cmd_func2, true, PRINTCMDFORMAT,
        "Prints a player <i>message</i>."),
    CMD(quit, exit, null_func1, quit_cmd_func2, false, "",
        "Quits <i><b>"PACKAGE_NAME"</b></i>."),
    CVAR_BOOL(r_althud, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles an alternate heads-up display when in\nwidescreen mode."),
    CVAR_INT(r_berserkintensity, "", int_cvars_func1, int_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The intensity of the effect when the player has the\nberserk power-up and their fists equipped (<b>0</b> to\n<b>8</b>)."),
    CVAR_INT(r_blood, "", r_blood_cvar_func1, r_blood_cvar_func2, CF_NONE, BLOODVALUEALIAS,
        "The colors of the blood of the player and monsters\n(<b>all</b>, <b>none</b> or <b>red</b>)."),
    CVAR_INT(r_bloodsplats_max, "", int_cvars_func1, int_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The maximum number of blood splats allowed in a\nmap (<b>0</b> to <b>1,048,576</b>)."),
    CVAR_INT(r_bloodsplats_total, "", int_cvars_func1, int_cvars_func2, CF_READONLY, NOVALUEALIAS,
        "The total number of blood splats in the current\nmap."),
    CVAR_BOOL(r_bloodsplats_translucency, "", bool_cvars_func1, r_bloodsplats_translucency_cvar_func2, BOOLVALUEALIAS,
        "Toggles the translucency of blood splats."),
    CVAR_BOOL(r_brightmaps, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles brightmaps on certain wall textures."),
    CVAR_BOOL(r_corpses_color, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles randomly colored marine corpses."),
    CVAR_BOOL(r_corpses_mirrored, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles randomly mirrored corpses."),
    CVAR_BOOL(r_corpses_moreblood, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles blood splats around corpses that are\nspawned when a map is loaded."),
    CVAR_BOOL(r_corpses_nudge, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles corpses being nudged when monsters walk\nover them."),
    CVAR_BOOL(r_corpses_slide, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles corpses sliding in reaction to barrel and\nrocket explosions."),
    CVAR_BOOL(r_corpses_smearblood, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles corpses leaving blood splats as they slide."),
    CVAR_BOOL(r_detail, "", r_detail_cvar_func1, r_detail_cvar_func2, DETAILVALUEALIAS,
        "Toggles the graphic detail (<b>low</b> or <b>high</b>)."),
    CVAR_BOOL(r_diskicon, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles showing a disk icon when loading and\nsaving."),
    CVAR_BOOL(r_dither, "", bool_cvars_func1, r_dither_cvar_func2, BOOLVALUEALIAS,
        "Toggles dithering of <i><b>BOOM</b></i>-compatible translucent\nwall textures."),
    CVAR_BOOL(r_fixmaperrors, "", bool_cvars_func1, r_fixmaperrors_cvar_func2, BOOLVALUEALIAS,
        "Toggles the fixing of mapping errors in the <i><b>DOOM</b></i>\nand <i><b>DOOM II</b></i> IWADs."),
    CVAR_BOOL(r_fixspriteoffsets, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles the fixing of sprite offsets."),
    CVAR_BOOL(r_floatbob, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles some power-ups bobbing up and down."),
    CVAR_INT(r_fov, "", int_cvars_func1, r_fov_cvar_func2, CF_NONE, NOVALUEALIAS,
        "The player's field of view (<b>45</b> to <b>135</b>)."),
    CVAR_FLOAT(r_gamma, "", r_gamma_cvar_func1, r_gamma_cvar_func2, CF_NONE,
        "The screen's gamma correction level (<b>off</b>, or <b>0.50</b>\nto <b>2.0</b>)."),
    CVAR_BOOL(r_homindicator, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles the flashing \"Hall Of Mirrors\" indicator."),
    CVAR_BOOL(r_hud, "", bool_cvars_func1, r_hud_cvar_func2, BOOLVALUEALIAS,
        "Toggles the heads-up display when in widescreen\nmode."),
    CVAR_BOOL(r_hud_translucency, "", bool_cvars_func1, r_hud_translucency_cvar_func2, BOOLVALUEALIAS,
        "Toggles the translucency of the heads-up display\nwhen in widescreen mode."),
    CVAR_BOOL(r_liquid_bob, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles the bobbing effect of liquid sectors."),
    CVAR_BOOL(r_liquid_clipsprites, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles clipping the bottom of sprites in liquid\nsectors."),
    CVAR_BOOL(r_liquid_current, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles a slight current being applied to liquid\nsectors."),
    CVAR_BOOL(r_liquid_lowerview, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles lowering the player's view when in a liquid\nsector."),
    CVAR_BOOL(r_liquid_swirl, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles the swirl effect of liquid sectors."),
    CVAR_SIZE(r_lowpixelsize, "", null_func1, r_lowpixelsize_cvar_func2,
        "The size of pixels when the graphic detail is low\n(<i>width</i><b>\xD7</b><i>height</i>)."),
    CVAR_POS(r_messagepos, "", null_func1, r_messagepos_cvar_func2,
        "The position of player messages (<b>(</b><i>x</i><b>,</b><i>y</i><b>)</b>)."),
    CVAR_BOOL(r_messagescale, "", r_messagescale_cvar_func1, r_messagescale_cvar_func2, SCALEVALUEALIAS,
        "The scale of player messages (<b>big</b> or <b>small</b>)."),
    CVAR_BOOL(r_mirroredweapons, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles randomly mirroring the weapons dropped\nby monsters."),
    CVAR_BOOL(r_playersprites, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles showing the player's weapon."),
    CVAR_BOOL(r_rockettrails, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles the trails behind rockets fired by the\nplayer and cyberdemons."),
    CVAR_INT(r_screensize, "", int_cvars_func1, r_screensize_cvar_func2, CF_NONE, NOVALUEALIAS,
        "The screen size (<b>0</b> to <b>7</b>)."),
    CVAR_BOOL(r_shadows, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles sprites casting shadows."),
    CVAR_BOOL(r_shadows_translucency, "", bool_cvars_func1, r_shadows_translucency_cvar_func2, BOOLVALUEALIAS,
        "Toggles the translucency of shadows cast by\nsprites."),
    CVAR_BOOL(r_shake_barrels, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles shaking the screen when the player is near\nan exploding barrel."),
    CVAR_INT(r_shake_damage, "", int_cvars_func1, int_cvars_func2, CF_PERCENT, NOVALUEALIAS,
        "The amount the screen shakes when the player is\nattacked."),
    CVAR_INT(r_skycolor, r_skycolour, r_skycolor_cvar_func1, r_skycolor_cvar_func2, CF_NONE, SKYVALUEALIAS,
        "The color of the sky (<b>none</b>, <b>0</b> to <b>255</b>, or <b>#</b><i>rrggbb</i>)."),
    CVAR_BOOL(r_textures, "", bool_cvars_func1, r_textures_cvar_func2, BOOLVALUEALIAS,
        "Toggles displaying all textures."),
    CVAR_BOOL(r_translucency, "", bool_cvars_func1, r_translucency_cvar_func2, BOOLVALUEALIAS,
        "Toggles the translucency of sprites and <i><b>BOOM</b></i>-\ncompatible wall textures."),
    CMD(regenhealth, "", null_func1, regenhealth_cmd_func2, true, "[<b>on</b>|<b>off</b>]",
        "Toggles regenerating health."),
    CMD(reset, "", null_func1, reset_cmd_func2, true, RESETCMDFORMAT,
        "Resets a <i>CVAR</i> to its default value."),
    CMD(resetall, "", null_func1, resetall_cmd_func2, false, "",
        "Resets all CVARs to their default values."),
    CMD(respawnitems, "", null_func1, respawnitems_cmd_func2, true, "[<b>on</b>|<b>off</b>]",
        "Toggles respawning items."),
    CMD(respawnmonsters, "", respawnmonsters_cmd_func1, respawnmonsters_cmd_func2, true, "[<b>on</b>|<b>off</b>]",
        "Toggles respawning monsters."),
    CMD(restartmap, "", game_func1, restartmap_cmd_func2, false, "",
        "Restarts the current map."),
    CMD(resurrect, "", resurrect_cmd_func1, resurrect_cmd_func2, false, "",
        "Resurrects the player."),
    CVAR_INT(s_channels, "", int_cvars_func1, int_cvars_func2, CF_NONE, NOVALUEALIAS,
        "The number of sound effects that can be played at\nthe same time (<b>8</b> to <b>64</b>)."),
    CVAR_INT(s_musicvolume, "", s_volume_cvars_func1, s_volume_cvars_func2, CF_PERCENT, NOVALUEALIAS,
        "The volume of music."),
    CVAR_BOOL(s_randommusic, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles randomizing the music at the start of each\nmap."),
    CVAR_BOOL(s_randompitch, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles randomizing the pitch of monster sound\neffects."),
    CVAR_INT(s_sfxvolume, "", s_volume_cvars_func1, s_volume_cvars_func2, CF_PERCENT, NOVALUEALIAS,
        "The volume of sound effects."),
    CMD(save, "", save_cmd_func1, save_cmd_func2, true, SAVECMDFORMAT,
        "Saves the game to a file."),
    CVAR_INT(savegame, "", int_cvars_func1, savegame_cvar_func2, CF_NONE, NOVALUEALIAS,
        "The currently selected savegame in the menu\n(<b>1</b> to <b>6</b>)."),
    CVAR_INT(skilllevel, "", int_cvars_func1, skilllevel_cvar_func2, CF_NONE, NOVALUEALIAS,
        "The currently selected skill level in the menu\n(<b>1</b> to <b>5</b>)."),
    CMD(spawn, summon, spawn_cmd_func1, spawn_cmd_func2, true, SPAWNCMDFORMAT,
        "Spawns a <i>monster</i> or <i>item</i>."),
    CVAR_INT(stillbob, "", int_cvars_func1, int_cvars_func2, CF_PERCENT, NOVALUEALIAS,
        "The amount the player's view and weapon bob up\nand down when they stand still."),
    CMD(teleport, "", game_func1, teleport_cmd_func2, true, TELEPORTCMDFORMAT,
        "Teleports the player to (<i>x</i>,<i>y</i>) in the current map."),
    CMD(thinglist, "", game_func1, thinglist_cmd_func2, false, "",
        "Shows a list of things in the current map."),
    CMD(timer, "", null_func1, timer_cmd_func2, true, TIMERCMDFORMAT,
        "Sets a time limit on each map."),
    CVAR_BOOL(tossdrop, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles tossing items dropped by monsters when\nthey die."),
    CVAR_INT(turbo, "", turbo_cvar_func1, turbo_cvar_func2, CF_PERCENT, NOVALUEALIAS,
        "The speed of the player (<b>10%</b> to <b>400%</b>)."),
    CMD(unbind, "", null_func1, unbind_cmd_func2, true, UNBINDCMDFORMAT,
        "Unbinds the action from a <i>control</i>."),
    CVAR_BOOL(units, "", units_cvar_func1, units_cvar_func2, UNITSVALUEALIAS,
        "The units used in the <b>mapstats</b> and <b>playerstats</b>\nCCMDs (<b>imperial</b> or <b>metric</b>)."),
    CMD(vanilla, "", null_func1, vanilla_cmd_func2, true, "[<b>on</b>|<b>off</b>]",
        "Toggles vanilla mode."),
    CVAR_STR(version, "", null_func1, str_cvars_func2, CF_READONLY,
        "<i><b>"PACKAGE_NAME"'s</b></i> version."),
    CVAR_INT(vid_capfps, "", vid_capfps_cvar_func1, vid_capfps_cvar_func2, CF_NONE, CAPVALUEALIAS,
        "The frames per second at which to cap the\nframerate (<b>off</b>, or <b>35</b> to <b>1,000</b>)."),
    CVAR_INT(vid_display, "", int_cvars_func1, vid_display_cvar_func2, CF_NONE, NOVALUEALIAS,
        "The display used to render the game."),
#if !defined(_WIN32)
    CVAR_STR(vid_driver, "", null_func1, str_cvars_func2, CF_NONE,
        "The video driver used to render the game."),
#endif
    CVAR_BOOL(vid_fullscreen, "", bool_cvars_func1, vid_fullscreen_cvar_func2, BOOLVALUEALIAS,
        "Toggles between fullscreen and a window."),
    CVAR_INT(vid_motionblur, "", int_cvars_func1, int_cvars_func2, CF_PERCENT, NOVALUEALIAS,
        "The amount of motion blur when the player turns\nquickly."),
    CVAR_BOOL(vid_pillarboxes, "", bool_cvars_func1, vid_fullscreen_cvar_func2, BOOLVALUEALIAS,
        "Toggles using the pillarboxes either side of the\nscreen for palette effects."),
#if defined(_WIN32)
    CVAR_STR(vid_scaleapi, "", vid_scaleapi_cvar_func1, vid_scaleapi_cvar_func2, CF_NONE,
        "The API used to scale the display (<b>\"direct3d\"</b>,\n<b>\"opengl\"</b> or <b>\"software\"</b>)."),
#else
    CVAR_STR(vid_scaleapi, "", vid_scaleapi_cvar_func1, vid_scaleapi_cvar_func2, CF_NONE,
        "The API used to scale the display (<b>\"direct3d\"</b>, <b>\"opengl\"</b>,\n<b>\"opengles\"</b>, <b>\"opengles2\"</b> or <b>\"software\"</b>)."),
#endif
    CVAR_STR(vid_scalefilter, "", vid_scalefilter_cvar_func1, vid_scalefilter_cvar_func2, CF_NONE,
        "The filter used to scale the display (<b>\"nearest\"</b>,\n<b>\"linear\"</b> or <b>\"nearest_linear\"</b>)."),
    CVAR_SIZE(vid_screenresolution, "", null_func1, vid_screenresolution_cvar_func2,
        "The screen's resolution when fullscreen (<b>desktop</b>\nor <i>width</i><b>\xD7</b><i>height</i>)."),
    CVAR_BOOL(vid_showfps, "", bool_cvars_func1, vid_showfps_cvar_func2, BOOLVALUEALIAS,
        "Toggles showing the average number of frames per\nsecond."),
    CVAR_BOOL(vid_vsync, "", bool_cvars_func1, vid_vsync_cvar_func2, BOOLVALUEALIAS,
        "Toggles vertical sync with the display's refresh\nrate."),
    CVAR_BOOL(vid_widescreen, "", bool_cvars_func1, vid_widescreen_cvar_func2, BOOLVALUEALIAS,
        "Toggles widescreen mode."),
    CVAR_POS(vid_windowpos, vid_windowposition, null_func1, vid_windowpos_cvar_func2,
        "The position of the window on the desktop\n(<b>centered</b> or <b>(</b><i>x</i><b>,</b><i>y</i><b>)</b>)."),
    CVAR_SIZE(vid_windowsize, "", null_func1, vid_windowsize_cvar_func2,
        "The size of the window on the desktop\n(<i>width</i><b>\xD7</b><i>height</i>)."),
#if defined(_WIN32)
    CVAR_STR(wad, "", null_func1, str_cvars_func2, CF_READONLY,
        "The last WAD to be opened by the WAD launcher."),
#endif
    CVAR_INT(weaponbob, "", int_cvars_func1, int_cvars_func2, CF_PERCENT, NOVALUEALIAS,
        "The amount the player's weapon bobs up and down\nwhen they move."),
    CVAR_BOOL(weaponrecoil, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles the player's weapon recoiling when fired\nwhile using mouselook."),
    CVAR_BOOL(wipe, "", bool_cvars_func1, bool_cvars_func2, BOOLVALUEALIAS,
        "Toggles wiping when transitioning between\nscreens."),

    { "", "", null_func1, NULL, 0, 0, CF_NONE, NULL, 0, 0, 0, "", "" }
};

static dboolean run(void)
{
    return (gamekeydown[keyboardrun] + !!mousebuttons[mouserun] + !!(gamepadbuttons & gamepadrun) + alwaysrun == 1);
}

static dboolean strafe(void)
{
    return (gamekeydown[keyboardstrafe] || mousebuttons[mousestrafe] || (gamepadbuttons & gamepadstrafe));
}

static void alwaysrun_action_func(void)
{
    G_ToggleAlwaysRun(ev_none);
}

static void automap_action_func(void)
{
    if (!automapactive)
    {
        if (!mapwindow)
            AM_Start(true);
    }
    else
        AM_Stop();
}

static void back_action_func(void)
{
    viewplayer->cmd.forwardmove -= forwardmove[run()];
    P_MovePlayer();
}

static void clearmark_action_func(void)
{
    if (automapactive || mapwindow)
        AM_clearMarks();
}

static void console_action_func(void)
{
    if (!consoleactive)
        C_ShowConsole();
}

static void fire_action_func(void)
{
    P_FireWeapon();
}

static void followmode_action_func(void)
{
    if (automapactive || mapwindow)
        AM_toggleFollowMode();
}

static void forward_action_func(void)
{
    viewplayer->cmd.forwardmove += forwardmove[run()];
    P_MovePlayer();
}

static void grid_action_func(void)
{
    if (automapactive || mapwindow)
        AM_toggleGrid();
}

static void left_action_func(void)
{
    if (strafe())
        viewplayer->cmd.sidemove -= sidemove[run()];
    else
        viewplayer->cmd.angleturn -= angleturn[run()];
}

static void mark_action_func(void)
{
    if (automapactive || mapwindow)
        AM_addMark();
}

static void maxzoom_action_func(void)
{
    if (automapactive || mapwindow)
        AM_toggleMaxZoom();
}

static void menu_action_func(void)
{
    M_StartControlPanel();
}

static void nextweapon_action_func(void)
{
    G_NextWeapon();
}

static void prevweapon_action_func(void)
{
    G_PrevWeapon();
}

static void right_action_func(void)
{
    if (strafe())
        viewplayer->cmd.sidemove += sidemove[run()];
    else
        viewplayer->cmd.angleturn += angleturn[run()];
}

static void rotatemode_action_func(void)
{
    if (automapactive || mapwindow)
        AM_toggleRotateMode();
}

static void screenshot_action_func(void)
{
    G_DoScreenShot();
}

static void strafeleft_action_func(void)
{
    viewplayer->cmd.sidemove -= sidemove[run()];
}

static void straferight_action_func(void)
{
    viewplayer->cmd.sidemove += sidemove[run()];
}

static void use_action_func(void)
{
    P_UseLines();
}

static void weapon1_action_func(void)
{
    P_ChangeWeapon(wp_fist);
}

static void weapon2_action_func(void)
{
    P_ChangeWeapon(wp_pistol);
}

static void weapon3_action_func(void)
{
    P_ChangeWeapon(wp_shotgun);
}

static void weapon4_action_func(void)
{
    P_ChangeWeapon(wp_chaingun);
}

static void weapon5_action_func(void)
{
    P_ChangeWeapon(wp_missile);
}

static void weapon6_action_func(void)
{
    P_ChangeWeapon(wp_plasma);
}

static void weapon7_action_func(void)
{
    P_ChangeWeapon(wp_bfg);
}

static int C_GetIndex(const char *cmd)
{
    int i = 0;

    while (*consolecmds[i].name)
    {
        if (M_StringCompare(cmd, consolecmds[i].name))
            break;

        i++;
    }

    return i;
}

static dboolean alive_func1(char *cmd, char *parms)
{
    return (gamestate == GS_LEVEL && viewplayer->health > 0);
}

static dboolean cheat_func1(char *cmd, char *parms)
{
    if (M_StringCompare(cmd, cheat_clev.sequence))
    {
        dboolean    result;

        if (gamemode == commercial)
        {
            mapcmdepisode = 1;
            mapcmdmap = (parms[0] - '0') * 10 + parms[1] - '0';
            M_snprintf(mapcmdlump, sizeof(mapcmdlump), "MAP%c%c", parms[0], parms[1]);
        }
        else
        {
            mapcmdepisode = parms[0] - '0';
            mapcmdmap = parms[1] - '0';
            M_snprintf(mapcmdlump, sizeof(mapcmdlump), "E%cM%c", parms[0], parms[1]);
        }

        result = (W_CheckNumForName(mapcmdlump) >= 0
            && (gamemission != pack_nerve || mapcmdmap <= 9)
            && (!BTSX || W_CheckMultipleLumps(mapcmdlump) > 1));

        if (gamestate == GS_LEVEL)
            return result;
        else if (result)
        {
            map_cmd_func2("map", mapcmdlump);
            return true;
        }
    }
    else if (gamestate != GS_LEVEL)
        return false;
    else if (M_StringCompare(cmd, cheat_god.sequence))
        return (gameskill != sk_nightmare);
    else if (M_StringCompare(cmd, cheat_ammonokey.sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(cmd, cheat_ammo.sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(cmd, cheat_mus.sequence))
        return (!nomusic && musicVolume);
    else if (M_StringCompare(cmd, cheat_noclip.sequence))
        return (gamemode != commercial && gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(cmd, cheat_commercial_noclip.sequence))
        return (gamemode == commercial && gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(cmd, cheat_powerup[0].sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(cmd, cheat_powerup[1].sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(cmd, cheat_powerup[2].sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(cmd, cheat_powerup[3].sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(cmd, cheat_powerup[4].sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(cmd, cheat_powerup[5].sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(cmd, cheat_powerup[6].sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(cmd, cheat_choppers.sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(cmd, cheat_buddha.sequence))
        return (gameskill != sk_nightmare && viewplayer->health > 0);
    else if (M_StringCompare(cmd, cheat_mypos.sequence))
        return true;
    else if (M_StringCompare(cmd, cheat_amap.sequence))
        return (automapactive || mapwindow);

    return false;
}

static dboolean game_func1(char *cmd, char *parms)
{
    return (gamestate == GS_LEVEL);
}

static dboolean null_func1(char *cmd, char *parms)
{
    return true;
}

//
// alias CCMD
//
dboolean C_ExecuteAlias(const char *alias)
{
    for (int i = 0; i < MAXALIASES; i++)
        if (M_StringCompare(alias, aliases[i].name))
        {
            char    *string = strdup(aliases[i].string);
            char    *strings[255];
            int     j = 0;

            strings[0] = strtok(string, ";");
            executingalias = true;

            while (strings[j])
            {
                if (!C_ValidateInput(trimwhitespace(strings[j])))
                    break;

                strings[++j] = strtok(NULL, ";");
            }

            executingalias = false;
            C_Input(alias);
            free(string);
            return true;
        }

    return false;
}

void alias_cmd_func2(char *cmd, char *parms)
{
    char    parm1[128] = "";
    char    parm2[128] = "";

    sscanf(parms, "%127s %127[^\n]", parm1, parm2);

    if (!*parm1)
    {
        C_Output("<b>%s</b> %s", cmd, ALIASCMDFORMAT);
        return;
    }

    C_StripQuotes(parm1);

    for (int i = 0; *consolecmds[i].name; i++)
        if (M_StringCompare(parm1, consolecmds[i].name))
            return;

    if (!*parm2)
    {
        for (int i = 0; i < MAXALIASES; i++)
            if (*aliases[i].name && M_StringCompare(parm1, aliases[i].name))
            {
                aliases[i].name[0] = '\0';
                aliases[i].string[0] = '\0';
                M_SaveCVARs();
                return;
            }

        return;
    }

    C_StripQuotes(parm2);

    for (int i = 0; i < MAXALIASES; i++)
        if (*aliases[i].name && M_StringCompare(parm1, aliases[i].name))
        {
            M_StringCopy(aliases[i].string, parm2, sizeof(aliases[i].string));
            M_SaveCVARs();
            return;
        }

    for (int i = 0; i < MAXALIASES; i++)
        if (!*aliases[i].name)
        {
            M_StringCopy(aliases[i].name, parm1, sizeof(aliases[i].name));
            M_StringCopy(aliases[i].string, parm2, sizeof(aliases[i].string));
            M_SaveCVARs();
            return;
        }
}

//
// bind CCMD
//
static void C_UnbindDuplicates(const int keep, const controltype_t type, const int value)
{
    for (int i = 0; *actions[i].action; i++)
        if (i != keep)
        {
            if (type == keyboardcontrol)
            {
                if (actions[i].keyboard1 && value == *(int *)actions[i].keyboard1)
                    *(int *)actions[i].keyboard1 = 0;

                if (actions[i].keyboard2 && value == *(int *)actions[i].keyboard2)
                    *(int *)actions[i].keyboard2 = 0;
            }
            else if (type == mousecontrol)
            {
                if (actions[i].mouse1 && value == *(int *)actions[i].mouse1)
                    *(int *)actions[i].mouse1 = -1;
            }
            else if (type == gamepadcontrol)
            {
                if (actions[i].gamepad1 && value == *(int *)actions[i].gamepad1)
                    *(int *)actions[i].gamepad1 = 0;

                if (actions[i].gamepad2 && value == *(int *)actions[i].gamepad2)
                    *(int *)actions[i].gamepad2 = 0;
            }
        }
}

void bind_cmd_func2(char *cmd, char *parms)
{
    int             i = 0;
    int             action = 0;
    char            parm1[128] = "";
    char            parm2[128] = "";
    const dboolean  mouselookcontrols = (keyboardmouselook || gamepadmouselook || mousemouselook != -1);

    sscanf(parms, "%127s %127[^\n]", parm1, parm2);

    if (!*parm1)
    {
        C_Output("<b>%s</b> %s", cmd, BINDCMDFORMAT);
        return;
    }

    C_StripQuotes(parm1);

    while (controls[i].type)
    {
        if (M_StringCompare(parm1, controls[i].control))
            break;

        i++;
    }

    if (*controls[i].control)
    {
        if (!*parm2)
        {
            while (*actions[action].action)
            {
                if (controls[i].type == keyboardcontrol)
                {
                    if (actions[action].keyboard1 && controls[i].value == *(int *)actions[action].keyboard1)
                        C_Output(actions[action].action);
                    else if (actions[action].keyboard2 && controls[i].value == *(int *)actions[action].keyboard2)
                        C_Output(actions[action].action);
                }
                else if (controls[i].type == mousecontrol)
                {
                    if (actions[action].mouse1 && controls[i].value == *(int *)actions[action].mouse1)
                        C_Output(actions[action].action);
                }
                else if (controls[i].type == gamepadcontrol)
                {
                    if (actions[action].gamepad1 && controls[i].value == *(int *)actions[action].gamepad1)
                        C_Output(actions[action].action);
                    else if (actions[action].gamepad2 && controls[i].value == *(int *)actions[action].gamepad2)
                        C_Output(actions[action].action);
                }

                action++;
            }
        }
        else if (M_StringCompare(cmd, "unbind"))
        {
            while (*actions[action].action)
            {
                switch (controls[i].type)
                {
                    case keyboardcontrol:
                        if (actions[action].keyboard1 && controls[i].value == *(int *)actions[action].keyboard1)
                        {
                            *(int *)actions[action].keyboard1 = 0;
                            M_SaveCVARs();
                        }

                        if (actions[action].keyboard2 && controls[i].value == *(int *)actions[action].keyboard2)
                        {
                            *(int *)actions[action].keyboard2 = 0;
                            M_SaveCVARs();
                        }

                        break;

                    case mousecontrol:
                        if (actions[action].mouse1 && controls[i].value == *(int *)actions[action].mouse1)
                        {
                            *(int *)actions[action].mouse1 = -1;
                            M_SaveCVARs();
                        }

                        break;

                    case gamepadcontrol:
                        if (actions[action].gamepad1 && controls[i].value == *(int *)actions[action].gamepad1)
                        {
                            *(int *)actions[action].gamepad1 = 0;
                            M_SaveCVARs();
                        }

                        if (actions[action].gamepad2 && controls[i].value == *(int *)actions[action].gamepad2)
                        {
                            *(int *)actions[action].gamepad2 = 0;
                            M_SaveCVARs();
                        }

                        break;

                    default:
                        break;
                }

                action++;
            }

            if (controls[i].type == keyboardcontrol)
                keyactionlist[controls[i].value][0] = '\0';
            else if (controls[i].type == mousecontrol)
                mouseactionlist[controls[i].value][0] = '\0';
        }
        else
        {
            dboolean    bound = false;

            while (*actions[action].action)
            {
                if (M_StringCompare(parm2, actions[action].action))
                    break;

                action++;
            }

            if (*actions[action].action)
            {
                switch (controls[i].type)
                {
                    case keyboardcontrol:
                        if (actions[action].keyboard1)
                        {
                            if (actions[action].keyboard2
                                && *(int *)actions[action].keyboard1
                                && *(int *)actions[action].keyboard1 != controls[i].value)
                            {
                                if (*(int *)actions[action].keyboard2)
                                {
                                    *(int *)actions[action].keyboard2 = *(int *)actions[action].keyboard1;
                                    *(int *)actions[action].keyboard1 = controls[i].value;
                                }
                                else
                                    *(int *)actions[action].keyboard2 = controls[i].value;
                            }
                            else
                                *(int *)actions[action].keyboard1 = controls[i].value;

                            bound = true;
                            C_UnbindDuplicates(action, keyboardcontrol, controls[i].value);
                        }

                        break;

                    case mousecontrol:
                        if (actions[action].mouse1)
                        {
                            *(int *)actions[action].mouse1 = controls[i].value;
                            bound = true;
                            C_UnbindDuplicates(action, mousecontrol, controls[i].value);
                        }

                        break;

                    case gamepadcontrol:
                        if (actions[action].gamepad1)
                        {
                            if (actions[action].gamepad2
                                && *(int *)actions[action].gamepad1
                                && *(int *)actions[action].gamepad1 != controls[i].value)
                            {
                                if (*(int *)actions[action].gamepad2)
                                {
                                    *(int *)actions[action].gamepad2 = *(int *)actions[action].gamepad1;
                                    *(int *)actions[action].gamepad1 = controls[i].value;
                                }
                                else
                                    *(int *)actions[action].gamepad2 = controls[i].value;
                            }
                            else
                                *(int *)actions[action].gamepad1 = controls[i].value;

                            bound = true;
                            C_UnbindDuplicates(action, gamepadcontrol, controls[i].value);
                        }

                        break;

                    default:
                        break;
                }

                M_SaveCVARs();

                if (!bound)
                {
                    if (strlen(controls[i].control) == 1)
                        C_Warning("The <b>%s</b> action can't be bound to '<b>%s</b>'.", parm2, controls[i].control);
                    else
                        C_Warning("The <b>%s</b> action can't be bound to <b>%s</b>.", parm2, controls[i].control);
                }
            }
            else
            {
                if (controls[i].type == keyboardcontrol)
                {
                    M_StringCopy(keyactionlist[controls[i].value], parm2, sizeof(keyactionlist[0]));
                    M_SaveCVARs();
                }
                else if (controls[i].type == mousecontrol)
                {
                    M_StringCopy(mouseactionlist[controls[i].value], parm2, sizeof(mouseactionlist[0]));
                    M_SaveCVARs();
                }
            }
        }
    }
    else if (M_StringCompare(cmd, "unbind"))
    {
        while (*actions[action].action)
        {
            if (M_StringCompare(parm2, actions[action].action))
            {
                if (actions[i].keyboard1)
                    *(int *)actions[i].keyboard1 = 0;

                if (actions[i].keyboard2)
                    *(int *)actions[i].keyboard2 = 0;

                if (actions[i].mouse1)
                    *(int *)actions[i].mouse1 = -1;

                if (actions[i].gamepad1)
                    *(int *)actions[i].gamepad1 = 0;

                if (actions[i].gamepad2)
                    *(int *)actions[i].gamepad2 = 0;

                break;
            }

            action++;
        }
    }
    else
        C_Warning("<b>%s</b> is not a valid control.", parm1);

    if (mouselookcontrols != (keyboardmouselook || gamepadmouselook || mousemouselook != -1))
    {
        if (gamestate == GS_LEVEL)
            R_InitSkyMap();

        R_InitColumnFunctions();
    }
}

//
// bindlist CCMD
//
static void C_DisplayBinds(const char *action, const int value, const controltype_t type, int *count)
{
    const int   tabs[8] = { 40, 130, 0, 0, 0, 0, 0, 0 };

    for (int i = 0; controls[i].type; i++)
    {
        const char  *control = controls[i].control;

        if (controls[i].type == type && controls[i].value == value)
        {
            if (strlen(control) == 1)
                C_TabbedOutput(tabs, "%i.\t'%s'\t%s", (*count)++, (control[0] == '=' ? "+" : control), action);
            else
                C_TabbedOutput(tabs, "%i.\t%s\t%s", (*count)++, control, action);

            break;
        }
    }
}

static void bindlist_cmd_func2(char *cmd, char *parms)
{
    const int   tabs[8] = { 40, 130, 0, 0, 0, 0, 0, 0 };
    int         count = 1;

    C_Header(tabs, BINDLISTTITLE);

    for (int i = 0; *actions[i].action; i++)
    {
        if (actions[i].keyboard1)
            C_DisplayBinds(actions[i].action, *(int *)actions[i].keyboard1, keyboardcontrol, &count);

        if (actions[i].keyboard2)
            C_DisplayBinds(actions[i].action, *(int *)actions[i].keyboard2, keyboardcontrol, &count);

        if (actions[i].mouse1)
            C_DisplayBinds(actions[i].action, *(int *)actions[i].mouse1, mousecontrol, &count);

        if (actions[i].gamepad1)
            C_DisplayBinds(actions[i].action, *(int *)actions[i].gamepad1, gamepadcontrol, &count);

        if (actions[i].gamepad2)
            C_DisplayBinds(actions[i].action, *(int *)actions[i].gamepad2, gamepadcontrol, &count);
    }

    for (int i = 0; controls[i].type; i++)
    {
        const char  *control = controls[i].control;
        const int   value = controls[i].value;

        if (controls[i].type == keyboardcontrol && keyactionlist[value][0])
        {
            if (strlen(control) == 1)
                C_TabbedOutput(tabs, "%i.\t'%s'\t%s", count++, (control[0] == '=' ? "+" : control),
                    keyactionlist[value]);
            else
                C_TabbedOutput(tabs, "%i.\t%s\t%s", count++, control, keyactionlist[value]);
        }
        else if (controls[i].type == mousecontrol && mouseactionlist[value][0])
            C_TabbedOutput(tabs, "%i.\t%s\t%s", count++, control, mouseactionlist[value]);
    }
}

//
// clear CCMD
//
static void clear_cmd_func2(char *cmd, char *parms)
{
    consolestrings = 0;
    consolestrings_max = 0;
    C_Output("");
}

//
// cmdlist CCMD
//
static void cmdlist_cmd_func2(char *cmd, char *parms)
{
    const int tabs[8] = { 40, 336, 0, 0, 0, 0, 0, 0 };
    int       count = 0;

    C_Header(tabs, CMDLISTTITLE);

    for (int i = 0; *consolecmds[i].name; i++)
        if (consolecmds[i].type == CT_CMD
            && *consolecmds[i].description
            && (!*parms || wildcard(consolecmds[i].name, parms)))
        {
            char    description1[255];
            char    description2[255] = "";
            char    *p;

            M_StringCopy(description1, consolecmds[i].description, sizeof(description1));

            if ((p = strchr(description1, '\n')))
            {
                *p = '\0';
                p++;
                M_StringCopy(description2, p, sizeof(description2));
            }

            C_TabbedOutput(tabs, "%i.\t<b>%s</b> %s\t%s", ++count, consolecmds[i].name,
                consolecmds[i].format, description1);

            if (*description2)
                C_TabbedOutput(tabs, "\t\t%s", description2);
        }
}

//
// condump CCMD
//
static void condump_cmd_func2(char *cmd, char *parms)
{
    if (consolestrings)
    {
        char        filename[MAX_PATH];
        FILE        *file;
        const char  *appdatafolder = M_GetAppDataFolder();

        M_MakeDirectory(appdatafolder);

        if (!*parms)
        {
            int count = 0;

            M_snprintf(filename, sizeof(filename), "%s"DIR_SEPARATOR_S"condump.txt", appdatafolder);

            while (M_FileExists(filename))
                M_snprintf(filename, sizeof(filename), "%s"DIR_SEPARATOR_S"condump (%i).txt", appdatafolder, ++count);
        }
        else
            M_snprintf(filename, sizeof(filename), "%s"DIR_SEPARATOR_S"%s", appdatafolder, parms);

        if ((file = fopen(filename, "wt")))
        {
            for (int i = 1; i < consolestrings - 1; i++)
            {
                if (console[i].type == dividerstring)
                    fprintf(file, "%s\n", DIVIDERSTRING);
                else
                {
                    char            *string = strdup(console[i].string);
                    int             len;
                    unsigned int    outpos = 0;
                    int             tabcount = 0;

                    strreplace(string, "<b>", "");
                    strreplace(string, "</b>", "");
                    strreplace(string, "<i>", "");
                    strreplace(string, "</i>", "");
                    len = (int)strlen(string);

                    if (console[i].type == warningstring)
                    {
                        fputs("! ", file);
                        len += 2;
                    }

                    for (int inpos = 0; inpos < len; inpos++)
                    {
                        const unsigned char letter = string[inpos];

                        if (letter != '\n')
                        {
                            if (letter == '\t')
                            {
                                const unsigned int  tabstop = console[i].tabs[tabcount] / 5;

                                if (outpos < tabstop)
                                {
                                    for (unsigned int spaces = 0; spaces < tabstop - outpos; spaces++)
                                        fputc(' ', file);

                                    outpos = tabstop;
                                    tabcount++;
                                }
                                else
                                {
                                    fputc(' ', file);
                                    outpos++;
                                }
                            }
                            else
                            {
                                const int   c = letter - CONSOLEFONTSTART;

                                if (((c >= 0 && c < CONSOLEFONTSIZE && letter != '~')
                                    || letter == 153 || letter == 169 || letter == 174 || letter == 215))
                                {
                                    fputc(letter, file);
                                    outpos++;
                                }
                            }
                        }
                    }

                    if ((console[i].type == playermessagestring || console[i].type == obituarystring)
                        && con_timestamps)
                    {
                        for (unsigned int spaces = 0; spaces < 91 - outpos; spaces++)
                            fputc(' ', file);

                        fputs(C_GetTimeStamp(console[i].tics), file);
                    }

                    fputc('\n', file);
                }
            }

            fclose(file);
            C_Output("Dumped the console to <b>%s</b>.", filename);
        }
    }
}

//
// cvarlist CCMD
//
static void cvarlist_cmd_func2(char *cmd, char *parms)
{
    const int   tabs[8] = { 40, 210, 318, 0, 0, 0, 0, 0 };
    int         count = 0;

    C_Header(tabs, CVARLISTTITLE);

    for (int i = 0; *consolecmds[i].name; i++)
        if (consolecmds[i].type == CT_CVAR && (!*parms || wildcard(consolecmds[i].name, parms)))
        {
            char    description1[255];
            char    description2[255] = "";
            char    description3[255] = "";
            char    *p;

            M_StringCopy(description1, consolecmds[i].description, sizeof(description1));

            if ((p = strchr(description1, '\n')))
            {
                *p = '\0';
                p++;
                M_StringCopy(description2, p, sizeof(description2));
            }

            if ((p = strchr(description2, '\n')))
            {
                *p = '\0';
                p++;
                M_StringCopy(description3, p, sizeof(description3));
            }

            if (M_StringCompare(consolecmds[i].name, stringize(ammo)))
                C_TabbedOutput(tabs, "%i.\t<b>%s\t%i</b>\t%s", ++count, consolecmds[i].name,
                    (gamestate == GS_LEVEL ? viewplayer->ammo[weaponinfo[viewplayer->readyweapon].ammo] : 0),
                    description1);
            else if (M_StringCompare(consolecmds[i].name, stringize(armor)))
                C_TabbedOutput(tabs, "%i.\t<b>%s\t%i%%</b>\t%s", ++count, consolecmds[i].name,
                    (gamestate == GS_LEVEL ? viewplayer->armorpoints : 0), description1);
            else if (M_StringCompare(consolecmds[i].name, stringize(health)))
                C_TabbedOutput(tabs, "%i.\t<b>%s\t%i%%</b>\t%s", ++count, consolecmds[i].name,
                    (gamestate == GS_LEVEL ? viewplayer->health : 0), description1);
            else if (consolecmds[i].flags & CF_BOOLEAN)
                C_TabbedOutput(tabs, "%i.\t<b>%s\t%s</b>\t%s", ++count, consolecmds[i].name,
                    C_LookupAliasFromValue(*(dboolean *)consolecmds[i].variable, consolecmds[i].aliases),
                    description1);
            else if ((consolecmds[i].flags & CF_INTEGER) && (consolecmds[i].flags & CF_PERCENT))
                C_TabbedOutput(tabs, "%i.\t<b>%s\t%i%%</b>\t%s", ++count, consolecmds[i].name,
                    *(int *)consolecmds[i].variable, description1);
            else if (consolecmds[i].flags & CF_INTEGER)
                C_TabbedOutput(tabs, "%i.\t<b>%s\t%s</b>\t%s", ++count, consolecmds[i].name,
                    C_LookupAliasFromValue(*(int *)consolecmds[i].variable, consolecmds[i].aliases),
                    description1);
            else if (consolecmds[i].flags & CF_FLOAT)
                C_TabbedOutput(tabs, "%i.\t<b>%s\t%s%s</b>\t%s", ++count, consolecmds[i].name,
                    striptrailingzero(*(float *)consolecmds[i].variable,
                    ((consolecmds[i].flags & CF_PERCENT) ? 1 : 2)),
                    ((consolecmds[i].flags & CF_PERCENT) ? "%" : ""), description1);
            else if (consolecmds[i].flags & CF_STRING)
                C_TabbedOutput(tabs, "%i.\t<b>%s\t\"%.14s%s\"</b>\t%s", ++count, consolecmds[i].name,
                    *(char **)consolecmds[i].variable,
                    (strlen(*(char **)consolecmds[i].variable) > 14 ? "..." : ""), description1);
            else if (consolecmds[i].flags & CF_POSITION)
                C_TabbedOutput(tabs, "%i.\t<b>%s\t%s</b>\t%s", ++count, consolecmds[i].name,
                    *(char **)consolecmds[i].variable, description1);
            else if (consolecmds[i].flags & CF_SIZE)
                C_TabbedOutput(tabs, "%i.\t<b>%s\t%s</b>\t%s", ++count, consolecmds[i].name,
                    formatsize(*(char **)consolecmds[i].variable), description1);
            else if (consolecmds[i].flags & CF_TIME)
            {
                const int   tics = *(int *)consolecmds[i].variable / TICRATE;

                C_TabbedOutput(tabs, "%i.\t<b>%s\t%02i:%02i:%02i</b>\t%s", ++count, consolecmds[i].name,
                    tics / 3600, (tics % 3600) / 60, (tics % 3600) % 60, description1);
            }

            if (*description2)
                C_TabbedOutput(tabs, "\t\t\t%s", description2);

            if (*description3)
                C_TabbedOutput(tabs, "\t\t\t%s", description3);
        }
}

//
// endgame CCMD
//
static void endgame_cmd_func2(char *cmd, char *parms)
{
    M_EndingGame();
    C_HideConsoleFast();
}

//
// exec CCMD
//
static void exec_cmd_func2(char *cmd, char *parms)
{
    if (!*parms)
        C_Output("<b>%s</b> %s", cmd, EXECCMDFORMAT);
    else
    {
        FILE    *file = fopen(parms, "r");

        if (!file)
            return;

        while (!feof(file))
        {
            char    strparm[256] = "";

            if (fscanf(file, "%255[^\n]\n", strparm) != 1)
                continue;

            if (strparm[0] == ';')
                continue;

            C_ValidateInput(strparm);
        }

        fclose(file);
    }
}

//
// exitmap CCMD
//
static void exitmap_cmd_func2(char *cmd, char *parms)
{
    G_ExitLevel();
    C_HideConsoleFast();
}

//
// fastmonsters CCMD
//
void G_SetFastMonsters(dboolean toggle);

static dboolean fastmonsters_cmd_func1(char *cmd, char *parms)
{
    return (gameskill != sk_nightmare);
}

static void fastmonsters_cmd_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, BOOLVALUEALIAS);

        if (value == 0)
        {
            if (!fastparm)
                return;

            fastparm = false;
        }
        else if (value == 1)
        {
            if (fastparm)
                return;

            fastparm = true;
        }
        else
            return;
    }
    else
        fastparm = !fastparm;

    G_SetFastMonsters(fastparm);
    HU_PlayerMessage((fastparm ? s_STSTR_FMON : s_STSTR_FMOFF), false);
}

//
// freeze CCMD
//
static void freeze_cmd_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, BOOLVALUEALIAS);

        if (value == 0)
            freeze = false;
        else if (value == 1)
            freeze = true;
        else
            return;
    }
    else
        freeze = !freeze;

    if (freeze)
    {
        HU_PlayerMessage(s_STSTR_FON, false);
        viewplayer->cheated++;
        stat_cheated = SafeAdd(stat_cheated, 1);
        M_SaveCVARs();
    }
    else
        HU_PlayerMessage(s_STSTR_FOFF, false);

    C_HideConsole();
}

//
// give CCMD
//
static dboolean give_cmd_func1(char *cmd, char *parms)
{
    char    *parm = removespaces(parms);
    int     num = -1;

    if (gamestate != GS_LEVEL)
        return false;

    if (!*parm)
        return true;

    if (M_StringCompare(parm, "all") || M_StringCompare(parm, "everything")
        || M_StringCompare(parm, "health") || M_StringCompare(parm, "weapons")
        || M_StringCompare(parm, "ammo") || M_StringCompare(parm, "armor")
        || M_StringCompare(parm, "armour") || M_StringCompare(parm, "keys"))
        return true;

    sscanf(parm, "%10i", &num);

    for (int i = 0; i < NUMMOBJTYPES; i++)
        if ((mobjinfo[i].flags & MF_SPECIAL) && (M_StringCompare(parm, removespaces(mobjinfo[i].name1))
            || (*mobjinfo[i].name2 && M_StringCompare(parm, removespaces(mobjinfo[i].name2)))
            || (*mobjinfo[i].name3 && M_StringCompare(parm, removespaces(mobjinfo[i].name3)))
            || (num == mobjinfo[i].doomednum && num != -1)))
            return true;

    return false;
}

static void give_cmd_func2(char *cmd, char *parms)
{
    char    *parm = removespaces(parms);

    if (!*parm)
        C_Output("<b>%s</b> %s", cmd, GIVECMDFORMAT);
    else
    {
        if (M_StringCompare(parm, "all") || M_StringCompare(parm, "everything"))
        {
            P_GiveBackpack(false, false);
            P_GiveMegaHealth(false);
            P_GiveAllWeapons();
            P_GiveFullAmmo(false);
            P_GiveArmor(blue_armor_class, false);
            P_GiveAllCards();
            C_HideConsole();
        }
        else if (M_StringCompare(parm, "health"))
        {
            P_GiveMegaHealth(false);
            C_HideConsole();
        }
        else if (M_StringCompare(parm, "weapons"))
        {
            P_GiveAllWeapons();
            C_HideConsole();
        }
        else if (M_StringCompare(parm, "ammo"))
        {
            P_GiveFullAmmo(false);
            C_HideConsole();
        }
        else if (M_StringCompare(parm, "armor") || M_StringCompare(parm, "armour"))
        {
            P_GiveArmor(blue_armor_class, false);
            C_HideConsole();
        }
        else if (M_StringCompare(parm, "keys"))
        {
            P_GiveAllCards();
            C_HideConsole();
        }
        else
        {
            int num = -1;

            sscanf(parm, "%10i", &num);

            for (int i = 0; i < NUMMOBJTYPES; i++)
            {
                if ((mobjinfo[i].flags & MF_SPECIAL)
                    && (M_StringCompare(parm, removespaces(mobjinfo[i].name1))
                    || (*mobjinfo[i].name2 && M_StringCompare(parm, removespaces(mobjinfo[i].name2)))
                    || (*mobjinfo[i].name3 && M_StringCompare(parm, removespaces(mobjinfo[i].name3)))
                    || (num == mobjinfo[i].doomednum && num != -1)))
                {
                    static char buffer[128];
                    dboolean    old_freeze = freeze;

                    if (gamemode != commercial && (i == MT_SUPERSHOTGUN || i == MT_MEGA))
                    {
                        M_StringCopy(buffer, mobjinfo[i].plural1, sizeof(buffer));

                        if (!*buffer)
                            M_snprintf(buffer, sizeof(buffer), "%ss", mobjinfo[i].name1);

                        buffer[0] = toupper(buffer[0]);
                        C_Warning("%s can't be given to the player in <i><b>DOOM</b></i>.", buffer);
                        return;
                    }

                    if (gamemode == shareware && (i == MT_MISC28 || i == MT_MISC25 || i == MT_MISC20 || i == MT_MISC21))
                    {
                        M_StringCopy(buffer, mobjinfo[i].plural1, sizeof(buffer));

                        if (!*buffer)
                            M_snprintf(buffer, sizeof(buffer), "%ss", mobjinfo[i].name1);

                        buffer[0] = toupper(buffer[0]);
                        C_Warning("%s can't be given to the player in <i><b>DOOM Shareware</b></i>.", buffer);
                        return;
                    }

                    freeze = false;
                    P_TouchSpecialThing(P_SpawnMobj(viewx, viewy, viewz, i), viewplayer->mo, false, false);
                    freeze = old_freeze;
                    C_HideConsole();
                    break;
                }
            }
        }

        viewplayer->cheated++;
        stat_cheated = SafeAdd(stat_cheated, 1);
        M_SaveCVARs();
    }
}

//
// god CCMD
//
static dboolean god_cmd_func1(char *cmd, char *parms)
{
    return (gamestate == GS_LEVEL && viewplayer->playerstate == PST_LIVE);
}

static void god_cmd_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, BOOLVALUEALIAS);

        if (value == 0)
            viewplayer->cheats &= ~CF_GODMODE;
        else if (value == 1)
            viewplayer->cheats |= CF_GODMODE;
        else
            return;
    }
    else
        viewplayer->cheats ^= CF_GODMODE;

    if (viewplayer->cheats & CF_GODMODE)
    {
        C_Output(s_STSTR_GODON);
        viewplayer->cheated++;
        stat_cheated = SafeAdd(stat_cheated, 1);
        M_SaveCVARs();
    }
    else
        C_Output(s_STSTR_GODOFF);
}

//
// help CCMD
//
static void help_cmd_func2(char *cmd, char *parms)
{
#if defined(_WIN32)
    ShellExecute(NULL, "open", PACKAGE_WIKI_HELP_URL, NULL, NULL, SW_SHOWNORMAL);
#elif defined(__linux__)
    system("xdg-open "PACKAGE_WIKI_HELP_URL);
#elif defined(__MACOSX__)
    system("open "PACKAGE_WIKI_HELP_URL);
#else
    C_HideConsoleFast();
    M_ShowHelp();
#endif
}

//
// if CCMD
//
static dboolean match(dboolean value, char *toggle)
{
    return ((value && M_StringCompare(toggle, "on")) || (!value && M_StringCompare(toggle, "off")));
}

static void if_cmd_func2(char *cmd, char *parms)
{
    char    parm1[64] = "";
    char    parm2[64] = "";
    char    parm3[128] = "";

    sscanf(parms, "%63s %63s then %127[^\n]", parm1, parm2, parm3);

    if (!*parm1 || !*parm2 || !*parm3)
    {
        C_Output("<b>%s</b> %s", cmd, IFCMDFORMAT);
        return;
    }

    C_StripQuotes(parm1);

    for (int i = 0; *consolecmds[i].name; i++)
        if (M_StringCompare(parm1, consolecmds[i].name))
        {
            dboolean    condition = false;

            C_StripQuotes(parm2);

            if (consolecmds[i].type == CT_CVAR)
            {
                if (consolecmds[i].flags & (CF_BOOLEAN | CF_INTEGER))
                {
                    int value = C_LookupValueFromAlias(parm2, consolecmds[i].aliases);

                    if (value == INT_MIN)
                        sscanf(parms, "%10i", &value);

                    condition = (value != INT_MIN && value == *(int *)consolecmds[i].variable);
                }
                else if (consolecmds[i].flags & CF_FLOAT)
                {
                    float value = FLT_MIN;

                    sscanf(parms, "%10f", &value);
                    condition = (value != FLT_MIN && value == *(float *)consolecmds[i].variable);
                }
                else
                    condition = M_StringCompare(parm2, *(char **)consolecmds[i].variable);
            }
            else if (M_StringCompare(parm1, stringize(fastmonsters)))
                condition = match(fastparm, parm2);
            else if (M_StringCompare(parm1, stringize(freeze)))
                condition = match(freeze, parm2);
            else if (M_StringCompare(parm1, stringize(god)))
                condition = match((gamestate == GS_LEVEL && (viewplayer->cheats & CF_GODMODE)), parm2);
            else if (M_StringCompare(parm1, stringize(noclip)))
                condition = match((gamestate == GS_LEVEL && (viewplayer->cheats & CF_NOCLIP)), parm2);
            else if (M_StringCompare(parm1, stringize(nomonsters)))
                condition = match(nomonsters, parm2);
            else if (M_StringCompare(parm1, stringize(notarget)))
                condition = match((gamestate == GS_LEVEL && (viewplayer->cheats & CF_NOTARGET)), parm2);
            else if (M_StringCompare(parm1, stringize(pistolstart)))
                condition = match(pistolstart, parm2);
            else if (M_StringCompare(parm1, stringize(regenhealth)))
                condition = match(regenhealth, parm2);
            else if (M_StringCompare(parm1, stringize(respawnitems)))
                condition = match(respawnitems, parm2);
            else if (M_StringCompare(parm1, stringize(respawnmonsters)))
                condition = match(respawnmonsters, parm2);
            else if (M_StringCompare(parm1, stringize(vanilla)))
                condition = match(vanilla, parm2);

            if (condition)
            {
                char    *strings[255];
                int     j = 0;

                C_StripQuotes(parm3);
                strings[0] = strtok(parm3, ";");

                while (strings[j])
                {
                    if (!C_ValidateInput(trimwhitespace(strings[j])))
                        break;

                    strings[++j] = strtok(NULL, ";");
                }
            }

            break;
        }
}

//
// kill CCMD
//
static int  killcmdtype = NUMMOBJTYPES;
dboolean    massacre;

static dboolean kill_cmd_func1(char *cmd, char *parms)
{
    if (gamestate == GS_LEVEL)
    {
        char    *parm = removespaces(parms);

        if (!*parm)
            return true;

        if (M_StringCompare(parm, "player") || M_StringCompare(parm, "me")
            || (*playername && M_StringCompare(parm, playername)))
            return (viewplayer->health > 0);

        if (M_StringCompare(parm, "monsters") || M_StringCompare(parm, "all"))
            return true;

        if (M_StringCompare(parm, "missile") || M_StringCompare(parm, "missiles"))
            return true;

        for (int i = 0; i < NUMMOBJTYPES; i++)
        {
            int num = -1;

            sscanf(parm, "%10i", &num);
            killcmdtype = mobjinfo[i].doomednum;

            if (killcmdtype >= 0 && (M_StringCompare(parm, removespaces(mobjinfo[i].name1))
                || M_StringCompare(parm, removespaces(mobjinfo[i].plural1))
                || (*mobjinfo[i].name2 && M_StringCompare(parm, removespaces(mobjinfo[i].name2)))
                || (*mobjinfo[i].plural2 && M_StringCompare(parm, removespaces(mobjinfo[i].plural2)))
                || (*mobjinfo[i].name3 && M_StringCompare(parm, removespaces(mobjinfo[i].name3)))
                || (*mobjinfo[i].plural3 && M_StringCompare(parm, removespaces(mobjinfo[i].plural3)))
                || (num == killcmdtype && num != -1)))
            {
                dboolean    kill = true;

                if (gamemode != commercial)
                {
                    if (killcmdtype >= ArchVile && killcmdtype <= MonstersSpawner)
                        kill = false;
                }
                else if (killcmdtype == WolfensteinSS && bfgedition && !states[S_SSWV_STND].dehacked)
                    killcmdtype = Zombieman;

                if (!(mobjinfo[i].flags & MF_SHOOTABLE))
                    kill = false;

                return kill;
            }
        }
    }
    return false;
}

void A_Fall(mobj_t *actor, player_t *player, pspdef_t *psp);

static void kill_cmd_func2(char *cmd, char *parms)
{
    char        *parm = removespaces(parms);
    static char buffer[1024];

    if (!*parm)
        C_Output("<b>%s</b> %s", cmd, KILLCMDFORMAT);
    else if (M_StringCompare(parm, "player") || M_StringCompare(parm, "me")
        || (*playername && M_StringCompare(parm, playername)))
    {
        viewplayer->health = 0;
        viewplayer->attacker = NULL;

        if (viewplayer->fixedcolormap == INVERSECOLORMAP)
            viewplayer->fixedcolormap = 0;

        viewplayer->mo->flags2 |= MF2_MASSACRE;
        P_KillMobj(viewplayer->mo, NULL, viewplayer->mo);
        M_snprintf(buffer, sizeof(buffer), "%s killed %s.", playername,
            (M_StringCompare(playername, "you") ? "yourself" : "themselves"));
        buffer[0] = toupper(buffer[0]);
        C_Output(buffer);
        C_HideConsole();
        HU_SetPlayerMessage(buffer, false);
        message_dontfuckwithme = true;
    }
    else
    {
        int kills = 0;

        if (M_StringCompare(parm, "all") || M_StringCompare(parm, "monsters"))
        {
            massacre = true;

            for (int i = 0; i < numsectors; i++)
            {
                mobj_t  *thing = sectors[i].thinglist;

                while (thing)
                {
                    if (thing->flags2 & MF2_MONSTERMISSILE)
                    {
                        thing->flags2 |= MF2_MASSACRE;
                        P_ExplodeMissile(thing);
                    }
                    else if (thing->health > 0)
                    {
                        const mobjtype_t    type = thing->type;

                        if (type == MT_PAIN)
                        {
                            A_Fall(thing, NULL, NULL);
                            P_SetMobjState(thing, S_PAIN_DIE6);
                            viewplayer->mobjcount[MT_PAIN]++;
                            stat_monsterskilled_painelementals = SafeAdd(stat_monsterskilled_painelementals, 1);
                            viewplayer->killcount++;
                            stat_monsterskilled = SafeAdd(stat_monsterskilled, 1);
                            kills++;
                        }
                        else if ((thing->flags & MF_SHOOTABLE) && type != MT_PLAYER && type != MT_BARREL
                            && type != MT_BOSSBRAIN && (type != MT_HEAD || !hacx))
                        {
                            thing->flags2 |= MF2_MASSACRE;
                            P_DamageMobj(thing, NULL, NULL, thing->health, false);

                            if (!(thing->flags & MF_NOBLOOD))
                            {
                                const int   r = M_RandomInt(-1, 1);

                                thing->momx += FRACUNIT * r;
                                thing->momy += FRACUNIT * M_RandomIntNoRepeat(-1, 1, (!r ? 0 : 2));
                            }

                            kills++;
                        }
                    }

                    thing = thing->snext;
                }
            }

            if (kills)
            {
                M_snprintf(buffer, sizeof(buffer), "%s monster%s killed.", commify(kills),
                    (kills == 1 ? "" : "s"));
                C_Output(buffer);
                C_HideConsole();
                HU_SetPlayerMessage(buffer, false);
                message_dontfuckwithme = true;
                viewplayer->cheated++;
                stat_cheated = SafeAdd(stat_cheated, 1);
                M_SaveCVARs();
            }
            else
                C_Warning("There are no monsters %s kill.", (!totalkills ? "to" : "left to"));
        }
        else if (M_StringCompare(parm, "missile") || M_StringCompare(parm, "missiles"))
        {
            for (int i = 0; i < numsectors; i++)
            {
                mobj_t  *thing = sectors[i].thinglist;

                while (thing)
                {
                    if (thing->flags2 & MF2_MONSTERMISSILE)
                    {
                        P_ExplodeMissile(thing);
                        kills++;
                    }

                    thing = thing->snext;
                }
            }

            if (kills)
            {
                M_snprintf(buffer, sizeof(buffer), "%s missile%s exploded.", commify(kills),
                    (kills == 1 ? "" : "s"));
                C_Output(buffer);
                C_HideConsole();
                HU_SetPlayerMessage(buffer, false);
                message_dontfuckwithme = true;
                viewplayer->cheated++;
                stat_cheated = SafeAdd(stat_cheated, 1);
                M_SaveCVARs();
            }
            else
                C_Warning("There are no missiles to explode.");
        }
        else
        {
            const mobjtype_t    type = P_FindDoomedNum(killcmdtype);
            int                 dead = 0;

            for (int i = 0; i < numsectors; i++)
            {
                mobj_t  *thing = sectors[i].thinglist;

                while (thing)
                {
                    if (type == thing->type)
                    {
                        if (type == MT_PAIN)
                        {
                            if (thing->health > 0)
                            {
                                A_Fall(thing, NULL, NULL);
                                P_SetMobjState(thing, S_PAIN_DIE6);
                                viewplayer->mobjcount[MT_PAIN]++;
                                stat_monsterskilled_painelementals = SafeAdd(stat_monsterskilled_painelementals, 1);
                                viewplayer->killcount++;
                                stat_monsterskilled = SafeAdd(stat_monsterskilled, 1);
                                kills++;
                            }
                            else
                                dead++;
                        }
                        else if ((thing->flags & MF_SHOOTABLE) && thing->health > 0)
                        {
                            thing->flags2 |= MF2_MASSACRE;
                            P_DamageMobj(thing, NULL, NULL, thing->health, false);

                            if (!(thing->flags & MF_NOBLOOD))
                            {
                                const int   r = M_RandomInt(-1, 1);

                                thing->momx += FRACUNIT * r;
                                thing->momy += FRACUNIT * M_RandomIntNoRepeat(-1, 1, (!r ? 0 : 2));
                            }

                            kills++;
                        }
                        else if (thing->flags & MF_CORPSE)
                            dead++;
                    }

                    thing = thing->snext;
                }
            }

            if (kills)
            {
                M_snprintf(buffer, sizeof(buffer), "%s %s %s.", commify(kills),
                    (kills == 1 ? mobjinfo[type].name1 : mobjinfo[type].plural1),
                    (type == MT_BARREL ? "exploded" : "killed"));
                C_Output(buffer);
                C_HideConsole();
                HU_SetPlayerMessage(buffer, false);
                message_dontfuckwithme = true;
                viewplayer->cheated++;
                stat_cheated = SafeAdd(stat_cheated, 1);
                M_SaveCVARs();
            }
            else
                C_Warning("There are no %s %s %s.", mobjinfo[type].plural1, (dead ? "left to" : "to"),
                    (type == MT_BARREL ? "explode" : "kill"));
        }
    }
}

//
// load CCMD
//
static void load_cmd_func2(char *cmd, char *parms)
{
    if (!*parms)
    {
        C_Output("<b>%s</b> %s", cmd, LOADCMDFORMAT);
        return;
    }

    G_LoadGame(M_StringJoin((M_StringStartsWith(parms, savegamefolder) ? "" : savegamefolder), parms,
        (M_StringEndsWith(parms, ".save") ? "" : ".save"), NULL));
}

//
// map CCMD
//
extern dboolean samelevel;
extern int      idclevtics;

static dboolean map_cmd_func1(char *cmd, char *parms)
{
    if (!*parms)
        return true;
    else
    {
        char        *map = uppercase(parms);
        dboolean    result = false;

        mapcmdepisode = 0;
        mapcmdmap = 0;

        if (M_StringCompare(map, "FIRST"))
        {
            if (gamemode == commercial)
            {
                if (gamemap != 1)
                {
                    mapcmdepisode = gameepisode;
                    mapcmdmap = 1;
                    M_StringCopy(mapcmdlump, "MAP01", sizeof(mapcmdlump));
                    result = true;
                }
            }
            else
            {
                if (!(gameepisode == 1 && gamemap == 1))
                {
                    mapcmdepisode = 1;
                    mapcmdmap = 1;
                    M_StringCopy(mapcmdlump, "E1M1", sizeof(mapcmdlump));
                    result = true;
                }
            }
        }
        else if ((M_StringCompare(map, "PREVIOUS") || M_StringCompare(map, "PREV"))
            && gamestate != GS_TITLESCREEN)
        {
            if (gamemode == commercial)
            {
                if (gamemap != 1)
                {
                    mapcmdepisode = gameepisode;
                    mapcmdmap = gamemap - 1;
                    M_snprintf(mapcmdlump, sizeof(mapcmdlump), "MAP%02i", mapcmdmap);
                    result = true;
                }
            }
            else
            {
                if (gamemap == 1)
                {
                    if (gameepisode != 1)
                    {
                        mapcmdepisode = gameepisode - 1;
                        mapcmdmap = 8;
                        result = true;
                    }
                }
                else
                {
                    mapcmdepisode = gameepisode;
                    mapcmdmap = gamemap - 1;
                    result = true;
                }

                M_snprintf(mapcmdlump, sizeof(mapcmdlump), "E%iM%i", mapcmdepisode, mapcmdmap);
            }
        }
        else if (M_StringCompare(map, "NEXT") && gamestate != GS_TITLESCREEN)
        {
            if (gamemode == commercial)
            {
                if (gamemap != (gamemission == pack_nerve ? 9 : (bfgedition ? 33 : 32)))
                {
                    mapcmdepisode = gameepisode;
                    mapcmdmap = gamemap + 1;
                    M_snprintf(mapcmdlump, sizeof(mapcmdlump), "MAP%02i", mapcmdmap);
                    result = true;
                }
            }
            else
            {
                if (gamemap == 9)
                {
                    if (gameepisode != (gamemode == retail ? 4 : gamemode == shareware ? 1 : 3))
                    {
                        mapcmdepisode = gameepisode + 1;
                        mapcmdmap = 1;
                        result = true;
                    }
                }
                else
                {
                    mapcmdepisode = gameepisode;
                    mapcmdmap = gamemap + 1;
                    result = true;
                }

                M_snprintf(mapcmdlump, sizeof(mapcmdlump), "E%iM%i", mapcmdepisode, mapcmdmap);
            }
        }
        else if (M_StringCompare(map, "LAST"))
        {
            if (gamemode == commercial)
            {
                if (gamemission == pack_nerve)
                {
                    if (gamemap != 9)
                    {
                        mapcmdepisode = gameepisode;
                        mapcmdmap = 9;
                        M_StringCopy(mapcmdlump, "MAP09", sizeof(mapcmdlump));
                        result = true;
                    }
                }
                else
                {
                    if (gamemap != (bfgedition ? 33 : 32))
                    {
                        mapcmdepisode = gameepisode;
                        mapcmdmap = (bfgedition ? 33 : 32);
                        M_snprintf(mapcmdlump, sizeof(mapcmdlump), "MAP%02i", (bfgedition ? 33 : 32));
                        result = true;
                    }
                }
            }
            else if (gamemode == retail)
            {
                if (!(gameepisode == 4 && gamemap == 9))
                {
                    mapcmdepisode = 4;
                    mapcmdmap = 9;
                    M_StringCopy(mapcmdlump, "E4M9", sizeof(mapcmdlump));
                    result = true;
                }
            }
            else if (gamemode == shareware)
            {
                if (!(gameepisode == 1 && gamemap == 9))
                {
                    mapcmdepisode = 1;
                    mapcmdmap = 9;
                    M_StringCopy(mapcmdlump, "E1M9", sizeof(mapcmdlump));
                    result = true;
                }
            }
            else
            {
                if (!(gameepisode == 4 && gamemap == 9))
                {
                    mapcmdepisode = 3;
                    mapcmdmap = 9;
                    M_StringCopy(mapcmdlump, "E3M9", sizeof(mapcmdlump));
                    result = true;
                }
            }
        }
        else if (M_StringCompare(map, "RANDOM"))
        {
            if (gamemode == commercial)
            {
                mapcmdepisode = gameepisode;
                mapcmdmap = M_RandomIntNoRepeat(1,
                    (gamemission == pack_nerve ? 9 : (bfgedition ? 33 : 32)), gamemap);
                M_snprintf(mapcmdlump, sizeof(mapcmdlump), "MAP%02i", mapcmdmap);
                result = true;
            }
            else
            {
                mapcmdepisode = M_RandomIntNoRepeat(1,
                    (gamemode == retail ? 4 : (gamemode == shareware ? 1 : 3)), gameepisode);
                mapcmdmap = M_RandomIntNoRepeat(1, 9, gamemap);
                M_snprintf(mapcmdlump, sizeof(mapcmdlump), "E%iM%i", mapcmdepisode, mapcmdmap);
                result = true;
            }
        }
        else
        {
            M_StringCopy(mapcmdlump, map, sizeof(mapcmdlump));

            if (gamemode == commercial)
            {
                mapcmdepisode = 1;

                if (sscanf(map, "MAP0%1i", &mapcmdmap) == 1 || sscanf(map, "MAP%2i", &mapcmdmap) == 1)
                {
                    if (!((BTSX && W_CheckMultipleLumps(map) == 1) || (gamemission == pack_nerve
                        && mapcmdmap > 9)))
                    {
                        if (gamestate != GS_LEVEL && gamemission == pack_nerve)
                        {
                            gamemission = doom2;
                            expansion = 1;
                        }

                        result = (W_CheckNumForName(map) >= 0);
                    }
                }
                else if (BTSX)
                {
                    if (sscanf(map, "MAP%02iC", &mapcmdmap) == 1)
                        result = (W_CheckNumForName(map) >= 0);
                    else
                    {
                        if (sscanf(map, "E%1iM0%1i", &mapcmdepisode, &mapcmdmap) != 2)
                            sscanf(map, "E%1iM%2i", &mapcmdepisode, &mapcmdmap);

                        if (mapcmdmap && ((mapcmdepisode == 1 && BTSXE1) || (mapcmdepisode == 2 && BTSXE2)
                            || (mapcmdepisode == 3 && BTSXE3)))
                        {
                            static char lump[6];

                            M_snprintf(lump, sizeof(lump), "MAP%02i", mapcmdmap);
                            result = (W_CheckMultipleLumps(lump) == 2);
                        }
                    }
                }
            }
            else if (sscanf(map, "E%1iM%1i", &mapcmdepisode, &mapcmdmap) == 2)
                result = (W_CheckNumForName(map) >= 0);
            else if (FREEDOOM && sscanf(map, "C%1iM%1i", &mapcmdepisode, &mapcmdmap) == 2)
            {
                static char lump[5];

                M_snprintf(lump, sizeof(lump), "E%iM%i", mapcmdepisode, mapcmdmap);
                result = (W_CheckNumForName(lump) >= 0);
            }
        }

        free(map);
        return result;
    }
}

static void map_cmd_func2(char *cmd, char *parms)
{
    static char buffer[1024];

    if (!*parms)
    {
        C_Output("<b>%s</b> %s", cmd, MAPCMDFORMAT);
        return;
    }

    samelevel = (gameepisode == mapcmdepisode && gamemap == mapcmdmap);
    gameepisode = mapcmdepisode;

    if (gamemission == doom && gameepisode <= 4)
    {
        episode = gameepisode;
        EpiDef.lastOn = episode - 1;
    }

    gamemap = mapcmdmap;
    M_snprintf(buffer, sizeof(buffer), (samelevel ? "Restarting %s..." : "Warping to %s..."), mapcmdlump);
    C_Output(buffer);
    HU_SetPlayerMessage(buffer, false);
    message_dontfuckwithme = true;

    if (gamestate == GS_LEVEL)
    {
        idclevtics = MAPCHANGETICS;
        drawdisk = true;
        C_HideConsole();
    }
    else
    {
        G_DeferredInitNew((gamestate == GS_LEVEL ? gameskill : skilllevel - 1), gameepisode, gamemap);
        C_HideConsoleFast();
    }

    viewplayer->cheated++;
    stat_cheated = SafeAdd(stat_cheated, 1);
    M_SaveCVARs();
}

//
// maplist CCMD
//
extern int  dehcount;
extern char **mapnames[];
extern char **mapnames2[];
extern char **mapnames2_bfg[];
extern char **mapnamesp[];
extern char **mapnamest[];
extern char **mapnamesn[];

static void maplist_cmd_func2(char *cmd, char *parms)
{
    const int   tabs[8] = { 40, 93, 370, 0, 0, 0, 0, 0 };
    int         count = 0;
    char        (*maplist)[256] = malloc(numlumps * sizeof(char *));

    C_Header(tabs, MAPLISTTITLE);

    // search through lumps for maps
    for (int i = 0; i < numlumps; i++)
    {
        int         ep = -1;
        int         map = -1;
        char        lump[8];
        char        wad[MAX_PATH];
        dboolean    replaced;
        dboolean    pwad;
        char        mapinfoname[128];

        M_StringCopy(lump, uppercase(lumpinfo[i]->name), sizeof(lump));

        if (gamemode == commercial)
        {
            ep = 1;
            sscanf(lump, "MAP0%1i", &map);

            if (map == -1)
                sscanf(lump, "MAP%2i", &map);
        }
        else
            sscanf(lump, "E%1iM%1i", &ep, &map);

        if (ep-- == -1 || map-- == -1)
            continue;

        M_StringCopy(wad, leafname(lumpinfo[i]->wadfile->path), sizeof(wad));
        replaced = (W_CheckMultipleLumps(lump) > 1 && !chex && !FREEDOOM);
        pwad = (lumpinfo[i]->wadfile->type == PWAD);
        M_StringCopy(mapinfoname, P_GetMapName(ep * 10 + map + 1), sizeof(mapinfoname));

        switch (gamemission)
        {
            case doom:
                if (!replaced || pwad)
                    M_snprintf(maplist[count++], 256, "%s\t%s\t%s", lump, (replaced && dehcount == 1
                        && !*mapinfoname ? "-" : titlecase(*mapinfoname ? mapinfoname :
                        *mapnames[ep * 9 + map])), wad);

                break;

            case doom2:
                if ((!M_StringCompare(wad, "NERVE.WAD") && ((!replaced || pwad || nerve) && (pwad || !BTSX)))
                    || hacx)
                {
                    if (BTSX)
                    {
                        if (!M_StringCompare(wad, "DOOM2.WAD"))
                            M_snprintf(maplist[count++], 256, "%s\t%s",
                                titlecase(M_StringReplace(*mapnames2[map], ": ", "\t")), wad);
                    }
                    else
                        M_snprintf(maplist[count++], 256, "%s\t%s\t%s", lump, (replaced && dehcount == 1
                            && !nerve && !*mapinfoname ? "-" : titlecase(*mapinfoname ? mapinfoname :
                            (bfgedition ? *mapnames2_bfg[map] : *mapnames2[map]))), wad);
                }

                break;

            case pack_nerve:
                if (M_StringCompare(wad, "NERVE.WAD"))
                    M_snprintf(maplist[count++], 256, "%s\t%s\t%s", lump,
                        titlecase(*mapinfoname ? mapinfoname : *mapnamesn[map]), wad);

                break;

            case pack_plut:
                if (!replaced || pwad)
                    M_snprintf(maplist[count++], 256, "%s\t%s\t%s", lump, (replaced && dehcount == 1
                        && !*mapinfoname ? "-" : titlecase(*mapinfoname ? mapinfoname : *mapnamesp[map])),
                        wad);

                break;

            case pack_tnt:
                if (!replaced || pwad)
                    M_snprintf(maplist[count++], 256, "%s\t%s\t%s", lump, (replaced && dehcount == 1
                        && !*mapinfoname ? "-" : titlecase(*mapinfoname ? mapinfoname : *mapnamest[map])),
                        wad);

                break;

            default:
                break;
        }
    }

    // sort the map list
    for (int i = 0; i < count; i++)
        for (int j = i + 1; j < count; j++)
            if (strcmp(maplist[i], maplist[j]) > 0)
            {
                char    temp[256];

                strcpy(temp, maplist[i]);
                strcpy(maplist[i], maplist[j]);
                strcpy(maplist[j], temp);
            }

    // display the map list
    for (int i = 0; i < count; i++)
        C_TabbedOutput(tabs, "%i.\t%s", i + 1, maplist[i]);

    free(maplist);
}

//
// mapstats CCMD
//
#define AA      "Andre Arsenault"
#define AD      "Andrew Dowswell"
#define AM      "American McGee"
#define BK      "Brian Kidby"
#define CB      "Christopher Buteau"
#define DB      "David Blanshine"
#define DC      "Dario Casali"
#define DC2     "David Calvin"
#define DJ      "Dean Johnson"
#define DO      "Drake O'Brien"
#define JA      "John Anderson"
#define JD      "Jim Dethlefsen"
#define JL      "Jim Lowell"
#define JM      "Jim Mentzer"
#define JM2     "John Minadeo"
#define JR      "John Romero"
#define JS      "Jimmy Sieben"
#define JW      "John Wakelin"
#define MB      "Michael Bukowski"
#define MC      "Milo Casali"
#define MS      "Mark Snell"
#define PT      "Paul Turnbull"
#define RH      "Richard Heath"
#define RM      "Russell Meakim"
#define RP      "Robin Patenall"
#define SG      "Shawn Green"
#define SP      "Sandy Petersen"
#define TH      "Tom Hall"
#define TH2     "Ty Halderman"
#define TM      "Tom Mustaine"
#define TW      "Tim Willits"
#define WW      "William D. Whitaker"
#define AMSP    AM" and "SP
#define BKTH2   BK" and "TH2
#define DC2DB   DC2" and "DB
#define DCMC    DC" and "MC
#define DCTH2   DC" and "TH2
#define JRTH    JR" and "TH
#define JSTH2   JS" and "TH2
#define MSJL    MS" and "JL
#define RPJM2   RP" and "JM2
#define SPTH    SP" and "TH

static void mapstats_cmd_func2(char *cmd, char *parms)
{
    const int   tabs[8] = { 120, 240, 0, 0, 0, 0, 0, 0 };

    C_Header(tabs, MAPSTATSTITLE);

    C_TabbedOutput(tabs, "Title\t<b>%s</b>", mapnumandtitle);

    {
        const char *authors[][6] =
        {
            /* xy      doom   doom2  tnt    plut   nerve */
            /* 00 */ { "",    "",    "",    DCMC,  "" },
            /* 01 */ { "",    SP,    TM,    DCMC,  RM },
            /* 02 */ { "",    AM,    JW,    DCMC,  RH },
            /* 03 */ { "",    AM,    RPJM2, DCMC,  RM },
            /* 04 */ { "",    AM,    TH2,   DCMC,  RM },
            /* 05 */ { "",    AM,    JD,    DCMC,  RH },
            /* 06 */ { "",    AM,    JSTH2, DCMC,  RH },
            /* 07 */ { "",    AMSP,  AD,    DCMC,  RH },
            /* 08 */ { "",    SP,    JM2,   DCMC,  RH },
            /* 09 */ { "",    SP,    JSTH2, DCMC,  RM },
            /* 10 */ { "",    SPTH,  TM,    DCMC,  "" },
            /* 11 */ { JR,    JR,    DJ,    DCMC,  "" },
            /* 12 */ { JR,    SP,    JL,    DCMC,  "" },
            /* 13 */ { JR,    SP,    BKTH2, DCMC,  "" },
            /* 14 */ { JRTH,  AM,    RP,    DCMC,  "" },
            /* 15 */ { JR,    JR,    WW,    DCMC,  "" },
            /* 16 */ { JR,    SP,    AA,    DCMC,  "" },
            /* 17 */ { JR,    JR,    TM,    DCMC,  "" },
            /* 18 */ { SPTH,  SP,    DCTH2, DCMC,  "" },
            /* 19 */ { JR,    SP,    TH2,   DCMC,  "" },
            /* 20 */ { DC2DB, JR,    DO,    DCMC,  "" },
            /* 21 */ { SPTH,  SP,    DO,    DCMC,  "" },
            /* 22 */ { SPTH,  AM,    CB,    DCMC,  "" },
            /* 23 */ { SPTH,  SP,    PT,    DCMC,  "" },
            /* 24 */ { SPTH,  SP,    DJ,    DCMC,  "" },
            /* 25 */ { SP,    SG,    JM,    DCMC,  "" },
            /* 26 */ { SP,    JR,    MSJL,  DCMC,  "" },
            /* 27 */ { SPTH,  SP,    DO,    DCMC,  "" },
            /* 28 */ { SP,    SP,    MC,    DCMC,  "" },
            /* 29 */ { SP,    JR,    JS,    DCMC,  "" },
            /* 30 */ { "",    SP,    JS,    DCMC,  "" },
            /* 31 */ { SP,    SP,    DC,    DCMC,  "" },
            /* 32 */ { SP,    SP,    DC,    DCMC,  "" },
            /* 33 */ { SPTH,  MB,    "",    "",    "" },
            /* 34 */ { SP,    "",    "",    "",    "" },
            /* 35 */ { SP,    "",    "",    "",    "" },
            /* 36 */ { SP,    "",    "",    "",    "" },
            /* 37 */ { SPTH,  "",    "",    "",    "" },
            /* 38 */ { SP,    "",    "",    "",    "" },
            /* 39 */ { SP,    "",    "",    "",    "" },
            /* 40 */ { "",    "",    "",    "",    "" },
            /* 41 */ { AM,    "",    "",    "",    "" },
            /* 42 */ { JR,    "",    "",    "",    "" },
            /* 43 */ { SG,    "",    "",    "",    "" },
            /* 44 */ { AM,    "",    "",    "",    "" },
            /* 45 */ { TW,    "",    "",    "",    "" },
            /* 46 */ { JR,    "",    "",    "",    "" },
            /* 47 */ { JA,    "",    "",    "",    "" },
            /* 48 */ { SG,    "",    "",    "",    "" },
            /* 49 */ { TW,    "",    "",    "",    "" }
        };

        const int   i = (gamemission == doom ? gameepisode * 10 : 0) + gamemap;
        const char  *author = P_GetMapAuthor(i);

        if (*author)
            C_TabbedOutput(tabs, "Author\t<b>%s</b>", author);
        else if (canmodify && *authors[i][gamemission])
            C_TabbedOutput(tabs, "Author\t<b>%s</b>", authors[i][gamemission]);
    }

    {
        static char lumpname[6];
        int         i;

        if (gamemode == commercial)
            M_snprintf(lumpname, sizeof(lumpname), "MAP%02i", startmap);
        else
            M_snprintf(lumpname, sizeof(lumpname), "E%iM%i", startepisode, startmap);

        i = (nerve && gamemission == doom2 ? W_GetNumForName2(lumpname) : W_CheckNumForName(lumpname));
        C_TabbedOutput(tabs, "%s\t<b>%s</b>", (lumpinfo[i]->wadfile->type == IWAD ? "IWAD" : "PWAD"),
            leafname(lumpinfo[i]->wadfile->path));
    }

    C_TabbedOutput(tabs, "Type\t<b>%s%s</b>", (boomlinespecials ? "<i>BOOM</i>-compatible" :
        (numsegs < 32768 ? "Vanilla-compatible" : "Limit removing")),
        (numsegs < 32768 || !boomlinespecials ? "" : " and limit removing"));

    C_TabbedOutput(tabs, "Things\t<b>%s</b>", commify(numthings));

    C_TabbedOutput(tabs, "   Monsters\t<b>%s</b>", commify(totalkills));

    C_TabbedOutput(tabs, "   Pickups\t<b>%s</b>", commify(totalpickups));

    C_TabbedOutput(tabs, "   Decorations\t<b>%s</b>", commify(numdecorations));

    C_TabbedOutput(tabs, "Lines\t<b>%s</b>", commify(numlines));

    C_TabbedOutput(tabs, "Sides\t<b>%s</b>", commify(numsides));

    C_TabbedOutput(tabs, "Vertices\t<b>%s</b>", commify(numvertexes));

    C_TabbedOutput(tabs, "Segments\t<b>%s</b>", commify(numsegs));

    C_TabbedOutput(tabs, "Subsectors\t<b>%s</b>", commify(numsubsectors));

    C_TabbedOutput(tabs, "Nodes\t<b>%s</b>", commify(numnodes));

    C_TabbedOutput(tabs, "Node format\t<b>%s nodes</b>", (mapformat == DOOMBSP ? "Regular" :
        (mapformat == DEEPBSP ? "<i>DeePBSP v4</i> extended" : "<i>ZDoom</i> uncompressed extended")));

    C_TabbedOutput(tabs, "Sectors\t<b>%s</b>", commify(numsectors));

    C_TabbedOutput(tabs, "   Secret\t<b>%s</b>", commify(totalsecret));

    C_TabbedOutput(tabs, "   Liquid\t<b>%s</b>", commify(numliquid));

    C_TabbedOutput(tabs, "   Damaging\t<b>%s</b>", commify(numdamaging));

    if (blockmaprecreated)
        C_TabbedOutput(tabs, "Blockmap\t<b>Recreated</b>");

    {
        int min_x = INT_MAX;
        int max_x = INT_MIN;
        int min_y = INT_MAX;
        int max_y = INT_MIN;
        int max_c = INT_MIN;
        int min_f = INT_MAX;
        int width;
        int height;
        int depth;

        for (int i = 0; i < numvertexes; i++)
        {
            const fixed_t   x = vertexes[i].x;
            const fixed_t   y = vertexes[i].y;

            if (x < min_x)
                min_x = x;
            else if (x > max_x)
                max_x = x;

            if (y < min_y)
                min_y = y;
            else if (y > max_y)
                max_y = y;
        }

        width = ((max_x >> FRACBITS) - (min_x >> FRACBITS)) / UNITSPERFOOT;
        height = ((max_y >> FRACBITS) - (min_y >> FRACBITS)) / UNITSPERFOOT;

        for (int i = 0; i < numsectors; i++)
        {
            if (max_c < sectors[i].ceilingheight)
                max_c = sectors[i].ceilingheight;

            if (min_f > sectors[i].floorheight)
                min_f = sectors[i].floorheight;
        }

        depth = ((max_c >> FRACBITS) - (min_f >> FRACBITS)) / UNITSPERFOOT;

        if (units == units_metric)
        {
            const float metricwidth = width / FEETPERMETER;
            const float metricheight = height / FEETPERMETER;
            const float metricdepth = depth / FEETPERMETER;

            if (metricwidth < METERSPERKILOMETER && metricheight < METERSPERKILOMETER
                && metricdepth < METERSPERKILOMETER)
                C_TabbedOutput(tabs, "Dimensions\t<b>%s\xD7%s\xD7%s meters</b>",
                    striptrailingzero(metricwidth, 1), striptrailingzero(metricheight, 1),
                    striptrailingzero(metricdepth, 1));
            else
                C_TabbedOutput(tabs, "Dimensions\t<b>%s\xD7%s\xD7%s kilometers</b>",
                    striptrailingzero(metricwidth / METERSPERKILOMETER, 1),
                    striptrailingzero(metricheight / METERSPERKILOMETER, 1),
                    striptrailingzero(metricdepth / METERSPERKILOMETER, 1));
        }
        else
        {
            if (width < FEETPERMILE && height < FEETPERMILE)
                C_TabbedOutput(tabs, "Dimensions\t<b>%s\xD7%s\xD7%s feet</b>",
                    commify(width), commify(height), commify(depth));
            else
                C_TabbedOutput(tabs, "Dimensions\t<b>%s\xD7%s\xD7%s miles</b>",
                    striptrailingzero((float)width / FEETPERMILE, 2),
                    striptrailingzero((float)height / FEETPERMILE, 2),
                    striptrailingzero((float)depth / FEETPERMILE, 2));
        }
    }

    if (mus_playing && !nomusic)
    {
        static char         lumpname[9];
        int                 lumps;
        const Mix_MusicType musictype = Mix_GetMusicType(NULL);

        M_snprintf(lumpname, sizeof(lumpname), "d_%s", mus_playing->name);
        lumps = W_CheckMultipleLumps(lumpname);

        if (((gamemode == commercial || gameepisode > 1) && lumps == 1)
            || (gamemode != commercial && gameepisode == 1 && lumps == 2))
            C_TabbedOutput(tabs, "Music title\t<b>%s</b>", mus_playing->title);

        if (musmusictype)
            C_TabbedOutput(tabs, "Music format\t<b>MIDI (converted from MUS)</b>");
        else if (midimusictype || musictype == MUS_MID)
            C_TabbedOutput(tabs, "Music format\t<b>MIDI</b>");
        else if (musictype == MUS_OGG)
            C_TabbedOutput(tabs, "Music format\t<b>Ogg Vorbis</b>");
        else if (musictype == MUS_MP3)
            C_TabbedOutput(tabs, "Music format\t<b>MP3</b>");
        else if (musictype == MUS_WAV)
            C_TabbedOutput(tabs, "Music format\t<b>WAV</b>");
        else if (musictype == MUS_FLAC)
            C_TabbedOutput(tabs, "Music format\t<b>FLAC</b>");
        else if (musictype == MUS_MOD)
            C_TabbedOutput(tabs, "Music format\t<b>MOD</b>");
    }
}

//
// newgame CCMD
//
static void newgame_cmd_func2(char *cmd, char *parms)
{
    C_HideConsoleFast();
    G_DeferredInitNew((skill_t)(skilllevel - 1), (gamemode == commercial ? expansion : episode), 1);
}

//
// noclip CCMD
//
static void noclip_cmd_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, BOOLVALUEALIAS);

        if (value == 0)
            viewplayer->cheats &= ~CF_NOCLIP;
        else if (value == 1)
            viewplayer->cheats |= CF_NOCLIP;
        else
            return;
    }
    else
        viewplayer->cheats ^= CF_NOCLIP;

    if (viewplayer->cheats & CF_NOCLIP)
    {
        HU_PlayerMessage(s_STSTR_NCON, false);
        viewplayer->cheated++;
        stat_cheated = SafeAdd(stat_cheated, 1);
        M_SaveCVARs();
    }
    else
        HU_PlayerMessage(s_STSTR_NCOFF, false);
}

//
// nomonsters CCMD
//
static void nomonsters_cmd_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, BOOLVALUEALIAS);

        if (value == 0)
            nomonsters = false;
        else if (value == 1)
            nomonsters = true;
        else
            return;
    }
    else
        nomonsters = !nomonsters;

    if (nomonsters)
    {
        if (gamestate == GS_LEVEL)
            for (int i = 0; i < numsectors; i++)
            {
                mobj_t  *thing = sectors[i].thinglist;

                while (thing)
                {
                    const mobjtype_t    type = thing->type;
                    const int           flags = thing->flags;

                    if (((flags & MF_SHOOTABLE) || (flags & MF_CORPSE) || (thing->flags2 & MF2_MONSTERMISSILE))
                        && type != MT_PLAYER && type != MT_BARREL && type != MT_BOSSBRAIN)
                        P_RemoveMobj(thing);

                    thing = thing->snext;
                }
            }

        HU_PlayerMessage(s_STSTR_NMON, false);
        viewplayer->cheated++;
        stat_cheated = SafeAdd(stat_cheated, 1);
        M_SaveCVARs();
    }
    else
    {
        HU_PlayerMessage(s_STSTR_NMOFF, false);

        if (gamestate == GS_LEVEL)
            C_Warning(PENDINGCHANGE);
    }
}

//
// notarget CCMD
//
static void notarget_cmd_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, BOOLVALUEALIAS);

        if (value == 0)
            viewplayer->cheats &= ~CF_NOTARGET;
        else if (value == 1)
            viewplayer->cheats |= CF_NOTARGET;
        else
            return;
    }
    else
        viewplayer->cheats ^= CF_NOTARGET;

    if (viewplayer->cheats & CF_NOTARGET)
    {
        for (int i = 0; i < numsectors; i++)
        {
            mobj_t   *mo = sectors[i].thinglist;

            while (mo)
            {
                if (mo->target && mo->target->player)
                    P_SetTarget(&mo->target, NULL);

                if (mo->tracer && mo->tracer->player)
                    P_SetTarget(&mo->tracer, NULL);

                if (mo->lastenemy && mo->lastenemy->player)
                    P_SetTarget(&mo->lastenemy, NULL);

                mo = mo->snext;
            }

            P_SetTarget(&sectors[i].soundtarget, NULL);
        }

        HU_PlayerMessage(s_STSTR_NTON, false);
        viewplayer->cheated++;
        stat_cheated = SafeAdd(stat_cheated, 1);
        M_SaveCVARs();
    }
    else
        HU_PlayerMessage(s_STSTR_NTOFF, false);
}

//
// pistolstart CCMD
//
static void pistolstart_cmd_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, BOOLVALUEALIAS);

        if (value == 0)
            pistolstart = false;
        else if (value == 1)
            pistolstart = true;
        else
            return;
    }
    else
        pistolstart = !pistolstart;

    HU_PlayerMessage((pistolstart ? s_STSTR_PSON : s_STSTR_PSOFF), false);

    if (gamestate == GS_LEVEL)
        C_Warning(PENDINGCHANGE);
}

//
// play CCMD
//
static int  playcmdid;
static int  playcmdtype;

static dboolean play_cmd_func1(char *cmd, char *parms)
{
    char    namebuf[9];

    if (!*parms)
        return true;

    for (int i = 1; i < NUMSFX; i++)
    {
        M_snprintf(namebuf, sizeof(namebuf), "ds%s", S_sfx[i].name);

        if (M_StringCompare(parms, namebuf) && W_CheckNumForName(namebuf) != -1)
        {
            playcmdid = i;
            playcmdtype = 1;
            return true;
        }
    }

    for (int i = 1; i < NUMMUSIC; i++)
    {
        M_snprintf(namebuf, sizeof(namebuf), "d_%s", S_music[i].name);

        if (M_StringCompare(parms, namebuf) && W_CheckNumForName(namebuf) != -1)
        {
            playcmdid = i;
            playcmdtype = 2;
            return true;
        }
    }

    return false;
}

static void play_cmd_func2(char *cmd, char *parms)
{
    if (!*parms)
        C_Output("<b>%s</b> %s", cmd, PLAYCMDFORMAT);
    else if (playcmdtype == 1)
        S_StartSound(NULL, playcmdid);
    else
        S_ChangeMusic(playcmdid, true, false, false);
}

static char *distance(fixed_t value, dboolean showunits)
{
    char    *result = malloc(20);

    value /= UNITSPERFOOT;

    if (units == units_metric)
    {
        const float meters = value / FEETPERMETER;

        if (!meters)
            M_StringCopy(result, (showunits ? "0 meters" : "0"), 20);
        else if (meters < METERSPERKILOMETER)
            M_snprintf(result, 20, "%s%s%s", striptrailingzero(meters, 1), (showunits ? " meter" : ""),
                (meters == 1.0f || !showunits ? "" : "s"));
        else
            M_snprintf(result, 20, "%s%s%s", striptrailingzero(meters / METERSPERKILOMETER, 2),
                (showunits ? " kilometer" : ""), (meters == METERSPERKILOMETER || !showunits ? "" : "s"));
    }
    else
    {
        if (value < FEETPERMILE)
            M_snprintf(result, 20, "%s%s", commify(value), (showunits ? (value == 1 ? " foot" : " feet") : ""));
        else
            M_snprintf(result, 20, "%s%s%s", striptrailingzero((float)value / FEETPERMILE, 2),
                (showunits ? " mile" : ""), (value == FEETPERMILE || !showunits ? "" : "s"));
    }

    return result;
}

//
// playerstats CCMD
//
static void C_PlayerStats_Game(void)
{
    const int   tabs[8] = { 160, 281, 0, 0, 0, 0, 0, 0 };
    const int   time1 = leveltime / TICRATE;
    const int   time2 = stat_time / TICRATE;

    C_Header(tabs, PLAYERSTATSTITLE);

    if ((viewplayer->cheats & CF_ALLMAP) || (viewplayer->cheats & CF_ALLMAP_THINGS))
        C_TabbedOutput(tabs, "Map explored\t<b>100%%</b>\t-");
    else
    {
        int totallines = numlines;
        int totallinesmapped = 0;

        for (int i = 0; i < numlines; i++)
            totallines -= !!(lines[i++].flags & ML_DONTDRAW);

        for (int i = 0; i < totallines; i++)
            totallinesmapped += !!(lines[i++].flags & ML_MAPPED);

        C_TabbedOutput(tabs, "Map explored\t<b>%s%%</b>\t-",
            striptrailingzero(totallinesmapped * 100.0f / totallines, 1));
    }

    C_TabbedOutput(tabs, "Maps completed\t-\t<b>%s</b>", commify(stat_mapscompleted));

    C_TabbedOutput(tabs, "Monsters killed\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
        commify(viewplayer->killcount), commify(totalkills),
        (totalkills ? viewplayer->killcount * 100 / totalkills : 0), commify(stat_monsterskilled));

    if (gamemode == commercial)
    {
        C_TabbedOutput(tabs, "   %s\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
            titlecase(mobjinfo[MT_BABY].plural1), commify(viewplayer->mobjcount[MT_BABY]),
            commify(monstercount[MT_BABY]),
            (monstercount[MT_BABY] ? viewplayer->mobjcount[MT_BABY] * 100 / monstercount[MT_BABY] : 0),
            commify(stat_monsterskilled_arachnotrons));

        C_TabbedOutput(tabs, "   %s\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
            titlecase(mobjinfo[MT_VILE].plural1), commify(viewplayer->mobjcount[MT_VILE]),
            commify(monstercount[MT_VILE]),
            (monstercount[MT_VILE] ? viewplayer->mobjcount[MT_VILE] * 100 / monstercount[MT_VILE] : 0),
            commify(stat_monsterskilled_archviles));
    }

    C_TabbedOutput(tabs, "   %s\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
        titlecase(mobjinfo[MT_BRUISER].plural1), commify(viewplayer->mobjcount[MT_BRUISER]),
        commify(monstercount[MT_BRUISER]),
        (monstercount[MT_BRUISER] ? viewplayer->mobjcount[MT_BRUISER] * 100 / monstercount[MT_BRUISER] : 0),
        commify(stat_monsterskilled_baronsofhell));

    C_TabbedOutput(tabs, "   %s\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
        titlecase(mobjinfo[MT_HEAD].plural1), commify(viewplayer->mobjcount[MT_HEAD]),
        commify(monstercount[MT_HEAD]),
        (monstercount[MT_HEAD] ? viewplayer->mobjcount[MT_HEAD] * 100 / monstercount[MT_HEAD] : 0),
        commify(stat_monsterskilled_cacodemons));

    C_TabbedOutput(tabs, "   %s\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
        titlecase(mobjinfo[MT_CYBORG].plural1), commify(viewplayer->mobjcount[MT_CYBORG]),
        commify(monstercount[MT_CYBORG]),
        (monstercount[MT_CYBORG] ? viewplayer->mobjcount[MT_CYBORG] * 100 / monstercount[MT_CYBORG] : 0),
        commify(stat_monsterskilled_cyberdemons));

    C_TabbedOutput(tabs, "   %s\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
        titlecase(mobjinfo[MT_SERGEANT].plural1), commify(viewplayer->mobjcount[MT_SERGEANT]),
        commify(monstercount[MT_SERGEANT]),
        (monstercount[MT_SERGEANT] ? viewplayer->mobjcount[MT_SERGEANT] * 100 / monstercount[MT_SERGEANT] : 0),
        commify(stat_monsterskilled_demons));

    if (gamemode == commercial)
    {
        C_TabbedOutput(tabs, "   %s\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
            titlecase(mobjinfo[MT_CHAINGUY].plural1), commify(viewplayer->mobjcount[MT_CHAINGUY]),
            commify(monstercount[MT_CHAINGUY]), (monstercount[MT_CHAINGUY] ?
            viewplayer->mobjcount[MT_CHAINGUY] * 100 / monstercount[MT_CHAINGUY] : 0),
            commify(stat_monsterskilled_heavyweapondudes));

        C_TabbedOutput(tabs, "   %s\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
            titlecase(mobjinfo[MT_KNIGHT].plural1), commify(viewplayer->mobjcount[MT_KNIGHT]),
            commify(monstercount[MT_KNIGHT]),
            (monstercount[MT_KNIGHT] ? viewplayer->mobjcount[MT_KNIGHT] * 100 / monstercount[MT_KNIGHT] : 0),
            commify(stat_monsterskilled_hellknights));
    }

    C_TabbedOutput(tabs, "   %s\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
        titlecase(mobjinfo[MT_TROOP].plural1), commify(viewplayer->mobjcount[MT_TROOP]),
        commify(monstercount[MT_TROOP]),
        (monstercount[MT_TROOP] ? viewplayer->mobjcount[MT_TROOP] * 100 / monstercount[MT_TROOP] : 0),
        commify(stat_monsterskilled_imps));

    C_TabbedOutput(tabs, "   %s\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
        titlecase(mobjinfo[MT_SKULL].plural1), commify(viewplayer->mobjcount[MT_SKULL]),
        commify(monstercount[MT_SKULL]),
        (monstercount[MT_SKULL] ? viewplayer->mobjcount[MT_SKULL] * 100 / monstercount[MT_SKULL] : 0),
        commify(stat_monsterskilled_lostsouls));

    if (gamemode == commercial)
    {
        C_TabbedOutput(tabs, "   %s\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
            titlecase(mobjinfo[MT_FATSO].plural1), commify(viewplayer->mobjcount[MT_FATSO]),
            commify(monstercount[MT_FATSO]),
            (monstercount[MT_FATSO] ? viewplayer->mobjcount[MT_FATSO] * 100 / monstercount[MT_FATSO] : 0),
            commify(stat_monsterskilled_mancubi));

        C_TabbedOutput(tabs, "   %s\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
            titlecase(mobjinfo[MT_PAIN].plural1), commify(viewplayer->mobjcount[MT_PAIN]),
            commify(monstercount[MT_PAIN]),
            (monstercount[MT_PAIN] ? viewplayer->mobjcount[MT_PAIN] * 100 / monstercount[MT_PAIN] : 0),
            commify(stat_monsterskilled_painelementals));
    }

    C_TabbedOutput(tabs, "   %s\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
        titlecase(mobjinfo[MT_UNDEAD].plural1), commify(viewplayer->mobjcount[MT_UNDEAD]),
        commify(monstercount[MT_UNDEAD]),
        (monstercount[MT_UNDEAD] ? viewplayer->mobjcount[MT_UNDEAD] * 100 / monstercount[MT_UNDEAD] : 0),
        commify(stat_monsterskilled_revenants));

    C_TabbedOutput(tabs, "   %s\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
        titlecase(mobjinfo[MT_SHOTGUY].plural1), commify(viewplayer->mobjcount[MT_SHOTGUY]),
        commify(monstercount[MT_SHOTGUY]),
        (monstercount[MT_SHOTGUY] ? viewplayer->mobjcount[MT_SHOTGUY] * 100 / monstercount[MT_SHOTGUY] : 0),
        commify(stat_monsterskilled_shotgunguys));

    C_TabbedOutput(tabs, "   %s\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
        titlecase(mobjinfo[MT_SHADOWS].plural1), commify(viewplayer->mobjcount[MT_SHADOWS]),
        commify(monstercount[MT_SHADOWS]),
        (monstercount[MT_SHADOWS] ? viewplayer->mobjcount[MT_SHADOWS] * 100 / monstercount[MT_SHADOWS] : 0),
        commify(stat_monsterskilled_spectres));

    C_TabbedOutput(tabs, "   %s\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
        titlecase(mobjinfo[MT_SPIDER].plural1), commify(viewplayer->mobjcount[MT_SPIDER]),
        commify(monstercount[MT_SPIDER]),
        (monstercount[MT_SPIDER] ? viewplayer->mobjcount[MT_SPIDER] * 100 / monstercount[MT_SPIDER] : 0),
        commify(stat_monsterskilled_spidermasterminds));

    C_TabbedOutput(tabs, "   %s\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
        titlecase(mobjinfo[MT_POSSESSED].plural1), commify(viewplayer->mobjcount[MT_POSSESSED]),
        commify(monstercount[MT_POSSESSED]),
        (monstercount[MT_POSSESSED] ? viewplayer->mobjcount[MT_POSSESSED] * 100 / monstercount[MT_POSSESSED] : 0),
        commify(stat_monsterskilled_zombiemen));

    C_TabbedOutput(tabs, "Barrels exploded\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
        commify(viewplayer->mobjcount[MT_BARREL]), commify(barrelcount),
        (barrelcount ? viewplayer->mobjcount[MT_BARREL] * 100 / barrelcount : 0), commify(stat_barrelsexploded));

    C_TabbedOutput(tabs, "Items picked up\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
        commify(viewplayer->itemcount), commify(totalitems),
        (totalitems ? viewplayer->itemcount * 100 / totalitems : 0), commify(stat_itemspickedup));

    C_TabbedOutput(tabs, "   Ammo\t<b>%s bullet%s</b>\t<b>%s bullet%s</b>",
        commify(viewplayer->itemspickedup_ammo_bullets), (viewplayer->itemspickedup_ammo_bullets == 1 ? "" : "s"),
        commify(stat_itemspickedup_ammo_bullets), (stat_itemspickedup_ammo_bullets == 1 ? "" : "s"));

    C_TabbedOutput(tabs, "\t<b>%s cell%s</b>\t<b>%s cell%s</b>",
        commify(viewplayer->itemspickedup_ammo_cells), (viewplayer->itemspickedup_ammo_cells == 1 ? "" : "s"),
        commify(stat_itemspickedup_ammo_cells), (stat_itemspickedup_ammo_cells == 1 ? "" : "s"));

    C_TabbedOutput(tabs, "\t<b>%s rocket%s</b>\t<b>%s rocket%s</b>",
        commify(viewplayer->itemspickedup_ammo_rockets), (viewplayer->itemspickedup_ammo_rockets == 1 ? "" : "s"),
        commify(stat_itemspickedup_ammo_rockets), (stat_itemspickedup_ammo_rockets == 1 ? "" : "s"));

    C_TabbedOutput(tabs, "\t<b>%s shell%s</b>\t<b>%s shell%s</b>",
        commify(viewplayer->itemspickedup_ammo_shells), (viewplayer->itemspickedup_ammo_shells == 1 ? "" : "s"),
        commify(stat_itemspickedup_ammo_shells), (stat_itemspickedup_ammo_shells == 1 ? "" : "s"));

    C_TabbedOutput(tabs, "   Armor\t<b>%s</b>\t<b>%s</b>",
        commify(viewplayer->itemspickedup_armor), commify(stat_itemspickedup_armor));

    C_TabbedOutput(tabs, "   Health\t<b>%s</b>\t<b>%s</b>",
        commify(viewplayer->itemspickedup_health), commify(stat_itemspickedup_health));

    C_TabbedOutput(tabs, "Secrets revealed\t<b>%s of %s (%i%%)</b>\t<b>%s</b>",
        commify(viewplayer->secretcount), commify(totalsecret),
        (totalsecret ? viewplayer->secretcount * 100 / totalsecret : 0), commify(stat_secretsrevealed));

    C_TabbedOutput(tabs, "Time played\t<b>%02i:%02i:%02i</b>\t<b>%02i:%02i:%02i</b>",
        time1 / 3600, (time1 % 3600) / 60, (time1 % 3600) % 60, time2 / 3600, (time2 % 3600) / 60,
        (time2 % 3600) % 60);

    C_TabbedOutput(tabs, "Damage inflicted\t<b>%s</b>\t<b>%s</b>",
        commify(viewplayer->damageinflicted), commify(stat_damageinflicted));

    C_TabbedOutput(tabs, "Damage received\t<b>%s</b>\t<b>%s</b>",
        commify(viewplayer->damagereceived), commify(stat_damagereceived));

    C_TabbedOutput(tabs, "Deaths\t<b>%s</b>\t<b>%s</b>",
        commify(viewplayer->deaths), commify(stat_deaths));

    C_TabbedOutput(tabs, "Cheated\t<b>%s</b>\t<b>%s</b>",
        commify(viewplayer->cheated), commify(stat_cheated));

    C_TabbedOutput(tabs, "Shots fired\t<b>%s</b>\t<b>%s</b>",
        commify(viewplayer->shotsfired), commify(stat_shotsfired));

    C_TabbedOutput(tabs, "Shots hit\t<b>%s</b>\t<b>%s</b>",
        commify(viewplayer->shotshit), commify(stat_shotshit));

    C_TabbedOutput(tabs, "Weapon accuracy\t<b>%s%%</b>\t<b>%s%%</b>",
        (viewplayer->shotsfired ? striptrailingzero(viewplayer->shotshit * 100.0f / viewplayer->shotsfired, 1) : "0"),
        (stat_shotsfired ? striptrailingzero(stat_shotshit * 100.0f / stat_shotsfired, 1) : "0"));

    C_TabbedOutput(tabs, "Distance traveled\t<b>%s</b>\t<b>%s</b>",
        distance(viewplayer->distancetraveled, true), distance(stat_distancetraveled, true));
}

static void C_PlayerStats_NoGame(void)
{
    const int   tabs[8] = { 160, 280, 0, 0, 0, 0, 0, 0 };
    const int   time2 = stat_time / TICRATE;

    C_Header(tabs, PLAYERSTATSTITLE);

    C_TabbedOutput(tabs, "Maps completed\t-\t<b>%s</b>", commify(stat_mapscompleted));

    C_TabbedOutput(tabs, "Monsters killed\t-\t<b>%s</b>", commify(stat_monsterskilled));

    if (gamemode == commercial)
    {
        C_TabbedOutput(tabs, "   %s\t-\t<b>%s</b>", titlecase(mobjinfo[MT_BABY].plural1),
            commify(stat_monsterskilled_arachnotrons));

        C_TabbedOutput(tabs, "   %s\t-\t<b>%s</b>", titlecase(mobjinfo[MT_VILE].plural1),
            commify(stat_monsterskilled_archviles));
    }

    C_TabbedOutput(tabs, "   %s\t-\t<b>%s</b>", titlecase(mobjinfo[MT_BRUISER].plural1),
        commify(stat_monsterskilled_baronsofhell));

    C_TabbedOutput(tabs, "   %s\t-\t<b>%s</b>", titlecase(mobjinfo[MT_HEAD].plural1),
        commify(stat_monsterskilled_cacodemons));

    C_TabbedOutput(tabs, "   %s\t-\t<b>%s</b>", titlecase(mobjinfo[MT_CYBORG].plural1),
        commify(stat_monsterskilled_cyberdemons));

    C_TabbedOutput(tabs, "   %s\t-\t<b>%s</b>", titlecase(mobjinfo[MT_SERGEANT].plural1),
        commify(stat_monsterskilled_demons));

    if (gamemode == commercial)
    {
        C_TabbedOutput(tabs, "   %s\t-\t<b>%s</b>", titlecase(mobjinfo[MT_CHAINGUY].plural1),
            commify(stat_monsterskilled_heavyweapondudes));

        C_TabbedOutput(tabs, "   %s\t-\t<b>%s</b>", titlecase(mobjinfo[MT_KNIGHT].plural1),
            commify(stat_monsterskilled_hellknights));
    }

    C_TabbedOutput(tabs, "   %s\t-\t<b>%s</b>", titlecase(mobjinfo[MT_TROOP].plural1),
        commify(stat_monsterskilled_imps));

    C_TabbedOutput(tabs, "   %s\t-\t<b>%s</b>", titlecase(mobjinfo[MT_SKULL].plural1),
        commify(stat_monsterskilled_lostsouls));

    if (gamemode == commercial)
    {
        C_TabbedOutput(tabs, "   %s\t-\t<b>%s</b>", titlecase(mobjinfo[MT_FATSO].plural1),
            commify(stat_monsterskilled_mancubi));

        C_TabbedOutput(tabs, "   %s\t-\t<b>%s</b>", titlecase(mobjinfo[MT_PAIN].plural1),
            commify(stat_monsterskilled_painelementals));
    }

    C_TabbedOutput(tabs, "   %s\t-\t<b>%s</b>", titlecase(mobjinfo[MT_UNDEAD].plural1),
        commify(stat_monsterskilled_revenants));

    C_TabbedOutput(tabs, "   %s\t-\t<b>%s</b>", titlecase(mobjinfo[MT_SHOTGUY].plural1),
        commify(stat_monsterskilled_shotgunguys));

    C_TabbedOutput(tabs, "   %s\t-\t<b>%s</b>", titlecase(mobjinfo[MT_SHADOWS].plural1),
        commify(stat_monsterskilled_spectres));

    C_TabbedOutput(tabs, "   %s\t-\t<b>%s</b>", titlecase(mobjinfo[MT_SPIDER].plural1),
        commify(stat_monsterskilled_spidermasterminds));

    C_TabbedOutput(tabs, "   %s\t-\t<b>%s</b>", titlecase(mobjinfo[MT_POSSESSED].plural1),
        commify(stat_monsterskilled_zombiemen));

    C_TabbedOutput(tabs, "Barrels exploded\t-\t<b>%s</b>", commify(stat_barrelsexploded));

    C_TabbedOutput(tabs, "Items picked up\t-\t<b>%s</b>", commify(stat_itemspickedup));

    C_TabbedOutput(tabs, "   Ammo\t-\t<b>%s bullet%s</b>",
        commify(stat_itemspickedup_ammo_bullets), (stat_itemspickedup_ammo_bullets == 1 ? "" : "s"));

    C_TabbedOutput(tabs, "\t-\t<b>%s cell%s</b>",
        commify(stat_itemspickedup_ammo_cells), (stat_itemspickedup_ammo_cells == 1 ? "" : "s"));

    C_TabbedOutput(tabs, "\t-\t<b>%s rocket%s</b>",
        commify(stat_itemspickedup_ammo_rockets), (stat_itemspickedup_ammo_rockets == 1 ? "" : "s"));

    C_TabbedOutput(tabs, "\t-\t<b>%s shell%s</b>",
        commify(stat_itemspickedup_ammo_shells), (stat_itemspickedup_ammo_shells == 1 ? "" : "s"));

    C_TabbedOutput(tabs, "   Armor\t-\t<b>%s</b>", commify(stat_itemspickedup_armor));

    C_TabbedOutput(tabs, "   Health\t-\t<b>%s</b>", commify(stat_itemspickedup_health));

    C_TabbedOutput(tabs, "Secrets revealed\t-\t<b>%s</b>", commify(stat_secretsrevealed));

    C_TabbedOutput(tabs, "Time played\t-\t<b>%02i:%02i:%02i</b>",
        time2 / 3600, (time2 % 3600) / 60, (time2 % 3600) % 60);

    C_TabbedOutput(tabs, "Damage inflicted\t-\t<b>%s</b>", commify(stat_damageinflicted));

    C_TabbedOutput(tabs, "Damage received\t-\t<b>%s</b>", commify(stat_damagereceived));

    C_TabbedOutput(tabs, "Deaths\t-\t<b>%s</b>", commify(stat_deaths));

    C_TabbedOutput(tabs, "Cheated\t-\t<b>%s</b>", commify(stat_cheated));

    C_TabbedOutput(tabs, "Shots fired\t-\t<b>%s</b>", commify(stat_shotsfired));

    C_TabbedOutput(tabs, "Shots hit\t-\t<b>%s</b>", commify(stat_shotshit));

    C_TabbedOutput(tabs, "Weapon accuracy\t-\t<b>%s%%</b>",
        (stat_shotsfired ? striptrailingzero(stat_shotshit * 100.0f / stat_shotsfired, 1) : "0"));

    C_TabbedOutput(tabs, "Distance traveled\t-\t<b>%s</b>", distance(stat_distancetraveled, true));
}

static void playerstats_cmd_func2(char *cmd, char *parms)
{
    if (gamestate == GS_LEVEL)
        C_PlayerStats_Game();
    else
        C_PlayerStats_NoGame();
}

//
// print CCMD
//
static void print_cmd_func2(char *cmd, char *parms)
{
    C_PlayerMessage(parms);

    if (gamestate == GS_LEVEL && !message_dontfuckwithme)
        HU_SetPlayerMessage(parms, false);
}

//
// quit CCMD
//
static void quit_cmd_func2(char *cmd, char *parms)
{
    I_Quit(true);
}

//
// reset CCMD
//
static void reset_cmd_func2(char *cmd, char *parms)
{
    if (!*parms)
    {
        C_Output("<b>%s</b> %s", cmd, RESETCMDFORMAT);
        return;
    }

    if (M_StringCompare(parms, "all"))
    {
        resetall_cmd_func2("resetall", "");
        return;
    }

    resettingcvar = true;

    for (int i = 0; *consolecmds[i].name; i++)
    {
        const int   flags = consolecmds[i].flags;

        if (consolecmds[i].type == CT_CVAR && M_StringCompare(parms, consolecmds[i].name) && !(flags & CF_READONLY))
        {
            if (flags & (CF_BOOLEAN | CF_INTEGER))
                C_ValidateInput(M_StringJoin(parms, " ",
                    uncommify(C_LookupAliasFromValue((int)consolecmds[i].defaultnumber, consolecmds[i].aliases)), NULL));
            else if (flags & CF_FLOAT)
                C_ValidateInput(M_StringJoin(parms, " ", striptrailingzero(consolecmds[i].defaultnumber, 1), NULL));
            else
                C_ValidateInput(M_StringJoin(parms, " ",
                    (*consolecmds[i].defaultstring ? consolecmds[i].defaultstring : EMPTYVALUE), NULL));

#if defined(_WIN32)
            if (M_StringCompare(parms, stringize(iwadfolder)))
            {
                wad = "";
                M_SaveCVARs();
            }
#endif

            break;
        }
    }

    resettingcvar = false;
}

//
// resetall CCMD
//
static void C_VerifyResetAll(const int key)
{
    messageToPrint = false;

    if (key == 'y')
    {
        resettingcvar = true;

        for (int i = 0; *consolecmds[i].name; i++)
        {
            const int   flags = consolecmds[i].flags;

            if (consolecmds[i].type == CT_CVAR && !(flags & CF_READONLY))
            {
                if (flags & (CF_BOOLEAN | CF_INTEGER))
                    consolecmds[i].func2(consolecmds[i].name,
                        uncommify(C_LookupAliasFromValue((int)consolecmds[i].defaultnumber,
                        consolecmds[i].aliases)));
                else if (flags & CF_FLOAT)
                    consolecmds[i].func2(consolecmds[i].name,
                        striptrailingzero(consolecmds[i].defaultnumber, 2));
                else
                    consolecmds[i].func2(consolecmds[i].name, (*consolecmds[i].defaultstring ?
                        consolecmds[i].defaultstring : EMPTYVALUE));
            }
        }

        resettingcvar = false;

#if defined(_WIN32)
        wad = "";
        M_SaveCVARs();
#endif

        C_Output("All CVARs have been reset to their default values.");
    }
}

static void resetall_cmd_func2(char *cmd, char *parms)
{
    static char buffer[128];

    M_snprintf(buffer, sizeof(buffer), "Are you sure you want to reset all\nCVARs to their default values?\n\n%s",
        s_PRESSYN);
    M_StartMessage(buffer, C_VerifyResetAll, true);
}

//
// regenhealth CCMD
//
static void regenhealth_cmd_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, BOOLVALUEALIAS);

        if (value == 0)
            regenhealth = false;
        else if (value == 1)
            regenhealth = true;
        else
            return;
    }
    else
        regenhealth = !regenhealth;

    if (regenhealth)
    {
        HU_PlayerMessage(s_STSTR_RHON, false);
        viewplayer->cheated++;
        stat_cheated = SafeAdd(stat_cheated, 1);
        M_SaveCVARs();
    }
    else
        HU_PlayerMessage(s_STSTR_RHOFF, false);
}

//
// respawnitems CCMD
//
static void respawnitems_cmd_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, BOOLVALUEALIAS);

        if (value == 0)
            respawnitems = false;
        else if (value == 1)
            respawnitems = true;
        else
            return;
    }
    else
        respawnitems = !respawnitems;

    if (respawnitems)
    {
        HU_PlayerMessage(s_STSTR_RION, false);
        viewplayer->cheated++;
        stat_cheated = SafeAdd(stat_cheated, 1);
        M_SaveCVARs();
    }
    else
        HU_PlayerMessage(s_STSTR_RIOFF, false);
}

//
// respawnmonsters CCMD
//
static dboolean respawnmonsters_cmd_func1(char *cmd, char *parms)
{
    return (gameskill != sk_nightmare);
}

static void respawnmonsters_cmd_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, BOOLVALUEALIAS);

        if (value == 0)
            respawnmonsters = false;
        else if (value == 1)
            respawnmonsters = true;
        else
            return;
    }
    else
        respawnmonsters = !respawnmonsters;

    HU_PlayerMessage((respawnmonsters ? s_STSTR_RMON : s_STSTR_RMOFF), false);
}

//
// restartmap CCMD
//
static void restartmap_cmd_func2(char *cmd, char *parms)
{
    viewplayer->playerstate = PST_REBORN;
    G_DoLoadLevel();
    C_HideConsoleFast();
}

//
// resurrect CCMD
//
static dboolean resurrect_cmd_func1(char *cmd, char *parms)
{
    return (gamestate == GS_LEVEL && viewplayer->playerstate == PST_DEAD);
}

static void resurrect_cmd_func2(char *cmd, char *parms)
{
    P_ResurrectPlayer(initial_health);
    viewplayer->cheated++;
    stat_cheated = SafeAdd(stat_cheated, 1);
    M_SaveCVARs();
}

//
// save CCMD
//
static dboolean save_cmd_func1(char *cmd, char *parms)
{
    return (gamestate == GS_LEVEL && viewplayer->playerstate == PST_LIVE);
}

static void save_cmd_func2(char *cmd, char *parms)
{
    if (!*parms)
    {
        C_Output("<b>%s</b> %s", cmd, SAVECMDFORMAT);
        return;
    }

    G_SaveGame(-1, "", M_StringJoin((M_StringStartsWith(parms, savegamefolder) ? "" : savegamefolder), parms,
        (M_StringEndsWith(parms, ".save") ? "" : ".save"), NULL));
}

//
// spawn CCMD
//
static int  spawncmdtype = NUMMOBJTYPES;

mobj_t *P_SpawnMapThing(mapthing_t *mthing, int index, dboolean nomonsters);

static dboolean spawn_cmd_func1(char *cmd, char *parms)
{
    char    *parm = removespaces(parms);

    if (!*parm)
        return true;

    if (gamestate == GS_LEVEL)
    {
        int num = -1;

        sscanf(parm, "%10i", &num);

        for (int i = 0; i < NUMMOBJTYPES; i++)
        {
            spawncmdtype = mobjinfo[i].doomednum;

            if (spawncmdtype >= 0 && (M_StringCompare(parm, removespaces(mobjinfo[i].name1))
                || (*mobjinfo[i].name2 && M_StringCompare(parm, removespaces(mobjinfo[i].name2)))
                || (num == spawncmdtype && num != -1)))
                return true;
        }
    }

    return false;
}

static void spawn_cmd_func2(char *cmd, char *parms)
{
    char    *parm = removespaces(parms);

    if (!*parm)
    {
        C_Output("<b>%s</b> %s", cmd, SPAWNCMDFORMAT);
        return;
    }
    else
    {
        dboolean    spawn = true;
        static char buffer[128];

        if (gamemode != commercial)
        {
            if (spawncmdtype >= ArchVile && spawncmdtype <= MonstersSpawner)
            {
                M_StringCopy(buffer, mobjinfo[P_FindDoomedNum(spawncmdtype)].plural1, sizeof(buffer));

                if (!*buffer)
                    M_snprintf(buffer, sizeof(buffer), "%ss", mobjinfo[P_FindDoomedNum(spawncmdtype)].name1);

                buffer[0] = toupper(buffer[0]);
                C_Warning("%s can't be spawned in <i><b>DOOM</b></i>.", buffer);
                spawn = false;
            }

            if (gamemode == shareware && (spawncmdtype == Cyberdemon || spawncmdtype == SpiderMastermind))
            {
                M_StringCopy(buffer, mobjinfo[P_FindDoomedNum(spawncmdtype)].plural1, sizeof(buffer));

                if (!*buffer)
                    M_snprintf(buffer, sizeof(buffer), "%ss", mobjinfo[P_FindDoomedNum(spawncmdtype)].name1);

                buffer[0] = toupper(buffer[0]);
                C_Warning("%s can't be spawned in <i><b>DOOM Shareware</b></i>.", buffer);
                spawn = false;
            }
        }
        else if (spawncmdtype == WolfensteinSS && bfgedition && !states[S_SSWV_STND].dehacked)
            spawncmdtype = Zombieman;

        if (spawn)
        {
            const mobjtype_t    type = P_FindDoomedNum(spawncmdtype);
            const int           flags = mobjinfo[type].flags;
            mapthing_t          mthing;
            mobj_t              *thing;

            mthing.x = (viewx + 100 * viewcos) >> FRACBITS;
            mthing.y = (viewy + 100 * viewsin) >> FRACBITS;
            mthing.angle = 0;
            mthing.type = spawncmdtype;
            mthing.options = (MTF_EASY | MTF_NORMAL | MTF_HARD);

            if ((thing = P_SpawnMapThing(&mthing, 0, false)))
            {
                thing->angle = R_PointToAngle2(thing->x, thing->y, viewx, viewy);

                if (flags & MF_COUNTITEM)
                {
                    stat_cheated = SafeAdd(stat_cheated, 1);
                    M_SaveCVARs();
                }

                C_HideConsole();
            }
        }
    }
}

//
// teleport CCMD
//
static void teleport_cmd_func2(char *cmd, char *parms)
{
    if (!*parms)
    {
        C_Output("<b>%s</b> %s", cmd, TELEPORTCMDFORMAT);
        return;
    }
    else
    {
        int x = INT_MAX;
        int y = INT_MAX;

        sscanf(parms, "%10i %10i", &x, &y);

        if (x != INT_MAX && y != INT_MAX)
        {
            mobj_t          *mo = viewplayer->mo;
            const fixed_t   oldx = viewx;
            const fixed_t   oldy = viewy;
            const fixed_t   oldz = viewz;

            x <<= FRACBITS;
            y <<= FRACBITS;

            if (P_TeleportMove(mo, x, y, ONFLOORZ, false))
            {
                // spawn teleport fog at source
                mobj_t  *fog = P_SpawnMobj(oldx, oldy, oldz, MT_TFOG);

                fog->angle = viewangle;
                S_StartSound(fog, sfx_telept);
                C_HideConsole();

                // spawn teleport fog at destination
                mo->z = mo->floorz;
                viewplayer->viewz = mo->z + viewplayer->viewheight;
                fog = P_SpawnMobj(x + 20 * viewcos, y + 20 * viewsin, ONFLOORZ, MT_TFOG);
                fog->angle = viewangle;
                S_StartSound(fog, sfx_telept);

                mo->reactiontime = 18;
                viewplayer->psprites[ps_weapon].sx = 0;
                viewplayer->psprites[ps_weapon].sy = WEAPONTOP;
                viewplayer->momx = 0;
                viewplayer->momy = 0;
                mo->momx = 0;
                mo->momy = 0;
                mo->momz = 0;
                viewplayer->lookdir = 0;
                viewplayer->oldlookdir = 0;
                viewplayer->recoil = 0;
                viewplayer->oldrecoil = 0;

                viewplayer->cheated++;
                stat_cheated = SafeAdd(stat_cheated, 1);
                M_SaveCVARs();
            }
        }
    }
}

//
// thinglist CCMD
//
static void thinglist_cmd_func2(char *cmd, char *parms)
{
    const int   tabs[8] = { 40, 268, 0, 0, 0, 0, 0, 0 };
    int         count = 0;

    C_Header(tabs, THINGLISTTITLE);

    for (thinker_t *th = thinkerclasscap[th_mobj].cnext; th != &thinkerclasscap[th_mobj]; th = th->cnext)
    {
        mobj_t  *mobj = (mobj_t *)th;

        C_TabbedOutput(tabs, "%i.\t%s\t(%i,%i,%i)", ++count, titlecase(mobj->info->name1),
            mobj->x >> FRACBITS, mobj->y >> FRACBITS, mobj->z >> FRACBITS);
    }
}

//
// timer CCMD
//
static void timer_cmd_func2(char *cmd, char *parms)
{
    if (!*parms)
    {
        C_Output("<b>%s</b> %s", cmd, TIMERCMDFORMAT);
        return;
    }
    else
    {
        int value = INT_MAX;

        sscanf(parms, "%10i", &value);

        if (value != INT_MAX)
            P_SetTimer(value);
    }
}

//
// unbind CCMD
//
static void unbind_cmd_func2(char *cmd, char *parms)
{
    if (!*parms)
    {
        C_Output("<b>%s</b> %s", cmd, UNBINDCMDFORMAT);
        return;
    }

    bind_cmd_func2(cmd, parms);
}

//
// vanilla CCMD
//
static void vanilla_cmd_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, BOOLVALUEALIAS);

        if (value == 0)
            vanilla = false;
        else if (value == 1)
            vanilla = true;
        else
            return;
    }
    else
        vanilla = !vanilla;

    togglingvanilla = true;

    if (vanilla)
    {
        SC_Open("VANILLA");

        while (SC_GetString())
        {
            char *cvar = strdup(sc_String);

            if (SC_GetString())
                C_ValidateInput(M_StringJoin(cvar, " ", sc_String, NULL));

            free(cvar);
        }

        SC_Close();
        HU_PlayerMessage(s_STSTR_VMON, false);

        if (gamestate == GS_LEVEL)
            C_HideConsole();
    }
    else
    {
        M_LoadCVARs(packageconfig);
        HU_PlayerMessage(s_STSTR_VMOFF, false);

        if (gamestate == GS_LEVEL)
            C_HideConsole();
    }

    togglingvanilla = false;
}

//
// boolean CVARs
//
static dboolean bool_cvars_func1(char *cmd, char *parms)
{
    return (!*parms || C_LookupValueFromAlias(parms, BOOLVALUEALIAS) != INT_MIN);
}

static void bool_cvars_func2(char *cmd, char *parms)
{
    for (int i = 0; *consolecmds[i].name; i++)
        if (M_StringCompare(cmd, consolecmds[i].name) && consolecmds[i].type == CT_CVAR
            && (consolecmds[i].flags & CF_BOOLEAN) && !(consolecmds[i].flags & CF_READONLY))
        {
            if (*parms)
            {
                const int   value = C_LookupValueFromAlias(parms, BOOLVALUEALIAS);

                if ((value == 0 || value == 1) && value != *(dboolean *)consolecmds[i].variable)
                {
                    *(dboolean *)consolecmds[i].variable = !!value;
                    M_SaveCVARs();
                }
            }
            else
            {
                C_Output(removenewlines(consolecmds[i].description));

                if (*(dboolean *)consolecmds[i].variable == (dboolean)consolecmds[i].defaultnumber)
                    C_Output("It is currently set to its default of <b>%s</b>.",
                        C_LookupAliasFromValue(*(dboolean *)consolecmds[i].variable, BOOLVALUEALIAS));
                else
                    C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                        C_LookupAliasFromValue(*(dboolean *)consolecmds[i].variable, BOOLVALUEALIAS),
                        C_LookupAliasFromValue((dboolean)consolecmds[i].defaultnumber, BOOLVALUEALIAS));
            }
            break;
        }
}

//
// color CVARs
//

static struct
{
    char    *name;
    int     value;
} color[] = {
    { "black",       0 }, { "blue",      207 }, { "brick",      32 }, { "brown",      64 },
    { "cream",      48 }, { "darkbrown",  79 }, { "darkgray",  111 }, { "darkgrey",  111 },
    { "darkgreen", 127 }, { "darkred",   191 }, { "gold",      163 }, { "gray",       95 },
    { "grey",       95 }, { "green",     112 }, { "lightblue", 193 }, { "olive",     152 },
    { "orange",    216 }, { "purple",    254 }, { "red",       176 }, { "tan",       144 },
    { "white",       4 }, { "yellow",    231 }, { "",            0 }
};

static dboolean color_cvars_func1(char *cmd, char *parms)
{
    for (int i = 0; *color[i].name; i++)
        if (M_StringCompare(parms, color[i].name))
            return true;

    return ((strlen(parms) == 7 && parms[0] == '#' && hextodec(M_SubString(parms, 1, 6)) >= 0)
        || int_cvars_func1(cmd, parms));
}

static void color_cvars_func2(char *cmd, char *parms)
{
    static char buffer[8];

    for (int i = 0; *color[i].name; i++)
        if (M_StringCompare(parms, color[i].name))
        {
            M_snprintf(buffer, sizeof(buffer), "%i", nearestcolors[color[i].value]);
            int_cvars_func2(cmd, buffer);
            AM_setColors();
            return;
        }

    if (strlen(parms) == 7 && parms[0] == '#')
    {
        M_snprintf(buffer, sizeof(buffer), "%i", FindNearestColor(W_CacheLumpName("PLAYPAL"),
            hextodec(M_SubString(parms, 1, 2)), hextodec(M_SubString(parms, 3, 2)),
            hextodec(M_SubString(parms, 5, 2))));
        int_cvars_func2(cmd, buffer);
    }
    else
        int_cvars_func2(cmd, parms);

    if (*parms)
        AM_setColors();
}

//
// float CVARs
//
static dboolean float_cvars_func1(char *cmd, char *parms)
{
    if (!*parms)
        return true;

    for (int i = 0; *consolecmds[i].name; i++)
        if (M_StringCompare(cmd, consolecmds[i].name) && consolecmds[i].type == CT_CVAR
            && (consolecmds[i].flags & CF_FLOAT))
        {
            float   value = FLT_MIN;

            sscanf(parms, "%10f", &value);
            return (value != FLT_MIN);
        }

    return false;
}

//
// integer CVARs
//
static dboolean int_cvars_func1(char *cmd, char *parms)
{
    if (!*parms)
        return true;

    for (int i = 0; *consolecmds[i].name; i++)
        if (M_StringCompare(cmd, consolecmds[i].name) && consolecmds[i].type == CT_CVAR
            && (consolecmds[i].flags & CF_INTEGER))
        {
            int value = C_LookupValueFromAlias(parms, consolecmds[i].aliases);

            if (value == INT_MIN)
                sscanf(parms, "%10i", &value);

            return (value >= consolecmds[i].minimumvalue && value <= consolecmds[i].maximumvalue);
        }

    return false;
}

static void int_cvars_func2(char *cmd, char *parms)
{
    for (int i = 0; *consolecmds[i].name; i++)
        if (M_StringCompare(cmd, consolecmds[i].name) && consolecmds[i].type == CT_CVAR
            && (consolecmds[i].flags & CF_INTEGER))
        {
            if (*parms && !(consolecmds[i].flags & CF_READONLY))
            {
                int value = C_LookupValueFromAlias(parms, consolecmds[i].aliases);

                if (value == INT_MIN)
                    sscanf(parms, "%10i", &value);

                if (value != INT_MIN && value != *(int *)consolecmds[i].variable)
                {
                    *(int *)consolecmds[i].variable = value;
                    M_SaveCVARs();
                }
            }
            else
            {
                C_Output(removenewlines(consolecmds[i].description));

                if (consolecmds[i].flags & CF_PERCENT)
                {
                    if (consolecmds[i].flags & CF_READONLY)
                        C_Output("It is currently set to <b>%i%%</b> and is read-only.",
                            *(int *)consolecmds[i].variable);
                    else if (*(int *)consolecmds[i].variable == (int)consolecmds[i].defaultnumber)
                        C_Output("It is currently set to its default of <b>%i%%</b>.",
                            *(int *)consolecmds[i].variable);
                    else
                        C_Output("It is currently set to <b>%i%%</b> and its default is <b>%i%%</b>.",
                            *(int *)consolecmds[i].variable, (int)consolecmds[i].defaultnumber);
                }
                else
                {
                    if (consolecmds[i].flags & CF_READONLY)
                        C_Output("It is currently set to <b>%s</b> and is read-only.",
                            C_LookupAliasFromValue(*(int *)consolecmds[i].variable, consolecmds[i].aliases));
                    else if (*(int *)consolecmds[i].variable == (int)consolecmds[i].defaultnumber)
                        C_Output("It is currently set to its default of <b>%s</b>.",
                            C_LookupAliasFromValue(*(int *)consolecmds[i].variable, consolecmds[i].aliases));
                    else
                        C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                            C_LookupAliasFromValue(*(int *)consolecmds[i].variable, consolecmds[i].aliases),
                            C_LookupAliasFromValue((int)consolecmds[i].defaultnumber, consolecmds[i].aliases));
                }
            }

            break;
        }
}

//
// string CVARs
//
static void str_cvars_func2(char *cmd, char *parms)
{
    for (int i = 0; *consolecmds[i].name; i++)
        if (M_StringCompare(cmd, consolecmds[i].name) && consolecmds[i].type == CT_CVAR
            && (consolecmds[i].flags & CF_STRING))
        {
            if (M_StringCompare(parms, EMPTYVALUE) && **(char **)consolecmds[i].variable
                && !(consolecmds[i].flags & CF_READONLY))
            {
                *(char **)consolecmds[i].variable = "";
                M_SaveCVARs();
            }
            else if (*parms)
            {
                if (!M_StringCompare(parms, *(char **)consolecmds[i].variable)
                    && !(consolecmds[i].flags & CF_READONLY))
                {
                    C_StripQuotes(parms);
                    *(char **)consolecmds[i].variable = strdup(parms);
                    M_SaveCVARs();
                }
            }
            else
            {
                C_Output(removenewlines(consolecmds[i].description));

                if (consolecmds[i].flags & CF_READONLY)
                    C_Output("It is currently set to <b>\"%s\"</b> and is read-only.",
                        *(char **)consolecmds[i].variable);
                else if (M_StringCompare(*(char **)consolecmds[i].variable, consolecmds[i].defaultstring))
                    C_Output("It is currently set to its default of <b>\"%s\"</b>.",
                        *(char **)consolecmds[i].variable);
                else
                    C_Output("It is currently set to <b>\"%s\"</b> and its default is <b>\"%s\"</b>.",
                        *(char **)consolecmds[i].variable, consolecmds[i].defaultstring);
            }

            break;
        }
}

//
// time CVARs
//
static void time_cvars_func2(char *cmd, char *parms)
{
    for (int i = 0; *consolecmds[i].name; i++)
        if (M_StringCompare(cmd, consolecmds[i].name) && consolecmds[i].type == CT_CVAR
            && (consolecmds[i].flags & CF_TIME))
        {
            const int   tics = *(int *)consolecmds[i].variable / TICRATE;

            C_Output(removenewlines(consolecmds[i].description));
            C_Output("It is currently set to <b>%02i:%02i:%02i</b> and is read-only.",
                tics / 3600, (tics % 3600) / 60, (tics % 3600) % 60);
            break;
        }
}

//
// alwaysrun CVAR
//
static void alwaysrun_cvar_func2(char *cmd, char *parms)
{
    bool_cvars_func2(cmd, parms);
    I_InitKeyboard();
}

//
// am_external CVAR
//
static void am_external_cvar_func2(char *cmd, char *parms)
{
    const dboolean  am_external_old = am_external;

    bool_cvars_func2(cmd, parms);

    if (am_external != am_external_old)
    {
        if (am_external)
        {
            I_CreateExternalAutomap(false);
            am_followmode = true;

            if (gamestate == GS_LEVEL)
                AM_Start(false);
        }
        else
        {
            I_DestroyExternalAutomap();
            mapscreen = *screens;

            if (gamestate == GS_LEVEL)
                AM_Stop();
        }
    }
}

//
// am_followmode CVAR
//
static dboolean am_followmode_cvar_func1(char *cmd, char *parms)
{
    return (!mapwindow && gamestate == GS_LEVEL);
}

//
// am_gridsize CVAR
//
static void am_gridsize_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        am_gridsize = strdup(parms);
        AM_getGridSize();

        if (!M_StringCompare(am_gridsize, parms))
            M_SaveCVARs();
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(am_gridsize))].description));

        if (M_StringCompare(am_gridsize, am_gridsize_default))
            C_Output("It is currently set to its default of <b>%s</b>.", formatsize(am_gridsize));
        else
            C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                formatsize(am_gridsize), formatsize(am_gridsize_default));
    }
}

//
// am_path CVAR
//
static void am_path_cvar_func2(char *cmd, char *parms)
{
    const dboolean  am_path_old = am_path;

    bool_cvars_func2(cmd, parms);

    if (!am_path && am_path_old)
        pathpointnum = 0;
}

//
// episode CVAR
//
static void episode_cvar_func2(char *cmd, char *parms)
{
    const int   episode_old = episode;

    int_cvars_func2(cmd, parms);

    if (episode != episode_old)
        EpiDef.lastOn = episode - 1;
}

//
// expansion CVAR
//
static void expansion_cvar_func2(char *cmd, char *parms)
{
    const int   expansion_old = expansion;

    int_cvars_func2(cmd, parms);

    if (expansion != expansion_old)
        ExpDef.lastOn = expansion - 1;
}

//
// gp_deadzone_left and gp_deadzone_right CVARs
//
static dboolean gp_deadzone_cvars_func1(char *cmd, char *parms)
{
    float value;

    if (!*parms)
        return true;

    if (parms[strlen(parms) - 1] == '%')
        parms[strlen(parms) - 1] = '\0';

    return sscanf(parms, "%10f", &value);
}

static void gp_deadzone_cvars_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        float   value = 0;

        if (parms[strlen(parms) - 1] == '%')
            parms[strlen(parms) - 1] = '\0';

        sscanf(parms, "%10f", &value);

        if (M_StringCompare(cmd, stringize(gp_deadzone_left)))
        {
            if (gp_deadzone_left != value)
            {
                gp_deadzone_left = BETWEENF(gp_deadzone_left_min, value, gp_deadzone_left_max);
                I_SetGamepadLeftDeadZone();
                M_SaveCVARs();
            }
        }
        else if (gp_deadzone_right != value)
        {
            gp_deadzone_right = BETWEENF(gp_deadzone_right_min, value, gp_deadzone_right_max);
            I_SetGamepadRightDeadZone();
            M_SaveCVARs();
        }
    }
    else if (M_StringCompare(cmd, stringize(gp_deadzone_left)))
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(gp_deadzone_left))].description));

        if (gp_deadzone_left == gp_deadzone_left_default)
            C_Output("It is currently set to its default of <b>%s%%</b>.",
                striptrailingzero(gp_deadzone_left, 1));
        else
            C_Output("It is currently set to <b>%s%%</b> and its default is <b>%s%%</b>.",
                striptrailingzero(gp_deadzone_left, 1), striptrailingzero(gp_deadzone_left_default, 1));
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(gp_deadzone_right))].description));

        if (gp_deadzone_right == gp_deadzone_right_default)
            C_Output("It is currently set to its default of <b>%s%%</b>.",
                striptrailingzero(gp_deadzone_right, 1));
        else
            C_Output("It is currently set to <b>%s%%</b> and its default is <b>%s%%</b>.",
                striptrailingzero(gp_deadzone_right, 1), striptrailingzero(gp_deadzone_right_default, 1));
    }
}

//
// gp_sensitivity CVAR
//
static void gp_sensitivity_cvar_func2(char *cmd, char *parms)
{
    const int   gp_sensitivity_old = gp_sensitivity;

    int_cvars_func2(cmd, parms);

    if (gp_sensitivity != gp_sensitivity_old)
        I_SetGamepadSensitivity();
}

//
// gp_swapthumbsticks CVAR
//
static void gp_swapthumbsticks_cvar_func2(char *cmd, char *parms)
{
    const int   gp_swapthumbsticks_old = gp_swapthumbsticks;

    bool_cvars_func2(cmd, parms);

    if (gp_swapthumbsticks != gp_swapthumbsticks_old)
        I_SetGamepadThumbSticks();
}

//
// mouselook CVAR
//
static void mouselook_cvar_func2(char *cmd, char *parms)
{
    const dboolean  mouselook_old = mouselook;

    bool_cvars_func2(cmd, parms);

    if (mouselook != mouselook_old)
    {
        R_InitSkyMap();
        R_InitColumnFunctions();

        if (!mouselook && gamestate == GS_LEVEL)
        {
            viewplayer->lookdir = 0;
            viewplayer->oldlookdir = 0;
            viewplayer->recoil = 0;
            viewplayer->oldrecoil = 0;
        }
    }
}

//
// ammo, armor and health CVARs
//
dboolean P_CheckAmmo(void);

static dboolean player_cvars_func1(char *cmd, char *parms)
{
    return (int_cvars_func1(cmd, parms) && gamestate == GS_LEVEL && (!M_StringCompare(cmd, stringize(health))
        || (!(viewplayer->cheats & CF_GODMODE) && !viewplayer->powers[pw_invulnerability])));
}

static void player_cvars_func2(char *cmd, char *parms)
{
    int value;

    if (resettingcvar)
        return;

    if (M_StringCompare(cmd, stringize(ammo)))
    {
        ammotype_t  ammotype = weaponinfo[viewplayer->readyweapon].ammo;

        if (*parms)
        {
            sscanf(parms, "%10i", &value);

            if (ammotype != am_noammo && value != viewplayer->ammo[ammotype] && viewplayer->playerstate == PST_LIVE)
            {
                if (value > viewplayer->ammo[ammotype])
                    P_AddBonus();

                viewplayer->ammo[ammotype] = MIN(value, viewplayer->maxammo[ammotype]);
                P_CheckAmmo();
                C_HideConsole();
            }
        }
        else
        {
            C_Output(removenewlines(consolecmds[C_GetIndex(stringize(ammo))].description));
            C_Output("It is currently set to <b>%i</b>.",
                (ammotype == am_noammo ? 0 : viewplayer->ammo[ammotype]));
        }
    }
    else if (M_StringCompare(cmd, stringize(armor)))
    {
        if (*parms)
        {
            sscanf(parms, "%10i", &value);

            if (value != viewplayer->armorpoints)
            {
                if (value > viewplayer->armorpoints)
                    P_AddBonus();

                viewplayer->armorpoints = MIN(value, max_armor);
                C_HideConsole();
            }
        }
        else
        {
            C_Output(removenewlines(consolecmds[C_GetIndex(stringize(armor))].description));
            C_Output("It is currently set to <b>%i%%</b>.", viewplayer->armorpoints);
        }
    }
    else if (M_StringCompare(cmd, stringize(health)))
    {
        if (*parms)
        {
            sscanf(parms, "%10i", &value);
            value = BETWEEN(health_min, value, maxhealth);

            if (viewplayer->health <= 0)
            {
                if (value <= 0)
                {
                    viewplayer->health = value;
                    viewplayer->mo->health = value;
                }
                else
                    P_ResurrectPlayer(value);
            }
            else
            {
                if (value < viewplayer->health)
                    P_DamageMobj(viewplayer->mo, viewplayer->mo, NULL, viewplayer->health - value, false);
                else
                {
                    viewplayer->health = value;
                    viewplayer->mo->health = value;
                    P_AddBonus();
                    S_StartSound(NULL, sfx_getpow);
                }

                C_HideConsole();
            }
        }
        else
        {
            C_Output(removenewlines(consolecmds[C_GetIndex(stringize(health))].description));
            C_Output("It is currently set to <b>%i%%</b>.", viewplayer->health);
        }
    }
}

//
// playername CVAR
//
static void playername_cvar_func2(char *cmd, char *parms)
{
    str_cvars_func2(cmd, (M_StringCompare(parms, EMPTYVALUE) ? playername_default : parms));
}

//
// r_blood CVAR
//
static dboolean r_blood_cvar_func1(char *cmd, char *parms)
{
    return (!*parms || C_LookupValueFromAlias(parms, BLOODVALUEALIAS) != INT_MIN);
}

static void r_blood_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, BLOODVALUEALIAS);

        if (value != INT_MIN && r_blood != value)
        {
            r_blood = value;
            M_SaveCVARs();
        }
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(r_blood))].description));

        if (r_blood == r_blood_default)
            C_Output("It is currently set to its default of <b>%s</b>.",
                C_LookupAliasFromValue(r_blood, BLOODVALUEALIAS));
        else
            C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                C_LookupAliasFromValue(r_blood, BLOODVALUEALIAS),
                C_LookupAliasFromValue(r_blood_default, BLOODVALUEALIAS));
    }
}

//
// r_bloodsplats_translucency CVAR
//
static void r_bloodsplats_translucency_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, BOOLVALUEALIAS);

        if ((value == 0 || value == 1) && value != r_bloodsplats_translucency)
        {
            r_bloodsplats_translucency = !!value;
            M_SaveCVARs();
            R_InitColumnFunctions();

            for (int i = 0; i < numsectors; i++)
            {
                bloodsplat_t    *splat = sectors[i].splatlist;

                while (splat)
                {
                    splat->colfunc = (splat->blood == FUZZYBLOOD ? fuzzcolfunc : bloodsplatcolfunc);
                    splat = splat->snext;
                }
            }
        }
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(r_bloodsplats_translucency))].description));

        if (r_bloodsplats_translucency == r_bloodsplats_translucency_default)
            C_Output("It is currently set to its default of <b>%s</b>.",
                C_LookupAliasFromValue(r_bloodsplats_translucency, BOOLVALUEALIAS));
        else
            C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                C_LookupAliasFromValue(r_bloodsplats_translucency, BOOLVALUEALIAS),
                C_LookupAliasFromValue(r_bloodsplats_translucency_default, BOOLVALUEALIAS));
    }
}

//
// r_detail CVAR
//
static dboolean r_detail_cvar_func1(char *cmd, char *parms)
{
    return (!*parms || C_LookupValueFromAlias(parms, DETAILVALUEALIAS) != INT_MIN);
}

static void r_detail_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, DETAILVALUEALIAS);

        if ((value == r_detail_low || value == r_detail_high) && r_detail != value)
        {
            r_detail = !!value;
            M_SaveCVARs();
        }
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(r_detail))].description));

        if (r_detail == r_detail_default)
            C_Output("It is currently set to its default of <b>%s</b>.",
                C_LookupAliasFromValue(r_detail, DETAILVALUEALIAS));
        else
            C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                C_LookupAliasFromValue(r_detail, DETAILVALUEALIAS),
                C_LookupAliasFromValue(r_detail_default, DETAILVALUEALIAS));
    }
}

//
// r_dither CVAR
//
static void r_dither_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, BOOLVALUEALIAS);

        if ((value == 0 || value == 1) && value != r_dither)
        {
            r_dither = !!value;
            M_SaveCVARs();
            R_InitColumnFunctions();

            if (W_CheckNumForName("TRANMAP") == -1)
                tranmap = (r_dither ? tinttab25 : tinttab50);
        }
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(r_dither))].description));

        if (r_dither == r_dither_default)
            C_Output("It is currently set to its default of <b>%s</b>.",
                C_LookupAliasFromValue(r_dither, BOOLVALUEALIAS));
        else
            C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                C_LookupAliasFromValue(r_dither, BOOLVALUEALIAS),
                C_LookupAliasFromValue(r_dither_default, BOOLVALUEALIAS));
    }
}

//
// r_fixmaperrors CVAR
//
static void r_fixmaperrors_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, BOOLVALUEALIAS);

        if ((value == 0 || value == 1) && value != r_fixmaperrors)
        {
            r_fixmaperrors = !!value;
            M_SaveCVARs();

            if (r_fixmaperrors && gamestate == GS_LEVEL && !togglingvanilla && !resettingcvar)
                C_Warning(PENDINGCHANGE);
        }
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(r_fixmaperrors))].description));

        if (r_fixmaperrors == r_fixmaperrors_default)
            C_Output("It is currently set to its default of <b>%s</b>.",
                C_LookupAliasFromValue(r_fixmaperrors, BOOLVALUEALIAS));
        else
            C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                C_LookupAliasFromValue(r_fixmaperrors, BOOLVALUEALIAS),
                C_LookupAliasFromValue(r_fixmaperrors_default, BOOLVALUEALIAS));
    }
}

//
// r_fov CVAR
//
static void r_fov_cvar_func2(char *cmd, char *parms)
{
    const int   r_fov_old = r_fov;

    int_cvars_func2(cmd, parms);

    if (r_fov != r_fov_old)
    {
        setsizeneeded = true;
        R_InitLightTables();
    }
}

//
// r_gamma CVAR
//
static dboolean r_gamma_cvar_func1(char *cmd, char *parms)
{
    return (C_LookupValueFromAlias(parms, GAMMAVALUEALIAS) != INT_MIN || float_cvars_func1(cmd, parms));
}

static void r_gamma_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        float   value = (float)C_LookupValueFromAlias(parms, GAMMAVALUEALIAS);

        if (value == INT_MIN)
            sscanf(parms, "%10f", &value);

        if (value != INT_MIN && value != r_gamma)
        {
            r_gamma = BETWEENF(r_gamma_min, value, r_gamma_max);
            I_SetGamma(r_gamma);
            I_SetPalette((byte *)W_CacheLumpName("PLAYPAL") + st_palette * 768);
            M_SaveCVARs();
        }
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(r_gamma))].description));

        if (r_gamma == r_gamma_default)
            C_Output("It is currently set to its default of <b>%s</b>.",
                (r_gamma == 1.0f ? "off" : striptrailingzero(r_gamma, 2)));
        else
            C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                (r_gamma == 1.0f ? "off" : striptrailingzero(r_gamma, 2)),
                (r_gamma_default == 1.0f ? "off" : striptrailingzero(r_gamma_default, 2)));
    }
}

//
// r_hud CVAR
//
static void r_hud_cvar_func2(char *cmd, char *parms)
{
    if (vid_widescreen || !*parms)
        bool_cvars_func2(cmd, parms);
}

//
// r_hud_translucency CVAR
//
static void r_hud_translucency_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int value = C_LookupValueFromAlias(parms, BOOLVALUEALIAS);

        if ((value == 0 || value == 1) && value != r_hud_translucency)
        {
            r_hud_translucency = !!value;
            M_SaveCVARs();
            HU_SetTranslucency();
        }
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(r_hud_translucency))].description));

        if (r_hud_translucency == r_hud_translucency_default)
            C_Output("It is currently set to its default of <b>%s</b>.",
                C_LookupAliasFromValue(r_hud_translucency, BOOLVALUEALIAS));
        else
            C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                C_LookupAliasFromValue(r_hud_translucency, BOOLVALUEALIAS),
                C_LookupAliasFromValue(r_hud_translucency_default, BOOLVALUEALIAS));
    }
}

//
// r_lowpixelsize CVAR
//
static void r_lowpixelsize_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        r_lowpixelsize = strdup(parms);
        GetPixelSize(false);

        if (!M_StringCompare(r_lowpixelsize, parms))
            M_SaveCVARs();
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(r_lowpixelsize))].description));

        if (M_StringCompare(r_lowpixelsize, r_lowpixelsize_default))
            C_Output("It is currently set to its default of <b>%s</b>.", formatsize(r_lowpixelsize));
        else
            C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                formatsize(r_lowpixelsize), formatsize(r_lowpixelsize_default));
    }
}

//
// r_messagepos CVAR
//
static void r_messagepos_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        if (!M_StringCompare(r_messagepos, parms))
        {
            r_messagepos = strdup(parms);
            HU_GetMessagePosition();
            M_SaveCVARs();
        }
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(r_messagepos))].description));

        if (M_StringCompare(r_messagepos, r_messagepos_default))
            C_Output("It is currently set to its default of <b>%s</b>.", r_messagepos);
        else
            C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                r_messagepos, r_messagepos_default);
    }
}

//
// r_messagescale CVAR
//
static dboolean r_messagescale_cvar_func1(char *cmd, char *parms)
{
    return (!*parms || C_LookupValueFromAlias(parms, SCALEVALUEALIAS) != INT_MIN);
}

static void r_messagescale_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, SCALEVALUEALIAS);

        if ((value == r_messagescale_small || value == r_messagescale_big) && value != r_messagescale)
        {
            r_messagescale = !!value;
            M_SaveCVARs();
        }
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(r_messagescale))].description));

        if (r_messagescale == r_messagescale_default)
            C_Output("It is currently set to its default of <b>%s</b>.",
                C_LookupAliasFromValue(r_messagescale, SCALEVALUEALIAS));
        else
            C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                C_LookupAliasFromValue(r_messagescale, SCALEVALUEALIAS),
                C_LookupAliasFromValue(r_messagescale_default, SCALEVALUEALIAS));
    }
}

//
// r_screensize CVAR
//
static void r_screensize_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = parms[0] - '0';

        if (strlen(parms) == 1 && value >= r_screensize_min && value <= r_screensize_max && value != r_screensize)
        {
            if (vid_widescreen || (returntowidescreen && gamestate != GS_LEVEL))
            {
                if (value < r_screensize_max)
                {
                    r_hud = true;
                    I_ToggleWidescreen(false);
                }
            }

            r_screensize = value;
            M_SaveCVARs();
            R_SetViewSize(r_screensize);
        }
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(r_screensize))].description));

        if (r_screensize == r_screensize_default)
            C_Output("It is currently set to its default of <b>%i</b>.", r_screensize);
        else
            C_Output("It is currently set to <b>%i</b> and its default is <b>%i</b>.",
                r_screensize, r_screensize_default);
    }
}

//
// r_shadows_translucency CVAR
//
static void r_shadows_translucency_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, BOOLVALUEALIAS);

        if ((value == 0 || value == 1) && value != r_shadows_translucency)
        {
            r_shadows_translucency = !!value;
            M_SaveCVARs();

            for (int i = 0; i < numsectors; i++)
            {
                mobj_t  *mo = sectors[i].thinglist;

                while (mo)
                {
                    P_SetShadowColumnFunction(mo);
                    mo = mo->snext;
                }
            }
        }
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(r_shadows_translucency))].description));

        if (r_shadows_translucency == r_shadows_translucency_default)
            C_Output("It is currently set to its default of <b>%s</b>.",
                C_LookupAliasFromValue(r_shadows_translucency, BOOLVALUEALIAS));
        else
            C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                C_LookupAliasFromValue(r_shadows_translucency, BOOLVALUEALIAS),
                C_LookupAliasFromValue(r_shadows_translucency_default, BOOLVALUEALIAS));
    }
}

//
// r_skycolor CVAR
//
static dboolean r_skycolor_cvar_func1(char *cmd, char *parms)
{
    return (C_LookupValueFromAlias(parms, SKYVALUEALIAS) == r_skycolor_none || color_cvars_func1(cmd, parms));
}

static void r_skycolor_cvar_func2(char *cmd, char *parms)
{
    const int   value = C_LookupValueFromAlias(parms, SKYVALUEALIAS);

    if (value != INT_MIN)
    {
        if (value != r_skycolor)
        {
            r_skycolor = r_skycolor_none;
            M_SaveCVARs();
            R_InitColumnFunctions();
        }
    }
    else
    {
        const int   r_skycolor_old = r_skycolor;

        color_cvars_func2(cmd, parms);

        if (r_skycolor != r_skycolor_old)
            R_InitColumnFunctions();
    }
}

//
// r_textures CVAR
//
static void r_textures_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, BOOLVALUEALIAS);

        if ((value == 0 || value == 1) && value != r_textures)
        {
            r_textures = !!value;
            M_SaveCVARs();
            R_InitColumnFunctions();

            for (int i = 0; i < numsectors; i++)
            {
                mobj_t          *mo = sectors[i].thinglist;
                bloodsplat_t    *splat = sectors[i].splatlist;

                while (mo)
                {
                    mo->colfunc = mo->info->colfunc;
                    P_SetShadowColumnFunction(mo);
                    mo = mo->snext;
                }

                while (splat)
                {
                    splat->colfunc = (splat->blood == FUZZYBLOOD ? fuzzcolfunc : bloodsplatcolfunc);
                    splat = splat->snext;
                }
            }
        }
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(r_textures))].description));

        if (r_textures == r_textures_default)
            C_Output("It is currently set to its default of <b>%s</b>.",
                C_LookupAliasFromValue(r_textures, BOOLVALUEALIAS));
        else
            C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                C_LookupAliasFromValue(r_textures, BOOLVALUEALIAS),
                C_LookupAliasFromValue(r_textures_default, BOOLVALUEALIAS));
    }
}

//
// r_translucency CVAR
//
static void r_translucency_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, BOOLVALUEALIAS);

        if ((value == 0 || value == 1) && value != r_translucency)
        {
            r_translucency = !!value;
            M_SaveCVARs();
            R_InitColumnFunctions();

            for (int i = 0; i < numsectors; i++)
            {
                mobj_t  *mo = sectors[i].thinglist;

                while (mo)
                {
                    mo->colfunc = mo->info->colfunc;
                    mo = mo->snext;
                }
            }
        }
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(r_translucency))].description));

        if (r_translucency == r_translucency_default)
            C_Output("It is currently set to its default of <b>%s</b>.",
                C_LookupAliasFromValue(r_translucency, BOOLVALUEALIAS));
        else
            C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                C_LookupAliasFromValue(r_translucency, BOOLVALUEALIAS),
                C_LookupAliasFromValue(r_translucency_default, BOOLVALUEALIAS));
    }
}

//
// s_musicvolume and s_sfxvolume CVARs
//
static dboolean s_volume_cvars_func1(char *cmd, char *parms)
{
    int value = INT_MIN;

    if (!*parms)
        return true;

    if (parms[strlen(parms) - 1] == '%')
        parms[strlen(parms) - 1] = '\0';

    sscanf(parms, "%10i", &value);

    return ((M_StringCompare(cmd, stringize(s_musicvolume)) && value >= s_musicvolume_min
        && value <= s_musicvolume_max) || (M_StringCompare(cmd, stringize(s_sfxvolume))
        && value >= s_sfxvolume_min && value <= s_sfxvolume_max));
}

static void s_volume_cvars_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        int value = INT_MIN;

        if (parms[strlen(parms) - 1] == '%')
            parms[strlen(parms) - 1] = '\0';

        sscanf(parms, "%10i", &value);

        if (M_StringCompare(cmd, stringize(s_musicvolume)) && s_musicvolume != value)
        {
            s_musicvolume = value;
            musicVolume = (BETWEEN(s_musicvolume_min, s_musicvolume, s_musicvolume_max) * 31 + 50) / 100;
            S_SetMusicVolume(musicVolume * MAX_MUSIC_VOLUME / 31);
            M_SaveCVARs();
        }
        else if (s_sfxvolume != value)
        {
            s_sfxvolume = value;
            sfxVolume = (BETWEEN(s_sfxvolume_min, s_sfxvolume, s_sfxvolume_max) * 31 + 50) / 100;
            S_SetSfxVolume(sfxVolume * MAX_SFX_VOLUME / 31);
            M_SaveCVARs();
        }
    }
    else if (M_StringCompare(cmd, stringize(s_musicvolume)))
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(s_musicvolume))].description));

        if (s_musicvolume == s_musicvolume_default)
            C_Output("It is currently set to its default of <b>%i%%</b>.", s_musicvolume);
        else
            C_Output("It is currently set to <b>%i%%</b> and its default is <b>%i%%</b>.",
                s_musicvolume, s_musicvolume_default);
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(s_sfxvolume))].description));

        if (s_sfxvolume == s_sfxvolume_default)
            C_Output("It is currently set to its default of <b>%i%%</b>.", s_sfxvolume);
        else
            C_Output("It is currently set to <b>%i%%</b> and its default is <b>%i%%</b>.",
                s_sfxvolume, s_sfxvolume_default);
    }
}

//
// savegame CVAR
//
static void savegame_cvar_func2(char *cmd, char *parms)
{
    const int   savegame_old = savegame;

    int_cvars_func2(cmd, parms);

    if (savegame != savegame_old)
    {
        SaveDef.lastOn = savegame - 1;
        LoadDef.lastOn = savegame - 1;
    }
}

//
// skilllevel CVAR
//
static void skilllevel_cvar_func2(char *cmd, char *parms)
{
    const int   skilllevel_old = skilllevel;

    int_cvars_func2(cmd, parms);

    if (skilllevel != skilllevel_old)
    {
        pendinggameskill = skilllevel;
        NewDef.lastOn = skilllevel - 1;

        if (gamestate == GS_LEVEL && !resettingcvar)
            C_Warning(PENDINGCHANGE);
    }
}

//
// turbo CVAR
//
static dboolean turbo_cvar_func1(char *cmd, char *parms)
{
    int value = INT_MIN;

    if (!*parms)
        return true;

    sscanf(parms, "%10i", &value);
    return (value >= turbo_min && value <= turbo_max);
}

static void turbo_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        int value = INT_MIN;

        sscanf(parms, "%10i", &value);

        if (value >= turbo_min && value <= turbo_max && value != turbo)
        {
            turbo = value;

            if (turbo > turbo_default)
            {
                viewplayer->cheated++;
                stat_cheated = SafeAdd(stat_cheated, 1);
            }

            M_SaveCVARs();
            G_SetMovementSpeed(turbo);
        }
    }
    else
    {
        char    *description = removenewlines(consolecmds[C_GetIndex(stringize(turbo))].description);

        strreplace(description, "%", "%%");
        C_Output(description);

        if (turbo == turbo_default)
            C_Output("It is currently set to its default of <b>%i%%</b>.", turbo);
        else
            C_Output("It is currently set to <b>%i%%</b> and its default is <b>%i%%</b>.",
                turbo, turbo_default);

        free(description);
    }
}

//
// units CVAR
//
static dboolean units_cvar_func1(char *cmd, char *parms)
{
    return (!*parms || C_LookupValueFromAlias(parms, UNITSVALUEALIAS) != INT_MIN);
}

static void units_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        const int   value = C_LookupValueFromAlias(parms, UNITSVALUEALIAS);

        if ((value == 0 || value == 1) && value != units)
        {
            units = !!value;
            M_SaveCVARs();
        }
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(units))].description));

        if (units == units_default)
            C_Output("It is currently set to its default of <b>%s</b>.",
                C_LookupAliasFromValue(units, UNITSVALUEALIAS));
        else
            C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                C_LookupAliasFromValue(units, UNITSVALUEALIAS),
                C_LookupAliasFromValue(units_default, UNITSVALUEALIAS));
    }
}

//
// vid_capfps CVAR
//
static dboolean vid_capfps_cvar_func1(char *cmd, char *parms)
{
    return (C_LookupValueFromAlias(parms, CAPVALUEALIAS) != INT_MIN || int_cvars_func1(cmd, parms));
}

static void vid_capfps_cvar_func2(char *cmd, char *parms)
{
    const int   value = C_LookupValueFromAlias(parms, CAPVALUEALIAS);

    if (value != INT_MIN)
    {
        if (value != vid_capfps)
        {
            vid_capfps = value;
            M_SaveCVARs();
            I_CapFPS(vid_capfps);
        }
    }
    else
    {
        const int   vid_capfps_old = vid_capfps;

        int_cvars_func2(cmd, parms);

        if (vid_capfps != vid_capfps_old)
            I_CapFPS(vid_capfps);
    }
}

//
// vid_display CVAR
//
static void vid_display_cvar_func2(char *cmd, char *parms)
{
    const int   vid_display_old = vid_display;

    int_cvars_func2(cmd, parms);

    if (vid_display != vid_display_old)
        I_RestartGraphics();
}

//
// vid_fullscreen CVAR
//
static void vid_fullscreen_cvar_func2(char *cmd, char *parms)
{
    const dboolean  vid_fullscreen_old = vid_fullscreen;

    bool_cvars_func2(cmd, parms);

    if (vid_fullscreen != vid_fullscreen_old)
        I_ToggleFullscreen();
}

//
// vid_scaleapi CVAR
//
static dboolean vid_scaleapi_cvar_func1(char *cmd, char *parms)
{
    return (!*parms || M_StringCompare(parms, vid_scaleapi_direct3d)
        || M_StringCompare(parms, vid_scaleapi_opengl)
#if !defined(_WIN32)
        || M_StringCompare(parms, vid_scaleapi_opengles)
        || M_StringCompare(parms, vid_scaleapi_opengles2)
#endif
        || M_StringCompare(parms, vid_scaleapi_software));
}

static void vid_scaleapi_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        if (!M_StringCompare(parms, vid_scaleapi))
        {
            vid_scaleapi = strdup(parms);
            M_SaveCVARs();
            I_RestartGraphics();
        }
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(vid_scaleapi))].description));

        if (M_StringCompare(vid_scaleapi, vid_scaleapi_default))
            C_Output("It is currently set to its default of <b>\"%s\"</b>.", vid_scaleapi);
        else
            C_Output("It is currently set to <b>\"%s\"</b> and its default is <b>\"%s\"</b>.",
                vid_scaleapi, vid_scaleapi_default);
    }
}

//
// vid_scalefilter CVAR
//
static dboolean vid_scalefilter_cvar_func1(char *cmd, char *parms)
{
    return (!*parms || M_StringCompare(parms, vid_scalefilter_nearest)
        || M_StringCompare(parms, vid_scalefilter_linear)
        || M_StringCompare(parms, vid_scalefilter_nearest_linear));
}

static void vid_scalefilter_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        if (!M_StringCompare(parms, vid_scalefilter))
        {
            vid_scalefilter = strdup(parms);
            M_SaveCVARs();
            I_RestartGraphics();
        }
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(vid_scalefilter))].description));

        if (M_StringCompare(vid_scalefilter, vid_scalefilter_default))
            C_Output("It is currently set to its default of <b>\"%s\"</b>.", vid_scalefilter);
        else
            C_Output("It is currently set to <b>\"%s\"</b> and its default is <b>\"%s\"</b>.",
                vid_scalefilter, vid_scalefilter_default);
    }
}

//
// vid_screenresolution CVAR
//
static void vid_screenresolution_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        if (!M_StringCompare(vid_screenresolution, parms))
        {
            vid_screenresolution = strdup(parms);
            GetScreenResolution();
            M_SaveCVARs();

            if (vid_fullscreen)
                I_RestartGraphics();
        }
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(vid_screenresolution))].description));

        if (M_StringCompare(vid_screenresolution, vid_screenresolution_default))
            C_Output("It is currently set to its default of <b>%s</b>.", formatsize(vid_screenresolution));
        else
            C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                formatsize(vid_screenresolution), formatsize(vid_screenresolution_default));
    }
}

//
// vid_showfps CVAR
//
extern int      minfps;
extern int      maxfps;
extern uint64_t starttime;
extern int      frames;

static void vid_showfps_cvar_func2(char *cmd, char *parms)
{
    const dboolean  vid_showfps_old = vid_showfps;

    bool_cvars_func2(cmd, parms);

    if (vid_showfps != vid_showfps_old)
    {
        I_UpdateBlitFunc(!!viewplayer->damagecount);

        if (!vid_showfps)
        {
            C_Output("The minimum was %s FPS (%.1fms) and the maximum was %s FPS (%.1fms).",
                commify(minfps), 1000.0 / minfps, commify(maxfps), 1000.0 / maxfps);
            minfps = INT_MAX;
            maxfps = 0;
            frames = -1;
        }

        starttime = SDL_GetPerformanceCounter();
    }
}

//
// vid_vsync CVAR
//
static void vid_vsync_cvar_func2(char *cmd, char *parms)
{
    const dboolean  vid_vsync_old = vid_vsync;

    bool_cvars_func2(cmd, parms);

    if (vid_vsync != vid_vsync_old)
        I_RestartGraphics();
}

//
// vid_widescreen CVAR
//
static void vid_widescreen_cvar_func2(char *cmd, char *parms)
{
    const dboolean  vid_widescreen_old = vid_widescreen;

    bool_cvars_func2(cmd, parms);

    if (vid_widescreen != vid_widescreen_old)
    {
        if (vid_widescreen)
        {
            if (gamestate == GS_LEVEL)
            {
                I_ToggleWidescreen(true);

                if (vid_widescreen && !togglingvanilla)
                    S_StartSound(NULL, sfx_stnmov);
            }
            else
            {
                returntowidescreen = true;
                r_hud = true;
                M_SaveCVARs();
            }
        }
        else
        {
            I_ToggleWidescreen(false);

            if (!vid_widescreen && !togglingvanilla)
                S_StartSound(NULL, sfx_stnmov);
        }
    }

    if (!vid_widescreen)
        returntowidescreen = false;
}

//
// vid_windowpos CVAR
//
static void vid_windowpos_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        if (!M_StringCompare(vid_windowpos, parms))
        {
            vid_windowpos = strdup(parms);
            GetWindowPosition();
            M_SaveCVARs();

            if (!vid_fullscreen)
                SDL_SetWindowPosition(window, windowx, windowy);
        }
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(vid_windowpos))].description));

        if (M_StringCompare(vid_windowpos, vid_windowpos_default))
            C_Output("It is currently set to its default of <b>%s</b>.", vid_windowpos);
        else
            C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                vid_windowpos, vid_windowpos_default);
    }
}

//
// vid_windowsize CVAR
//
static void vid_windowsize_cvar_func2(char *cmd, char *parms)
{
    if (*parms)
    {
        if (!M_StringCompare(vid_windowsize, parms))
        {
            vid_windowsize = strdup(parms);
            GetWindowSize();
            M_SaveCVARs();

            if (!vid_fullscreen)
                SDL_SetWindowSize(window, windowwidth, windowheight);
        }
    }
    else
    {
        C_Output(removenewlines(consolecmds[C_GetIndex(stringize(vid_windowsize))].description));

        if (M_StringCompare(vid_windowsize, vid_windowsize_default))
            C_Output("It is currently set to its default of <b>%s</b>.", formatsize(vid_windowsize));
        else
            C_Output("It is currently set to <b>%s</b> and its default is <b>%s</b>.",
                formatsize(vid_windowsize), formatsize(vid_windowsize_default));
    }
}
