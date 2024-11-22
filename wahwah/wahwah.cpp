#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include "hls_math.h"

// AXI stream data type
typedef ap_axiu<32, 1, 1, 1> AXI_DATA;

// Wah-Wah filter parameters
//#define SAMPLE_RATE 48000
//#define MIN_FREQ 500.0f
//#define MAX_FREQ 3000.0f
//#define LFO_FREQ 0.2f // Frequency of the low-frequency oscillator (LFO)

// Filter state
static float previous_input = 0.0f;
static float previous_output = 0.0f;
static float previous_freq = 0.0f;

// Wah-Wah filter function
float wah_wah_filter(float input, float time, float sample_rate, float min_freq, float max_freq, float lfo_freq) {
    float LFO = 0.5f * (1.0f + hls::sin(2.0f * 3.14159265358979f * lfo_freq * time)); // Normalized LFO
    float cutoff_freq = min_freq + LFO * (max_freq - min_freq);

    // Calculate RC filter coefficients
    float RC = 1.0f / (2.0f * 3.14159265358979f * cutoff_freq);
    float alpha = RC / (RC + 1.0f / sample_rate);

    // Apply the filter
    float output = alpha * previous_output + (1.0f - alpha) * input;

    // Update previous states
    previous_output = output;
    previous_input = input;
    previous_freq = cutoff_freq;

    return output;
}

// Top-level function for Vitis HLS
void wah_wah_filter_axi(hls::stream<AXI_DATA>& input_stream, hls::stream<AXI_DATA>& output_stream, int &sample_rate, float &min_freq, float &max_freq, float &lfo_freq) 
{
#pragma HLS INTERFACE axis port=input_stream
#pragma HLS INTERFACE axis port=output_stream
#pragma HLS INTERFACE s_axilite port = sample_rate
#pragma HLS INTERFACE s_axilite port = min_freq
#pragma HLS INTERFACE s_axilite port = max_freq
#pragma HLS INTERFACE s_axilite port = lfo_freq
#pragma HLS INTERFACE s_axilite port = return

    AXI_DATA input_data;
    AXI_DATA output_data;
    static float time = 0.0f;

    // Process incoming data
    while (true) 
    {
        input_data = input_stream.read();

        // Extract the input value
        float input_sample = *reinterpret_cast<float*>(&input_data.data);

        // Apply the Wah-Wah filter
        float filtered_sample = wah_wah_filter(input_sample, time, sample_rate, min_freq, max_freq, lfo_freq);

        // Update the time for the LFO
        time += 1.0f / sample_rate;

        // Prepare output data
        output_data.data = *reinterpret_cast<uint32_t*>(&filtered_sample);
        output_data.keep = input_data.keep;
        output_data.strb = input_data.strb;
        output_data.user = input_data.user;
        output_data.last = input_data.last;
        output_data.id = input_data.id;
        output_data.dest = input_data.dest;

        // Write to the output stream
        output_stream.write(output_data);

        if(input_data.last)
            break;
    }
}
