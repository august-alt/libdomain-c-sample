cmake_minimum_required(VERSION 3.5)

project(libdomain-c-sample LANGUAGES C)

if(UNIX)
  include(GNUInstallDirs)
else()
    message(FATAL_ERROR "System not UNIX - not implemented yet")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(FindLdap)
include(FindLibdomain)

find_package(Ldap REQUIRED)
find_package(Libdomain REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Talloc REQUIRED IMPORTED_TARGET talloc)
pkg_check_modules(Libverto REQUIRED IMPORTED_TARGET libverto)
pkg_check_modules(Libconfig REQUIRED IMPORTED_TARGET libconfig)

add_executable(libdomain-c-sample main.c)

target_link_libraries(libdomain-c-sample domain)

target_link_libraries(libdomain-c-sample Ldap::Ldap)
target_link_libraries(libdomain-c-sample Libdomain::Libdomain)
target_link_libraries(libdomain-c-sample PkgConfig::Libverto)
target_link_libraries(libdomain-c-sample PkgConfig::Libconfig)
target_link_libraries(libdomain-c-sample PkgConfig::Talloc)

install(TARGETS libdomain-c-sample DESTINATION ${CMAKE_INSTALL_BINDIR})
