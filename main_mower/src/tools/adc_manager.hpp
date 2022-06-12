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
    typedef enum
    {
        adc_ref_internal_ref = 0b11,
        adc_ref_default = 0b01,
        adc_ref_external = 0b00,
    } adc_manager_ref_volt;

    typedef enum
    {
        adc_div_factor_128 = 0b111,
        adc_div_factor_64 = 0b110,
        adc_div_factor_32 = 0b101,
        adc_div_factor_16 = 0b100,
        adc_div_factor_8 = 0b011,
        adc_div_factor_4 = 0b010,
        adc_div_factor_2 = 0b001,
    } adc_manager_div_factor;

    static unsigned short *analogue_reads_channel(const unsigned char channel,
                                                  const unsigned char sample,
                                                  const adc_manager_div_factor div_factor,
                                                  const adc_manager_ref_volt ref_volt = adc_ref_default);

    static unsigned short analogue_read_channel(const unsigned char channel,
                                                const adc_manager_ref_volt ref_volt = adc_ref_default);

    // not used
    static unsigned short get_avg_channel_value(const unsigned char channel);
    static bool get_min_max_channel_value(const unsigned char channel,
                                          unsigned short &min, unsigned short &max);

private:
    static void define_channel_to_capture(const unsigned char channel, const unsigned char sampleCount,
                                          const adc_manager_div_factor div_factor,
                                          const adc_manager_ref_volt ref_volt);

    adc_manager() = delete;
    adc_manager(const adc_manager &) = delete;
    adc_manager(adc_manager &&) = delete;
    adc_manager &operator=(const adc_manager &) = delete;
    adc_manager &operator=(adc_manager &&) = delete;
};

#endif