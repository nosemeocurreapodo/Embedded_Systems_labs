#include <iostream>

#include <stdio.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "conv2D_3x3.h"

int main(void)
{
    cv::Mat inMat = cv::imread("/mnt/nvme0n1p5/datasets/desktop_dataset/images/scene_000.png", cv::IMREAD_GRAYSCALE);
    //cv::resize(imageMat, imageMat, cv::Size(width, height), cv::INTER_AREA);
    
    int width = inMat.cols;
    int height = inMat.rows;

    std::cout << "height " << height << " width " << width << std::endl;

    cv::Mat outMat(height, width, CV_32FC1, cv::Scalar(0));

	hls::stream<packet> s_in;
	hls::stream<packet> s_out;

    for (int y = 0; y < height; y++)
	{
        for(int x = 0; x < width; x++)
        {
            packet in_packet;

            in_packet.data = float(inMat.at<uchar>(y, x));
            in_packet.last = false;
            in_packet.keep = -1;
            if (x == width - 1 && y == height - 1)
                in_packet.last = true;
            s_in.write(in_packet);
        }
	}

    float kernel[3*3] = {0};
/*
    kernel[0] = 1.0;
    kernel[1] = 1.0;
    kernel[2] = 1.0;

    kernel[3] = 0.0;
    kernel[4] = 0.0;
    kernel[5] = 0.0;

    kernel[6] = -1.0;
    kernel[7] = -1.0;
    kernel[8] = -1.0;
*/

    kernel[4] = 1.0;
/*
    kernel[0] = 1.0/9.0;
    kernel[1] = 1.0/9.0;
    kernel[2] = 1.0/9.0;

    kernel[3] = 1.0/9.0;
    kernel[4] = 1.0/9.0;
    kernel[5] = 1.0/9.0;

    kernel[6] = 1.0/9.0;
    kernel[7] = 1.0/9.0;
    kernel[8] = 1.0/9.0;
*/
    conv2D_3x3(s_in, s_out, width, kernel); 

    for (int y = 0; y < height; y++)
	{
        for(int x = 0; x < width; x++)
        {
            packet out_packet;
            s_out.read(out_packet);

            outMat.at<float>(y, x) = out_packet.data;
        }
	}

    cv::imwrite("scene_000_filtered.png", outMat);

    return 0;
}
