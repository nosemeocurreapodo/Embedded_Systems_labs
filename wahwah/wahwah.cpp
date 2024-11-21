#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include "hls_math.h"

// AXI stream data type
typedef ap_axiu<32, 1, 1, 1> AXI_DATA;

// Wah-Wah filter parameters
#define SAMPLE_RATE 48000
#define MIN_FREQ 500.0f
#define MAX_FREQ 3000.0f
#define LFO_FREQ 0.2f // Frequency of the low-frequency oscillator (LFO)

// Filter state
static float previous_input = 0.0f;
static float previous_output = 0.0f;
static float previous_freq = 0.0f;

// Wah-Wah filter function
float wah_wah_filter(float input, float time) {
    float LFO = 0.5f * (1.0f + hls::sin(2.0f * 3.14159265358979f * LFO_FREQ * time)); // Normalized LFO
    float cutoff_freq = MIN_FREQ + LFO * (MAX_FREQ - MIN_FREQ);

    // Calculate RC filter coefficients
    float RC = 1.0f / (2.0f * 3.14159265358979f * cutoff_freq);
    float alpha = RC / (RC + 1.0f / SAMPLE_RATE);

    // Apply the filter
    float output = alpha * previous_output + (1.0f - alpha) * input;

    // Update previous states
    previous_output = output;
    previous_input = input;
    previous_freq = cutoff_freq;

    return output;
}

// Top-level function for Vitis HLS
void wah_wah_filter_axi(hls::stream<AXI_DATA>& input_stream, hls::stream<AXI_DATA>& output_stream) {
#pragma HLS INTERFACE axis port=input_stream
#pragma HLS INTERFACE axis port=output_stream
#pragma HLS INTERFACE ap_ctrl_none port=return

    AXI_DATA input_data;
    AXI_DATA output_data;
    static float time = 0.0f;

    // Process incoming data
    if (!input_stream.empty()) {
        input_data = input_stream.read();

        // Extract the input value
        float input_sample = *reinterpret_cast<float*>(&input_data.data);

        // Apply the Wah-Wah filter
        float filtered_sample = wah_wah_filter(input_sample, time);

        // Update the time for the LFO
        time += 1.0f / SAMPLE_RATE;

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
    }
}
