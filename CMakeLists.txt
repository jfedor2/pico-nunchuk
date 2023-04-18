cmake_minimum_required(VERSION 3.13)

add_compile_definitions(PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64)

set(PICO_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}/pico-sdk")
set(PICO_TINYUSB_PATH "${CMAKE_CURRENT_LIST_DIR}/tinyusb")

include(pico_sdk_import.cmake)

project(nunchuk)

pico_sdk_init()

add_compile_options(-Wall)

add_executable(nunchuk_mouse src/nunchuk.c src/tinyusb_stuff.c)
target_include_directories(nunchuk_mouse PRIVATE src)
target_link_libraries(nunchuk_mouse pico_stdlib hardware_i2c tinyusb_device tinyusb_board)
target_compile_definitions(nunchuk_mouse PUBLIC MOUSE)
pico_add_extra_outputs(nunchuk_mouse)

add_executable(nunchuk_gamepad src/nunchuk.c src/tinyusb_stuff.c)
target_include_directories(nunchuk_gamepad PRIVATE src)
target_link_libraries(nunchuk_gamepad pico_stdlib hardware_i2c tinyusb_device tinyusb_board)
target_compile_definitions(nunchuk_gamepad PUBLIC GAMEPAD)
pico_add_extra_outputs(nunchuk_gamepad)
