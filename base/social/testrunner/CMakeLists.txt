set(CMAKE_AUTOMOC ON)

########### Source files ###############
SET(sourceFiles
    main.cpp
    socialtestrunner.cpp
    )

IF(WIN32)
    SET(GUI_TYPE WIN32)
    ADD_DEFINITIONS(-DNODEFAULTLIB:library)
ENDIF(WIN32)

SET(libs ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY}
    ${QT_QTOPENGL_LIBRARY}
    ${QT_QTSVG_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
    ${QT_QTSQL_LIBRARY}
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${PLEXY_UI_CORE_LIBRARY}
    ${QT_QTXMLPATTERNS_LIBRARY}
    ${Qt5X11Extras_LIBRARIES}
    )


########### prepare files ###############
ADD_EXECUTABLE(social_runtime ${WIN_APPLICATION} ${sourceFiles})
SET_PROPERTY(TARGET social_runtime PROPERTY QT5_NO_LINK_QTMAIN ON)
QT5_USE_MODULES(social_runtime Widgets Xml OpenGL Core)

IF(MSVC)
    MSVC_SET_PROPER_CONSOLE_HANDLING(social_runtime)
ENDIF(MSVC)

TARGET_LINK_LIBRARIES(social_runtime
    ${PLEXY_UI_CORE_LIBRARY}
    ${QUETZAL_SOCIAL_KIT_LIBRARY}
    ${libs}
    )

 #DBUS_ADD_ACTIVATION_SERVICE(org.plexydesk.social.service.in)

########### Install files ###############
INSTALL(TARGETS social_runtime DESTINATION bin)
