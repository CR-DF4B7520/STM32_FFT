#include "main.h"

#define FFT_Length 256
extern uint16_t Adc_Buff[FFT_Length];
extern __IO uint8_t Adc_Convert_End_Flag;
extern float FFT_Input[FFT_Length*2];
extern float FFT_Output[FFT_Length];
extern uint8_t fft_bias;

void run_adc_fft(void);
void run_adc_sample(void);
