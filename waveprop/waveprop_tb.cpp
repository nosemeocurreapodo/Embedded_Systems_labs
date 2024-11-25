#include <iostream>
#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "waveprop.h"

int main() 
{
    cv::Mat output_buffer(size, size, CV_8UC1, cv::Scalar(0));

	hls::stream<packet> s_out;
    
    for(int i = 0; i < 100; i++)
    {
        waveprop(s_out);

        for (int y = 0; y < size; y++)
        {
            for(int x = 0; x < size; x++)
            {
                packet out_packet;
                s_out.read(out_packet);

                output_buffer.at<unsigned char>(y, x) = (unsigned char)((float(out_packet.data) + 1.0)*255.0/2.0);
            }
        }
    }

    cv::imwrite("waveprop_ouput.png", output_buffer);

    return 0;
}