#include "system.h"

/**
*   @author     YujiaZhang
*   @brief      使用Direct Memory Access的方式，进行不定长数据的接收与发送；
*   @attention  
*   
*/

// 全局变量定义区
uint8_t rx_buffer1[MAX_MESSAGE_LENGTH];
uint8_t rx_buffer2[MAX_MESSAGE_LENGTH];
ring_buffer_t uart_rb1,uart_rb3, uart_rb5;

// 初始化环形队列
void ring_buffer_init(ring_buffer_t *rb, UART_HandleTypeDef *huart)
{
    memset(rb, 0, sizeof(ring_buffer_t));
    rb->head = 0;
    rb->tail = 0;
    rb->is_transmitting = 0;
    rb->huart = huart;
}

// 检查队列是否为空
uint8_t ring_buffer_is_empty(ring_buffer_t *rb)
{
    return (rb->head == rb->tail);
}

// 检查队列是否已满
uint8_t ring_buffer_is_full(ring_buffer_t *rb)
{
    return ((rb->head + 1) % RING_BUFFER_SIZE == rb->tail);
}

// 获取队列中可用的消息数量
uint16_t ring_buffer_available(ring_buffer_t *rb)
{
    if (rb->head >= rb->tail) {
        return rb->head - rb->tail;
    } else {
        return RING_BUFFER_SIZE - rb->tail + rb->head;
    }
}

// 格式化字符串方式放入队列
uint8_t ring_buffer_put(ring_buffer_t *rb, uint8_t transmit_flag, const char *format, ...)
{
    // 检查队列是否已满
    if (ring_buffer_is_full(rb)) {
        return 0; // 队列满，放入失败
    }
    
    va_list args;
    va_start(args, format);
    
    // 使用vsnprintf安全地格式化字符串
    uint16_t msg_len = vsnprintf((char*)rb->messages[rb->head].data, 
                                MAX_MESSAGE_LENGTH, format, args);
    
    va_end(args);
    
    // 确保字符串以null结尾，并计算实际长度（不包括null）
    if (msg_len >= MAX_MESSAGE_LENGTH) {
        msg_len = MAX_MESSAGE_LENGTH - 1;
        rb->messages[rb->head].data[msg_len] = '\0';
    }
    
    rb->messages[rb->head].length = msg_len;
    
    // 更新头指针
    rb->head = (rb->head + 1) % RING_BUFFER_SIZE;
    
    // 如果DMA空闲，立即启动传输
    if (!rb->is_transmitting && transmit_flag) {
        ring_buffer_start_transmission(rb);
    }
    
    return 1; // 成功放入
}

// 放入原始数据
uint8_t ring_buffer_put_data(ring_buffer_t *rb, uint8_t transmit_flag, const uint8_t *data, uint16_t len)
{
    if (ring_buffer_is_full(rb) || len > MAX_MESSAGE_LENGTH) {
        return 0; // 队列满或数据过长
    }
    
    memcpy(rb->messages[rb->head].data, data, len);
    rb->messages[rb->head].length = len;
    
    // 更新头指针
    rb->head = (rb->head + 1) % RING_BUFFER_SIZE;
    
    // 如果DMA空闲，立即启动传输
    if (!rb->is_transmitting && transmit_flag) {
        ring_buffer_start_transmission(rb);
    }
    
    return 1; // 成功放入
}


// 启动DMA传输
void ring_buffer_start_transmission(ring_buffer_t *rb)
{
    if (!ring_buffer_is_empty(rb) && !rb->is_transmitting) {
        rb->is_transmitting = 1;
        
        // 从队列中取出消息并启动DMA传输
        uart_message_t *msg = &rb->messages[rb->tail];
        HAL_UART_Transmit_DMA(rb->huart, msg->data, msg->length);
    }
}

// 清空队列
void ring_buffer_clear(ring_buffer_t *rb)
{
    rb->head = 0;
    rb->tail = 0;
    rb->is_transmitting = 0;
}

void ring_buffer_tx_complete_callback(ring_buffer_t *rb)
{
    if (rb->is_transmitting) {
        // 更新尾指针，表示该消息已发送完成
        rb->tail = (rb->tail + 1) % RING_BUFFER_SIZE;
        rb->is_transmitting = 0;
        
        // 检查是否还有更多消息需要发送
        ring_buffer_start_transmission(rb);
    }
}

// 串口不定长DMA发送回调
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == UART5) 
    {
        // 关键：清除传输标志，让队列可以发送下一条消息
        ring_buffer_tx_complete_callback(&uart_rb5);
    }
    if (huart->Instance == USART1) 
    {
        ring_buffer_tx_complete_callback(&uart_rb1);
    }
    if (huart->Instance == USART3) 
    {
        ring_buffer_tx_complete_callback(&uart_rb3);
    }
}