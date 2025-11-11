#include "ap_axi_sdata.h"
#include "hls_stream.h"

struct pixel
{
   unsigned char ch1;
   unsigned char ch2;
   unsigned char ch3;
   bool last;
};

// Convert RGB image to Y'UV format
void rgb2yuv(hls::stream<ap_axis<24, 2, 5, 6>> &stream_in,
             hls::stream<pixel> &stream_out)
{

rgb2yuv_loop:
   while (1)
   {
      ap_axis<24, 2, 5, 6> data_in;

      stream_in.read(data_in);

      unsigned char R = data_in.data.range(23, 16);
      unsigned char G = data_in.data.range(15, 8);
      unsigned char B = data_in.data.range(7, 0);

      unsigned char Y = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16;
      unsigned char U = (((-38) * R + (-74) * G + 112 * B + 128) >> 8) + 128;
      unsigned char V = ((122 * R + (-94) * G + (-18) * B + 128) >> 8) + 128;

      pixel out;

      out.ch1 = Y;
      out.ch2 = U;
      out.ch3 = V;
      out.last = data_in.last;

      stream_out.write(out);

      if (data_in.last)
      {
         break;
      }
   }
}

void scale_y(
    hls::stream<pixel> &stream_in,
    hls::stream<pixel> &stream_out,
    float Y_scale)
{

   while (1)
   {
      pixel data;

      stream_in.read(data);

      unsigned char Y = data.ch1;
      unsigned char U = data.ch2;
      unsigned char V = data.ch3;

      Y = (unsigned char)(float(Y) * Y_scale);

      data.ch1 = Y;
      data.ch2 = U;
      data.ch3 = V;

      stream_out.write(data);

      if (data.last)
      {
         break;
      }
   }
}

// Convert YUV image to RGB format
void yuv2rgb(hls::stream<pixel> &stream_in,
             hls::stream<ap_axis<24, 2, 5, 6>> &stream_out)
{

   while (1)
   {
      pixel data;

      stream_in.read(data);

      unsigned char Y = data.ch1;
      unsigned char U = data.ch2;
      unsigned char V = data.ch3;

      unsigned char R = (298 * (Y - 16) + 409 * (V - 128) + 128) >> 8;
      unsigned char G = (298 * (Y - 16) + (-100) * (U - 128) + (-208) * (V - 128) + 128) >> 8;
      unsigned char B = (298 * (Y - 16) + 516 * (U - 128) + 128) >> 8;

      ap_axis<24, 2, 5, 6> data_out;

      data_out.data.range(23, 16) = Y;
      data_out.data.range(15, 8) = U;
      data_out.data.range(7, 0) = V;
      data_out.keep = -1;
      data_out.last = data.last;

      stream_out.write(data_out);

      if (data.last)
      {
         break;
      }
   }
}

extern "C"
{
   void yuv_filter(
       hls::stream<ap_axis<24, 2, 5, 6>> &stream_in,
       hls::stream<ap_axis<24, 2, 5, 6>> &stream_out,
       float scale_Y)
   {
#pragma HLS INTERFACE axis port = stream_in
#pragma HLS INTERFACE axis port = stream_out
      // #pragma HLS INTERFACE s_axilite port = width
      // #pragma HLS INTERFACE s_axilite port = height

      hls::stream<pixel> stream_yuv;
      hls::stream<pixel> stream_scale;

      rgb2yuv(stream_in, stream_yuv);
      scale_y(stream_yuv, stream_scale, scale_Y);
      yuv2rgb(stream_scale, stream_out);
   }
}
