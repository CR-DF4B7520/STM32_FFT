#include "main.h"
#include "usart.h"

#define RING_BUFFER_SIZE     64     // 队列容量
#define MAX_MESSAGE_LENGTH   128    // 单条消息最大长度
#define TRANSMIT    1

// 消息结构体
typedef struct {
    uint8_t data[MAX_MESSAGE_LENGTH];
    uint16_t length;
} uart_message_t;

// 环形队列结构体
typedef struct {
    uart_message_t messages[RING_BUFFER_SIZE];
    volatile uint16_t head;          // 写入位置
    volatile uint16_t tail;          // 读取位置
    volatile uint8_t is_transmitting; // 传输状态标志
    UART_HandleTypeDef *huart;       // UART句柄
} ring_buffer_t;

extern uint8_t rx_buffer1[MAX_MESSAGE_LENGTH];
extern uint8_t rx_buffer2[MAX_MESSAGE_LENGTH];
extern ring_buffer_t uart_rb1, uart_rb3, uart_rb5;

void ring_buffer_init(ring_buffer_t *rb, UART_HandleTypeDef *huart);
uint8_t ring_buffer_put(ring_buffer_t *rb, uint8_t transmit_flag, const char *format, ...);
uint8_t ring_buffer_put_data(ring_buffer_t *rb, uint8_t transmit_flag, const uint8_t *data, uint16_t len);
void ring_buffer_start_transmission(ring_buffer_t *rb);
uint8_t ring_buffer_is_empty(ring_buffer_t *rb);
uint8_t ring_buffer_is_full(ring_buffer_t *rb);
uint16_t ring_buffer_available(ring_buffer_t *rb);
void ring_buffer_clear(ring_buffer_t *rb);
void ring_buffer_tx_complete_callback(ring_buffer_t *rb);
