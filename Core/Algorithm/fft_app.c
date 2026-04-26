#include "system.h"
#include "adc.h"

uint16_t Adc_Buff[FFT_Length];                 
__IO uint8_t Adc_Convert_End_Flag = 0;  
float FFT_Input[FFT_Length*2];
float FFT_Output[FFT_Length];
uint8_t fft_bias = 0;

void run_adc_fft(void)
{
    if (motor_turn_on || motor_vibrate)
    {
        HAL_ADC_Start_DMA(&hadc2,(uint32_t *)Adc_Buff,FFT_Length);
        while (!Adc_Convert_End_Flag); 
        
        float mean = 0;
        for (uint16_t i = 0; i < FFT_Length; i++) {
            mean += Adc_Buff[i];
        }
        mean /= FFT_Length;
        
        for (uint16_t i = 0; i < FFT_Length; i++)
        {
            float centered_value = (float)Adc_Buff[i] - mean;                           // Normalize
            float window = 0.5f - 0.5f * arm_cos_f32(2 * PI * i / (FFT_Length - 1));    // Hanning
            FFT_Input[2*i] = centered_value * window;                                   // Re(e^jw)
            FFT_Input[2*i+1] = 0;                                                       // Im(e^jw)
        }
        arm_cfft_f32(&arm_cfft_sR_f32_len256,FFT_Input,0,1);
        arm_cmplx_mag_f32(FFT_Input,FFT_Output,FFT_Length);
        
        FFT_Output[0] /= FFT_Length;
        float window_compensation = 2.0f;
        
        for (uint16_t i = 0; i < FFT_Length; i++)
        {
            FFT_Output[i] = FFT_Output[i] * window_compensation / (FFT_Length/2);
        }
        if (motor_vibrate)
        {
            ring_buffer_put(&uart_rb5, TRANSMIT, "duty_val=%d\xff\xff\xffrps_val=%d\xff\xff\xff",duty_to_angle(85.f), rps_to_angle(rps));
            ring_buffer_put(&uart_rb1, !TRANSMIT, "duty_val=%d\trps_val=%d\r\n",duty_to_angle(85.f), rps_to_angle(rps));
        }
        else
        {
            ring_buffer_put(&uart_rb5, TRANSMIT, "duty_val=%d\xff\xff\xffrps_val=%d\xff\xff\xff",duty_to_angle(pid_duty), rps_to_angle(rps));
            ring_buffer_put(&uart_rb1, !TRANSMIT, "duty_val=%d\trps_val=%d\r\n",duty_to_angle(pid_duty), rps_to_angle(rps));
        }

        for (uint8_t i = 0; i < 30; i += 5)  // 5 Data; 5 epoch
        {
            uint8_t base_index = 30 * fft_bias;
            for (uint8_t j = 0; j < 5 && (i + j) < 50; j++)
            {
                uint8_t current_index = i + j + base_index;
                ring_buffer_put(&uart_rb5, !TRANSMIT, "fft_val%d=%d\xFF\xFF\xFF", i+j, (int)(FFT_Output[current_index] / 10.0 * 100));
                ring_buffer_put(&uart_rb1, !TRANSMIT, "fft_val%d=%d\t", current_index, (int)(FFT_Output[current_index] / 10.0 * 100));
            }
            ring_buffer_start_transmission(&uart_rb5);
            ring_buffer_put(&uart_rb1, !TRANSMIT, "\r\n");
        }
        ring_buffer_put(&uart_rb1, !TRANSMIT, "end\r\n");
        if (!uart_error)
        {
            ring_buffer_start_transmission(&uart_rb1);
        }
    }
}

void run_adc_sample(void)
{
    HAL_ADC_Start_DMA(&hadc2,(uint32_t *)Adc_Buff,FFT_Length);
    while (!Adc_Convert_End_Flag); 
    
    for (uint16_t i = 0; i < 200; i++)
    {
//        ring_buffer_put(&uart_rb1, TRANSMIT, "%.3f\n", Adc_Buff[i] * 3.3 / 4095);
        ring_buffer_put(&uart_rb1, TRANSMIT, "%3d\n", Adc_Buff[i]);
    }
}