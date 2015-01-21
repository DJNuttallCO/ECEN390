################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ClockLab/clockControl.c \
../src/ClockLab/clockMain.c \
../src/ClockLab/clockdisplay.c 

OBJS += \
./src/ClockLab/clockControl.o \
./src/ClockLab/clockMain.o \
./src/ClockLab/clockdisplay.o 

C_DEPS += \
./src/ClockLab/clockControl.d \
./src/ClockLab/clockMain.d \
./src/ClockLab/clockdisplay.d 


# Each subdirectory must supply rules for building sources it contributes
src/ClockLab/%.o: ../src/ClockLab/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM g++ compiler'
	arm-xilinx-eabi-g++ -Wall -O3 -finline-functions -g3 -I"C:\Users\hutch\ZYBO\HWTest3_SW_Env\Consolidated_330_SW" -c -fmessage-length=0 -I../../HW3_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


