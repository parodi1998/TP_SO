################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/conexiones.c \
../src/main.c \
../src/utils.c 

C_DEPS += \
./src/conexiones.d \
./src/main.d \
./src/utils.d 

OBJS += \
./src/conexiones.o \
./src/main.o \
./src/utils.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/home/utnso/git/tp-2022-2c-Los_OlvidadOS/cpu/src -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/conexiones.d ./src/conexiones.o ./src/main.d ./src/main.o ./src/utils.d ./src/utils.o

.PHONY: clean-src

