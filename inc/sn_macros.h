#if !defined(SN_MACROS_H_INCLUDED)
#define SN_MACROS_H_INCLUDED

#pragma once

#define SN_APPLY_TYPES(C, B, M)        \
    M(C, B, short, Short)              \
    M(C, B, int, Int)                  \
    M(C, B, long, Long)                \
    M(C, B, long long, LongLong)       \
    M(C, B, float, Float)              \
    M(C, B, double, Double)            \
    M(C, B, long double, LongDouble)

#define SNI_FORWARD(C, B, T, N)        \
    typedef SNI_Real<T> SNI_##N;

#define SNI_CONSTRUCTOR(C, B, T, N)    \
    C(SNI::SNI_##N *p_##N);

#define SN_FORWARD(C, B, T, N)         \
    typedef SN_Real<T> SN_##N;

#define SN_CONSTRUCTOR(C, B, T, N)     \
    C(const SN_##N &p_##N);

#define DUMMY 0

#define SN_CONSTRUCTOR_IMPL(C, B, T, N)                                     \
	SN_##C::SN_##C(const SN_##N &p_Number)                                  \
		: B(dynamic_cast<SNI::SNI_##C *>(p_Number.GetSNI_Real()))           \
	{                                                                       \
	}

#define SNI_CONSTRUCTOR_IMPL(C, B, T, N)                                    \
	SN_##C::SN_##C(SNI::SNI_##N *p_##N)                                     \
		: B(dynamic_cast<SNI::SNI_##C *>(p_##N))                            \
	{                                                                       \
	}

#endif // !defined(SN_MACROS_H_INCLUDED)
