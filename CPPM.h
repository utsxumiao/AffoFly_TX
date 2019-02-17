#ifndef CPPM_H_
#define CPPM_H_

#if defined(SIMULATOR) || defined(BUDDY)

void CPPM_init();
void CPPM_outputData(ControlData controlData);

#endif

#endif /* CPPM_H_ */
