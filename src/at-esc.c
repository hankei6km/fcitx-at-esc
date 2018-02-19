/***************************************************************************
 *
 * fcitx-at-esc
 *
 * Copyright (c) 2018 hankei6km
 * Licensed under the MIT License. See LICENSE.txt in the project root.
 *
 *
 ***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010~2010 by Felix Yan                                  *
 *   felixonmars@gmail.com                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.              *
 ***************************************************************************/

#include "fcitx/fcitx.h"

#include <libintl.h>
#include <errno.h>

#include "fcitx/module.h"
#include "fcitx-utils/utf8.h"
#include "fcitx-utils/uthash.h"
#include "fcitx-config/xdg.h"
#include "fcitx/hook.h"
#include "fcitx/keys.h"
#include "fcitx/ui.h"
#include "fcitx-utils/log.h"
#include "fcitx/instance.h"
#include "fcitx/context.h"
#include "fcitx-utils/utils.h"
#include "at-esc.h"
#include "at-esc-util.h"

#define _(x) dgettext("fcitx-at-esc", (x))

typedef struct _FcitxAtEsc {
    FcitxGenericConfig gconfig;
    boolean enabled;
    boolean forceDisableIME;
    boolean commitPreedit;
    FcitxHotkey toggleHotkey[2];
    FcitxInstance* owner;
} FcitxAtEsc;

void* AtEscCreate(FcitxInstance* instance);
void ToggleatEscState(void* arg);
boolean GetAtEscEnabled(void* arg);
boolean LoadAtEscConfig(FcitxAtEsc* atEscState);
static FcitxConfigFileDesc* GetAtEscConfigDesc();
void SaveAtEscConfig(FcitxAtEsc* atEscState);
void ReloadAtEsc(void* arg);
INPUT_RETURN_VALUE HotkeyToggleatEscState(void* arg);
INPUT_RETURN_VALUE HotkeyEscapeatEscState(void* arg);
boolean IMSelectorAtEscPreFilter(void* arg, FcitxKeySym sym, unsigned int state, INPUT_RETURN_VALUE* retval);
boolean IMSelectorAtEscPostFilter(void* arg, FcitxKeySym sym, unsigned int state, INPUT_RETURN_VALUE* retval);

CONFIG_BINDING_BEGIN(FcitxAtEsc)
CONFIG_BINDING_REGISTER("Basic", "Enabled", enabled)
CONFIG_BINDING_REGISTER("Basic", "ForceDisableIME", forceDisableIME)
CONFIG_BINDING_REGISTER("Basic", "CommitPreedit", commitPreedit)
CONFIG_BINDING_REGISTER("Basic", "ToggleHotkey", toggleHotkey)
CONFIG_BINDING_END()

FCITX_EXPORT_API
FcitxModule module = {
  AtEscCreate,
  NULL,
  NULL,
  NULL,
  ReloadAtEsc
};
// FcitxModule module = {
//   AtEscCreate,
//   MySetFD,
//   EventHandler,
//   NULL,
//   ReloadAtEsc
// };

FCITX_EXPORT_API
int ABI_VERSION = FCITX_ABI_VERSION;

void* AtEscCreate(FcitxInstance* instance)
{
    FcitxAtEsc* atEscState = fcitx_utils_malloc0(sizeof(FcitxAtEsc));
    atEscState->owner = instance;
    if (!LoadAtEscConfig(atEscState)) {
        free(atEscState);
        return NULL;
    }

    FcitxHotkeyHook thk;
    thk.arg = atEscState;
    thk.hotkey = atEscState->toggleHotkey;
    thk.hotkeyhandle = HotkeyToggleatEscState;

    FcitxKeyFilterHook prekf;
    prekf.arg = atEscState;
    prekf.func = IMSelectorAtEscPreFilter;

    FcitxKeyFilterHook postkf;
    postkf.arg = atEscState;
    postkf.func = IMSelectorAtEscPostFilter;

    FcitxInstanceRegisterHotkeyFilter(instance, thk);
    FcitxInstanceRegisterPreInputFilter(instance, prekf);
    FcitxInstanceRegisterPostInputFilter(instance, postkf);
    // FcitxUIRegisterStatus(instance, atEscState, "@esc", _("@esc"), _("@esc"), ToggleatEscState, GetAtEscEnabled);

    return atEscState;
}

void ToggleatEscState(void* arg)
{
    FcitxAtEsc* atEscState = (FcitxAtEsc*) arg;
    atEscState->enabled = !atEscState->enabled;
    SaveAtEscConfig(atEscState);
}

boolean GetAtEscEnabled(void* arg)
{
    FcitxAtEsc* atEscState = (FcitxAtEsc*) arg;
    return atEscState->enabled;
}

boolean LoadAtEscConfig(FcitxAtEsc* atEscState)
{
    FcitxConfigFileDesc* configDesc = GetAtEscConfigDesc();
    if (configDesc == NULL)
        return false;

    FILE *fp;
    char *file;
    fp = FcitxXDGGetFileUserWithPrefix("conf", "fcitx-at-esc.config", "r", &file);
    FcitxLog(DEBUG, "Load Config File %s", file);
    free(file);
    if (!fp) {
        if (errno == ENOENT)
            SaveAtEscConfig(atEscState);
    }

    FcitxConfigFile *cfile = FcitxConfigParseConfigFileFp(fp, configDesc);

    FcitxAtEscConfigBind(atEscState, cfile, configDesc);
    FcitxConfigBindSync((FcitxGenericConfig*)atEscState);

    if (fp)
        fclose(fp);

    return true;
}

CONFIG_DESC_DEFINE(GetAtEscConfigDesc, "fcitx-at-esc.desc")

void SaveAtEscConfig(FcitxAtEsc* atEscState)
{
    FcitxConfigFileDesc* configDesc = GetAtEscConfigDesc();
    char *file;
    FILE *fp = FcitxXDGGetFileUserWithPrefix("conf", "fcitx-at-esc.config", "w", &file);
    FcitxLog(DEBUG, "Save Config to %s", file);
    FcitxConfigSaveConfigFileFp(fp, &atEscState->gconfig, configDesc);
    free(file);
    if (fp)
        fclose(fp);
}

void ReloadAtEsc(void* arg)
{
    FcitxAtEsc* atEscState = (FcitxAtEsc*) arg;
    LoadAtEscConfig(atEscState);
}

INPUT_RETURN_VALUE HotkeyToggleatEscState(void* arg)
{
  // FcitxAtEsc* atEscState = (FcitxAtEsc*) arg;

  // FcitxUIStatus *status = FcitxUIGetStatusByName(atEscState->owner, "@esc");
  // if (status->visible){
  //     FcitxUIUpdateStatus(atEscState->owner, "@esc");
  //     return IRV_DO_NOTHING;
  // }
  // else
  //     return IRV_TO_PROCESS;
  ToggleatEscState(arg);
  return IRV_DO_NOTHING;
}

boolean HandleAtEscFilter(void* arg, FcitxKeySym sym, unsigned int state, INPUT_RETURN_VALUE* retval)
{
    boolean handled = true;
    FcitxAtEsc* atEscState = (FcitxAtEsc*) arg;

    if(atEscState->enabled){
      FcitxLog(DEBUG, "hooked esc in pre filter");
      if(sym == FcitxKey_Escape){
        if(atEscState->commitPreedit){
          if(AtEscUtilCurrentImIsSkk(atEscState->owner)){
            const FcitxSkk *skk =
              AtEscUtilGetSkkInstanceFromCurrentIM(atEscState->owner);
            if(AtEscUtilIsSkkConvertMode(atEscState->owner,
                  FcitxInstanceGetInputState(atEscState->owner))){
              AtEscUtilSkkCommitForce(atEscState->owner, skk, retval);
            }
          }else{
            FcitxUICommitPreedit(atEscState->owner);
          }
        }
        FcitxInstanceCloseIM(atEscState->owner,
            FcitxInstanceGetCurrentIC(atEscState->owner));
      }
    }
    return handled;
}

boolean IMSelectorAtEscPreFilter(void* arg, FcitxKeySym sym, unsigned int state, INPUT_RETURN_VALUE* retval)
{
    boolean handled = true;
    FcitxAtEsc* atEscState = (FcitxAtEsc*) arg;

    if(atEscState->forceDisableIME){
      handled = HandleAtEscFilter(arg, sym, state, retval);
    }
    return handled;
}

boolean IMSelectorAtEscPostFilter(void* arg, FcitxKeySym sym, unsigned int state, INPUT_RETURN_VALUE* retval)
{
    boolean handled = true;
    FcitxAtEsc* atEscState = (FcitxAtEsc*) arg;

    if(!atEscState->forceDisableIME){
      handled = HandleAtEscFilter(arg, sym, state, retval);
    }
    return handled;
}
