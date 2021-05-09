find_package(Qt5 COMPONENTS Widgets LinguistTools)

set(ghostwriter_TRANSLATIONS
  ghostwriter_ar.ts
  ghostwriter_cs.ts
  ghostwriter_de.ts
  ghostwriter_en.ts
  ghostwriter_es.ts
  ghostwriter_fr.ts
  ghostwriter_it.ts
  ghostwriter_ja.ts
  ghostwriter_nb_NO.ts
  ghostwriter_nl.ts
  ghostwriter_pl.ts
  ghostwriter_pt_BR.ts
  ghostwriter_ru.ts
  ghostwriter_sv.ts
  ghostwriter_zh.ts
)

qt5_add_translation(QM_FILES
  ${ghostwriter_TRANSLATIONS}
)
add_custom_target(add_translations ALL DEPENDS ${QM_FILES})

qt5_create_translation(QM_FILE_UPDATES
  ${CMAKE_SOURCE_DIR}/src
  ${ghostwriter_TRANSLATIONS}
)
add_custom_target(update_translations DEPENDS ${QM_FILE_UPDATES})

install(FILES ${QM_FILES}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/ghostwriter/translations
)
