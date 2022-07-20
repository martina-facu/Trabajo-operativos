################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/CPU.c \
../src/utlis.c 

OBJS += \
./src/CPU.o \
./src/utlis.o 

C_DEPS += \
./src/CPU.d \
./src/utlis.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/new_tp2/tp-2022-1c-9-12/Shared/utils" -I"/home/utnso/new_tp2/tp-2022-1c-9-12/Shared" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


