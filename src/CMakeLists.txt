add_executable(drmresolution main.cpp drmresolutionwindow.cpp logind.cpp udev.cpp)

target_link_libraries(drmresolution
    Qt5::Gui Qt5::DBus udev drm
)

find_path(DRM_INCLUDES drm.h PATH_SUFFIXES drm)
target_include_directories(drmresolution PRIVATE ${DRM_INCLUDES})

install( TARGETS drmresolution ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )
install( PROGRAMS org.example.drmresolution.desktop DESTINATION ${KDE_INSTALL_APPDIR} )
install( FILES org.example.drmresolution.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR} )
