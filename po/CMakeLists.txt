set(PO_LANGS ja)
foreach(lang ${PO_LANGS})
  fcitx_translate_add_po_file("${lang}" "${lang}.po")
endforeach()
fcitx_translate_set_pot_target(pot fcitx-at-esc fcitx-at-esc.pot)
