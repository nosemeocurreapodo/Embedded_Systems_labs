#include "imagefiltering.h"

int imagefiltering_compute(hls::stream<packet> &input, hls::stream<packet> &output, int &in_width, int &in_height, float kernel_data[3 * 3])
{
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
#pragma HLS INTERFACE s_axilite port = in_width
#pragma HLS INTERFACE s_axilite port = kernel_data
#pragma HLS INTERFACE s_axilite port = return

    shift_register<data_type, MAX_WIDTH * 3> shift_reg;
    // #pragma HLS ARRAY_PARTITION variable = in_buffer complete dim = 2

    mat3<data_type> kernel;
#pragma HLS ARRAY_PARTITION variable = kernel.data dim = 0 type = complete

    // initialize buffers

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

init_buffer_loop:
    for (int i = 0; i < 3 * MAX_WIDTH; i++)
    {
        data_type in_data = data_type(0.0);
        if (!last_was_read)
        {
            packet in_packet;
            input.read(in_packet);
            in_data = data_type(in_packet.data);
            if (in_packet.last == 1)
                last_was_read = true;
        }

        shift_reg.shift_down(0);
    }

main_loop:
    for (int i = 0; i < in_width * in_height; i++)
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
        }

        shift_reg.shift_down(in_data);

        mat3<data_type> data = shift_reg.getMat3(in_width);
        data_type conv = data.mul_v2(kernel);

        packet out_packet;
        out_packet.data = conv;
        out_packet.keep = -1;
        out_packet.strb = -1;
        // output.user = input_data.user;
        // output.last = input_data.last;
        // output.id = input_data.id;
        // output.dest = input_data.dest;

        if (i == in_width * in_height - 1)
            out_packet.last = true;
        else
            out_packet.last = false;
            
        output.write(out_packet);
    }

    return 1;
}