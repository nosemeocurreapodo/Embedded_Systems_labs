#include <iostream>
#include <stdio.h>
#include <ap_fixed.h>
#include <ap_axi_sdata.h>
#include <hls_stream.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

extern "C"
{
    void yuv_filter(
        hls::stream<ap_axis<32, 2, 5, 6>> &stream_in,
        hls::stream<ap_axis<32, 2, 5, 6>> &stream_out,
        float scale_Y);
}

int main()
{
    const char *path = "/home/emanuel/workspace/Embedded_Systems_labs/yuv_filter/input.jpg";
    int width;
    int height;
    int channels_file;

    unsigned char *rgb_in = stbi_load(path, &width, &height, &channels_file, /*req_channels=*/3);
    if (!rgb_in)
    {
        std::fprintf(stderr, "stb_image load error: %s\n", stbi_failure_reason());
        return 1;
    }

    hls::stream<ap_axis<32, 2, 5, 6>> stream_in;
    hls::stream<ap_axis<32, 2, 5, 6>> stream_out;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            ap_axis<32, 2, 5, 6> tmp;
            tmp.data.range(31, 24) = 0;
            tmp.data.range(23, 16) = rgb_in[y * width * 3 + x * 3 + 0];
            tmp.data.range(15, 8) = rgb_in[y * width * 3 + x * 3 + 1];
            tmp.data.range(7, 0) = rgb_in[y * width * 3 + x * 3 + 2];
            tmp.keep = -1;
            if (x == width - 1 && y == height - 1)
                tmp.last = true;
            else
                tmp.last = false;
            stream_in.write(tmp);
        }
    }

    yuv_filter(stream_in, stream_out, 1.2f);

    unsigned char rgb_out[width * height * 3];

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            ap_axis<32, 2, 5, 6> tmp;
            stream_out.read(tmp);
            rgb_out[y * width * 3 + x * 3 + 0] = tmp.data.range(23, 16);
            rgb_out[y * width * 3 + x * 3 + 1] = tmp.data.range(15, 8);
            rgb_out[y * width * 3 + x * 3 + 2] = tmp.data.range(7, 0);
        }
    }

    stbi_write_png("out.png", width, height, 3,
                   rgb_out, width * 3);

    return 0;
}