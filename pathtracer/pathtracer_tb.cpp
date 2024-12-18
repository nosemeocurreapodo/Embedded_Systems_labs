#include <iostream>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "pathtracer.h"

int main() 
{
    int width = 50;
    int height = 50;
    int samples_per_pixel = 2;

    cv::Mat image(height, width, CV_8UC3, cv::Scalar(0, 0, 0));

	hls::stream<packet> r_s_out, g_s_out, b_s_out;
    
    pathtracer_compute(r_s_out, g_s_out, b_s_out, width, height, samples_per_pixel);

    for (int y = 0; y < height; y++)
	{
        for(int x = 0; x < width; x++)
        {
            packet r_packet, g_packet, b_packet;
            r_s_out.read(r_packet);
            g_s_out.read(g_packet);
            b_s_out.read(b_packet);

            cv::Vec3b pix;
            pix.val[2] = (unsigned char)(r_packet.data);
            pix.val[1] = (unsigned char)(g_packet.data);
            pix.val[0] = (unsigned char)(b_packet.data);

            image.at<cv::Vec3b>(y, x) = pix;
        }
	}

    cv::imwrite("pathtracer_ouput.png", image);

    return 0;
}