ADD_SUBDIRECTORY(youtube)

SET(sourceFiles
    videoentity.cpp
    youtube.cpp
    youtubeinterface.cpp
    youtubewidget.cpp
    )

SET(headerFiles
    videoentity.h
    youtube.h
    youtubeinterface.h
    youtubewidget.h
    config.h
    )

SET(QTMOC_SRCS
    youtube.h
    youtubeinterface.h
    youtubewidget.h
    )

QT5_WRAP_CPP(QT_MOC_SRCS ${QTMOC_SRCS})

SET(sourceFiles
    ${sourceFiles}
    ${headerFiles}
    )

ADD_LIBRARY(plexyyoutube SHARED ${sourceFiles} ${QT_MOC_SRCS})

IF(MINGW)
    SET_TARGET_PROPERTIES(plexyyoutube PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
        )
ENDIF(MINGW)

TARGET_LINK_LIBRARIES(plexyyoutube
    ${PLEXY_UI_CORE_LIBRARY}
    )

QT5_USE_MODULES(plexyyoutube Widgets Xml OpenGL Core WebKitWidgets Svg Declarative)

INSTALL(TARGETS plexyyoutube DESTINATION ${CMAKE_INSTALL_LIBDIR}/plexyext)
INSTALL(FILES youtube.desktop DESTINATION share/plexy/ext/groups)
