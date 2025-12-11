#ifndef HUMID_H
#define HUMID_H

#include <stdint.h>

// DHT11 Return codes
#define DHTLIB_OK                0
#define DHTLIB_ERROR_CHECKSUM   -1
#define DHTLIB_ERROR_TIMEOUT    -2

// DHT11 Data structure
typedef struct {
    uint8_t humidity;
    uint8_t temperature;
} dht11_data_t;

// Functions
void humid_init(void);
void humid_start_signal(void);
uint8_t humid_check_response(void);
uint32_t humid_read_data(void);
uint8_t humid_get_data(uint32_t *humidity, uint32_t *temperature);

// Legacy functions (for compatibility)
void humid_port_init(void);
void humid_delay_ms(uint32_t ms);
void humid_delay_us(uint32_t us);
int8_t humid_read(dht11_data_t *data);

#endif

