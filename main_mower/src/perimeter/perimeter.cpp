/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "perimeter.hpp"
#include "../mower/mower.hpp"
#include "../tools/adc_manager.hpp"

void perimeter::setup()
{
    DEBUG_PRINT("SETUP : ");
    DEBUG_PRINT(class_name);
    DEBUG_PRINTLN(" : DONE");
    // Serial.begin(115200);
}

void perimeter::update()
{
    // https://github.com/kwrtz/PerimeterSensorUNO
    // http://grauonline.de/alexwww/ardumower/filter/filter.html
    unsigned short *signal_read = adc_manager::analogue_reads_channel(PIN_A_WIRE, RAW_SIGNAL_SAMPLE_SIZE);

    // Normalize
    /*int16_t Hsum = 0;
    for (unsigned char i = 0; i < SENDER_ARRAY_SIZE; i++)
    {
        Hsum += abs(_signal_code[i]);
    }
    Hsum *= RAW_SIGNAL_SAMPLE_SIZE;*/

    // Low pass filter
    /*float weight = 0.1;
    for (unsigned char i = 1; i < RAW_SIGNAL_SAMPLE_SIZE; i++)
    {
        signal_read[i - 1] = (1.0 - weight) * signal_read[i - 1] + weight * signal_read[i]; // low-pass values
    }*/

    // Debug
    /*for (int i = 0; i < RAW_SIGNAL_SAMPLE_SIZE; i++)
    {
        Serial.println(signal_read[i]);
    }*/

    int sum_max = 0; // max correlation sum
    int sum_min = 0; // min correlation sum
    int sum = 0;

    for (unsigned char i = 0; i < CORELLATION_ARRAY_SIZE; i++)
    {
        sum = 0;
        for (unsigned char j = 0; j < SENDER_ARRAY_SIZE; j++)
        {
            sum += static_cast<short>(_signal_code[j]) * static_cast<short>(signal_read[i + j]);
        } // end inner loop
        _correlation_signal[i] = sum;
        if (sum > sum_max)
        {
            sum_max = sum;
        }
        if (sum < sum_min)
        {
            sum_min = sum;
        }
        // Divide by the length of the operator
    } // end outer loop

    // Serial.println("CORRELATION :");
    /*for (int i = 0; i < CORELLATION_ARRAY_SIZE; i++)
    {
      Serial.println(_correlation_signal[i]);
    }*/

    // normalize to 4095
    // Divide by the length of the operator = senderAnzEinsen
    // sum_min = static_cast<float>(sum_min) / static_cast<float>(Hsum * 127) * 4095.0;
    // sum_max = static_cast<float>(sum_max) / static_cast<float>(Hsum * 127) * 4095.0;

    if (sum_max > -sum_min)
    {
        _filter_quality = static_cast<float>(sum_max) / static_cast<float>(-sum_min);
        _magnitude = sum_max;
    }
    else
    {
        _filter_quality = static_cast<float>(-sum_min) / static_cast<float>(sum_max);
        _magnitude = sum_min;
    }

    _smooth_magnitude = 0.99 * _smooth_magnitude + 0.01 * static_cast<float>(abs(_magnitude));

    // perimeter inside/outside detection
    if (abs(_magnitude) < 2000 || isinf(_filter_quality) || _filter_quality > 20.f)
    {
        _signal_counter = min(_signal_counter + 1, 3);
    }
    else
    {
        _signal_counter = max(_signal_counter - 1, -3);
    }
    if (_signal_counter < 0)
    {
        _last_inside_time = millis();
    }

    if (abs(_magnitude) > 2000 && _filter_quality > 0.5f && _filter_quality <= 5.f)
    {
        // Large signal, the in/out detection is reliable.
        // Using mag yields very fast in/out transition reporting.
        _is_inside = (_magnitude < 0);
    }
    else
    {
        // Low signal, use filtered value for increased reliability
        _is_inside = (_signal_counter < 0);
    }
    adc_manager::clean_channel(PIN_A_WIRE);
}

void perimeter::set_simulate_is_inside(const bool simulate)
{
    _simulate_is_inside = simulate;
}

const bool perimeter::get_simulate_is_inside() const
{
    return _simulate_is_inside;
}

const short perimeter::get_magnitude() const
{
    return _magnitude;
}

const short perimeter::get_smooth_magnitude() const
{
    return _smooth_magnitude;
}

const float perimeter::get_filter_quality() const
{
    return _filter_quality;
}

const bool perimeter::is_inside() const
{
#ifdef DEBUG_SIMULATE_WIRE
    return true;
#endif
    if (_simulate_is_inside)
    {
        return true;
    }
    return _is_inside;
}

const bool perimeter::is_signal_timed_out() const
{
#ifdef DEBUG_SIMULATE_WIRE
    return false;
#endif
    if (_simulate_is_inside)
    {
        return false;
    }
    if (_smooth_magnitude < 300)
    {
        return true;
    }
    return (millis() - _last_inside_time > SIGNAL_TIME_OUT_MS);
}

const String perimeter::get_json() const
{
    return "{\"in\":\"" + String(is_inside()) + "\",\"mag\":\"" +
           String(_magnitude) + "\"}";
}
