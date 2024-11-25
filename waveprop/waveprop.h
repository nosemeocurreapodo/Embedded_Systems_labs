#pragma once

#include "ap_axi_sdata.h"
//#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_stream.h"
#include "hls_math.h"

//#include "types.h"

#define size 100

typedef  hls::axis<float, 0, 0, 0, (AXIS_ENABLE_KEEP | AXIS_ENABLE_LAST | AXIS_ENABLE_STRB), false> packet;

extern int waveprop(hls::stream<packet> &output_stream);