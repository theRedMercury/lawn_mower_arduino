/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "adc_manager.hpp"
#include "tools.hpp"

#define CHANNELS 8 // MAX 16

//#ifndef cbi
//#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
//#endif
//#ifndef sbi
//#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
//#endif

struct PACKING adc_channel
{
    unsigned short *values = NULL;
    volatile unsigned char value_size = 0;
};

volatile bool _is_capturing;
volatile unsigned char _current_cha_index;
volatile unsigned char _current_position;
volatile adc_channel _analogChannels[CHANNELS];

void adc_manager::define_channel_to_capture(const unsigned char channel, const unsigned char sampleCount,
                                            const adc_manager_div_factor div_factor,
                                            const adc_manager_ref_volt ref_volt)
{
    _is_capturing = false;
    _current_cha_index = (channel - PIN_A0);
    _current_position = 0;

    if (sampleCount == 0)
    {
        return;
    }

    if (sampleCount > _analogChannels[_current_cha_index].value_size)
    {
        free(_analogChannels[_current_cha_index].values);
        _analogChannels[_current_cha_index].values = new unsigned short[sampleCount];
    }
    _analogChannels[_current_cha_index].value_size = sampleCount;
    memset(_analogChannels[_current_cha_index].values, 0, _analogChannels[_current_cha_index].value_size);

    // ADMUX register
    // REFS1 | REFS0 | ADLAR | - | MUX3 | MUX2 | MUX1 | MUX0
    ADMUX = 0x00;
    ADMUX |= _current_cha_index & 0x0F;
    ADMUX |= (0 << ADLAR);
    ADMUX |= ref_volt << 6;

    // ADCSRB register
    ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((_current_cha_index >> 3) & 0x01) << MUX5); // PIN >7 for MEGA

    // ADCSRA register
    // ADEN | ADCS | ADATE | ADIF | ADIE | ADPS2 | ADPS1 | ADPS0
    // ADEN  : enable ADC
    // ADCS  : start ADC measurements
    // ADATE : enabble auto trigger
    // ADIF  : interrupt Flag
    // ADIE  : enable interrupts when measurement complete
    // ADPSX : ADPS2 | ADPS1 | ADPS0 (check adc_manager_div_factor)
    ADCSRA = 0x00;
    ADCSRA |= (div_factor << 0);
    ADCSRA |= _BV(ADEN) | _BV(ADSC) | _BV(ADATE) | _BV(ADIE);

    //  https://github.com/kwrtz/PerimeterSensorUNO/tree/5fc35fbd4e88e39873c805ead79a658d8e2fd3ea
    _is_capturing = true;
}

unsigned short *adc_manager::analogue_reads_channel(const unsigned char channel,
                                                    const unsigned char sample,
                                                    const adc_manager_div_factor div_factor,
                                                    const adc_manager_ref_volt ref_volt)
{
    adc_manager::define_channel_to_capture(channel, sample, div_factor, ref_volt);
    while (_is_capturing)
    {
        delayMicroseconds(10);
    }
    return _analogChannels[channel - PIN_A0].values;
}

unsigned short adc_manager::analogue_read_channel(const unsigned char channel, const adc_manager_ref_volt ref_volt)
{
    adc_manager::define_channel_to_capture(channel, 1, adc_div_factor_128, ref_volt);
    while (_is_capturing)
    {
        delayMicroseconds(10);
    }
    return _analogChannels[channel - PIN_A0].values[0];
}

unsigned short adc_manager::get_avg_channel_value(const unsigned char channel)
{
    unsigned int avg_value = 0;
    const unsigned char cha_index = channel - PIN_A0;
    for (unsigned char i = 0; i < _analogChannels[cha_index].value_size; i++)
    {
        avg_value += _analogChannels[cha_index].values[i];
    }
    return round(static_cast<float>(avg_value) / static_cast<float>(_analogChannels[cha_index].value_size));
}

bool get_min_max_channel_value(const unsigned char channel,
                               unsigned short &min, unsigned short &max)
{
    bool is_updated = false;
    const unsigned char cha_index = channel - PIN_A0;
    for (unsigned char i = 0; i < _analogChannels[cha_index].value_size; i++)
    {
        const unsigned short v = _analogChannels[cha_index].values[i];
        if (v < min)
        {
            min = v;
            is_updated = true;
        }
        if (v > max)
        {
            max = v;
            is_updated = true;
        }
    }
    return is_updated;
}

// free running ADC fills capture buffer
ISR(ADC_vect)
{
    volatile const unsigned char adc_low = ADCL;
    volatile const unsigned char adc_high = ADCH;

    if (!_is_capturing)
    {
        return;
    }

    if (_current_position >= _analogChannels[_current_cha_index].value_size)
    {
        _is_capturing = false;
        ADCSRA = 0x00;
        ADMUX = 0x00;
        return;
    }

    _analogChannels[_current_cha_index].values[_current_position] = (adc_high << 8) | adc_low;
    _current_position++;
}
