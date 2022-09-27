cmake_minimum_required (VERSION 3.16.3)

set (CMAKE_CXX_STANDARD 17)

project (rpi-ed900)

find_package (Threads REQUIRED)

find_package (PkgConfig REQUIRED)
pkg_check_modules (SYSTEMD REQUIRED libsystemd)
pkg_check_modules (SDL2 REQUIRED sdl2)
pkg_check_modules (SDL2_IMAGE REQUIRED SDL2_image)

set (RGB_MATRIX 0)

add_executable (rpi-ed900
  ed900.cpp
  app.cpp
  bluetooth.cpp
  menu/menu.cpp
  menu/players.cpp
  menu/x01.cpp
  settings.cpp
  game/game.cpp
  game/count-up.cpp
  game/x01.cpp
  game/cricket.cpp
  font.cpp
  main.cpp
)

target_include_directories (rpi-ed900 PUBLIC
  ${SYSTEMD_INCLUDE_DIRS}
  ${SDL2_INCLUDE_DIRS}
  ${SDL2_IMAGE_INCLUDE_DIRS}
)

target_link_libraries (rpi-ed900 PUBLIC ${SYSTEMD_LIBRARIES} ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} Threads::Threads)

if (RGB_MATRIX)
  set (RGB_MATRIX_DIR ../rpi-rgb-led-matrix)
  target_compile_definitions (rpi-ed900 PUBLIC ED900_RGB_MATRIX)

  set (RGB_MATRIX_INCLUDE_DIRS ${RGB_MATRIX_DIR}/include)
  target_include_directories (rpi-ed900 PUBLIC ${RGB_MATRIX_INCLUDE_DIRS})

  set (RGB_MATRIX_LIBRARIES -L${RGB_MATRIX_DIR}/lib -lrgbmatrix)
  target_link_libraries (rpi-ed900 PUBLIC ${RGB_MATRIX_LIBRARIES})
endif ()

