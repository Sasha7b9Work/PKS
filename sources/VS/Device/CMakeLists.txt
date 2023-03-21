# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

project (Device)

set(CMAKE_UNITY_BUILD OFF)

include_directories(.
                    Device
                    ../../Device/src
                    ../../Device/src/Hardware/CDC
                    ../../Device/src/Hardware/HAL
                    ../../VS/ThirdParty/wxWidgets/include
                    ../../generated/ThirdParty/lib/vc_dll/mswud
                    ../../common/_VS_KEIL/CMSIS
                    ../../common/_VS_LIBS/STM32F1
                    ../../common/STM32Cube_F1/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
                    ../../common/STM32Cube_F1/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
                    ../../common/STM32Cube_F1/Drivers/CMSIS/Device/ST/STM32F1xx/Include
                    ../../common/STM32Cube_F1/Drivers/STM32F1xx_HAL_Driver/Inc)

link_directories(../../generated/ThirdParty/lib/vc_dll)

add_definitions(-DSTM32F103xB -DWXUSINGDLL -DGUI -DDEVICE)

if(WIN32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
    
    # Older cmake versions don't support string(APPEND ...) provide a workaround
    macro(wx_string_append var str)
        set(${var} ${${var}}${str})
    endmacro()
    
    if(BUILD_USE_STATIC_RUNTIME)
        set(MSVC_LIB_USE "/MTd")
        set(MSVC_LIB_REPLACE "/MDd")
        
        # Set MSVC runtime flags for all configurations
        foreach(cfg "" ${CMAKE_CONFIGURATION_TYPES})
            set(c_flag_var CMAKE_C_FLAGS)
            set(cxx_flag_var CMAKE_CXX_FLAGS)
            if(cfg)
                string(TOUPPER ${cfg} cfg_upper)
                wx_string_append(c_flag_var "_${cfg_upper}")
                wx_string_append(cxx_flag_var "_${cfg_upper}")
            endif()
            if(${c_flag_var} MATCHES ${MSVC_LIB_REPLACE})
                string(REPLACE ${MSVC_LIB_REPLACE} ${MSVC_LIB_USE} ${c_flag_var} "${${c_flag_var}}")
                set(${c_flag_var} ${${c_flag_var}} CACHE STRING
                "Flags used by the C compiler during ${cfg_upper} builds." FORCE)
            endif()
            if(${cxx_flag_var} MATCHES ${MSVC_LIB_REPLACE})
                string(REPLACE ${MSVC_LIB_REPLACE} ${MSVC_LIB_USE} ${cxx_flag_var} "${${cxx_flag_var}}")
                set(${cxx_flag_var} ${${cxx_flag_var}} CACHE STRING
                "Flags used by the CXX compiler during ${cfg_upper} builds." FORCE)
            endif()
        endforeach()
    endif()

#    add_compile_options(/wd4005)
    add_compile_options(/wd4100)
    add_compile_options(/wd4255)
    add_compile_options(/wd4365)
    add_compile_options(/wd4464)
    add_compile_options(/wd4548)
    add_compile_options(/wd4514)
    add_compile_options(/wd4623)
    add_compile_options(/wd4625)
    add_compile_options(/wd4626)
    add_compile_options(/wd4774)
    add_compile_options(/wd4820)
    add_compile_options(/wd4996)
    add_compile_options(/wd5026)
    add_compile_options(/wd5027)
    add_compile_options(/wd5039)    # 'xxx': pointer or reference to potentially throwing function passed to extern C
                                    # function under -EHc.
    add_compile_options(/wd5045)
    add_compile_options(/wd5220)
    add_compile_options(/wd26812)
endif()

file(GLOB SRC                   *.h *.cpp
                                ../../Device/src/defines.h
                                ../../Device/src/Device.*)
file(GLOB BLOCKS_CALCULATOR     ../../Device/src/Blocks/Calculator/*.*)
file(GLOB BLOCKS_MEASURER       ../../Device/src/Blocks/Measurer/*.*)
file(GLOB DISPLAY_FONT          ../../Device/src/Display/Font/*.*)
file(GLOB HARDWARE              ../../Device/src/Hardware/*.*)
file(GLOB HARDWARE_HAL          ../../Device/src/Hardware/HAL/*.*
                                                 Hardware/HAL/*.*)
file(GLOB MODULES_BH1750        ../../Device/src/Modules/BH1750/*.*)
file(GLOB MODULES_BME280        ../../Device/src/Modules/BME280/*.*)
file(GLOB MODULES_CG_ANEM       ../../Device/src/Modules/CG-Anem/*.*)
file(GLOB MODULES_HC12          ../../Device/src/Modules/HC12/*.*)
file(GLOB MODULES_ST7735        ../../Device/src/Modules/ST7735/*.*)
file(GLOB SETTINGS              ../../Device/src/Settings/*.*)
file(GLOB UTILS                 ../../Device/src/Utils/*.*)
file(GLOB UTILS_TEXT            ../../Device/src/Utils/Text/*.*)

source_group(src                 FILES ${SRC})
source_group(Blocks/Calculator   FILES ${BLOCKS_CALCULATOR})
source_group(Blocks/Measurer     FILES ${BLOCKS_MEASURER})
source_group(Hardware            FILES ${HARDWARE})
source_group(Hardware/HAL        FILES ${HARDWARE_HAL})
source_group(Modules/BH1750      FILES ${MODULES_BH1750})
source_group(Modules/BME280      FILES ${MODULES_BME280})
source_group(Modules/CG-Anem     FILES ${MODULES_CG_ANEM})
source_group(Modules/HC12        FILES ${MODULES_HC12})
source_group(Modules/ST7735      FILES ${MODULES_ST7735})
source_group(Settings            FILES ${SETTINGS})
source_group(Utils               FILES ${UTILS})
source_group(Utils/Text          FILES ${UTILS_TEXT})


add_executable( ${PROJECT_NAME}
                ${SRC}
                ${BLOCKS_CALCULATOR}
                ${BLOCKS_MEASURER}
                ${HARDWARE}
                ${HARDWARE_HAL}
                ${MODULES_BH1750}
                ${MODULES_BME280}
                ${MODULES_CG_ANEM}
                ${MODULES_HC12}
                ${MODULES_ST7735}
                ${SETTINGS}
                ${UTILS}
                ${UTILS_TEXT}
                )

target_link_libraries(${PROJECT_NAME} wxbase32ud wxmsw32ud_core Ws2_32)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND copy ..\\..\\ThirdParty\\lib\\vc_dll\\wxbase32ud_vc_custom.dll $(OutputPath) /Y)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND copy ..\\..\\ThirdParty\\lib\\vc_dll\\wxmsw32ud_core_vc_custom.dll $(OutputPath) /Y)
