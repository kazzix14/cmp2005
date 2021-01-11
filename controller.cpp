// 自作VST用のインクルードファイル
#include "myvst3def.h"
#include "myvst3fuid.h"
#include "controller.h"
#include "processor.h"

// VST3 SDKのインクルードファイル
#include "vstgui/plugin-bindings/vst3editor.h"



// VST3作成に必要なの名前空間を使用
namespace Steinberg {
	namespace Vst {


		// クラスを初期化する関数(必須)
		tresult PLUGIN_API MyVSTController::initialize(FUnknown* context)
		{
			// まず継承元クラスの初期化を実施
			tresult result = EditController::initialize(context);
			if (result == kResultTrue)
			{
				using Vst::MyVSTProcessor;

				MyVSTProcessor::compressionRatioMin;

				// 以下固有の初期化を実施。
				setKnobMode(VSTGUI::CKnobMode::kLinearMode);

				RangeParameter* param = new RangeParameter(STR16("Ratio Left/Mid"), PARAMETER_COMPRESSION_RATIO_ML, STR16(""),
					MyVSTProcessor::compressionRatioMin,
					MyVSTProcessor::compressionRatioMax,
					1.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Ratio Right/Side"), PARAMETER_COMPRESSION_RATIO_SR, STR16(""),
					MyVSTProcessor::compressionRatioMin,
					MyVSTProcessor::compressionRatioMax,
					1.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);



				param = new RangeParameter(STR16("Threshold Left/Mid"), PARAMETER_THRESHOLD_ML, STR16(""),
					MyVSTProcessor::thresholdLevelMin,
					MyVSTProcessor::thresholdLevelMax,
					0.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Threshold Right/Side"), PARAMETER_THRESHOLD_SR, STR16(""),
					MyVSTProcessor::thresholdLevelMin,
					MyVSTProcessor::thresholdLevelMax,
					0.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);



				param = new RangeParameter(STR16("Makeup Left/Mid"), PARAMETER_MAKEUP_ML, STR16(""),
					MyVSTProcessor::makeupLevelMin,
					MyVSTProcessor::makeupLevelMax,
					0.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Makeup Right/Side"), PARAMETER_MAKEUP_SR, STR16(""),
					MyVSTProcessor::makeupLevelMin,
					MyVSTProcessor::makeupLevelMax,
					0.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Attack Left/Mid"), PARAMETER_ATTACK_ML, STR16(""),
					MyVSTProcessor::attackTimeMin,
					MyVSTProcessor::attackTimeMax,
					30.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Attack Right/Side"), PARAMETER_ATTACK_SR, STR16(""),
					MyVSTProcessor::attackTimeMin,
					MyVSTProcessor::attackTimeMax,
					30.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Release Left/Mid"), PARAMETER_RELEASE_ML, STR16(""),
					MyVSTProcessor::releaseTimeMin,
					MyVSTProcessor::releaseTimeMax,
					150.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Release Right/Side"), PARAMETER_RELEASE_SR, STR16(""),
					MyVSTProcessor::releaseTimeMin,
					MyVSTProcessor::releaseTimeMax,
					150.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Knee Left/Mid"), PARAMETER_KNEE_WIDTH_ML, STR16(""),
					MyVSTProcessor::kneeWidthMin,
					MyVSTProcessor::kneeWidthMax,
					0.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Knee Right/Side"), PARAMETER_KNEE_WIDTH_SR, STR16(""),
					MyVSTProcessor::kneeWidthMin,
					MyVSTProcessor::kneeWidthMax,
					0.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Smoothness Left/Mid"), PARAMETER_SMOOTHNESS_ML, STR16(""),
					0.0f,
					1.0f,
					0.5f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(2); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Smoothness Right/Side"), PARAMETER_SMOOTHNESS_SR, STR16(""),
					0.0f,
					1.0f,
					0.5f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(2); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Trigger Signal Left/Mid"), PARAMETER_TRIGGER_SIGNAL_RATIO_ML, STR16(""),
					0.0f,
					1.0f,
					0.5f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Trigger Signal Right/Side"), PARAMETER_TRIGGER_SIGNAL_RATIO_SR, STR16(""),
					0.0f,
					1.0f,
					0.5f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Mix Left/Mid"), PARAMETER_WET_RATIO_ML, STR16(""),
					0.0f,
					1.0f,
					1.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Mix Right/Side"), PARAMETER_WET_RATIO_SR, STR16(""),
					0.0f,
					1.0f,
					1.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Clipping Hardness Left/Mid"), PARAMETER_CLIPPING_HARDNESS_ML, STR16(""),
					MyVSTProcessor::clippingHardnessMin,
					MyVSTProcessor::clippingHardnessMax,
					1.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Clipping Hardness Right/Side"), PARAMETER_CLIPPING_HARDNESS_SR, STR16(""),
					MyVSTProcessor::clippingHardnessMin,
					MyVSTProcessor::clippingHardnessMax,
					1.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Clipping Threshold Left/Mid"), PARAMETER_CLIPPING_THRESHOLD_ML, STR16(""),
					MyVSTProcessor::clippingThresholdLevelMin,
					MyVSTProcessor::clippingThresholdLevelMax,
					0.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Clipping Threshold Right/Side"), PARAMETER_CLIPPING_THRESHOLD_SR, STR16(""),
					MyVSTProcessor::clippingThresholdLevelMin,
					MyVSTProcessor::clippingThresholdLevelMax,
					0.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Clipping Left/Mid"), PARAMETER_CLIPPING_ENABLED_ML, STR16(""),
					0.0f,
					1.0f,
					1.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Clipping Right/Side"), PARAMETER_CLIPPING_ENABLED_SR, STR16(""),
					0.0f,
					1.0f,
					1.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Mid Side Mode"), PARAMETER_MID_SIDE_MODE_ENABLED, STR16(""),
					0.0f,
					1.0f,
					0.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				param = new RangeParameter(STR16("Control Link"), PARAMETER_CONTROL_LINK_ENABLED, STR16(""),
					0.0f,
					1.0f,
					0.0f,
					0,
					ParameterInfo::kCanAutomate);
				param->setPrecision(3); // 小数第何位まで表示するか
				parameters.addParameter(param);

				parameters.addParameter(STR16("Output Left/Mid"), STR16(""), 0.0, 1.0, ParameterInfo::kIsReadOnly, PARAMETER_OUTPUT_LEVEL_ML);
				parameters.addParameter(STR16("Output Right/Side"), STR16(""), 0.0, 1.0, ParameterInfo::kIsReadOnly, PARAMETER_OUTPUT_LEVEL_SR);
				parameters.addParameter(STR16("Clipping Left/Mid"), STR16(""), -20.3, 3.0, ParameterInfo::kIsReadOnly, PARAMETER_CLIPPING_LEVEL_ML);
				parameters.addParameter(STR16("Clippint Right/Side"), STR16(""), -20.3, 3.0, ParameterInfo::kIsReadOnly, PARAMETER_CLIPPING_LEVEL_SR);
				//parameters.addParameter(STR16("Reduction Left/Mid"), STR16("dB"), -60.0, 0.0, ParameterInfo::kIsReadOnly, PARAMETER_REDUCTION_LEVEL_ML);
				//parameters.addParameter(STR16("Reduction Right/Side"), STR16("dB"), -60.0, 0.0, ParameterInfo::kIsReadOnly, PARAMETER_REDUCTION_LEVEL_SR);

			}

			// 初期化が成功すればkResultTrueを返す。
			result = kResultTrue;
			return result;
		}

		// 自作VST GUIEditorを作成する関数
		IPlugView* PLUGIN_API MyVSTController::createView(const char* name)
		{
			// editorを指定された場合
			if (strcmp(name, "editor") == 0)
			{
				// 自作GUIクラスのインスタンスを作成し返す
				MyVSTGUIEditor* view = new MyVSTGUIEditor(this);
				return view;
			}
			return 0;
		}

	}
} // namespace SteinbergとVstの終わり
