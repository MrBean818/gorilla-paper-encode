/* Author: kun_tsai  GuangZhou 2019 */
/* E-mail: jacicson1987@163.com  */

#pragma once

#include <stdbool.h>

typedef unsigned char u8;
typedef unsigned int  u32;
typedef unsigned long u64;
typedef double f64;

typedef struct bitwriter_s {
        u8 cache[1024];
        u32 pos;
        u8 byte;
        u8 bit_count;

} bitwriter_t;

typedef struct bitreader_s {
    u8* data;
    u32 len;

    u64 v;   // bit buffer
    u32 n;   // available bits

} bitreader_t;

typedef struct float_encoder_s {
    bitwriter_t w;

    u64 val;

    u64 leading;
    u64 trailing;

    bool first;
    bool finished;
} float_encoder_t;

typedef struct float_decoder_s  {
    u64 val;

    u64 leading;
    u64 trailing;

    bitreader_t br;
    u8  b[1024];

    bool first;
    bool finished;
    int err;
} float_decoder_t;

int float_encoder_init(float_encoder_t* s);
int float_encode_write(float_encoder_t* s, f64 v);
int float_encode_flush(float_encoder_t* s, u8* dst, u32* len);
int float_cache_print(float_encoder_t* s);
int float_decode_block(float_decoder_t* s,u8* data, u32 len, f64* res, u32* res_len);

