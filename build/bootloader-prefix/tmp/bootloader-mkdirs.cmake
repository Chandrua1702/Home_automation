# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/chandrasekar/esp/esp-idf/components/bootloader/subproject"
  "/home/chandrasekar/Project/home_automation/build/bootloader"
  "/home/chandrasekar/Project/home_automation/build/bootloader-prefix"
  "/home/chandrasekar/Project/home_automation/build/bootloader-prefix/tmp"
  "/home/chandrasekar/Project/home_automation/build/bootloader-prefix/src/bootloader-stamp"
  "/home/chandrasekar/Project/home_automation/build/bootloader-prefix/src"
  "/home/chandrasekar/Project/home_automation/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/chandrasekar/Project/home_automation/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/chandrasekar/Project/home_automation/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
