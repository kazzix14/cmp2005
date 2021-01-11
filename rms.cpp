#include <algorithm>
#include <cmath>
#include "rms.h"

// iterate over all samples
//template CMyRMS<0>;

// pick given number of smaples
template CMyRMS<8>;
template CMyRMS<16>;
template CMyRMS<32>;
template CMyRMS<64>;

// --------------------------------------------------------------------------------
// コンストラクタ
// --------------------------------------------------------------------------------
template<size_t N>
CMyRMS<N>::CMyRMS(float lengthInMillis, float sampleRate) {
	float bufferLength = lengthInMillis / 1000.0f * sampleRate;

	this->sampleRate = sampleRate;
	buffer = std::valarray<float>((size_t)bufferLength);
	buffer_head = 0;
}

//template<>
//CMyRMS<0>::CMyRMS(float lengthInMillis, float sampleRate) {
//	float bufferLength = lengthInMillis / 1000.0f * sampleRate;
//
//	this->sampleRate = sampleRate;
//	buffer = std::valarray<float>((size_t)bufferLength);
//	buffer_head = 0;
//}


template<size_t N>
float CMyRMS<N>::process(float in) {
	float square = pow(in, 2);
	buffer[buffer_head++] = square;

	if (buffer.size() <= buffer_head) {
		buffer_head = 0;
	}

	float sumSquare = 0.0f;
	for (size_t idx : sampleIndexes) {
		sumSquare += buffer[idx];
	}

	float meanSquare = sumSquare / (float)N;

	return sqrt(meanSquare);
}

template<size_t N>
void CMyRMS<N>::setLengthInMillis(float lengthInMillis) {
	float bufferLength = lengthInMillis / 1000.0f * sampleRate;
	buffer.resize((size_t)bufferLength);

	float step = (float)buffer.size() / (float)N;
	for (size_t i = 0; i < N; i++) {
		sampleIndexes[i] = std::min((int)(step * (float)i), (int)buffer.size() - 1);
	}
}

template<size_t N>
void CMyRMS<N>::setSampleRate(float sampleRate) {
	float bufferLengthInMillis = 1000.0f * buffer.size() / this->sampleRate;

	this->sampleRate = sampleRate;
	setLengthInMillis(bufferLengthInMillis);
}
