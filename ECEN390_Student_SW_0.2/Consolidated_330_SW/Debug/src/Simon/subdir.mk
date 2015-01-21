################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Simon/buttonHandler.c \
../src/Simon/flashSequence.c \
../src/Simon/globals.c \
../src/Simon/simonControl.c \
../src/Simon/simonDisplay.c \
../src/Simon/simonMain.c \
../src/Simon/verifySequence.c 

OBJS += \
./src/Simon/buttonHandler.o \
./src/Simon/flashSequence.o \
./src/Simon/globals.o \
./src/Simon/simonControl.o \
./src/Simon/simonDisplay.o \
./src/Simon/simonMain.o \
./src/Simon/verifySequence.o 

C_DEPS += \
./src/Simon/buttonHandler.d \
./src/Simon/flashSequence.d \
./src/Simon/globals.d \
./src/Simon/simonControl.d \
./src/Simon/simonDisplay.d \
./src/Simon/simonMain.d \
./src/Simon/verifySequence.d 


# Each subdirectory must supply rules for building sources it contributes
src/Simon/%.o: ../src/Simon/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM g++ compiler'
	arm-xilinx-eabi-g++ -Wall -O3 -finline-functions -g3 -I"C:\Users\hutch\ZYBO\HWTest3_SW_Env\Consolidated_330_SW" -c -fmessage-length=0 -I../../HW3_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


