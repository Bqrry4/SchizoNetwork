cmake_minimum_required(VERSION 3.27)
project(SI_Proiect C)

set(CMAKE_C_STANDARD 23)

include_directories(src/algorithms/RC6)

add_executable(SI_Proiect
        src/algorithms/RC6/RC6.c
        src/algorithms/RC6/RC6.h
        src/algorithms/RSA/RSA.c
        src/algorithms/RSA/RSA.h
        src/main.c
        src/communication/client/client.c
        src/communication/client/client.h
        src/communication/server/server.c
        src/communication/server/server.h
        src/communication/communication_commons.h
        src/communication/handshake/handshake.c
        src/communication/handshake/handshake.h
        src/communication/communication_commons.c
        src/cli/cli.c
        src/commons/app_defines.h)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GMP REQUIRED IMPORTED_TARGET gmp)
target_link_libraries(SI_Proiect PkgConfig::GMP)