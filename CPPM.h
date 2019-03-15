#ifndef CPPM_H_
#define CPPM_H_

#if defined(SIMULATOR) || defined(BUDDY)

#ifdef SHOW_RATE
extern uint16_t CPPM_COUNT;
#endif

void CPPM_init();
void CPPM_outputData(ControlData controlData);

#endif

#endif /* CPPM_H_ */
