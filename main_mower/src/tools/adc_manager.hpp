/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef ADCMANAGER_H
#define ADCMANAGER_H
#include <Arduino.h>
#include <stdint.h>

class adc_manager
{
public:
    static void define_channel_to_capture(const uint8_t channel, uint8_t sampleCount);
    static uint16_t analogue_read_channel(const uint8_t channel, const uint8_t sample = 8);
    static uint16_t *analogue_reads_channel(const uint8_t channel, const uint8_t sample = 8);
    static bool is_read_data_channel_done(const uint8_t channel);
    static uint16_t *read_data_channel(const uint8_t channel);
    static uint16_t get_avg_channel_value(const uint8_t channel);

private:
    adc_manager() = delete;
    adc_manager(const adc_manager &) = delete;
    adc_manager(adc_manager &&) = delete;
    adc_manager &operator=(const adc_manager &) = delete;
    adc_manager &operator=(adc_manager &&) = delete;
};

#endif