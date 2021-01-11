// 自作VST用のインクルードファイル
#include "processor.h"
#include "myvst3def.h"
#include "myvst3fuid.h"
#include "fmath.h"
#include "filter.h"
#include "converter.h"

#include <cfloat>

// VST3作成に必要なの名前空間を使用
namespace Steinberg {
	namespace Vst {


		// ============================================================================================
		// コンストラクタ
		// ============================================================================================
		MyVSTProcessor::MyVSTProcessor()
		{
			// コントローラーのFUIDを設定する
			setControllerClass(ControllerUID);
		}

		// ============================================================================================
		// クラスを初期化する関数
		// ============================================================================================
		tresult PLUGIN_API MyVSTProcessor::initialize(FUnknown* context)
		{
			// まず継承元クラスの初期化を実施
			tresult result = AudioEffect::initialize(context);
			if (result == kResultTrue)
			{
				// 入力と出力のAudioBusを追加する
				addAudioInput(STR16("AudioInput"), SpeakerArr::kStereo);
				addAudioOutput(STR16("AudioOutput"), SpeakerArr::kStereo);

				// 入力のEventBusを追加する
				addEventInput(STR16("Event Input"), 1);

				// 以下固有の初期化を実施
				//ratio = 1.0f;
				//ratioInverse = 1.0f / ratio;
				//reductionRatio = 1.0f - ratioInverse;
				//threshold = 1.0f;
				//makeupGain = 0.0f;
				//makeupLevel = Converter::decibelToLevel(makeupGain);
				//attackTime = 5.0f;
				//attackRate = fminf(1000.0f / attackTime * sampleRateInverse, 1.0f - FLT_MIN);
				//releaseTime = 100.0f;
				//releaseRate = fminf(1000.0f / releaseTime * sampleRateInverse, 1.0f - FLT_MIN);
				//kneeSize = 0.0f;
				//smoothness = 100.0f;
				//kneeCoefficient = (float)threshold - ((float)kneeSize / 2.0f);

				//if (kneeSize != 0.0f)
				//	kneeSizeInverse = 1.0f / kneeSize;


				for (int idx : {0, 1}) {
					this->compressionRatio[idx] = this->compressionRatioDefault;
					this->thresholdLevel[idx] = this->thresholdLevelDefault;
					this->makeupLevel[idx] = this->makeupLevelDefault;
					this->attackTime[idx] = this->attackTimeDefault;
					this->releaseTime[idx] = this->releaseTimeDefault;
					this->kneeWidth[idx] = this->kneeWidthDefault;
					this->inputFilterSmoothness[idx] = this->inputFilterSmoothnessDefault;
					this->wetRatio[idx] = this->wetRatioDefault;
					this->triggerSignalRatio[idx] = this->triggerSignalRatioDefault;
					this->clippingHardness[idx] = this->clippingHardnessDefault;
					this->clippingThresholdLevel[idx] = this->clippingThresholdLevelDefault;
					this->clippingEnabled[idx] = this->clippingEnabledDefault;
					this->bypassed[idx] = this->bypassedDefault;
					this->muted[idx] = this->mutedDefault;

					// pre-calcurated coefficients
					this->compressionRatioInverse[idx] = 1.0f / this->compressionRatio[idx];
					this->reductionRatio[idx] = 1.0f - this->compressionRatioInverse[idx];
					this->makeupGain[idx] = Converter::levelToDecibel(this->makeupLevel[idx]);
					//this->attackRate[idx] = fminf(1000.0f / this->attackTime[idx] * this->sampleRateInverse, 1.0f - FLT_MIN);
					//this->releaseRate[idx] = fminf(1000.0f / this->releaseRate[idx] * this->sampleRateInverse, 1.0f - FLT_MIN);
					this->kneeLowerEdgeLevel[idx] = this->thresholdLevel[idx] - (this->kneeWidth[idx] / 2.0f);
					if (this->kneeWidth[idx] != 0.0f)
						this->kneeWidthInverse[idx] = 1.0f / this->kneeWidth[idx];
					this->reductionLevelLast[idx] = 0.0f;
					//this->inputLevelLast[idx] = -120.0f;

					this->inputSmoothing[idx].reset();
				}

				this->midSideModeEnabled = this->midSideModeEnabledDefault;
			}

			// 初期化が成功すればkResultTrueを返す
			return result;
		}


		tresult PLUGIN_API MyVSTProcessor::setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts)
		{
			// inputとoutputのバスが1つずつで、かつinputとoutputの構成がステレオ場合
			if (numIns == 1 && numOuts == 1 && inputs[0] == SpeakerArr::kStereo && outputs[0] == SpeakerArr::kStereo)
			{
				return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
			}

			// 対応していないバス構成の場合、kResultFalseを返す。
			return kResultFalse;
		}

		tresult PLUGIN_API MyVSTProcessor::setupProcessing(ProcessSetup& setup)
		{
			this->sampleRate = (float)setup.sampleRate;
			this->sampleRateInverse = 1.0f / this->sampleRate;

			this->attackRate[0] = fminf(1000.0f / this->attackTime[0] * this->sampleRateInverse, 1.0f - FLT_MIN);
			this->attackRate[1] = fminf(1000.0f / this->attackTime[1] * this->sampleRateInverse, 1.0f - FLT_MIN);
			this->releaseRate[0] = fminf(1000.0f / this->releaseTime[0] * this->sampleRateInverse, 1.0f - FLT_MIN);
			this->releaseRate[1] = fminf(1000.0f / this->releaseTime[1] * this->sampleRateInverse, 1.0f - FLT_MIN);

			return true;
		}

		// ============================================================================================
		// 音声信号を処理する関数
		// ============================================================================================
		tresult PLUGIN_API MyVSTProcessor::process(ProcessData& data)
		{
			// パラメーター変更の処理
			// 与えられたパラメーターがあるとき、dataのinputParameterChangesに
			// IParameterChangesクラスへのポインタのアドレスが入る
			if (data.inputParameterChanges != NULL)
			{
				// 与えられたパラメーターの数を取得
				int32 paramChangeCount = data.inputParameterChanges->getParameterCount();

				// 与えられたパラメーター分、処理を繰り返す。
				for (int32 i = 0; i < paramChangeCount; i++)
				{
					// パラメーター変更のキューを取得
					// (処理するサンプル内に複数のパラメーター変更情報がある可能性があるため、
					// キューという形になっている。)
					IParamValueQueue* queue = data.inputParameterChanges->getParameterData(i);
					if (queue != NULL)
					{
						// どのパラメーターが変更されたか知るため、パラメーターtagを取得
						int32 tag = queue->getParameterId();

						// 変更された回数を取得
						int32 valueChangeCount = queue->getPointCount();
						ParamValue value_d;
						int32 sampleOffset;

						// 最後に変更された値を取得
						if (queue->getPoint(valueChangeCount - 1, sampleOffset, value_d) == kResultTrue)
						{
							float value = static_cast<float>(value_d);

							// tagに応じた処理を実施
							switch (tag)
							{
							case PARAMETER_COMPRESSION_RATIO_ML:
								this->compressionRatio[0] = this->compressionRatioMin + (value * (this->compressionRatioMax - 1.0f));
								this->compressionRatioInverse[0] = 1.0f / this->compressionRatio[0];
								this->reductionRatio[0] = 1.0f - this->compressionRatioInverse[0];
								break;
							case PARAMETER_COMPRESSION_RATIO_SR:
								this->compressionRatio[1] = this->compressionRatioMin + (value * (this->compressionRatioMax - 1.0f));
								this->compressionRatioInverse[1] = 1.0f / this->compressionRatio[1];
								this->reductionRatio[1] = 1.0f - this->compressionRatioInverse[1];
								break;

							case PARAMETER_THRESHOLD_ML:

								// 逆変換する
								//const float lastOutputGainSR = 1.0f + ((inputLevelLastSR + this->makeupLevel[1] - this->reductionLevelLast[1]) - 120.0f + 3.2f) * scale;
								//const float guiOutputGainML = (0.738f * (23.4f * lastOutputGainML) / (1.0f + fabsf(23.4f * lastOutputGainML)) + 0.999f * 0.738f - 0.022f);

								// wolframalphaすげえ でも長すぎw
								// (0.5 (-6.084×10^16 x^2 + 1.50781×10^16 x + 5.34771×10^16))/(9.50625×10^15 x^2 - 1.35989×10^16 x - 3.14127×10^14) ± (0.5 (9.75×10^16 x^2 - 1.39476×10^17 x + 4.9881×10^16))/(9.50625×10^15 x^2 - 1.35989×10^16 x - 3.14127×10^14)
									value -= 0.05f;
								if (value < 0.000001f) {
									this->thresholdLevel[0] = -120.0f;
								}
								else {
									this->thresholdLevel[0] = (0.5f * (-6.084f * fmath::pow10(16.0f) * powf(value, 2.0f) + 1.50781 * fmath::pow10(16.0f) * value + 5.34771 * fmath::pow10(16.0f))) / (9.50625 * fmath::pow10(15.0f) * powf(value, 2.0f) - 1.35989f * fmath::pow10(16.0f) * value - 3.14127f * fmath::pow10(14.0f)) + (0.5f * (9.75f * fmath::pow10(16.0f) * powf(value, 2.0f) - 1.39476f * fmath::pow10(17.0f) * value + 4.9881f * fmath::pow10(16.0f))) / (9.50625f * fmath::pow10(15.0f) * powf(value, 2.0f) - 1.35989 * fmath::pow10(16.0f) * value - 3.14127f * fmath::pow10(14.0f)) - (0.5f * (9.75f * fmath::pow10(16.0f) * powf(value, 2.0f) - 1.39476f * fmath::pow10(17.0f) * value + 4.9881f * fmath::pow10(16.0f))) / (9.50625f * fmath::pow10(15.0f) * powf(value, 2.0f) - 1.35989 * fmath::pow10(16.0f) * value - 3.14127f * fmath::pow10(14.0f));
								}
								this->kneeLowerEdgeLevel[0] = this->thresholdLevel[0] - (this->kneeWidth[0] / 2.0f);
								break;
							case PARAMETER_THRESHOLD_SR:
									value -= 0.05f;
								if (value < 0.000001f) {
									this->thresholdLevel[1] = -120.0f;
								}
								else {
									this->thresholdLevel[1] = (0.5f * (-6.084f * fmath::pow10(16.0f) * powf(value, 2.0f) + 1.50781 * fmath::pow10(16.0f) * value + 5.34771 * fmath::pow10(16.0f))) / (9.50625 * fmath::pow10(15.0f) * powf(value, 2.0f) - 1.35989f * fmath::pow10(16.0f) * value - 3.14127f * fmath::pow10(14.0f)) + (0.5f * (9.75f * fmath::pow10(16.0f) * powf(value, 2.0f) - 1.39476f * fmath::pow10(17.0f) * value + 4.9881f * fmath::pow10(16.0f))) / (9.50625f * fmath::pow10(15.0f) * powf(value, 2.0f) - 1.35989 * fmath::pow10(16.0f) * value - 3.14127f * fmath::pow10(14.0f)) - (0.5f * (9.75f * fmath::pow10(16.0f) * powf(value, 2.0f) - 1.39476f * fmath::pow10(17.0f) * value + 4.9881f * fmath::pow10(16.0f))) / (9.50625f * fmath::pow10(15.0f) * powf(value, 2.0f) - 1.35989 * fmath::pow10(16.0f) * value - 3.14127f * fmath::pow10(14.0f));
								}
								this->kneeLowerEdgeLevel[1] = this->thresholdLevel[1] - (this->kneeWidth[1] / 2.0f);
								break;

							case PARAMETER_MAKEUP_ML:
								this->makeupLevel[0] = this->makeupLevelMin + value * this->makeupLevelMax;
								this->makeupGain[0] = Converter::decibelToLevel(this->makeupLevel[0]);
								break;
							case PARAMETER_MAKEUP_SR:
								this->makeupLevel[1] = this->makeupLevelMin + value * this->makeupLevelMax;
								this->makeupGain[1] = Converter::decibelToLevel(this->makeupLevel[1]);
								break;

							case PARAMETER_ATTACK_ML:
								this->attackTime[0] = this->attackTimeMin + (this->attackTimeMax - this->attackTimeMin) * value;
								this->attackRate[0] = fminf(1000.0f / this->attackTime[0] * this->sampleRateInverse, 1.0f - FLT_MIN);
								break;
							case PARAMETER_ATTACK_SR:
								this->attackTime[1] = this->attackTimeMin + (this->attackTimeMax - this->attackTimeMin) * value;
								this->attackRate[1] = fminf(1000.0f / this->attackTime[1] * this->sampleRateInverse, 1.0f - FLT_MIN);
								break;

							case PARAMETER_RELEASE_ML:
								this->releaseTime[0] = this->releaseTimeMin + (this->releaseTimeMax - this->releaseTimeMin) * value;
								this->releaseRate[0] = fminf(1000.0f / this->releaseTime[0] * this->sampleRateInverse, 1.0f - FLT_MIN);
								break;
							case PARAMETER_RELEASE_SR:
								this->releaseTime[1] = this->releaseTimeMin + (this->releaseTimeMax - this->releaseTimeMin) * value;
								this->releaseRate[1] = fminf(1000.0f / this->releaseTime[1] * this->sampleRateInverse, 1.0f - FLT_MIN);
								break;

							case PARAMETER_KNEE_WIDTH_ML:
								this->kneeWidth[0] = this->kneeWidthMin + (this->kneeWidthMax - this->kneeWidthMin) * value;
								this->kneeLowerEdgeLevel[0] = this->thresholdLevel[0] - (this->kneeWidth[0] / 2.0f);
								if (this->kneeWidth[0] != 0.0f)
									this->kneeWidthInverse[0] = 1.0f / this->kneeWidth[0];
								break;
							case PARAMETER_KNEE_WIDTH_SR:
								this->kneeWidth[1] = this->kneeWidthMin + (this->kneeWidthMax - this->kneeWidthMin) * value;
								this->kneeLowerEdgeLevel[1] = this->thresholdLevel[1] - (this->kneeWidth[1] / 2.0f);
								if (this->kneeWidth[1] != 0.0f)
									this->kneeWidthInverse[1] = 1.0f / this->kneeWidth[1];
								break;

							case PARAMETER_SMOOTHNESS_ML:
								this->inputFilterSmoothness[0] = this->inputFilterSmoothnessMin + (this->inputFilterSmoothnessMax - this->inputFilterSmoothnessMin) * value;// Converter::mySlopeMapper1(value, 0.0f, 3.0f);
								break;
							case PARAMETER_SMOOTHNESS_SR:
								this->inputFilterSmoothness[1] = this->inputFilterSmoothnessMin + (this->inputFilterSmoothnessMax - this->inputFilterSmoothnessMin) * value;//Converter::mySlopeMapper1(value, 0.0f, 3.0f);
								break;

							case PARAMETER_TRIGGER_SIGNAL_RATIO_ML:
								this->triggerSignalRatio[0] = value;
								break;
							case PARAMETER_TRIGGER_SIGNAL_RATIO_SR:
								this->triggerSignalRatio[1] = value;
								break;

							case PARAMETER_WET_RATIO_ML:
								this->wetRatio[0] = value;
								break;
							case PARAMETER_WET_RATIO_SR:
								this->wetRatio[1] = value;
								break;

							case PARAMETER_CLIPPING_HARDNESS_ML:
								this->clippingHardness[0] = this->clippingHardnessMin + (this->clippingHardnessMax - this->clippingHardnessMin) * value;
								break;
							case PARAMETER_CLIPPING_HARDNESS_SR:
								this->clippingHardness[1] = this->clippingHardnessMin + (this->clippingHardnessMax - this->clippingHardnessMin) * value;
								break;

							case PARAMETER_CLIPPING_THRESHOLD_ML:
								this->clippingThresholdLevel[0] = this->clippingThresholdLevelMin + (this->clippingThresholdLevelMax - this->clippingThresholdLevelMin) * value;
								break;
							case PARAMETER_CLIPPING_THRESHOLD_SR:
								this->clippingThresholdLevel[1] = this->clippingThresholdLevelMin + (this->clippingThresholdLevelMax - this->clippingThresholdLevelMin) * value;
								break;

							case PARAMETER_CLIPPING_ENABLED_ML:
								if (0.4f < value)
									this->clippingEnabled[0] = true;
								else
									this->clippingEnabled[0] = false;
								break;
							case PARAMETER_CLIPPING_ENABLED_SR:
								if (0.4f < value)
									this->clippingEnabled[1] = true;
								else
									this->clippingEnabled[1] = false;
								break;

							case PARAMETER_MID_SIDE_MODE_ENABLED:
								if (0.4f < value)
									this->midSideModeEnabled = true;
								else
									this->midSideModeEnabled = false;
								break;
							}
						}
					}
				}
			}

			// 入力・出力バッファのポインタをわかりやすい変数に格納
			// inputs[]、outputs[]はAudioBusの数だけある(addAudioInput()、addAudioOutput()で追加した分だけ)
			// 今回はAudioBusは1つだけなので 0 のみとなる
			// channelBuffers32は32bit浮動小数点型のバッファで音声信号のチャンネル数分ある
			// モノラル(kMono)なら 0 のみで、ステレオ(kStereo)なら 0(Left) と 1(Right) となる
			Sample32** in = data.inputs[0].channelBuffers32;
			Sample32** out = data.outputs[0].channelBuffers32;

			// numSamplesで示されるサンプル分、音声を処理する
			//for (int32 i = 0; i < data.numSamples; i++)
			for (int32 i = 0; i < data.numSamples; i++)
			{
				//const bool midSideMode = true;
				const float inputGainLeft = in[0][i];
				const float inputGainRight = in[1][i];

				if (this->midSideModeEnabled == true) {
					const float side = in[0][i] - in[1][i];
					in[0][i] = in[0][i] + in[1][i];
					in[1][i] = side;
				}

				// mid if midSideModeEnable == true
				// side gain if midSideModeEnable == true
				const float inputMidLeft = in[0][i] * (1.0f - this->triggerSignalRatio[0])
					+ in[1][i] * this->triggerSignalRatio[0];

				const float inputSideRight = in[0][i] * (1.0f - this->triggerSignalRatio[1])
					+ in[1][i] * this->triggerSignalRatio[1];

				// TODO メータどうするか考える
				// BUG 一回閉じて開くと音出ない
				// FIlterとoutput meterがおかしい
				// smoothness よくバグる
				// smoothenss 絶賛バグり中
				const float inputGain[] = {
					//inputSmoothing[0].process(inputMidLeft, this->sampleRate, 120.0f) * 1.569858f,
					//inputSmoothing[1].process(inputSideRight, this->sampleRate, 120.0f) * 1.569858f,
					inputSmoothing[0].process(inputMidLeft, this->sampleRate, this->inputFilterSmoothness[0]) * 1.569858f,
					inputSmoothing[1].process(inputSideRight, this->sampleRate, this->inputFilterSmoothness[1]) * 1.569858f,
				};
				//const float inputGain[] = {
				//	inputMidLeft,
				//	inputSideRight
				//};

				for (int32 channel = 0; channel < 2; channel++) {

					//const float channelLinkage = 0.0f;
					//const float channelLinkageCoefficient = 1.0f - 0.5f * this->channelLinkage;

					//const float inputGainMixed =
					//	inputGain[channel] * channelLinkageCoefficient
					//	+ inputGain[1 - channel] * (1.0f - channelLinkageCoefficient);
					const float inputGainMixed = inputGain[channel];
					//inputGain[0] * (1.0f - this->triggerSignalRatio[channel])
					//+ inputGain[1] * this->triggerSignalRatio[channel];

					//const float inputGainMixed =
					//	inputGain[channel]
					//	+ inputGain[1 - channel] * channelLinkage;

					//const float inputGainMixed = inputGain[channel] * + inputGain[1 - channel];

					// 入力信号をフィルタにかけて音圧を計算
					// ピークが0.637程度なので増幅する

					// level to decibel
					// -120.0 ~
					//float inputLevel;
					//if (inputGainMixed < 0.000001f)
					float inputLevel;
					if (inputGainMixed < 0.000001f) // input is small enough so omit processing
					{
						inputLevel = -120.0f;
						// dB
						//float inputGain = -120.0f;
						//this->inputLevelLast[channel] = -120.0f;
						//this->reductionLevelLast[channel] = (1.0f - this->releaseRate[channel]) * this->reductionLevelLast[channel];
						////const float clippedOutputGain = outputGain * (1.1885f - 71.0f / (1.0f + fabsf(outputGain - 120.0f))) - 12.55f;
						//float outputLevel = this->makeupGain[channel] - this->reductionLevelLast[channel];
						//if (this->clippingEnabled[2] == true) {
						//	outputLevel = Converter::adjustableClipper(outputLevel, this->clippingThresholdLevel[channel], this->clippingHardness[channel]);
						//}
						//float outputGain = Converter::decibelToLevel(outputLevel);
						//out[channel][i] = in[channel][i] * outputGain;
						//continue;
					}
					else // calculate in right way
					{
						inputLevel = Converter::levelToDecibel(inputGainMixed);
					}

					float targetOutputLevel;

					if (this->kneeWidth[channel] < 2.0f * (inputLevel - this->thresholdLevel[channel])) // above knee
					{
						targetOutputLevel = this->thresholdLevel[channel] + (inputLevel - this->thresholdLevel[channel]) * this->compressionRatioInverse[channel];
					}
					else if (2.0f * fabsf(inputLevel - this->thresholdLevel[channel]) <= this->kneeWidth[channel]) // in knee
					{
						targetOutputLevel = this->thresholdLevel[channel]
							- (this->kneeWidth[channel] / 2.0f)
							+ (1.0f + this->compressionRatioInverse[channel]) * ((inputLevel - this->thresholdLevel[channel] + (this->kneeWidth[channel] / 2.0f)) / 2.0f);
					}
					else // below knee
					{
						this->reductionLevelLast[channel] *= (1.0f - this->releaseRate[channel]);
						//const float outputLevel = this->makeupLevel[channel] - reductionLevelLast[channel];
						//const float clippedOutputGain = outputGain * (1.1885f - 71.0f / (1.0f + fabsf(outputGain - 120.0f))) - 12.55f;
						//const float clippedOutputGain = Converter::adjustableClipper(outputGain, 0.0f, 1.0f);
						//float outputLevel = Converter::decibelToLevel(clippedOutputGain);

						float outputLevel = this->makeupLevel[channel] - this->reductionLevelLast[channel];

						if (this->clippingEnabled[channel] == true) {
							outputLevel = Converter::adjustableClipper(outputLevel, this->clippingThresholdLevel[channel], this->clippingHardness[channel]);
						}

						float outputGain = Converter::decibelToLevel(outputLevel);
						out[channel][i] = in[channel][i] * outputGain;
						continue;
					}

					// slow below this line
					const float	targetReductionLevel = (inputLevel - targetOutputLevel) * this->reductionRatio[channel];
					const float reductionGap = targetReductionLevel - this->reductionLevelLast[channel];

					const float transferRate = 0.0f < reductionGap ? this->attackRate[channel] : this->releaseRate[channel];
					const float reductionLevel = this->reductionLevelLast[channel] + transferRate * reductionGap;

					this->reductionLevelLast[channel] = reductionLevel;

					// TODO
					// clip before makeup
					float outputLevel = this->makeupLevel[channel] - reductionLevel;
					if (this->clippingEnabled[channel] == true) {
						outputLevel = Converter::adjustableClipper(outputLevel, this->clippingThresholdLevel[channel], this->clippingHardness[channel]);
					}
					float outputGain = Converter::decibelToLevel(outputLevel);
					out[channel][i] = in[channel][i] * outputGain;
				}

				if (this->bypassed[0] == true)
				{
					out[0][i] = in[0][i];
				}
				else if (this->muted[0] == true)
				{
					out[0][i] = 0.0f;
				}
				else
				{
					out[0][i] = (out[0][i] * this->wetRatio[0] + inputGainLeft * (1.0f - this->wetRatio[0]));
				}

				if (this->bypassed[1] == true)
				{
					out[1][i] = in[1][i];
				}
				else if (this->muted[1] == true)
				{
					out[1][i] = 0.0f;
				}
				else
				{
					out[1][i] = (out[1][i] * this->wetRatio[1] + inputGainRight * (1.0f - this->wetRatio[1]));
				}

				// decode from [mid, side] to [left, right]
				if (this->midSideModeEnabled == true) {
					const float right = out[0][i] - out[1][i];
					out[0][i] = (out[0][i] + out[1][i]) * 0.5f;
					out[1][i] = right * 0.5f;
				}

			}

			// REOPEN -> BRAKE
			// kanpeki
			// meter muzui

			const float inputLevelLastML = Converter::levelToDecibel(fmaxf(0.000001, this->inputSmoothing[0].last_value() * 1.569858f));// -1.21f;
			const float inputLevelLastSR = Converter::levelToDecibel(fmaxf(0.000001, this->inputSmoothing[1].last_value() * 1.569858f));// -1.21f;

			const float scale = 1.0f / 120.0f;

			// TODO: FIX METER

			const float lastOutputGainML = 1.0f + ((inputLevelLastML + this->makeupLevel[0] - this->reductionLevelLast[0]) - 120.0f + 3.2f) * scale;
			const float lastOutputGainSR = 1.0f + ((inputLevelLastSR + this->makeupLevel[1] - this->reductionLevelLast[1]) - 120.0f + 3.2f) * scale;
			const float guiOutputGainML = (0.738f * (23.4f * lastOutputGainML) / (1.0f + fabsf(23.4f * lastOutputGainML)) + 0.999f * 0.738f - 0.022f);
			const float guiOutputGainSR = (0.738f * (23.4f * lastOutputGainSR) / (1.0f + fabsf(23.4f * lastOutputGainSR)) + 0.999f * 0.738f - 0.022f);

			const float guiReductionGainML = this->reductionLevelLast[0] * scale;
			const float guiReductionGainSR = this->reductionLevelLast[1] * scale;

			// パラメーターのフィードバックの処理
			// 音声処理中に変更されたパラメーターがある時、dataのoutputParameterChangesに
			// 変更を設定することで、パラメーター操作クラスに値をフィードバックできる

			// フィードバック用のIParameterChangesクラスのポインタを取得
			IParameterChanges* outParamChanges = data.outputParameterChanges;

			if (outParamChanges) // 正常にポインタが取得出来たか確認
			{
				// フィードバックするパラメーターを追加するとともにパラメーター値変更用のキューを取得
				// (処理するサンプル内に複数回パラメーター変更情報がある可能性があるため、
				// キューという形になっている。)
				int32 index = 0;  // 何番目のパラメーターかを設定(0から順に増やす)
				IParamValueQueue* paramQueue = outParamChanges->addParameterData(PARAMETER_OUTPUT_LEVEL_ML, index);
				if (paramQueue)
				{
					// フィードバックする値を設定する
					int32 queueIndex = 0;
					paramQueue->addPoint(0, guiOutputGainML, queueIndex); //feedbackValueはフィードバックする値とする
				}

				//index = 1;
				paramQueue = outParamChanges->addParameterData(PARAMETER_OUTPUT_LEVEL_SR, index);
				if (paramQueue)
				{
					// フィードバックする値を設定する
					int32 queueIndex = 0;
					paramQueue->addPoint(0, guiOutputGainSR, queueIndex); //feedbackValueはフィードバックする値とする
				}

				//index = 2;
				paramQueue = outParamChanges->addParameterData(PARAMETER_CLIPPING_LEVEL_ML, index);
				if (paramQueue)
				{
					//TODO clippingチャンと計算する
					// フィードバックする値を設定する
					int32 queueIndex = 0;
					paramQueue->addPoint(0, guiReductionGainML, queueIndex); //feedbackValueはフィードバックする値とする
				}

				//index = 3;
				paramQueue = outParamChanges->addParameterData(PARAMETER_CLIPPING_LEVEL_SR, index);
				if (paramQueue)
				{
					// フィードバックする値を設定する
					int32 queueIndex = 0;
					paramQueue->addPoint(0, guiReductionGainSR, queueIndex); //feedbackValueはフィードバックする値とする
				}
			}

			// 問題なければkResultTrueを返す(おそらく必ずkResultTrueを返す)
			return kResultTrue;
		}


	}
} // namespace SteinbergとVstの終わり
