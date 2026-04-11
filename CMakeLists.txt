# Set CMake minimum require
cmake_minimum_required(VERSION 3.16)

# Set CMake sysytem name
set(CMAKE_SYSTEM_NAME Generic)


# Skipping compiler detection and use custom toolchain
SET(CMAKE_C_COMPILER_WORKS TRUE)
SET(CMAKE_CXX_COMPILER_WORKS TRUE)
SET(CMAKE_ASM_COMPILER_WORKS TRUE)

# Use MRS toolchian to compile
set(TOOLCHAIN_PREFIX "riscv64-unknown-elf-")
set(CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}g++")
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PREFIX}gcc")
set(CMAKE_OBJCOPY "${TOOLCHAIN_PREFIX}objcopy")
set(CMAKE_OBJDUMP "${TOOLCHAIN_PREFIX}objdump")
set(CMAKE_SIZE "${TOOLCHAIN_PREFIX}size")
set(CMAKE_AR "${TOOLCHAIN_PREFIX}ar")

set(PROJECT_NAME "HID_Mouse")
set(TARGET_ARTIFACT "HID_Mouse.elf")

set(CMAKE_C_OUTPUT_EXTENSION .o)
set(CMAKE_CXX_OUTPUT_EXTENSION .o)

project("${PROJECT_NAME}" LANGUAGES C ASM CXX)

# Use relative path
add_compile_options(-fmacro-prefix-map=${CMAKE_SOURCE_DIR}=..)


set(SRC_FILES 
"${CMAKE_CURRENT_SOURCE_DIR}/Profile/battservice.c"
"${CMAKE_CURRENT_SOURCE_DIR}/Profile/devinfoservice.c"
"${CMAKE_CURRENT_SOURCE_DIR}/Profile/hiddev.c"
"${CMAKE_CURRENT_SOURCE_DIR}/Profile/hidmouseservice.c"
"${CMAKE_CURRENT_SOURCE_DIR}/Profile/scanparamservice.c"
"${CMAKE_CURRENT_SOURCE_DIR}/APP/ch32v20x_it.c"
"${CMAKE_CURRENT_SOURCE_DIR}/APP/hidmouse.c"
"${CMAKE_CURRENT_SOURCE_DIR}/APP/hidmouse_main.c"
"${CMAKE_CURRENT_SOURCE_DIR}/APP/system_ch32v20x.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Startup/startup_ch32v20x_D8W.S"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_adc.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_bkp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_can.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_crc.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_dbgmcu.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_dma.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_exti.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_flash.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_gpio.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_i2c.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_iwdg.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_misc.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_opa.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_pwr.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_rcc.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_rtc.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_spi.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_tim.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_usart.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/src/ch32v20x_wwdg.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Debug/debug.c"
"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Core/core_riscv.c"
"${CMAKE_CURRENT_SOURCE_DIR}/LIB/ble_task_scheduler.S"
"${CMAKE_CURRENT_SOURCE_DIR}/HAL/MCU.c"
"${CMAKE_CURRENT_SOURCE_DIR}/HAL/RTC.c"
"${CMAKE_CURRENT_SOURCE_DIR}/HAL/SLEEP.c"
)
set(CMAKE_C_FLAGS " -march=rv32imacxw -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -g -I\"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Core\" -I\"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Startup\" -I\"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Debug\" -I\"${CMAKE_CURRENT_SOURCE_DIR}/SRC/Peripheral/inc\" -I\"${CMAKE_CURRENT_SOURCE_DIR}/APP/include\" -I\"${CMAKE_CURRENT_SOURCE_DIR}/Profile/include\" -I\"${CMAKE_CURRENT_SOURCE_DIR}/HAL/include\" -I\"${CMAKE_CURRENT_SOURCE_DIR}/LIB/\" -DCH32V20x_D8W -std=gnu99")
set(CMAKE_CXX_FLAGS " -march=rv32imacxw -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -g -std=gnu++11 -fabi-version=0")
set(CMAKE_ASM_FLAGS " -march=rv32imacxw -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -g -x assembler-with-cpp")

add_executable("${TARGET_ARTIFACT}" ${SRC_FILES})

set_target_properties("${TARGET_ARTIFACT}" PROPERTIES LINK_FLAGS " -march=rv32imacxw -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -g -T \"${CMAKE_CURRENT_SOURCE_DIR}/HAL/Link.ld\"  -L\"${CMAKE_CURRENT_SOURCE_DIR}/\" -L\"${CMAKE_CURRENT_SOURCE_DIR}/LIB/\"  -nostartfiles -Xlinker --gc-sections -Xlinker --print-memory-usage -Wl,-Map,\"HID_Mouse.map\" --specs=nano.specs --specs=nosys.specs -o \"HID_Mouse.elf\" ")

target_link_libraries(
"${TARGET_ARTIFACT}" 
wchble
c
)

add_custom_command(TARGET "${TARGET_ARTIFACT}" 
POST_BUILD 
COMMAND ${CMAKE_OBJCOPY} ARGS -O ihex  "HID_Mouse.elf" "HID_Mouse.hex"
COMMAND ${CMAKE_OBJDUMP} ARGS  --source --all-headers --demangle -M xw --line-numbers --wide "HID_Mouse.elf" > "HID_Mouse.lst"
COMMAND ${CMAKE_SIZE} ARGS  --format=berkeley "HID_Mouse.elf"
)

