FILE(GLOB PNGFILES *.png )

IF (NOT APPLE)
INSTALL(FILES ${PNGFILES}  DESTINATION share/plexy/skins/default/clock)
ENDIF (NOT APPLE)
