#include "conv2D_3x3.h"

int conv2D_3x3(hls::stream<packet> &input, hls::stream<packet> &output, int &in_width, float kernel_data[3 * 3])
{
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
#pragma HLS INTERFACE s_axilite port = in_width
#pragma HLS INTERFACE s_axilite port = kernel_data
#pragma HLS INTERFACE s_axilite port = return

    shift_register<data_type, MAX_WIDTH*3> shift_reg;
    // #pragma HLS ARRAY_PARTITION variable = in_buffer complete dim = 2

    mat3<data_type> kernel;
#pragma HLS ARRAY_PARTITION variable = kernel.data dim = 0 type = complete

    // initialize buffers

init_buffer_loop:
    for (int i = 0; i < 3 * MAX_WIDTH; i++)
    {
        shift_reg.shift_down(0);
    }

init_kernel_y_loop:
    for (int y = 0; y < 3; y++)
    {
#pragma HLS UNROLL
    init_kernel_x_loop:
        for (int x = 0; x < 3; x++)
        {
#pragma HLS UNROLL
            kernel.data[y][x] = kernel_data[x + y * 3];
        }
    }

    bool last_was_read = false;
    int data_counter = 0;
    int padding_data_counter = 0;

main_loop:
    while (true)
    {
// #pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min = 307200 max = 307200

        data_type in_data = data_type(0.0);
        if (!last_was_read)
        {
            packet in_packet;
            input.read(in_packet);
            in_data = data_type(in_packet.data);
            if (in_packet.last == 1)
                last_was_read = true;
            data_counter++;
        }
        else
        {
            padding_data_counter++;
        }

        shift_reg.shift_down(in_data);

        if (data_counter <= MAX_WIDTH*3 - in_width - 1)
            continue;

        if (padding_data_counter > MAX_WIDTH*3 - in_width - 1)
            break;

        mat3<data_type> data = shift_reg.getMat3(in_width);
        data_type conv = data.mul_v2(kernel);

        packet out_packet;
        out_packet.data = conv;
        output.write(out_packet);
    }

    return 1;
}