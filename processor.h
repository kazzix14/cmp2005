#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

// VST3 SDKのインクルードファイル
#include "public.sdk/source/vst/vstaudioeffect.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

// 自作VST用のインクルードファイル
#include "myvst3def.h"
#include "rms.h"
#include "smoothing.h"

#include <initializer_list>

// VST3作成に必要なの名前空間を使用
namespace Steinberg {
	namespace Vst {

		// ============================================================================================
		// 音声信号を処理するProcessorクラス
		// ============================================================================================
		class MyVSTProcessor : public AudioEffect
		{
		protected:
			// gain  -> scale
			// level -> dB

			// parameter values
			float compressionRatio[2];
			float thresholdLevel[2];
			float makeupLevel[2];
			float attackTime[2];
			float releaseTime[2];//
			float kneeWidth[2];
			float inputFilterSmoothness[2];
			float triggerSignalRatio[2];//
			float wetRatio[2];//
			float clippingHardness[2];//
			float clippingThresholdLevel[2];
			bool clippingEnabled[2];
			bool midSideModeEnabled;//
			bool bypassed[2];
			bool muted[2];

			// pre-calcurated coefficients
			float compressionRatioInverse[2];
			float reductionRatio[2];
			float makeupGain[2];
			float attackRate[2];
			float releaseRate[2];
			float kneeWidthInverse[2];
			float kneeLowerEdgeLevel[2];

			float reductionLevelLast[2];

			CMySmoothing inputSmoothing[2] = { CMySmoothing(), CMySmoothing() };

			float sampleRate = 44100.0f; // サンプルレート
			float sampleRateInverse = 1.0f / 44100.0f; // サンプルレートの逆数

		public:
			static inline const float compressionRatioMax = 60.0f;
			static inline const float compressionRatioMin = 1.0f;
			static inline const float compressionRatioDefault = 1.0f;
			static inline const float thresholdLevelMax = 0.0f;
			static inline const float thresholdLevelMin = -60.0f;
			static inline const float thresholdLevelDefault = 0.0f;
			static inline const float makeupLevelMax = 24.0f;
			static inline const float makeupLevelMin = 0.0f;
			static inline const float makeupLevelDefault = 0.0f;
			static inline const float attackTimeMax = 999.999f;
			static inline const float attackTimeMin = 0.001f;
			static inline const float attackTimeDefault = 30.000f;
			static inline const float releaseTimeMax = 999.999f;
			static inline const float releaseTimeMin = 0.001f;
			static inline const float releaseTimeDefault = 150.000f;
			static inline const float kneeWidthMax = 24.0f;
			static inline const float kneeWidthMin = 0.0f;
			static inline const float kneeWidthDefault = 0.0f;
			static inline const float inputFilterSmoothnessMax = 12800.0f;
			static inline const float inputFilterSmoothnessMin = 1.0f;
			static inline const float inputFilterSmoothnessDefault = 640.0f;
			static inline const float clippingHardnessMax = 1000.0f;
			static inline const float clippingHardnessMin = 0.001f;
			static inline const float clippingHardnessDefault = 1.000f;
			static inline const float clippingThresholdLevelMax = 0.0f;
			static inline const float clippingThresholdLevelMin = -12.0f;
			static inline const float clippingThresholdLevelDefault = 0.0f;
			static inline const float wetRatioDefault = 1.0f;
			static inline const float triggerSignalRatioDefault = 0.5f;
			static inline const bool clippingEnabledDefault = true;
			static inline const bool midSideModeEnabledDefault = false;
			static inline const bool bypassedDefault = false;
			static inline const bool mutedDefault = false;

			MyVSTProcessor();

			// クラスを初期化する関数(必須)
			tresult PLUGIN_API initialize(FUnknown* context);

			// バス構成を設定する関数
			tresult PLUGIN_API setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts);

			// 処理を設定する関数
			tresult PLUGIN_API setupProcessing(ProcessSetup &setup);

			// 音声信号を処理する関数
			tresult PLUGIN_API process(ProcessData& data);

			// 自作VST Processorクラスのインスタンスを作成するための関数(必須)
			static FUnknown* createInstance(void*) { return (IAudioProcessor*)new MyVSTProcessor(); }
		};


	}
} // namespace SteinbergとVstの終わり

#endif