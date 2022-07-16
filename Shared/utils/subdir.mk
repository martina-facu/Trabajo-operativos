################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../utils/coordenada.c \
../utils/instrucciones.c \
../utils/logs.c \
../utils/mmu.c \
../utils/pcb.c \
../utils/protocolo.c \
../utils/tabla_paginas.c 

OBJS += \
./utils/coordenada.o \
./utils/instrucciones.o \
./utils/logs.o \
./utils/mmu.o \
./utils/pcb.o \
./utils/protocolo.o \
./utils/tabla_paginas.o 

C_DEPS += \
./utils/coordenada.d \
./utils/instrucciones.d \
./utils/logs.d \
./utils/mmu.d \
./utils/pcb.d \
./utils/protocolo.d \
./utils/tabla_paginas.d 


# Each subdirectory must supply rules for building sources it contributes
utils/%.o: ../utils/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


