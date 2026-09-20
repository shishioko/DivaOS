#pragma once

#define null nullptr

typedef bool u1;

typedef unsigned char u8 __attribute__((mode(QI)));
typedef signed char s8 __attribute__((mode(QI)));
typedef char t8;

typedef unsigned short u16 __attribute__((mode(HI)));
typedef signed short s16 __attribute__((mode(HI)));
typedef short t16;

typedef unsigned int u32 __attribute__((mode(SI)));
typedef signed int s32 __attribute__((mode(SI)));
typedef int t32;

typedef unsigned long u64 __attribute__((mode(DI)));
typedef signed long s64 __attribute__((mode(DI)));

typedef unsigned long long u128 __attribute__((mode(TI)));
typedef signed long long s128 __attribute__((mode(TI)));

typedef float f32 __attribute__((mode(SF)));
typedef double f64 __attribute__((mode(DF)));
typedef long double f80 __attribute__((mode(XF)));
typedef long double f128 __attribute__((mode(TF)));

#define v volatile