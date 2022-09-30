if (NOT MSVC)

include(GNUInstallDirs)

  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/man1/cmark-gfm.1
    DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)

  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/man3/cmark-gfm.3
    DESTINATION ${CMAKE_INSTALL_MANDIR}/man3)
endif(NOT MSVC)
