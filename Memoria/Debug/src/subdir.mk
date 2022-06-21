################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/memoria.c \
../src/paginacion.c \
../src/swap.c \
../src/util.c 

OBJS += \
./src/memoria.o \
./src/paginacion.o \
./src/swap.o \
./src/util.o 

C_DEPS += \
./src/memoria.d \
./src/paginacion.d \
./src/swap.d \
./src/util.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/home/utnso/tp912/tp-2022-1c-9-12/Shared/utils -I/home/utnso/tp912/tp-2022-1c-9-12/Shared -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


