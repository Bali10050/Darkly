add_library(darklycommon${QT_MAJOR_VERSION} OBJECT darklyboxshadowrenderer.cpp)

set_target_properties(darklycommon${QT_MAJOR_VERSION} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

target_link_libraries(darklycommon${QT_MAJOR_VERSION}
    PUBLIC
        Qt${QT_MAJOR_VERSION}::Core
        Qt${QT_MAJOR_VERSION}::Gui)
