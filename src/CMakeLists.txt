include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${LIBSKK_INCLUDE_DIRS})
link_directories(${LIBSKK_LIBRARY_DIRS})

set(FCITX_AT_ESC_SOURCES
  at-esc.c
  at-esc-util.c
  )
fcitx_add_addon_full(at-esc DESC
  HEADERS at-esc.h at-esc-util.h
  SOURCES ${FCITX_AT_ESC_SOURCES}
  LINK_LIBS ${LIBSKK_LIBRARIES})
