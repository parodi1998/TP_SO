################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/client.c \
../src/client_memoria.c \
../src/conexiones.c \
../src/config_cpu.c \
../src/instrucciones.c \
../src/main.c \
../src/server.c \
../src/tlb.c \
../src/utils.c 

OBJS += \
./src/client.o \
./src/client_memoria.o \
./src/conexiones.o \
./src/config_cpu.o \
./src/instrucciones.o \
./src/main.o \
./src/server.o \
./src/tlb.o \
./src/utils.o 

C_DEPS += \
./src/client.d \
./src/client_memoria.d \
./src/conexiones.d \
./src/config_cpu.d \
./src/instrucciones.d \
./src/main.d \
./src/server.d \
./src/tlb.d \
./src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/home/utnso/git/tp-2022-2c-Los_OlvidadOS/cpu/src -ICommons -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


