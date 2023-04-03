# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

project (Device)

message("------------------------------------------------------------------------ ${PROJECT_NAME}")

include_directories(../common/_VS_KEIL/CMSIS/
                    ../common/_VS_KEIL/std_includes/
                    ../common/GD32F30x_Library/Firmware/GD32F30x_standard_peripheral/Include/
                    ../common/GD32F30x_Library/Firmware/CMSIS/GD/GD32F30x/Include/
                    src/Hardware/HAL/
                    src/
                    )

add_definitions(-DGD32F30X_CL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")

add_compile_options(/wd4013)
add_compile_options(/wd4131)
add_compile_options(/wd4201)
add_compile_options(/wd4514)    # 'function' : unreferenced inline function has been removed
add_compile_options(/wd4623)    # default constructor was implicitly defined as deleted
add_compile_options(/wd4626)    # assignment operator was implicitly defined as deleted
add_compile_options(/wd4820)    # 'xxx': '4' bytes padding added after data member 'yyy'
add_compile_options(/wd5027)    # move assignment operator was implicitly defined as deleted
add_compile_options(/wd5045)    # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
add_compile_options(/wd5220)    # a non-static data member with a volatile qualified type no longer implies
add_compile_options(/wd26812)   # The enum type %type% is unscoped. Prefer 'enum class' over 'enum' (Enum.3)


file(GLOB STM32F3                  ../common/STM32Cube_F3/Drivers/STM32F3xx_HAL_Driver/Src/*.c
                                   ../common/STM32Cube_F3/Drivers/STM32F3xx_HAL_Driver/Inc/*.h
                                   ../common/STM32Cube_F3/Drivers/STM32F3xx_HAL_Driver/Inc/Legacy/*.h)
file(GLOB SRC                      src/*.*)
file(GLOB CALCULATOR               src/Calculator/*.*)
file(GLOB DISPLAY                  src/Display/*.*)
file(GLOB FLASHDISK                src/FlashDisk/*.*)
file(GLOB HARDWARE                 src/Hardware/*.*)
file(GLOB HARDWARE_CMSIS           src/Hardware/CMSIS/*.*)
file(GLOB HARDWARE_HAL             src/Hardware/HAL/*.*)
file(GLOB HARDWARE_MODULES_SIM800C src/Hardware/Modules/SIM800C/*.*)
file(GLOB HARDWARE_MODULES_SSD1306 src/Hardware/Modules/SSD1306/*.*)
file(GLOB HARDWARE_MODULES_ZMPT101 src/Hardware/Modules/ZMPT101/*.*)
file(GLOB MEASURER                 src/Measurer/*.*)
file(GLOB MODEM                    src/Modem/*.*)
file(GLOB UTILS                    src/Utils/*.*)

source_group(STM32F3                  FILES ${STM32F3})
source_group(src                      FILES ${SRC})
source_group(Calculator               FILES ${CALCULATOR})
source_group(Display                  FILES ${DISPLAY})
source_group(FlashDisk                FILES ${FLASHDISK})
source_group(Hardware                 FILES ${HARDWARE})
source_group(Hardware/CMSIS           FILES ${HARDWARE_CMSIS})
source_group(Hardware/HAL             FILES ${HARDWARE_HAL})
source_group(Hardware/Modules/SIM800C FILES ${HARDWARE_MODULES_SIM800C})
source_group(Hardware/Modules/SSD1306 FILES ${HARDWARE_MODULES_SSD1306})
source_group(Hardware/Modules/ZMPT101 FILES ${HARDWARE_MODULES_ZMPT101})
source_group(Measurer                 FILES ${MEASURER})
source_group(Modem                    FILES ${MODEM})
source_group(Utils                    FILES ${UTILS})

set_source_files_properties(${STM32F3} ${HARDWARE_CMSIS}
                            PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(${PROJECT_NAME}
            ${STM32F3}
            ${SRC}
            ${CALCULATOR}
            ${DISPLAY}
            ${FLASHDISK}
            ${HARDWARE}
            ${HARDWARE_CMSIS}
            ${HARDWARE_HAL}
            ${HARDWARE_MODULES_SIM800C}
            ${HARDWARE_MODULES_SSD1306}
            ${HARDWARE_MODULES_ZMPT101}
            ${MEASURER}
            ${MODEM}
            ${UTILS}
            )
