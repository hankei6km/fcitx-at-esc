include_directories(${PROJECT_SOURCE_DIR}/src)
# include_directories(${LIBSKK_INCLUDE_DIRS})
# link_directories(${LIBSKK_LIBRARY_DIRS})

set(FCITX_AT_ESC_SOURCES
  at-esc.c
  )
fcitx_add_addon_full(at-esc DESC
  HEADERS at-esc.h
  SOURCES ${FCITX_AT_ESC_SOURCES})
