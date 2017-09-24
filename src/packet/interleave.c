/* @file interleave.c
 * @addtogroup packet
 * @{
 */

#include "../firmware.h"

#define swap(a0, a1, j, m) t = (a0 ^ (a1 >> j)) & m; \
                           a0 = a0 ^ t; \
                           a1 = a1 ^ (t << j)

/**
 * @brief Interleave (transpose) a 32x32 bit buffer.
 *
 * @description Implementation from Hacker's Delight, by Henry S. Warren
 *
 * @param buffer Pointer to the buffer to be interleaved.
 */
void packet_interleave_32x32(uint8_t *buffer)
{
   uint32_t m, t;
   uint32_t *A = (uint32_t*)buffer;

   m = 0x0000FFFF;
   swap(A[0],  A[16], 16, m);
   swap(A[1],  A[17], 16, m);
   swap(A[2],  A[18], 16, m);
   swap(A[3],  A[19], 16, m);
   swap(A[4],  A[20], 16, m);
   swap(A[5],  A[21], 16, m);
   swap(A[6],  A[22], 16, m);
   swap(A[7],  A[23], 16, m);
   swap(A[8],  A[24], 16, m);
   swap(A[9],  A[25], 16, m);
   swap(A[10], A[26], 16, m);
   swap(A[11], A[27], 16, m);
   swap(A[12], A[28], 16, m);
   swap(A[13], A[29], 16, m);
   swap(A[14], A[30], 16, m);
   swap(A[15], A[31], 16, m);
   m = 0x00FF00FF;
   swap(A[0],  A[8],   8, m);
   swap(A[1],  A[9],   8, m);
   swap(A[2],  A[10],  8, m);
   swap(A[3],  A[11],  8, m);
   swap(A[4],  A[12],  8, m);
   swap(A[5],  A[13],  8, m);
   swap(A[6],  A[14],  8, m);
   swap(A[7],  A[15],  8, m);
   swap(A[16], A[24],  8, m);
   swap(A[17], A[25],  8, m);
   swap(A[18], A[26],  8, m);
   swap(A[19], A[27],  8, m);
   swap(A[20], A[28],  8, m);
   swap(A[21], A[29],  8, m);
   swap(A[22], A[30],  8, m);
   swap(A[23], A[31],  8, m);
   m = 0x0F0F0F0F;
   swap(A[0],  A[4],   4, m);
   swap(A[1],  A[5],   4, m);
   swap(A[2],  A[6],   4, m);
   swap(A[3],  A[7],   4, m);
   swap(A[8],  A[12],  4, m);
   swap(A[9],  A[13],  4, m);
   swap(A[10], A[14],  4, m);
   swap(A[11], A[15],  4, m);
   swap(A[16], A[20],  4, m);
   swap(A[17], A[21],  4, m);
   swap(A[18], A[22],  4, m);
   swap(A[19], A[23],  4, m);
   swap(A[24], A[28],  4, m);
   swap(A[25], A[29],  4, m);
   swap(A[26], A[30],  4, m);
   swap(A[27], A[31],  4, m);
   m = 0x33333333;
   swap(A[0],  A[2],   2, m);
   swap(A[1],  A[3],   2, m);
   swap(A[4],  A[6],   2, m);
   swap(A[5],  A[7],   2, m);
   swap(A[8],  A[10],  2, m);
   swap(A[9],  A[11],  2, m);
   swap(A[12], A[14],  2, m);
   swap(A[13], A[15],  2, m);
   swap(A[16], A[18],  2, m);
   swap(A[17], A[19],  2, m);
   swap(A[20], A[22],  2, m);
   swap(A[21], A[23],  2, m);
   swap(A[24], A[26],  2, m);
   swap(A[25], A[27],  2, m);
   swap(A[28], A[30],  2, m);
   swap(A[29], A[31],  2, m);
   m = 0x55555555;
   swap(A[0],  A[1],   1, m);
   swap(A[2],  A[3],   1, m);
   swap(A[4],  A[5],   1, m);
   swap(A[6],  A[7],   1, m);
   swap(A[8],  A[9],   1, m);
   swap(A[10], A[11],  1, m);
   swap(A[12], A[13],  1, m);
   swap(A[14], A[15],  1, m);
   swap(A[16], A[17],  1, m);
   swap(A[18], A[19],  1, m);
   swap(A[20], A[21],  1, m);
   swap(A[22], A[23],  1, m);
   swap(A[24], A[25],  1, m);
   swap(A[26], A[27],  1, m);
   swap(A[28], A[29],  1, m);
   swap(A[30], A[31],  1, m);
}

/**
 * @}
 */