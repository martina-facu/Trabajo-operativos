################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/CPU.c \
../src/utils.c 

OBJS += \
./src/CPU.o \
./src/utils.o 

C_DEPS += \
./src/CPU.d \
./src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/CPU.o: ../src/CPU.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -Icommons -Ipthread -I/home/utnso/tp-2022-1c-9-12/Shared/utils -I/home/utnso/tp-2022-1c-9-12/Shared -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/CPU.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -Icommons -Ipthread -I"/home/utnso/tp-2022-1c-9-12/Shared" -I"/home/utnso/tp-2022-1c-9-12/Shared/utils" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


