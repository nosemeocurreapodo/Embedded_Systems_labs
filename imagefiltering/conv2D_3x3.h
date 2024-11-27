#pragma once

#include "ap_axi_sdata.h"
//#include "ap_int.h"
#include "ap_fixed.h"
#include "ap_shift_reg.h"
#include "hls_stream.h"
#include "data_types.h"
#include "types.h"

#define MAX_WIDTH 640

extern int conv2D_3x3(hls::stream<packet> &input, hls::stream<packet> &output, int &in_width, float kernel[9]);
