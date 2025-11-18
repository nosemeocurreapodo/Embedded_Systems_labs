
#include "ap_axi_sdata.h"
#include "hls_stream.h"

#define grid_width 1024
#define grid_height 1024

template <typename T, int width>
class fifo
{
public:
    fifo()
    {
    }

    T shift(T new_data)
    {
        T last_data = data[width - 1];
    fifo_loop:
        for (int i = width - 1; i > 0; i--)
        {
            data[i] = data[i - 1];
        }
        data[0] = new_data;
        return last_data;
    }

private:
    T data[width];
};

extern "C"
{
    int gameoflife_compute(
        hls::stream<ap_axis<32, 2, 5, 6>> &stream_in,
        hls::stream<ap_axis<32, 2, 5, 6>> &stream_out)
    {
#pragma HLS INTERFACE axis port = stream_in
#pragma HLS INTERFACE axis port = stream_out
#pragma HLS INTERFACE s_axilite port = return

        int mat3[9];

        fifo<int, grid_width - 1> line_1;
        fifo<int, grid_width - 1> line_2;

    gameoflife_y_loop:
        for (int y = -1; y <= grid_height; y++)
        {
        gameoflife_x_loop:
            for (int x = -1; x <= grid_width; x++)
            {
                ap_axis<32, 2, 5, 6> package;
                int data = 0;

                if (y >= 0 && x >= 0 && y < grid_height && x < grid_width)
                {
                    stream_in.read(package);
                    data = package.data;
                }

                int last_line_1 = line_1.shift(mat3[2]);
                int last_line_2 = line_2.shift(mat3[5]);

                mat3[8] = mat3[7];
                mat3[7] = mat3[6];
                mat3[6] = last_line_2;

                mat3[5] = mat3[4];
                mat3[4] = mat3[3];
                mat3[3] = last_line_1;

                mat3[2] = mat3[1];
                mat3[1] = mat3[0];
                mat3[0] = data;

                // Compute the sum of the 8 neighbors
                int total = 0;
            compute_sum_loop:
                for (int i = 0; i < 9; i++)
                {
                    if (i == 4)
                        continue;
                    total += mat3[i];
                }

                int old_val = mat3[4];
                int new_val = old_val;
                // Apply Conway's Game of Life rules
                if (old_val == 1)
                {
                    if (total < 2 || total > 3)
                    {
                        new_val = 0;
                    }
                }
                else
                {
                    if (total == 3)
                    {
                        new_val = 1;
                    }
                }

                if (y >= 1 && x >= 1)
                {
                    package.data = new_val;
                    stream_out.write(package);
                }
            }
        }

        return 0;
    }
}
