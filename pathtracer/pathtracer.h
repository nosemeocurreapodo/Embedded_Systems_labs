#pragma once

#include "ap_axi_sdata.h"
//#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_stream.h"
#include "hls_math.h"

#include "types.h"

typedef ap_axis<8, 2, 5, 6> packet;

extern void render(hls::stream<packet> &r_stream, hls::stream<packet> &g_stream, hls::stream<packet> &b_stream, int &width, int &height, int &samples_per_pixel);