################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/funciones_cpu.c \
../src/funciones_kernel.c \
../src/main.c \
../src/swap.c 

OBJS += \
./src/funciones_cpu.o \
./src/funciones_kernel.o \
./src/main.o \
./src/swap.o 

C_DEPS += \
./src/funciones_cpu.d \
./src/funciones_kernel.d \
./src/main.d \
./src/swap.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2022-1c-9-12/Shared/utils" -I"/home/utnso/tp-2022-1c-9-12/Shared" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


