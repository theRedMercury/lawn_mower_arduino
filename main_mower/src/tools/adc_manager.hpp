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
    static unsigned short *analogue_reads_channel(const unsigned char channel, const unsigned char sample = 8);
    static unsigned short *read_data_channel(const unsigned char channel);
    static const bool is_read_data_channel_done(const unsigned char channel);
    static const unsigned short analogue_read_channel(const unsigned char channel, const unsigned char sample = 8);
    static const unsigned short get_avg_channel_value(const unsigned char channel);

    static void clean_channel(const unsigned char channel);

private:
    static void define_channel_to_capture(const unsigned char channel, unsigned char sampleCount);

    adc_manager() = delete;
    adc_manager(const adc_manager &) = delete;
    adc_manager(adc_manager &&) = delete;
    adc_manager &operator=(const adc_manager &) = delete;
    adc_manager &operator=(adc_manager &&) = delete;
};

#endif