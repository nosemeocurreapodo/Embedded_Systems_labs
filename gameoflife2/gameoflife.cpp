
#include "ap_fixed.h"

extern "C"
{
    int gameoflife_compute(bool *in_grid, bool *out_grid, unsigned int grid_width, unsigned int grid_height)
    {
#pragma HLS INTERFACE m_axi port = in_grid bundle = gmem0
#pragma HLS INTERFACE m_axi port = out_grid bundle = gmem1
#pragma HLS INTERFACE mode = s_axilite port = grid_width
#pragma HLS INTERFACE mode = s_axilite port = grid_height
#pragma HLS INTERFACE mode = s_axilite port = return

        int max_add = grid_width * grid_height;

    gameoflife_i_loop:
        for (int i = 0; i < grid_height; ++i)
        {
#pragma HLS loop_tripcount min = 1024 max = 1024 avg = 1024

        gameoflife_j_loop:
            for (int j = 0; j < grid_width; ++j)
            {
#pragma HLS loop_tripcount min = 1024 max = 1024 avg = 1024

                int add = i * grid_width + j;
                int add_up = (i + 1) * grid_width + j;
                int add_down = (i - 1) * grid_width + j;
                int add_left = i * grid_width + (j - 1);
                int add_right = i * grid_width + (j + 1);
                int add_up_left = (i + 1) * grid_width + (j - 1);
                int add_up_right = (i + 1) * grid_width + (j + 1);
                int add_down_left = (i - 1) * grid_width + (j - 1);
                int add_down_right = (i - 1) * grid_width + (j + 1);

                bool old_val = in_grid[add];
                bool new_val = old_val;

                if (add_up < 0 || add_up >= max_add ||
                    add_down < 0 || add_down >= max_add ||
                    add_left < 0 || add_left >= max_add ||
                    add_right < 0 || add_right >= max_add ||
                    add_up_left < 0 || add_up_left >= max_add ||
                    add_up_right < 0 || add_up_right >= max_add ||
                    add_down_left < 0 || add_down_left >= max_add ||
                    add_down_right < 0 || add_down_right >= max_add)
                {
                    new_val = old_val;
                }
                else
                {
                    // Compute the sum of the 8 neighbors
                    int total = in_grid[add_up] + in_grid[add_down] + in_grid[add_left] + in_grid[add_right] +
                                in_grid[add_up_left] + in_grid[add_up_right] + in_grid[add_down_left] + in_grid[add_down_right];

                    // Apply Conway's Game of Life rules
                    if (old_val == 1)
                    {
                        if (total < 2 || total > 3)
                        {
                            new_val = 0;
                        }
                    }
                    else
                    {
                        if (total == 3)
                        {
                            new_val = 1;
                        }
                    }
                }

                out_grid[add] = new_val;
            }
        }

        return 0;
    }
}
