################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/OldSimon/simon.cpp \
../src/OldSimon/simonGame.cpp 

CC_SRCS += \
../src/OldSimon/main.cc 

OBJS += \
./src/OldSimon/main.o \
./src/OldSimon/simon.o \
./src/OldSimon/simonGame.o 

CC_DEPS += \
./src/OldSimon/main.d 

CPP_DEPS += \
./src/OldSimon/simon.d \
./src/OldSimon/simonGame.d 


# Each subdirectory must supply rules for building sources it contributes
src/OldSimon/%.o: ../src/OldSimon/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: ARM g++ compiler'
	arm-xilinx-eabi-g++ -Wall -O3 -finline-functions -g3 -I"C:\Users\hutch\ZYBO\HWTest3_SW_Env\Consolidated_330_SW" -c -fmessage-length=0 -I../../HW3_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/OldSimon/%.o: ../src/OldSimon/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: ARM g++ compiler'
	arm-xilinx-eabi-g++ -Wall -O3 -finline-functions -g3 -I"C:\Users\hutch\ZYBO\HWTest3_SW_Env\Consolidated_330_SW" -c -fmessage-length=0 -I../../HW3_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


