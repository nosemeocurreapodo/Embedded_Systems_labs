#include "gameoflife.h"

// Global grid array
static bool grid[N][N];
static bool newGrid[N][N];
static int step = 0;

ap_uint<16> lfsr_random()
{
#pragma HLS INLINE off
    static ap_uint<16> lfsr = 0xACE1u; // Initial seed value (non-zero)

    // Tap positions for a 16-bit LFSR with a maximal length sequence
    bool bit = lfsr[15] ^ lfsr[13] ^ lfsr[12] ^ lfsr[10];

    // Shift left by 1 and insert the new bit
    lfsr = (lfsr << 1) | bit;

    return lfsr;
}

float lfsr_uniform_random()
{
    ap_uint<16> r = lfsr_random();
    return r / 65536.0f;
}

static void initialize_grid()
{
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            float r = lfsr_uniform_random();
            grid[i][j] = (r < 0.2) ? 1 : 0;
        }
    }
}

int gameoflife_compute(hls::stream<packet> &output_stream)
{
#pragma HLS INTERFACE mode = s_axilite port = return
#pragma HLS INTERFACE mode = axis port = output_stream

    if (step == 0)
    {
        initialize_grid();
    }

    // Create a copy of the current grid
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            newGrid[i][j] = grid[i][j];
        }
    }

    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            // Compute the sum of the 8 neighbors
            int total =
                grid[i][(j - 1 + N) % N] + grid[i][(j + 1) % N] +
                grid[(i - 1 + N) % N][j] + grid[(i + 1) % N][j] +
                grid[(i - 1 + N) % N][(j - 1 + N) % N] + grid[(i - 1 + N) % N][(j + 1) % N] +
                grid[(i + 1) % N][(j - 1 + N) % N] + grid[(i + 1) % N][(j + 1) % N];

            // Apply Conway's Game of Life rules
            if (grid[i][j] == 1)
            {
                if (total < 2 || total > 3)
                {
                    newGrid[i][j] = 0;
                }
            }
            else
            {
                if (total == 3)
                {
                    newGrid[i][j] = 1;
                }
            }
        }
    }

    // Update the original grid
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            grid[i][j] = newGrid[i][j];

            packet output;
            output.data = newGrid[i][j];
            output.keep = -1;
            output.strb = -1;
            // output.user = input_data.user;
            // output.last = input_data.last;
            // output.id = input_data.id;
            // output.dest = input_data.dest;

            if (i == N - 1 && j == N - 1)
                output.last = true;
            else
                output.last = false;

            output_stream.write(output);
        }
    }
    step++;

    return 0;
}
