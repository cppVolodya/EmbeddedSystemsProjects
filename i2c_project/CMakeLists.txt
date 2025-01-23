set(CMAKE_SYSTEM_NAME STM32F401RE_CORTEX_M4)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_SYSTEM_VERSION 1)

cmake_minimum_required(VERSION 3.20)
project(i2c_project C CXX)

enable_language(C ASM)

#set(CMAKE_C_COMPILER   D:/data/direction_in_my_life/information_technology/c_embedded/studying/tools/arm-mingw-13.3/bin/arm-none-eabi-gcc.exe)
#set(CMAKE_CXX_COMPILER D:/data/direction_in_my_life/information_technology/c_embedded/studying/tools/arm-mingw-13.3/bi/arm-none-eabi-g++.exe)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

set(CMAKE_SYSTEM_PROCESSOR cortex-m4)
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

set(LINKER_SCRIPT_NAME STM32F103C8TX_FLASH)
set(STARTUP_FILE_NAME  startup_stm32f103c8tx)

set(LINKER_SCRIPT    "${CMAKE_SOURCE_DIR}/libraries/${LINKER_SCRIPT_NAME}.ld")
set(STARTUP_LOCATION "${CMAKE_SOURCE_DIR}/libraries/${STARTUP_FILE_NAME}.s"  )

if(NOT CMAKE_BUILD_TYPE)
    message(FATAL_ERROR "CMAKE_BUILD_TYPE is not set. Please specify one of: Debug, Release.")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(COMPILER_FLAGS "-DDEBUG -g3 -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(COMPILER_FLAGS "-O3 -Os")
else()
    message(FATAL_ERROR "Unsupported build type: ${CMAKE_BUILD_TYPE}. Please choose either Debug or Release.")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=${CMAKE_SYSTEM_PROCESSOR} \
                                        ${COMPILER_FLAGS} -Wall \
                                        -std=gnu++${CMAKE_CXX_STANDARD} \
                                        -ffunction-sections -fdata-sections -fno-strict-aliasing -fstack-usage \
                                        -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit \
                                        --specs=nano.specs -mfloat-abi=soft -mthumb")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=${CMAKE_SYSTEM_PROCESSOR} \
                                    ${COMPILER_FLAGS} -Wall \
                                    -std=gnu${CMAKE_C_STANDARD} \
                                    -ffunction-sections -fdata-sections -fno-strict-aliasing -fstack-usage \
                                    --specs=nano.specs -mfloat-abi=soft -mthumb")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mcpu=${CMAKE_SYSTEM_PROCESSOR} \
                                                      ${COMPILER_FLAGS} -T ${LINKER_SCRIPT} -static \
                                                      --specs=nosys.specs --specs=nano.specs \
                                                      -mfloat-abi=soft -mthumb \
                                                      -Wl,-Map=${PROJECT_NAME}.map -Wl,--print-memory-usage -Wl,--gc-sections \
                                                      -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group")

set(CMAKE_C_LINK_EXECUTABLE   "<CMAKE_C_COMPILER>   <LINK_FLAGS> -o <TARGET> <OBJECTS>")
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <LINK_FLAGS> -o <TARGET> <OBJECTS>")

add_definitions(-DSTM32F103xx)
add_definitions(-DSTM32F103xB)

include_directories(.)

include_directories(./libraries/cmis_library/)
include_directories(./libraries/main_library/)

set(SOURCE_FILES ./main.c
                 ./libraries/system_library/syscalls.c
                 ./libraries/system_library/sysmem.c
                 ./libraries/main_library/system_stm32f1xx.c)

add_executable(${PROJECT_NAME} ${STARTUP_LOCATION} ${SOURCE_FILES})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_OBJCOPY} ARGS -Oihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex)
