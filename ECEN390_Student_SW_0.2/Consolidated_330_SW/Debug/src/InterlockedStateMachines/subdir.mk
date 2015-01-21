################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/InterlockedStateMachines/flashControl.c \
../src/InterlockedStateMachines/flashLed.c \
../src/InterlockedStateMachines/flashMain.c \
../src/InterlockedStateMachines/globals.c 

OBJS += \
./src/InterlockedStateMachines/flashControl.o \
./src/InterlockedStateMachines/flashLed.o \
./src/InterlockedStateMachines/flashMain.o \
./src/InterlockedStateMachines/globals.o 

C_DEPS += \
./src/InterlockedStateMachines/flashControl.d \
./src/InterlockedStateMachines/flashLed.d \
./src/InterlockedStateMachines/flashMain.d \
./src/InterlockedStateMachines/globals.d 


# Each subdirectory must supply rules for building sources it contributes
src/InterlockedStateMachines/%.o: ../src/InterlockedStateMachines/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM g++ compiler'
	arm-xilinx-eabi-g++ -Wall -O3 -finline-functions -g3 -I"C:\Users\hutch\ZYBO\HWTest3_SW_Env\Consolidated_330_SW" -c -fmessage-length=0 -I../../HW3_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


