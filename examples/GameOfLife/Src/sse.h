#pragma once
#ifndef _SSE_H_
#define _SSE_H_

#include <emmintrin.h>
#include <string.h>

static const int VEC_SIZE=16;
static const __m128i EPI8_ONE = _mm_set1_epi8(1);
static const __m128i EPI8_TWO = _mm_set1_epi8(2);
static const __m128i EPI8_THREE = _mm_set1_epi8(3);
static const char ON=(char)0xff;
static const char OFF=0x00;
static __m128i LEFT_BORDER_MASK = _mm_set_epi8(ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,ON,OFF);

static inline
__m128i sse_compute_neighbors(const unsigned char *const INDEX, const int X_SIZE_EXT_ ) {
	__m128i neighbors = _mm_add_epi8(_mm_loadu_si128((const __m128i*)(INDEX+1)),
                                     _mm_loadu_si128((const __m128i*)(INDEX-1)));
	const unsigned char *LO = INDEX-X_SIZE_EXT_;
	neighbors=_mm_add_epi8(neighbors, _mm_loadu_si128((const __m128i*)(LO-1)));
	neighbors=_mm_add_epi8(neighbors, _mm_load_si128((const __m128i*)LO));
	neighbors=_mm_add_epi8(neighbors, _mm_loadu_si128((const __m128i*)(LO+1)));
	const unsigned char *UP = INDEX+X_SIZE_EXT_;
	neighbors=_mm_add_epi8(neighbors, _mm_loadu_si128((const __m128i*)(UP-1)));
	neighbors=_mm_add_epi8(neighbors, _mm_load_si128((const __m128i*)UP));
	neighbors=_mm_add_epi8(neighbors, _mm_loadu_si128((const __m128i*)(UP+1)));
	return neighbors;
}

static inline
__m128i sse_apply_rule(const __m128i &neighbors, const unsigned char *const INDEX) {
	// find 3 neighbor fields and 2 neighbor fields, that already were alive
	const __m128i alive2 = 
		_mm_and_si128( _mm_loadu_si128((const __m128i *)INDEX),
		               _mm_cmpeq_epi8(neighbors, EPI8_TWO));
	// all bits will be set by _mm_cmpeq_epi8 (->255), so we
	// have to do an and(1) here(?)
	const __m128i alive3 = 
		_mm_and_si128(EPI8_ONE, _mm_cmpeq_epi8(neighbors, EPI8_THREE));
	// combine both fields to get next gen
	return _mm_or_si128(alive2, alive3);
}

//
// main function: an if-free, SSE-based kernel
//
static inline
void sse_next_gen( const unsigned char * fields, unsigned char * next, int ARRAY_SIZE,
                   int X_START, int X_END, int X_SIZE_EXT, int Y_START, int Y_END, 
                   const unsigned char * rmask )
{
	for(int y=Y_START; y<Y_END; y++) {
		const unsigned char *INDEX = fields + y*X_SIZE_EXT;
		unsigned char *NEXT = next + y*X_SIZE_EXT;

		const __m128i result = sse_apply_rule( sse_compute_neighbors( INDEX, X_SIZE_EXT ), INDEX );
		_mm_store_si128((__m128i*)NEXT, _mm_and_si128( result, LEFT_BORDER_MASK ) );
		
		for(int x=X_START+VEC_SIZE; x<X_END-VEC_SIZE; x+=VEC_SIZE) {
			// move array pointers
			INDEX+=VEC_SIZE;
			NEXT+=VEC_SIZE;
			_mm_store_si128( (__m128i*)NEXT, sse_apply_rule( sse_compute_neighbors(INDEX,X_SIZE_EXT), INDEX) );
		}
		// move array pointers
		INDEX+=VEC_SIZE;
		NEXT+=VEC_SIZE;
		const __m128i RIGHT_BORDER_MASK=_mm_load_si128((const __m128i *)rmask);	
		_mm_store_si128((__m128i*)NEXT, 
			_mm_and_si128( sse_apply_rule( sse_compute_neighbors(INDEX,X_SIZE_EXT), INDEX), RIGHT_BORDER_MASK ) );
	}
}

#endif //_SSE_H_
