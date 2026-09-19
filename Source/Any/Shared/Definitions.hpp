#pragma once

using t8  = char;

using u8  = unsigned char;
using s8  = signed char;

using u16 = unsigned short;
using s16 = signed short;

using u32 = unsigned int;
using s32 = signed int;
using f32 = float;

using u64 = unsigned long long;
using s64 = signed long long;
using f64 = double;

using f80 = long double;

#ifdef __SIZEOF_INT128__
using u128 = unsigned __int128;
using s128 = __int128;
#endif
using f128 = _Float128;