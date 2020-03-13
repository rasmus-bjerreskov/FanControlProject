#include "Sleeper.h"
static Sleeper *rtc;
Sleeper::Sleeper() : TICKRATE_HZ1(1000) {
	uint32_t sysTickRate;

	Chip_Clock_SetSysTickClockDiv(1);
	sysTickRate = Chip_Clock_GetSysTickClockRate();
	SysTick_Config(sysTickRate / TICKRATE_HZ1);
	rtc = this;
}

Sleeper::Sleeper(int tickrate0) : TICKRATE_HZ1(tickrate0) {
	uint32_t sysTickRate;

	Chip_Clock_SetSysTickClockDiv(1);
	sysTickRate = Chip_Clock_GetSysTickClockRate();
	SysTick_Config(sysTickRate / TICKRATE_HZ1);
	rtc = this;
}

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Handle interrupt from SysTick timer
 * @return Nothing
 */
void SysTick_Handler(void)
{
	if(rtc != NULL) rtc->tick();

}
#ifdef __cplusplus
}
#endif

void Sleeper::sleep(int ms)
{
	counter = ms;
	while(counter > 0) {
		__WFI();
	}
}

void Sleeper::tick(){
	if(counter > 0) counter--;
}

Sleeper::~Sleeper() {
	// TODO Auto-generated destructor stub
}

