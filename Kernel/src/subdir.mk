################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Kernel.c \
../src/conexiones.c \
../src/fifo.c \
../src/planificador_largo_plazo.c \
../src/planificador_mediano_plazo.c \
../src/sjf.c 

OBJS += \
./src/Kernel.o \
./src/conexiones.o \
./src/fifo.o \
./src/planificador_largo_plazo.o \
./src/planificador_mediano_plazo.o \
./src/sjf.o 

C_DEPS += \
./src/Kernel.d \
./src/conexiones.d \
./src/fifo.d \
./src/planificador_largo_plazo.d \
./src/planificador_mediano_plazo.d \
./src/sjf.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2022-1c-9-12/Shared" -I"/home/utnso/tp-2022-1c-9-12/Shared/utils" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


