################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
INO_SRCS += \
../uMulti.ino 

CPP_SRCS += \
../sloeber.ino.cpp \
../uMulti.cpp 

LINK_OBJ += \
./sloeber.ino.cpp.o \
./uMulti.cpp.o 

INO_DEPS += \
./uMulti.ino.d 

CPP_DEPS += \
./sloeber.ino.cpp.d \
./uMulti.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
sloeber.ino.cpp.o: ../sloeber.ino.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/home/byronh/eclipse-c++-oxygen//arduinoPlugin/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.4-arduino2/bin/avr-g++" -c -g2 -O0 -Wall -Wextra -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -mmcu=atmega328p -DF_CPU=8000000L -DARDUINO=10802 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR   -I"/home/byronh/eclipse-c++-oxygen/arduinoPlugin/packages/arduino/hardware/avr/1.6.20/cores/arduino" -I"/home/byronh/eclipse-c++-oxygen/arduinoPlugin/packages/arduino/hardware/avr/1.6.20/variants/eightanaloginputs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '

uMulti.cpp.o: ../uMulti.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/home/byronh/eclipse-c++-oxygen//arduinoPlugin/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.4-arduino2/bin/avr-g++" -c -g2 -O0 -Wall -Wextra -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -mmcu=atmega328p -DF_CPU=8000000L -DARDUINO=10802 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR   -I"/home/byronh/eclipse-c++-oxygen/arduinoPlugin/packages/arduino/hardware/avr/1.6.20/cores/arduino" -I"/home/byronh/eclipse-c++-oxygen/arduinoPlugin/packages/arduino/hardware/avr/1.6.20/variants/eightanaloginputs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '

uMulti.o: ../uMulti.ino
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/home/byronh/eclipse-c++-oxygen//arduinoPlugin/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.4-arduino2/bin/avr-g++" -c -g2 -O0 -Wall -Wextra -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -mmcu=atmega328p -DF_CPU=8000000L -DARDUINO=10802 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR   -I"/home/byronh/eclipse-c++-oxygen/arduinoPlugin/packages/arduino/hardware/avr/1.6.20/cores/arduino" -I"/home/byronh/eclipse-c++-oxygen/arduinoPlugin/packages/arduino/hardware/avr/1.6.20/variants/eightanaloginputs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '


