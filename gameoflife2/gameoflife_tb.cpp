#include <iostream>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "ap_fixed.h"

extern "C"
{
    int gameoflife_compute(bool *in_grid, bool *out_grid, unsigned int width, unsigned int height);
}

ap_uint<16> lfsr_random()
{
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

static void initialize_grid(bool *grid, unsigned int width, unsigned int height)
{
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            float r = lfsr_uniform_random();
            grid[i * width + j] = (r < 0.2) ? 1 : 0;
        }
    }
}

int main()
{
    int width = 1024;
    int height = 1024;

    bool grid1[width * height];
    bool grid2[width * height];

    initialize_grid(grid1, width, height);

    bool *grid = grid1;
    bool *back_grid = grid2;

    for (int i = 0; i < 1000; i++)
    {
        gameoflife_compute(grid, back_grid, width, height);
        //std::swap(grid, back_grid);
        bool *temp = grid;
        grid = back_grid;
        back_grid = temp;
    }

    cv::Mat output_buffer(height, width, CV_8UC1, cv::Scalar(0));

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            output_buffer.at<uchar>(y, x) = back_grid[y * width + x] * 255;
        }
    }

    cv::imwrite("gemeoflife_ouput.png", output_buffer);

    return 0;
}