/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "adc_manager.hpp"

#define CHANNELS 8 // MAX 16

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
//#ifndef sbi
//#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
//#endif

struct adc_channel
{
    unsigned short *values = NULL;
    volatile unsigned char value_size = 0;
};

volatile bool _is_capturing;
volatile unsigned char _current_cha_index;
volatile unsigned char _current_position;
volatile adc_channel _analogChannels[CHANNELS];

void adc_manager::define_channel_to_capture(const unsigned char channel, unsigned char sampleCount)
{
    _is_capturing = false;
    _current_cha_index = (channel - PIN_A0);
    _current_position = 0;
    sampleCount = constrain(sampleCount, 1, 255);

    if (sampleCount > _analogChannels[_current_cha_index].value_size)
    {
        free(_analogChannels[_current_cha_index].values);
        _analogChannels[_current_cha_index].values = new unsigned short[sampleCount];
    }
    _analogChannels[_current_cha_index].value_size = sampleCount;

    memset(_analogChannels[_current_cha_index].values, 0, _analogChannels[_current_cha_index].value_size);

    ADMUX = 0x00;
    ADMUX |= (1 << REFS0);
    ADMUX |= (0 << REFS1);
    ADMUX |= (0 << ADLAR);
    ADMUX |= _current_cha_index & 0x0F;

    // Start conversion
    ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((_current_cha_index >> 3) & 0x01) << MUX5);   // PIN >7 for MEGA
    ADCSRA = _BV(ADSC) | _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1); // prescaler 19231 Hz

    if (_current_cha_index < 8)
    {
        DIDR0 |= (1 << _current_cha_index);
    }
    else
    {
        DIDR2 |= (1 << (_current_cha_index - 8));
    }

    //  https://github.com/kwrtz/PerimeterSensorUNO/tree/5fc35fbd4e88e39873c805ead79a658d8e2fd3ea

    // PS_16 = (1 << ADPS2);
    // PS_32 = (1 << ADPS2) | (1 << ADPS0);                 prescaler 32 : 38462 Hz
    // PS_64 = (1 << ADPS2) | (1 << ADPS1);                 prescaler 64 : 19231 Hz
    // PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); prescaler 128 : 9615 Hz

    /*  REFS0 : VCC use as a ref, IR_AUDIO : channel selection, ADEN : ADC Enable, ADSC : ADC Start, ADATE : ADC Auto Trigger Enable, ADIE : ADC Interrupt Enable,  ADPS : ADC Prescaler  */
    // free running ADC mode, f = ( 16MHz / prescaler ) / 13 cycles per conversion
    _is_capturing = true;
}

unsigned short *adc_manager::analogue_reads_channel(const unsigned char channel, const unsigned char sample)
{
    adc_manager::define_channel_to_capture(channel, sample);
    while (_is_capturing)
    {
        delayMicroseconds(10);
    }
    return adc_manager::read_data_channel(channel);
}

unsigned short *adc_manager::read_data_channel(const unsigned char channel)
{
    if (_is_capturing)
    {
        return NULL;
    }
    return _analogChannels[channel - PIN_A0].values;
}

const unsigned short adc_manager::analogue_read_channel(const unsigned char channel, const unsigned char sample)
{
    adc_manager::define_channel_to_capture(channel, sample);
    while (_is_capturing)
    {
        delayMicroseconds(10);
    }
    return adc_manager::get_avg_channel_value(channel);
}

const unsigned short adc_manager::get_avg_channel_value(const unsigned char channel)
{
    unsigned int _value = 0;
    const unsigned char _channel = channel - PIN_A0;
    for (unsigned char i = 0; i < _analogChannels[_channel].value_size; i++)
    {
        _value += _analogChannels[_channel].values[i];
    }
    return round(static_cast<float>(_value) / static_cast<float>(_analogChannels[_channel].value_size));
}

// free running ADC fills capture buffer
ISR(ADC_vect)
{
    volatile unsigned char _adcl = ADCL;
    volatile unsigned char _adch = ADCH;

    if (!_is_capturing)
    {
        return;
    }

    if (_current_position >= _analogChannels[_current_cha_index].value_size)
    {
        _is_capturing = false;
        cbi(ADCSRA, ADSC);
        cbi(ADCSRA, ADEN);
        ADMUX = 0x00;
        return;
    }

    _analogChannels[_current_cha_index].values[_current_position] = (_adch << 8) | _adcl;
    _current_position++;
}
