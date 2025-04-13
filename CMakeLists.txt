cmake_minimum_required(VERSION 3.16)
project(WebBrowser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

# Enable Qt MOC and UI auto-processing
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets WebEngineWidgets WebChannel)
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

# Source files
add_executable(WebBrowser
    src/main.cpp
    src/Browser.cpp
    src/NetworkManager.c
    src/PrivacyManager.cpp
    src/MainWindow.cpp
)

# Include directories
target_include_directories(WebBrowser PRIVATE
    ${CURL_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(WebBrowser PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::WebChannel
    CURL::libcurl
    OpenSSL::SSL
    OpenSSL::Crypto
)
