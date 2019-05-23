/* Author: kun_tsai  GuangZhou 2019 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gorilla.h"

int main()
{
        u64 arr[7] = {2300.0, 2400.0, 2500.0, 2600.0, 2700.0, 2800.0, 2900.0};

        float_encoder_t encode;
        float_encoder_init(&encode);

        int i = 0;
        for (i = 0; i<7; i++) {
                float_encode_write(&encode, arr[i]);
        }
        float_encode_flush(&encode);

        float_cache_print(&encode);
	return 0;
}
