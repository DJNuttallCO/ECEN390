################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/SwitchLab/buttons.c \
../src/SwitchLab/switchLabMain.c \
../src/SwitchLab/switches.c 

OBJS += \
./src/SwitchLab/buttons.o \
./src/SwitchLab/switchLabMain.o \
./src/SwitchLab/switches.o 

C_DEPS += \
./src/SwitchLab/buttons.d \
./src/SwitchLab/switchLabMain.d \
./src/SwitchLab/switches.d 


# Each subdirectory must supply rules for building sources it contributes
src/SwitchLab/%.o: ../src/SwitchLab/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM g++ compiler'
	arm-xilinx-eabi-g++ -Wall -O2 -finline-functions -g3 -I"C:\Users\hutch\ZYBO\HWTest3_SW_Env\Consolidated_330_SW" -c -fmessage-length=0 -I../../HW3_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


