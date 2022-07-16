################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/algoritmos.c \
../src/memoria.c \
../src/mensajes.c \
../src/server.c \
../src/swap.c 

OBJS += \
./src/algoritmos.o \
./src/memoria.o \
./src/mensajes.o \
./src/server.o \
./src/swap.o 

C_DEPS += \
./src/algoritmos.d \
./src/memoria.d \
./src/mensajes.d \
./src/server.d \
./src/swap.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2022-1c-9-12/Shared/utils" -I"/home/utnso/tp-2022-1c-9-12/Shared" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


