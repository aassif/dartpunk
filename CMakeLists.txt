cmake_minimum_required (VERSION 3.16.3)

set (CMAKE_CXX_STANDARD 17)

project (dartpunk)

find_package (PNG REQUIRED)
find_package (Threads REQUIRED)
find_package (PkgConfig REQUIRED)

add_executable (dartpunk
  app.cpp
  font.cpp
  image.cpp
  bluetooth.cpp
  menu/menu.cpp
  menu/players.cpp
  menu/x01.cpp
  settings.cpp
  game/game.cpp
  game/count-up.cpp
  game/x01.cpp
  game/cricket.cpp
  board/event.cpp
  board/board.cpp
  main.cpp
)

target_link_libraries (dartpunk PUBLIC Threads::Threads PNG::PNG)

if (BOARD MATCHES ed900)
  target_compile_definitions (dartpunk PUBLIC DARTPUNK_BOARD_ED900)
  target_sources (dartpunk PUBLIC board/ble.cpp board/ed900.cpp)
  set (DARTPUNK_SYSTEMD true)
elseif (BOARD MATCHES dartslive)
  target_compile_definitions (dartpunk PUBLIC DARTPUNK_BOARD_DARTSLIVE)
  target_sources (dartpunk PUBLIC board/ble.cpp board/dartslive.cpp)
  set (DARTPUNK_SYSTEMD true)
elseif (BOARD MATCHES virtual)
  target_compile_definitions (dartpunk PUBLIC DARTPUNK_BOARD_VIRTUAL)
  target_sources (dartpunk PUBLIC board/virtual.cpp)
  set (DARTPUNK_SDL2 true)
else ()
  message (FATAL_ERROR "BOARD")
endif ()

if (INPUT MATCHES joystick)
  target_compile_definitions (dartpunk PUBLIC DARTPUNK_INPUT_JOYSTICK)
  target_sources (dartpunk PUBLIC input/device.cpp)
  set (DARTPUNK_EVDEV true)
elseif (INPUT MATCHES keyboard)
  target_compile_definitions (dartpunk PUBLIC DARTPUNK_INPUT_KEYBOARD)
  target_sources (dartpunk PUBLIC input/device.cpp)
  set (DARTPUNK_EVDEV true)
else ()
  message (FATAL_ERROR "INPUT")
endif ()

if (DISPLAY MATCHES matrix)
  target_compile_definitions (dartpunk PUBLIC DARTPUNK_DISPLAY_MATRIX)
  target_sources (dartpunk PUBLIC display/matrix.cpp)
  set (RGB_MATRIX_DIR ../rpi-rgb-led-matrix)
  set (RGB_MATRIX_INCLUDE_DIRS ${RGB_MATRIX_DIR}/include)
  set (RGB_MATRIX_LIBRARIES -L${RGB_MATRIX_DIR}/lib -lrgbmatrix)
  target_include_directories (dartpunk PUBLIC ${RGB_MATRIX_INCLUDE_DIRS})
  target_link_libraries (dartpunk PUBLIC ${RGB_MATRIX_LIBRARIES})
elseif (DISPLAY MATCHES virtual)
  target_compile_definitions (dartpunk PUBLIC DARTPUNK_DISPLAY_VIRTUAL)
  target_sources (dartpunk PUBLIC display/virtual.cpp)
  set (DARTPUNK_SDL2 true)
else ()
  message (FATAL_ERROR "DISPLAY")
endif ()

if (DARTPUNK_SYSTEMD)
  pkg_check_modules (SYSTEMD REQUIRED libsystemd)
  target_include_directories (dartpunk PUBLIC ${SYSTEMD_INCLUDE_DIRS})
  target_link_libraries (dartpunk PUBLIC ${SYSTEMD_LIBRARIES})
endif ()

if (DARTPUNK_EVDEV)
  pkg_check_modules (EVDEV REQUIRED libevdev)
  target_include_directories (dartpunk PUBLIC ${EVDEV_INCLUDE_DIRS})
  target_link_libraries (dartpunk PUBLIC ${EVDEV_LIBRARIES})
endif ()

if (DARTPUNK_SDL2)
  pkg_check_modules (SDL2 REQUIRED sdl2)
  target_compile_definitions (dartpunk PUBLIC DARTPUNK_SDL2)
  target_include_directories (dartpunk PUBLIC ${SDL2_INCLUDE_DIRS})
  target_link_libraries (dartpunk PUBLIC ${SDL2_LIBRARIES})
endif ()

