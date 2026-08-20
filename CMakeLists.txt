cmake_minimum_required(VERSION 3.16)

project(android_prime VERSION 0.1 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt" ${CMAKE_PREFIX_PATH})
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt;/opt/homebrew" ${CMAKE_PREFIX_PATH})
set(WITH_POSTGRESQL ON CACHE INTERNAL "")

find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(Boost REQUIRED json log thread)
find_package(OpenSSL REQUIRED)
find_package(Qt6Keychain REQUIRED)
find_package(libsodium REQUIRED)
find_package(Protobuf REQUIRED)
find_package(libpqxx REQUIRED)
qt_standard_project_setup(REQUIRES 6.10)

# Создаем пустую объектную библиотеку и сразу добавляем в нее исходный .proto файл
add_library(proto_objects OBJECT message.proto)

# Связываем с protobuf от Conan
target_link_libraries(proto_objects PUBLIC protobuf::libprotobuf)

# Вызываем генерацию кода прямо для этого таргета
protobuf_generate(TARGET proto_objects LANGUAGE cpp)

# Настраиваем директорию генерации, чтобы компилятор нашел файлы
target_include_directories(proto_objects PUBLIC ${CMAKE_CURRENT_BINARY_DIR})


# Content of downloaded design from Figma to Qt or similar
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

qt_add_executable(appandroid_prime
    main.cpp
)

qt_add_qml_module(appandroid_prime
    URI android_prime
        QML_FILES
          Main.qml
          RegisterScreen.qml
          LoginScreen.qml
          ChatsScreen.qml
        SOURCES
          client.h
          client.cpp
          websocket.h
          websocket.cpp
          $<TARGET_OBJECTS:proto_objects>
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appandroid_prime PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appandroid_prime
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appandroid_prime
    PRIVATE Qt6::Quick Boost::boost
            Boost::json Boost::headers OpenSSL::SSL OpenSSL::Crypto
            Qt6Keychain::Qt6Keychain
            protobuf::libprotobuf
)

install(TARGETS appandroid_prime
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
