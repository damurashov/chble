#############################################################[Chinese Manual]###################################################################
# *****工程路径不要包含中文字符和空格,否则可能会报错！*****
# 第一步： 确认机器上已经安装了MRS的工具链(假设当前平台为Linux,工具链路径为"/path/to/toolchain/RISC-V Embedded GCC12",工具链相关前缀为"riscv-wch-elf-")
# 第二步: 设置工具链路径,例如修改替换下方的set(TOOLCHAIN_FOLDER "/path/to/toolchain/RISC-V Embedded GCC12")
# 第三步: 设置工具链中相关可执行文件的前缀,例如修改替换下方的set(TOOLCHAIN_PREFIX "riscv-wch-elf-")
# 第四步: 执行 cd your/path/of/project
# 第五步: 执行 cmake -B build -G "Unix Makefiles"
# 第六步: 执行 cmake --build build
# 注意: 在Windows平台运行,需要a.安装GNU MAKE并将其路径(bin目录)添加到系统环境变量,之后执行cmake -B build -G "Unix Makefiles"(推荐)
#       或者b.安装MinGW,将其路径(bin目录)添加到系统环境变量,之后执行cmake -B build -G "MinGW Makefiles"
# 如有相关疑问,可通过support@mounriver.com联系我们
#################################################################################################################################################


#############################################################[English Manual]###################################################################
# Step 1: Confirm that the MRS toolchain has been installed on the machine. Assuming the current platform is Linux, 
#         the toolchain path is "/path/to/toolchain/RISC-V Embedded GCC12", and the toolchain-related prefix is "riscv-wch-elf-"
# Step 2: Set the toolchain path, for example, modify and replace the following "set (TOOLCHAIN_FOLDER "/path/to/toolchain/RISC-V Embedded GCC12")"
# Step 3: Set the prefix of the executable files in the toolchain,for example, modify and replace the following "set(TOOLCHAIN_PREFIX "riscv-wch-elf-")"
# Step 4: Call "cd your/path/of/project"
# Step 5: Call "cmake -B build -G "Unix Makefiles""
# Step 6: Call "cmake --build build"
# Note:   To run on Windows, you need: a. Install GNU MAKE and add its path (the bin directory) to the system environment PATH. 
#         Then call the execution of "cmake -B build -G "Unix Makefiles""(Recommanded).
#         or b. Install MinGW and add its path (the bin directory) to the system environment PATH. 
#         Then call the execution of "cmake -B build -G "MinGW Makefiles"".
# If you have any questions, please contact us at support@mounriver.com
#################################################################################################################################################


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

