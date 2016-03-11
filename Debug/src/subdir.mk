################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Distributed\ Computing\ Project.c \
../src/data.c \
../src/hash.c 

OBJS += \
./src/Distributed\ Computing\ Project.o \
./src/data.o \
./src/hash.o 

C_DEPS += \
./src/Distributed\ Computing\ Project.d \
./src/data.d \
./src/hash.d 


# Each subdirectory must supply rules for building sources it contributes
src/Distributed\ Computing\ Project.o: ../src/Distributed\ Computing\ Project.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/Distributed Computing Project.d" -MT"src/Distributed\ Computing\ Project.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


