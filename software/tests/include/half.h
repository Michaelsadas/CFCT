#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define hf_exp 5
#define hf_man 10
#define fl_exp 8
#define fl_man 23
#define do_exp 11
#define do_man 52

float from_double_to_float(double a) {
    long long reserve = *(unsigned long long*) & (a);
    long long sig = reserve >> (do_exp + do_man);
    long long exp = (reserve >> (do_man)) & 0x7ff;
    long long man = reserve & 0xfffffffffffff;
    int x = (exp - (1 << (do_exp - 1)) + (1 << (fl_exp - 1)));
    int new_exp = x > 0 ? (x & 0xff) : 0;
    int result = (sig << (fl_exp + fl_man)) | (new_exp << fl_man) | (man >> (do_man - fl_man));
    return *(float*)&(result);
}

double from_float_to_double(float a) {
    int reserve = *(int*) & (a);
    long long unsigned sig = reserve >> (fl_exp + fl_man);
    long long unsigned exp = (reserve >> fl_man) & 0xff;
    long long unsigned man = reserve & 0x7fffff;
    long long unsigned new_exp = (exp - (1 << (fl_exp - 1)) + (1 << (do_exp - 1))) & 0x7ff;
    long long unsigned result = (sig << (do_exp + do_man)) | (new_exp << do_man) | (man << (do_man - fl_man));
    return *(double*)&(result);
}

unsigned short from_float_to_half(float a) {
    int reserve = *(int*)&(a);
    unsigned int sig = reserve >> (fl_exp + fl_man);
    unsigned int exp = (reserve >> fl_man) & 0xff;
    unsigned int man = reserve & 0x7fffff;
    int x = (exp - (1 << (fl_exp - 1)) + (1 << (hf_exp - 1)));
    unsigned int new_exp = x > 0 ? (x & 0x1f) : 0;
    unsigned short result = (sig << (hf_exp + hf_man)) | (new_exp << hf_man) | (man >> (fl_man - hf_man));
    return result;
}

float from_half_to_float(unsigned short a) {
    int sig = a >> (hf_exp + hf_man);
    int exp = (a >> hf_man) & 0x1f;
    int man = a & 0x3ff;
    int new_exp = (exp - (1 << (hf_exp - 1)) + (1 << (fl_exp - 1))) & 0xff;
    int result = (sig << (fl_exp + fl_man)) | (new_exp << fl_man) | (man << (fl_man - hf_man));
    return *(float*)&(result);
}