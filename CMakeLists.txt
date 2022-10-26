cmake_minimum_required (VERSION 3.16.3)

set (CMAKE_CXX_STANDARD 17)

project (rpi-ed900)

find_package (PNG REQUIRED)
find_package (Threads REQUIRED)

find_package (PkgConfig REQUIRED)
pkg_check_modules (SYSTEMD REQUIRED libsystemd)
pkg_check_modules (EVDEV REQUIRED libevdev)

add_executable (rpi-ed900
  app.cpp
  font.cpp
  image.cpp
  input.cpp
  ed900.cpp
  bluetooth.cpp
  menu/menu.cpp
  menu/players.cpp
  menu/x01.cpp
  settings.cpp
  game/game.cpp
  game/count-up.cpp
  game/x01.cpp
  game/cricket.cpp
  main.cpp
)

target_include_directories (rpi-ed900 PUBLIC
  ${SYSTEMD_INCLUDE_DIRS}
  ${EVDEV_INCLUDE_DIRS}
)

target_link_libraries (rpi-ed900 PUBLIC
  Threads::Threads
  PNG::PNG
  ${SYSTEMD_LIBRARIES}
  ${EVDEV_LIBRARIES}
)

if (RGB_MATRIX)
  set (RGB_MATRIX_DIR ../rpi-rgb-led-matrix)
  target_compile_definitions (rpi-ed900 PUBLIC ED900_RGB_MATRIX)

  set (RGB_MATRIX_INCLUDE_DIRS ${RGB_MATRIX_DIR}/include)
  target_include_directories (rpi-ed900 PUBLIC ${RGB_MATRIX_INCLUDE_DIRS})

  set (RGB_MATRIX_LIBRARIES -L${RGB_MATRIX_DIR}/lib -lrgbmatrix)
  target_link_libraries (rpi-ed900 PUBLIC ${RGB_MATRIX_LIBRARIES})
else ()
  pkg_check_modules (SDL2 REQUIRED sdl2)
  target_include_directories (rpi-ed900 PUBLIC ${SDL2_INCLUDE_DIRS})
  target_link_libraries (rpi-ed900 PUBLIC ${SDL2_LIBRARIES})
endif ()

