#ifndef __CONVERTER_H__
#define __CONVERTER_H__

#include "fmath.h"

namespace Converter {
	inline float levelToDecibel(const float level) {
		return 20.0f * fmath::fast_log10(level);
	}

	inline float decibelToLevel(const float decibel) {
		return fmath::pow10(decibel * 0.05f);

	}

	// attack: 0.0 ~ 1.0
	inline float mySlopeMapper1(const float x, const float attack, const float accelation) {
		return attack * fmath::pow(x, 0.5f) + (1.0f - attack) * fmath::pow(x, accelation);
	}

	// x : -inf ~ inf
	// return : -1 : 1
	// greater gain -> steeper
	// less gain -> less steeper
	inline float sigmoid(const float x, const float gain) {
		return gain * x / (1.0f + fabsf(gain * x));
	}

	// use fminf() and/or fmaxf() to clamp the returned value
	// if abs(x)    == xRange    -> return 1.0f;
	// if linearity == 0         -> return x.is_positive() ? 1.0f : -1.0f
	// if linearity == +infinity -> return x * xRange
	// if linearity <  0         -> Never Do!!
	inline float flexibleActivation(const float x, const float xRange, const float linearity) {
		return (x / (fabsf(x) + xRange * linearity)) * (1.0f + linearity);
	}

	// clipping function
	// just for decibel
	// -120.0 ~ 0.0 dB
	inline float softClipperDecibel(const float x) {
		return x * (1.1885f - 71.0f / (1.0f + fabsf(x - 120.0f))) - 12.55f;
	}

	// adjustable clipping function
	inline float adjustableClipper(const float x, const float threshold, const float hardness) {
		return (x * (sigmoid(x, -hardness) * 0.5 + 0.5) - 0.5 / hardness) + threshold;
	}
}

#endif
