/* Author: kun_tsai  GuangZhou 2023 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gorilla.h"


void print_hex(u8* buf, u32 len)
{
    int i = 0;
    for (i = 0; i < len; i++) {
        printf("%02x ", buf[i]);
        if ((i+1) % 16 == 0)
                printf("\n");
        else if ((i + 1) % 8 == 0)
                printf("  ");
    }
    printf("\n");
}

void print_f64_array(f64* arr, u32 len)
{
    int i = 0;
    for (i = 0; i < len; i++) {
        printf("%.2f\n", arr[i]);
    }
}
int main()
{
    u8  buffer[1024] = {0};
    u32 length = 0;
    f64 arr[8] = {2300.0, 2400.0, 2500.0, 2600.0, 2700.0, 2800.0, 2900.0, 3000 };
    f64 de_arr[64] = {0};
    u32 de_len = 0;
    f64 ff= 100.0;

    float_encoder_t encode;
    float_decoder_t decode;
    float_encoder_init(&encode);

    int i = 0;
    for (i = 0; i < 8; i++) {
        float_encode_write(&encode, arr[i]);
    }
    float_encode_flush(&encode, buffer, &length);
    printf("Encoded Bytes: \n");
    print_hex(buffer, length);

    float_decode_block(&decode, buffer, length, de_arr, &de_len);

    printf("Decoded Float: \n");
    print_f64_array(de_arr, de_len);
    return 0;
}
