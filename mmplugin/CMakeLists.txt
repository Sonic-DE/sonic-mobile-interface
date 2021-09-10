add_library(ppc-mmqmlplugin)
target_sources(ppc-mmqmlplugin PRIVATE
    mmqmlplugin.cpp
    signalindicator.cpp
)

target_link_libraries(ppc-mmqmlplugin
    Qt5::Qml
    KF5::ModemManagerQt
)

set_property(TARGET ppc-mmqmlplugin PROPERTY LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/org/kde/plasma)
file(COPY qmldir DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/org/kde/plasma)

install(TARGETS ppc-mmqmlplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma)
install(FILES qmldir ${qml_SRC} DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma)
