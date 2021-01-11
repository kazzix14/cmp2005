#ifndef __RMS_H__
#define __RMS_H__

#include <valarray>
#include <math.h>

// --------------------------------------------------------------------------------
// CMyRMS
// --------------------------------------------------------------------------------
template<size_t N>
class CMyRMS
{
private:
	// バッファ
	std::valarray<float> buffer;
	size_t buffer_head;
	size_t sampleIndexes[N];
	float sampleRate;

public:
	CMyRMS(float lengthInMillis, float sampleRate);

	// 入力信号にフィルタを適用する関数
	float process(float in);
	void setLengthInMillis(float lengthInMillis);
	void setSampleRate(float sampleRate);
};

#endif
