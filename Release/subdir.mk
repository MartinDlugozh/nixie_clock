################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
INO_SRCS += \
../nixie_clock.ino 

CPP_SRCS += \
../.ino.cpp \
../DS3231.cpp 

LINK_OBJ += \
./.ino.cpp.o \
./DS3231.cpp.o 

INO_DEPS += \
./nixie_clock.ino.d 

CPP_DEPS += \
./.ino.cpp.d \
./DS3231.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
.ino.cpp.o: ../.ino.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/home/kaktus/sloeber/arduinoPlugin/tools/arduino/avr-gcc/4.9.2-atmel3.5.3-arduino2/bin/avr-g++" -c -g -Os -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -flto -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10609 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR   -I"/home/kaktus/sloeber/arduinoPlugin/packages/arduino/hardware/avr/1.6.14/cores/arduino" -I"/home/kaktus/sloeber/arduinoPlugin/packages/arduino/hardware/avr/1.6.14/variants/standard" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<" -o "$@"  -Wall
	@echo 'Finished building: $<'
	@echo ' '

DS3231.cpp.o: ../DS3231.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/home/kaktus/sloeber/arduinoPlugin/tools/arduino/avr-gcc/4.9.2-atmel3.5.3-arduino2/bin/avr-g++" -c -g -Os -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -flto -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10609 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR   -I"/home/kaktus/sloeber/arduinoPlugin/packages/arduino/hardware/avr/1.6.14/cores/arduino" -I"/home/kaktus/sloeber/arduinoPlugin/packages/arduino/hardware/avr/1.6.14/variants/standard" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<" -o "$@"  -Wall
	@echo 'Finished building: $<'
	@echo ' '

nixie_clock.o: ../nixie_clock.ino
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/home/kaktus/sloeber/arduinoPlugin/tools/arduino/avr-gcc/4.9.2-atmel3.5.3-arduino2/bin/avr-g++" -c -g -Os -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -flto -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10609 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR   -I"/home/kaktus/sloeber/arduinoPlugin/packages/arduino/hardware/avr/1.6.14/cores/arduino" -I"/home/kaktus/sloeber/arduinoPlugin/packages/arduino/hardware/avr/1.6.14/variants/standard" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<" -o "$@"  -Wall
	@echo 'Finished building: $<'
	@echo ' '


