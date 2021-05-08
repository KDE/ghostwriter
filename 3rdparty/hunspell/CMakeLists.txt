set(hunspell_SOURCES
  affentry.cxx
  affixmgr.cxx
  csutil.cxx
  filemgr.cxx
  hashmgr.cxx
  hunspell.cxx
  hunzip.cxx
  phonet.cxx
  replist.cxx
  suggestmgr.cxx
)

if(NOT TARGET hunspell)
  add_library(hunspell
    ${hunspell_SOURCES}
  )
endif()
target_include_directories(hunspell PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_definitions(hunspell PUBLIC
  HUNSPELL_STATIC
)

target_link_libraries(ghostwriter PRIVATE hunspell)