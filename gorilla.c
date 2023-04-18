/* Author: kun_tsai  GuangZhou 2019 */
/* E-mail: jacicson1987@163.com  */

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "gorilla.h"

const static u64 de_bruijn64 = 0x03f79d71b4ca8b09;

const static u8 de_bruijn64_tab[64] = {
	0, 1, 56, 2, 57, 49, 28, 3, 61, 58, 42, 50, 38, 29, 17, 4,
	62, 47, 59, 36, 45, 43, 51, 22, 53, 39, 33, 30, 24, 18, 12, 5,
	63, 55, 48, 27, 60, 41, 37, 16, 46, 35, 44, 21, 52, 32, 23, 11,
	54, 26, 40, 15, 34, 20, 31, 10, 25, 14, 19, 9, 13, 8, 7, 6,
};
const static u8 len8_tab[256] = {
	0x00, 0x01, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
};
const static u64 Nan = 0x7FF8000000000001;

int bitwriter_init(bitwriter_t* w)
{
    assert(w);
    memset(w, 0, sizeof(*w));
    w->bit_count = 8;
    return 0;
}

static int append_to_cache(bitwriter_t* w)
{
    if (w->pos > 1024 - 1)
            return -1;
    w->cache[w->pos] = w->byte;
    w->pos++;
    return 0;
}

int write_bit(bitwriter_t* w, bool bit)
{
    if (bit) {
        w->byte |= 1 << (w->bit_count - 1);
    }
    w->bit_count--;

    if (w->bit_count == 0) {
        if (append_to_cache(w) != 0)
                return -1;
        w->byte = 0;
        w->bit_count = 8;
    }
    return 0;
}

int write_byte(bitwriter_t* w, u8 b) 
{
    w->byte |= b >> (8 - w->bit_count);
    if (append_to_cache(w) != 0)
            return -1;
    w->byte = b << w->bit_count;
    return 0;
}

int write_bits(bitwriter_t* w, u64 u, int nbits)
{
    if (nbits > 64 || nbits < 0)
        return -1;

    u8 byte = 0;
    u <<= (64 - nbits);
    while (nbits >= 8) {
        byte = u >> 56;
        if (write_byte(w, byte) != 0)
            return -1;
        u <<= 8;
        nbits -= 8;
    }

    while (nbits > 0) {
        if (write_bit(w, u >> 63) != 0)
            return -2;
        u <<= 1;
        nbits--;
    }
    return 0;
}

int write_flush(bitwriter_t* w, bool bit)
{
    while (w->bit_count != 8) {
        if (write_bit(w, bit) != 0)
            return -1;
    }
    return 0;
}
int float_encoder_init(float_encoder_t* s)
{
    assert(s);
    memset(s, 0, sizeof(*s));

    bitwriter_init(&s->w);
    s->first = true;
    s->leading = ~0;
    write_byte(&s->w, 0x10);
    return 0;
}

int bitslen(u64 x) 
{
    int n = 0;
    if (x >= ((u64)1<<32)) {
        x >>= 32;
        n = 32;
    }
    if (x >= 1<<16) {
        x >>= 16;
        n += 16;
    }
    if (x >= 1<<8) {
        x >>= 8;
        n += 8;
    }
    return n + len8_tab[x];   
}

int leading_zero64(u64 u)
{
    return 64 - bitslen(u);
}


int trailing_zero64(u64 u)
{
    if (u == 0)
        return 64;

    return de_bruijn64_tab[(u&-u)*de_bruijn64 >> (64 - 6)];
}

int float_encode_write(float_encoder_t* s, f64 v)
{
    u64* vp = (u64*)&v;
    if (s->first) {
        s->val = *vp;
        s->first = false;
        write_bits(&s->w, *vp, 64);
        return 0;
    }

    u64 vdelta = *vp ^ s->val;

    if (vdelta == 0) {
        write_bit(&s->w, 0);
    }
    else {
        write_bit(&s->w, 1);
    }

    u64 leading = leading_zero64(vdelta);
    u64 trailing = trailing_zero64(vdelta);

    leading &= 0x1F;
    if (leading >= 32)
        leading = 31;

    if (s->leading != ~0 && leading > s->leading &&
        trailing >= s->trailing) {
        write_bit(&s->w, 0);
        write_bits(&s->w, vdelta >> s->trailing, 
        64 - s->leading - s->trailing);
    }
    else {
        s->leading = leading;
        s->trailing = trailing;
        write_bit(&s->w, 1);
        write_bits(&s->w, leading, 5);

        u64 sigbits = 64 -leading - trailing;
        write_bits(&s->w, sigbits, 6);
        write_bits(&s->w, vdelta >> trailing, sigbits);
    }

    s->val = *vp;
    return 0;
}


int float_encode_flush(float_encoder_t* s, u8* dst, u32* length)
{
    f64* na = (f64*)&Nan;
    
    if (!s->finished) {
        s->finished = true;
        float_encode_write(s, *na);
        write_flush(&s->w, 0);
    }

    *length = s->w.pos + 1;
    memcpy(dst, s->w.cache, s->w.pos + 1);
    return *length;
}

int float_cache_print(float_encoder_t* s)
{
    int i = 0;
    u8 *a = s->w.cache;
    
    for (i = 0; i < s->w.pos + 1; i++) {
        printf("%02x ", a[i]);
        if ((i+1) % 16 == 0)
                printf("\n");
        else if ((i + 1) % 8 == 0)
                printf("  ");
    }
    printf("\n");
}

int bit_readbuf(bitreader_t* br)
{
    u32 i = 0;
    u32 byte_n = 8 - (br->n / 8);

    if (br->len && byte_n > br->len) {
        byte_n = br->len;
    }

    if (byte_n == 8) {
        br->v = (u64)(br->data[7]) | (u64)(br->data[6])<<8 |
                (u64)(br->data[5])<<16 | (u64)(br->data[4])<<24 |
                (u64)(br->data[3])<<32 | (u64)(br->data[2])<<40 |
                (u64)(br->data[1])<<48 | (u64)(br->data[0])<<56;
        br->n = 64;
        br->data = br->data + 8;
        br->len = br->len - 8;
        return 0;
    }

    for (i = 0; i < byte_n; i++) {
        br->n += 8;
        br->v |= (u64)(br->data[i]) << (64 - br->n);
    }

    br->data = br->data + byte_n;
    br->len = br->len - byte_n;
    return 0;
}

int bitread_reset(bitreader_t* br, u8* data, u32 len) 
{
    br->data = data;
    br->len = len;
    br->n = 0;
    br->v = 0;
    bit_readbuf(br);
    return 0;
}

u64 read_bits(bitreader_t* br, u32 nbits)
{
    u64 v = 0, n = 0;
    if (br->n == 0) {
        return ~0;
    }

    if (nbits <= br->n) {
        if (nbits == 64) {
            v = br->v;
            br->n = 0;
            br->v = 0;
            bit_readbuf(br);
            return v;
        }

        v = br->v >> (64 - nbits);
        br->v = br->v << nbits;
        br->n = br->n - nbits;
        if (br->n == 0) {
            bit_readbuf(br);
        }
        return v;
    }

    //read all available bits in current buffer.
    v = br->v;
    n = br->n;

    br->v = 0;
    br->n = 0;
    bit_readbuf(br);

    // Append new buffer to previous buffer and shift to remove unnecessary bits.
    v |= (br->v >> n);
    v >>= 64 - nbits;

    // Remove used bits from new buffer.
    u32 buf_n = nbits - n;
    if (buf_n > br->n) {
        buf_n = br->n;
    }
    br->v <<= buf_n;
    br->n -= buf_n;

    if (br->n == 0) {
        bit_readbuf(br);
    }

    return v;
}

u64  read_bit(bitreader_t* br)
{
    return read_bits(br, 1);
}

bool can_read_bitfast(bitreader_t* br)
{
    if (br->n > 1)
        return true;
    else
        return false;
}

bool read_bitfast(bitreader_t* br)
{
    bool v = ((br->v & ((u64)1<<63)) != 0);
    br->v <<= 1;
    br->n -= 1;
    return v;
}

// Next returns true if there are remaining values to read.
bool read_next(float_decoder_t* s)
{
    u64 v = 0;
    u64 bits, mbits, vbits;
    if (s->finished || s->err) {
        return false;
    }

    if (s->first) {
        s->first = false;
        // mark as finished if there were no values.
        if (s->val == Nan) { // IsNaN
            s->finished = true;
            return false;
        }
        return true;
    }

    // read compressed value
    bool abit;
    if (can_read_bitfast(&s->br)) {
        abit = read_bitfast(&s->br);
    } else if (v = read_bit(&s->br) == ~0) {
        s->err = 1;
        return false;
    } else {
        abit = v;
    }

    if (abit) {
        bool bit;
        if (can_read_bitfast(&s->br)) {
            bit = read_bitfast(&s->br);
        } else if (v = read_bit(&s->br) == ~0) {
            s->err = 1;
            return false;
        } else {
            bit = v;
        }

        if (!bit) {
            // reuse leading/trailing zero bits
            // it.leading, it.trailing = it.leading, it.trailing
            ;
        } else {
            bits = read_bits(&s->br, 5);
            if (bits == ~0) {
                s->err = 1;
                return false;
            }
            s->leading = bits;

            bits = read_bits(&s->br, 6);
            if (bits == ~0) {
                s->err = 1;
                return false;
            }
            mbits = bits;
            // 0 significant bits here means we overflowed and we actually need 64;
            //see comment in encoder
            if (mbits == 0) {
                mbits = 64;
            }
            s->trailing = 64 - s->leading - mbits;
        }

        mbits = 64 - s->leading - s->trailing;
        bits = read_bits(&s->br, mbits);
        if (bits == ~0) {
            s->err = 1;
            return false;
        }

        vbits = s->val;
        vbits ^= (bits << s->trailing);

        if (vbits == Nan) { // IsNaN
            s->finished = true;
            return false;
        }
        s->val = vbits;
    }

    return true;
}

int float_decode_setbytes(float_decoder_t* s,u8* data, u32 data_len)
{
    memset(s->b, 0, sizeof(s->b));
    memcpy(s->b, data, data_len);

    bitread_reset(&s->br, s->b + 1, data_len - 1);
    u64 v = read_bits(&s->br, 64);
    if (v == ~0) {
        return -1;
    }
    s->val = v;
    s->leading = 0;
    s->trailing = 0;
    s->first = true;
    s->finished = false;
    s->err = false;

    return 0;
}

int float_decode_block(float_decoder_t* s, u8* data, u32 len, f64* res, u32* res_len)
{
    u32 cnt = 0;
    f64 val = 0.0;
    int ret = float_decode_setbytes(s, data, len);
    if (ret != 0) {
        printf("Error\n");
        return ret;
    }

    while(read_next(s)) {
        val = *(f64*)(&s->val);
        //printf("%f\n", val);
        res[cnt] = val;
        cnt++;
        if (cnt > 20) {
            break;
        }
    }
    *res_len = cnt;
    if (s->err) {
        printf("Error when decode\n");
        return -1;
    }

    return 0;
}
