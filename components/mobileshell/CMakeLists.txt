set(mobileshellplugin_SRCS
    mobileshellplugin.cpp
    shellutil.cpp
)

add_library(mobileshellplugin ${mobileshellplugin_SRCS})

target_link_libraries(mobilehomescreencomponentsplugin
        PUBLIC
            Qt::Core
        PRIVATE
            Qt::DBus
            Qt::Qml
            Qt::Gui
            Qt::Quick
            KF5::KIOGui
            KF5::Plasma
            KF5::I18n
            KF5::Notifications
            KF5::PlasmaQuick
            KF5::WaylandClient
            KF5::Service
    )

install(TARGETS mobileshellplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/private/mobileshell)

install(DIRECTORY qml/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/private/mobileshell)
