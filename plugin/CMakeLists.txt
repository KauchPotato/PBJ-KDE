add_definitions(-DTRANSLATION_DOMAIN=\"plasma_wallpaper_org.kde.plasma.pbj\")

add_library(pbjplugin SHARED pbjplugin.cpp)

target_link_libraries(pbjplugin
    KF6::I18n
    Qt6::Gui
    Qt6::Qml
)

# Install plugin inside the wallpaper package
set(WALLPAPER_PLUGIN_DIR "${KDE_INSTALL_DATADIR}/plasma/wallpapers/org.kde.plasma.pbj/contents/ui/Plugin")
install(TARGETS pbjplugin DESTINATION ${WALLPAPER_PLUGIN_DIR})
install(FILES qmldir DESTINATION ${WALLPAPER_PLUGIN_DIR})
