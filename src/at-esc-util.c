/***************************************************************************
 *
 * fcitx-at-esc
 *
 * Copyright (c) 2018 hankei6km
 * Licensed under the MIT License. See LICENSE.txt in the project root.
 *
 *
 ***************************************************************************/

#include "fcitx/fcitx.h"

#include <libintl.h>
#include <errno.h>

#include "fcitx/module.h"
#include "fcitx-utils/utf8.h"
#include "fcitx-utils/uthash.h"
#include "fcitx-config/xdg.h"
#include "fcitx/hook.h"
#include "fcitx/ui.h"
#include "fcitx-utils/log.h"
#include "fcitx/instance.h"
#include "fcitx/context.h"
#include "fcitx-utils/utils.h"
#include <libskk/libskk.h>

#include "at-esc-util.h"

const char* AtEscUtilGetSkkPreeditString(struct _FcitxInstance * instance,
    FcitxInputState *input, boolean trim)
{
  char *ret = NULL;

  FcitxMessages *pem = FcitxInputStateGetPreedit(input);
  int cnt = FcitxMessagesGetMessageCount(pem);
  if(cnt > 0){
    ret = FcitxMessagesGetMessageString(pem, 0);
  }else{
    FcitxMessages *cpem = FcitxInputStateGetClientPreedit(input);
    int ccnt = FcitxMessagesGetMessageCount(cpem);
    if(ccnt > 0){
      ret = FcitxMessagesGetMessageString(cpem, 0);
    }
  }
  if(trim && ret != NULL){
    if (fcitx_utf8_strncmp(ret, SKK_MARKER_WHITE, 1) == 0){
      ret = fcitx_utf8_get_nth_char(ret, fcitx_utf8_strlen(SKK_MARKER_WHITE));
    }else if (fcitx_utf8_strncmp(ret, SKK_MARKER_BLACK, 1) == 0){
      ret = fcitx_utf8_get_nth_char(ret, fcitx_utf8_strlen(SKK_MARKER_BLACK));
    }else{
    }
  }
  return ret;
}

boolean AtEscUtilIsSkkConvertMode(struct _FcitxInstance * instance,
    FcitxInputState *input)
{
  boolean ret = false;

  const char *pestr = AtEscUtilGetSkkPreeditString(instance, input, false);
  if(pestr != NULL && 
      (fcitx_utf8_strncmp(pestr, SKK_MARKER_WHITE, fcitx_utf8_strlen(SKK_MARKER_WHITE)) == 0 ||
       fcitx_utf8_strncmp(pestr, SKK_MARKER_BLACK, fcitx_utf8_strlen(SKK_MARKER_BLACK)) == 0)){
    // free(pestr);
    ret = true;
  }

  return ret;
}

const FcitxSkk* AtEscUtilGetSkkInstanceFromCurrentIM(struct _FcitxInstance * instance)
{
  FcitxSkk *ret = NULL;

  FcitxIM *im = FcitxInstanceGetCurrentIM(instance);
  if(strcmp(im->uniqueName, SKK_UNIQUE_NAME) == 0){
    ret = (FcitxSkk*)(im->owner->addonInstance);
  }

  return ret;
}

boolean AtEscUtilCurrentImIsSkk(struct _FcitxInstance * instance)
{
  FcitxIM *im = FcitxInstanceGetCurrentIM(instance);
  return (strcmp(im->uniqueName, SKK_UNIQUE_NAME) ==0);
}

void AtEscUtilSkkProcessEvent(struct _FcitxInstance * instance, const FcitxSkk *skk, FcitxKeySym sym, unsigned int state, INPUT_RETURN_VALUE *retval)
{
  SkkModifierType modifiers = (SkkModifierType) state & (FcitxKeyState_SimpleMask | (1 << 30));
  SkkKeyEvent* key = skk_key_event_new_from_x_keysym(sym, modifiers, NULL);
  if (!key){
    *retval = IRV_TO_PROCESS;
  }

  gboolean ret = skk_context_process_key_event(skk->context, key);
  gchar* output = skk_context_poll_output(skk->context);

  g_object_unref(key);

  if (output && strlen(output) > 0) {
    FcitxInstanceCommitString(instance, FcitxInstanceGetCurrentIC(instance), output);
    FcitxLog(DEBUG, "output %s", output);
  }

  g_free(output);
  //
  //*retval = ret ? (skk->updatePreedit || skk->update_candidate ?  IRV_DISPLAY_CANDWORDS : IRV_DO_NOTHING) : IRV_TO_PROCESS;
};

void AtEscUtilSkkCommitForce(struct _FcitxInstance * instance, const FcitxSkk *skk, INPUT_RETURN_VALUE *retval)
{
  AtEscUtilSkkProcessEvent(instance, skk, FcitxKey_j, FcitxKeyState_Ctrl, retval);
}

void AtEscUtilSkkCommitKatakana(struct _FcitxInstance * instance, const FcitxSkk *skk, INPUT_RETURN_VALUE *retval)
{
  AtEscUtilSkkProcessEvent(instance, skk, FcitxKey_q, FcitxKeyState_None, retval);
}
