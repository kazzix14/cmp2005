#ifndef __SMOOTHING_H__
#define __SMOOTHING_H__

#include "fmath.h"

// TODO: constexpr smoothness

// --------------------------------------------------------------------------------
// CMySmoothing
// --------------------------------------------------------------------------------
// S: smoothness, R: sample rate
template<int S, int R>
class CMySmoothingFixed
{
private:
	float buf = 0;

public:
	// 入力信号にフィルタを適用する関数
	// sinで0.637(-3.92 dB)ぐらいに収束する...?
	inline float process(const float in) {
		constexpr float samplerate44100Inverse = 1.0f / 44100.0f;
		constexpr float effectiveSmoothness = (float)R * samplerate44100Inverse * (float)S;
		constexpr float effectiveSmoothnessInverse = 1.0f / effectiveSmoothness;
		constexpr float bufferWeight = 1.0f - effectiveSmoothnessInverse;

		buf = bufferWeight * buf + fabsf(in) * effectiveSmoothnessInverse;
		//buf += effectiveSmoothnessInverse * fabsf(in) - effectiveSmoothnessInverse * buf;
		return buf;
	}
};

class CMySmoothing
{
private:
	float buf = 0;

public:
	// 入力信号にフィルタを適用する関数
	// sinで0.637(-3.92 dB)ぐらいに収束する...?
	inline float process(const float in, const float sampleRate, float smoothness) {
		constexpr float samplerate44100Inverse = 1.0f / 44100.0f;
		const float effectiveSmoothness = smoothness * sampleRate * samplerate44100Inverse;
		const float effectiveSmoothnessInverse = 1.0f / effectiveSmoothness;

		buf = ((effectiveSmoothness - 1.0f) * buf + fabsf(in)) / effectiveSmoothness;
		return buf;
	}

	inline const float last_value() {
		return this->buf;
	}

	inline void reset() {
		this->buf = 0.0f;
	}
};

#endif
