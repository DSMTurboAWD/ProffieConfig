# ProffieConfig, All-In-One Proffieboard Management Utility!
#
# components/ui/CMakeLists.txt

add_library(ui SHARED
    frame.cpp
    dialog.cpp
    build.cpp
    info.cpp
    text.cpp

    priv/helpers.cpp
    priv/winbase.cpp

    detail/descriptor.cpp
    detail/scaffold.cpp

    dialogs/message.cpp
    dialogs/progress.cpp

    static/divider.cpp
    static/image.cpp
    static/label.cpp
    static/hyperlink.cpp

    indication/progress.cpp

    helpers/dialog_buttons.cpp

    controls/button.cpp
    controls/checkbox.cpp
    controls/choice.cpp
    controls/radios.cpp
    controls/stepper.cpp
    controls/text.cpp
    controls/filepicker.cpp

    layout/collapsible.cpp
    layout/group.cpp
    layout/scrolled.cpp
    layout/selector.cpp
    layout/spacer.cpp
    layout/stack.cpp
    layout/panel.cpp
    layout/priv/groupbox.cpp
)

target_link_libraries(ui 
    wxWidgets
    app
    data
)

include(../Component.cmake)
setup_component_and_static(ui 1.0.12)

if (CMAKE_SYSTEM_NAME STREQUAL Windows) 
    target_link_libraries(ui dwmapi)
    target_link_libraries(ui-static dwmapi)
endif()
