find_package(Gettext REQUIRED)

file(GLOB _poFileList *.po)

foreach(_poFile ${_poFileList})
    get_filename_component(_poFileName ${_poFile} NAME)
    string(REGEX REPLACE "\\.po$" "" _lang ${_poFileName})

    if(_lang)
        gettext_process_po_files(${_lang} ALL PO_FILES ${_poFile})
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo
            DESTINATION ${LOCALE_INSTALL_DIR}/${_lang}/LC_MESSAGES
            RENAME kfilebox.mo)
    endif()
endforeach()
