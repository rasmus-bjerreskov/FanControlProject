################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/BarGraph.cpp \
../src/Blinker.cpp \
../src/DigitalIoPin.cpp \
../src/ITMwrapper.cpp \
../src/Imutex.cpp \
../src/LiquidCrystal.cpp \
../src/LpcUart.cpp \
../src/RealTimeClock.cpp \
../src/SimpleRingBuffer.cpp \
../src/Sleeper.cpp \
../src/Uartmorse.cpp \
../src/cstm_sleep.cpp \
../src/morseencoder.cpp 

C_SRCS += \
../src/ITM_write.c 

OBJS += \
./src/BarGraph.o \
./src/Blinker.o \
./src/DigitalIoPin.o \
./src/ITM_write.o \
./src/ITMwrapper.o \
./src/Imutex.o \
./src/LiquidCrystal.o \
./src/LpcUart.o \
./src/RealTimeClock.o \
./src/SimpleRingBuffer.o \
./src/Sleeper.o \
./src/Uartmorse.o \
./src/cstm_sleep.o \
./src/morseencoder.o 

CPP_DEPS += \
./src/BarGraph.d \
./src/Blinker.d \
./src/DigitalIoPin.d \
./src/ITMwrapper.d \
./src/Imutex.d \
./src/LiquidCrystal.d \
./src/LpcUart.d \
./src/RealTimeClock.d \
./src/SimpleRingBuffer.d \
./src/Sleeper.d \
./src/Uartmorse.d \
./src/cstm_sleep.d \
./src/morseencoder.d 

C_DEPS += \
./src/ITM_write.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C++ Compiler'
	arm-none-eabi-c++ -DDEBUG -D__CODE_RED -D__NEWLIB__ -DCORE_M3 -D__USE_LPCOPEN -D__LPC15XX__ -I"C:\Users\Rasmus\Documents\MCUXpressoIDE_11.1.0_3209\FanControlProject\my_lib\inc" -I"C:\Users\Rasmus\Documents\MCUXpressoIDE_11.1.0_3209\FanControlProject\lpc_board_nxp_lpcxpresso_1549\inc" -I"C:\Users\Rasmus\Documents\MCUXpressoIDE_11.1.0_3209\FanControlProject\lpc_chip_15xx\inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m3 -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=c11 -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_LPCOPEN -D__LPC15XX__ -D__NEWLIB__ -I"C:\Users\Rasmus\Documents\MCUXpressoIDE_11.1.0_3209\FanControlProject\my_lib\inc" -I"C:\Users\Rasmus\Documents\MCUXpressoIDE_11.1.0_3209\FanControlProject\lpc_board_nxp_lpcxpresso_1549\inc" -I"C:\Users\Rasmus\Documents\MCUXpressoIDE_11.1.0_3209\FanControlProject\lpc_chip_15xx\inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m3 -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


