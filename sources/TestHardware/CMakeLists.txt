# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

project (TestHardware)

message("------------------------------------------------------------------------ ${PROJECT_NAME}")

include_directories(../common/_VS_KEIL/CMSIS/
                    ../common/_VS_KEIL/std_includes/
                    ../common/STM32Cube_F3/Drivers/STM32F3xx_HAL_Driver/Inc/
                    ../common/STM32Cube_F3/Drivers/CMSIS/Device/ST/STM32F3xx/Include/
                    src/Hardware/HAL/
                    src/
                    )

add_definitions(-DSTM32F303xC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")

add_compile_options(/wd4201)
add_compile_options(/wd4514)    # 'function' : unreferenced inline function has been removed
add_compile_options(/wd4623)    # default constructor was implicitly defined as deleted
add_compile_options(/wd4626)    # assignment operator was implicitly defined as deleted
add_compile_options(/wd4820)    # 'xxx': '4' bytes padding added after data member 'yyy'
add_compile_options(/wd5027)    # move assignment operator was implicitly defined as deleted
add_compile_options(/wd5045)    # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
add_compile_options(/wd5220)    # a non-static data member with a volatile qualified type no longer implies
add_compile_options(/wd26812)   # The enum type %type% is unscoped. Prefer 'enum class' over 'enum' (Enum.3)


file(GLOB STM32F3                ../common/STM32Cube_F3/Drivers/STM32F3xx_HAL_Driver/Src/*.c
                                 ../common/STM32Cube_F3/Drivers/STM32F3xx_HAL_Driver/Inc/*.h
                                 ../common/STM32Cube_F3/Drivers/STM32F3xx_HAL_Driver/Inc/Legacy/*.h)
file(GLOB SRC                    src/*.*)
file(GLOB BLOCKS_CALCULATOR      src/Blocks/Calculator/*)
file(GLOB HARDWARE               src/Hardware/*.*)
file(GLOB HARDWARE_CMSIS         src/Hardware/CMSIS/*.*)
file(GLOB HARDWARE_HAL           src/Hardware/HAL/*.*)
file(GLOB MODULES_DISPLAY        src/Modules/Display/*.*)
file(GLOB MODULES_FLASHDISK      src/Modules/FlashDisk/*.*)
file(GLOB MODULES_MEASURER       src/Modules/Measurer/*.*)
file(GLOB MODULES_MODEM          src/Modules/Modem/*.*)
file(GLOB UTILS                  src/Utils/*.*)

source_group(STM32F3             FILES ${STM32F3})
source_group(src                 FILES ${SRC})
source_group(Blocks/Calculator   FILES ${BLOCKS_CALCULATOR})
source_group(Hardware            FILES ${HARDWARE})
source_group(Hardware/CMSIS      FILES ${HARDWARE_CMSIS})
source_group(Hardware/HAL        FILES ${HARDWARE_HAL})
source_group(Modules/Display     FILES ${MODULES_DISPLAY})
source_group(Modules/FlashDisk   FILES ${MODULES_FLASHDISK})
source_group(Modules/Measurer    FILES ${MODULES_MEASURER})
source_group(Modules/Modem       FILES ${MODULES_MODEM})
source_group(Utils               FILES ${UTILS})

set_source_files_properties(${STM32F3} ${HARDWARE_CMSIS}
                            PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(${PROJECT_NAME}
            ${STM32F3}
            ${SRC}
            ${BLOCKS_CALCULATOR}
            ${HARDWARE}
            ${HARDWARE_CMSIS}
            ${HARDWARE_HAL}
            ${MODULES_DISPLAY}
            ${MODULES_FLASHDISK}
            ${MODULES_MEASURER}
            ${MODULES_MODEM}
            ${UTILS}    
            )
