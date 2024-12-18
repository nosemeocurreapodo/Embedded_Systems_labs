#include <iostream>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "fluidsimulation.h"

int main() 
{
    cv::Mat output_buffer(SIZE, SIZE, CV_8UC1, cv::Scalar(0));

	hls::stream<packet> s_out;
    
    for(int i = 0; i < 100; i++)
    {
        fluidsimulation_compute(s_out, i);

        for (int y = 0; y < SIZE; y++)
        {
            for(int x = 0; x < SIZE; x++)
            {
                packet out_packet;
                s_out.read(out_packet);

                output_buffer.at<unsigned char>(y, x) = (unsigned char)((float(out_packet.data) + 255)/2.0);
            }
        }
    }

    cv::imwrite("fluidsimulation_ouput.png", output_buffer);

    return 0;
}