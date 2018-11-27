################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Framework.cpp 

OBJS += \
./src/Framework.o 

CPP_DEPS += \
./src/Framework.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -I/home/robert/git/pjproject/pjsip/include -I/home/robert/git/pjproject/pjmedia/include -I/home/robert/git/pjproject/pjnath/include -I/home/robert/git/pjproject/pjlib/include -I/home/robert/git/pjproject/pjlib-util/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


