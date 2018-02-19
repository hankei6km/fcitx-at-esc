/***************************************************************************
 *
 * fcitx-at-esc
 *
 * Copyright (c) 2018 hankei6km
 * Licensed under the MIT License. See LICENSE.txt in the project root.
 *
 *
 ***************************************************************************/

#ifndef _FCITX_MODULE_AT_ESC_UTIL_H_
#define _FCITX_MODULE_AT_ESC_UTIL_H_

#include <libintl.h>
#include <libskk/libskk.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SKK_UNIQUE_NAME "skk"

#define SKK_MARKER_WHITE "▽"
#define SKK_MARKER_BLACK "▼"

// It's not full implementatinon.
typedef struct {
  FcitxInstance *owner;
  SkkContext *context;
} FcitxSkk;


/**
 * Get preedit string.
 *
 * @param instance fcitx instance.
 * @param input	input state.
 * @param trim	trim maker char at top of preedint string.
 * @return string.
 */
const char* AtEscUtilGetSkkPreeditString(struct _FcitxInstance * instance, FcitxInputState *input, boolean trim);

/**
 * Get state whether that mode(skk) is preediting with "▽"/"▼".
 *
 * @param instance fcitx instance.
 * @param input	input state.
 * @return state.
 */
boolean AtEscUtilIsSkkConvertMode(struct _FcitxInstance * instance, FcitxInputState *input);

/**
 * Get instance of fcitx-skk.
 *
 * @param instance fcitx instance.
 * @return instance.
 */
const FcitxSkk* AtEscUtilGetSkkInstanceFromCurrentIM(struct _FcitxInstance * instance);

boolean AtEscUtilCurrentImIsSkk(struct _FcitxInstance * instance);

void AtEscUtilSkkProcessEvent(struct _FcitxInstance * instance, const FcitxSkk *skk, FcitxKeySym sym, unsigned int state, INPUT_RETURN_VALUE *retval);

void AtEscUtilSkkCommitForce(struct _FcitxInstance * instance, const FcitxSkk *skk, INPUT_RETURN_VALUE *retval);
void AtEscUtilSkkCommitKatakana(struct _FcitxInstance * instance, const FcitxSkk *skk, INPUT_RETURN_VALUE *retval);

#ifdef __cplusplus
}
#endif

#endif
