#include<stdio.h>
#include<stdint.h>

typedef union {
    double     real_value;
    uint64_t   uint_value;
} real;


typedef enum {
    PlusZero      = 0x00,
    MinusZero     = 0x01,
    PlusInf       = 0xF0,
    MinusInf      = 0xF1,
    PlusRegular   = 0x10,
    MinusRegular  = 0x11,
    PlusDenormal  = 0x20,
    MinusDenormal = 0x21,
    SignalingNaN  = 0x30,
    QuietNaN      = 0x31
} float_class_t;

int SIGN_SHIFT = 63;
int MANTISSA_SHIFT = 12;
int EXP_SHIFT = 52;
int EXP_SIZE = 11;
int NaN_BIT_SHIFT = 51;

extern float_class_t
classify(double *value_ptr) {
	real pivot;
	
	pivot.real_value = *value_ptr;
	uint64_t sign = pivot.uint_value >> SIGN_SHIFT;
	uint64_t mantissa = (pivot.uint_value << MANTISSA_SHIFT) >> MANTISSA_SHIFT;
	uint64_t exp = ((pivot.uint_value << 1) >> 1) >> EXP_SHIFT;

	uint64_t MegaExp = (((uint64_t)1) << EXP_SIZE)-1;

	if (exp == 0 && mantissa == 0 && sign == 0) {
		return PlusZero;
	} else if (exp == 0 && mantissa == 0 && sign == 1) {
		return MinusZero;
	} else if (exp ==  MegaExp && mantissa == 0 && sign == 1) {
		return MinusInf;	
	} else if (exp == MegaExp && mantissa == 0 && sign == 0) {
		return PlusInf;	
	} else if (exp == 0 && sign == 0 && mantissa != 0) {
		return PlusDenormal;
	} else if (exp == 0 && sign == 1 && mantissa != 0) {
		return MinusDenormal;
	} else if (exp == MegaExp && mantissa >> NaN_BIT_SHIFT == 0 && mantissa!=0) {
		return SignalingNaN;
	} else if (exp == MegaExp && mantissa >> NaN_BIT_SHIFT == 1 && mantissa!=0) {
		return QuietNaN;
	} else if (sign == 0) {
		return PlusRegular;
	} else {
		return MinusRegular;
	}
}
