################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../queue.c 

OBJS += \
./queue.o 

C_DEPS += \
./queue.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM g++ compiler'
	arm-xilinx-eabi-g++ -Wall -O3 -finline-functions -g3 -I"C:\Users\DJ\Documents\School\College\ECEN390TA\Workspace\ECEN390_Student_SW_0.2\Consolidated_330_SW" -c -fmessage-length=0 -MT"$@" -I../../HW3_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


