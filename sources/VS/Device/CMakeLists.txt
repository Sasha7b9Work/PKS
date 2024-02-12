# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

project (Device-GUI)

set(CMAKE_UNITY_BUILD OFF)

include_directories(
    .
    Device
    ../../Device/src
    ../../Device/src/Hardware/CDC
    ../../Device/src/Hardware/HAL
    ../../VS/ThirdParty/wxWidgets/include
    ../../generated/ThirdParty/lib/vc_dll/mswud
    ../../common/_VS_KEIL/CMSIS
    ../../common/GD32F30x_Library/Firmware/GD32F30x_standard_peripheral/Include
    ../../common/GD32F30x_Library/Firmware/CMSIS/GD/GD32F30x/Include
)

link_directories(../../generated/ThirdParty/lib/vc_dll)

add_definitions(-DGD32F30X_HD -DWXUSINGDLL -DGUI -DDEVICE)

if(WIN32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
    

    add_compile_options(
        /wd4005
        /wd4100
        /wd4201
        /wd4255
        /wd4365
        /wd4464
        /wd4548
        /wd4514
        /wd4623
        /wd4625
        /wd4626
        /wd4668
        /wd4774
        /wd4820
        /wd4996
        /wd5026
        /wd5027
        /wd5039    # 'xxx': pointer or reference to potentially throwing function passed to extern C
                    # function under -EHc.
        /wd5045
        /wd5220
        /wd26812
    )
endif()

file(GLOB SRC                       *.h *.cpp
                                    ../../Device/src/defines.h
                                    ../../Device/src/Device.*
                                    ../../Device/src/Log.*
                                    ../../Device/src/Test.*)
file(GLOB LIBS                      ../../common/_VS_LIBS/GD32F3/*.*)
file(GLOB DISPLAY                   ../../Device/src/Display/*.*)
file(GLOB DISPLAY_FONT              ../../Device/src/Display/Font/*.*)
file(GLOB FLASHDISK                 ../../Device/src/FlashDisk/*.*)
file(GLOB HARDWARE                  ../../Device/src/Hardware/*.*)
file(GLOB HARDWARE_HAL              ../../Device/src/Hardware/HAL/HAL_PINS.*
                                    ../../Device/src/Hardware/HAL/HAL.*
                                                     Hardware/HAL/*.*)
file(GLOB HARDWARE_MODULES_SIM800C  ../../Device/src/Hardware/Modules/SIM800C/*.*)
file(GLOB HARDWARE_MODULES_M25P80   ../../Device/src/Hardware/Modules/M25P80/*.*)
file(GLOB HARDWARE_MODULES_SSD1306  ../../Device/src/Hardware/Modules/SSD1306/*.*)
file(GLOB MEASURER                  ../../Device/src/Measurer/*.*)
file(GLOB MODEM                     ../../Device/src/Modem/*.*)
file(GLOB UTILS                     ../../Device/src/Utils/*.*)

source_group(src                        FILES ${SRC})
source_group(libs                       FILES ${LIBS})
source_group(Display                    FILES ${DISPLAY})
source_group(Display/Font               FILES ${DISPLAY_FONT})
source_group(FlashDisk                  FILES ${FLASHDISK})
source_group(Hardware                   FILES ${HARDWARE})
source_group(Hardware/HAL               FILES ${HARDWARE_HAL})
source_group(Hardware/Modules/SIM800C   FILES ${HARDWARE_MODULES_SIM800C})
source_group(Hardware/Modules/M25P80    FILES ${HARDWARE_MODULES_M25P80})
source_group(Hardware/Modules/SSD1306   FILES ${HARDWARE_MODULES_SSD1306})
source_group(Measurer                   FILES ${MEASURER})
source_group(Modem                      FILES ${MODEM})
source_group(Utils                      FILES ${UTILS})

set_source_files_properties(
    ../../Device/src/Hardware/HAL/systick.c
    PROPERTIES HEADER_FILE_ONLY TRUE
)

add_executable(
    ${PROJECT_NAME}
    ${SRC}
    ${LIBS}
    ${DISPLAY}
    ${DISPLAY_FONT}
    ${FLASHDISK}
    ${HARDWARE}
    ${HARDWARE_HAL}
    ${HARDWARE_MODULES_SIM800C}
    ${HARDWARE_MODULES_M25P80}
    ${HARDWARE_MODULES_SSD1306}
    ${MEASURER}
    ${MODEM}
    ${UTILS}
)

target_link_libraries(${PROJECT_NAME} wxbase32ud wxmsw32ud_core Ws2_32)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND copy ..\\..\\ThirdParty\\lib\\vc_dll\\wxbase32ud_vc_custom.dll $(OutputPath) /Y)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND copy ..\\..\\ThirdParty\\lib\\vc_dll\\wxmsw32ud_core_vc_custom.dll $(OutputPath) /Y)
