#include <iostream>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "pathtracer.h"

int main() 
{
    int width = 200;
    int height = 200;
    int samples_per_pixel = 10;

    cv::Mat r_buffer(height, width, CV_8UC1, cv::Scalar(0));
    cv::Mat g_buffer(height, width, CV_8UC1, cv::Scalar(0));
    cv::Mat b_buffer(height, width, CV_8UC1, cv::Scalar(0));

	hls::stream<packet> r_s_out, g_s_out, b_s_out;
    
    render(r_s_out, g_s_out, b_s_out, width, height, samples_per_pixel);

    for (int y = 0; y < height; y++)
	{
        for(int x = 0; x < width; x++)
        {
            packet r_packet, g_packet, b_packet;
            r_s_out.read(r_packet);
            g_s_out.read(g_packet);
            b_s_out.read(b_packet);

            r_buffer.at<unsigned char>(y, x) = r_packet.data;
            g_buffer.at<unsigned char>(y, x) = g_packet.data;
            b_buffer.at<unsigned char>(y, x) = b_packet.data;
        }
	}

    cv::imwrite("pathtracer_ouput.png", r_buffer);

    return 0;
}