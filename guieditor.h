#ifndef __GUIEDITOR_H__
#define __GUIEDITOR_H__

// VST3 SDKのインクルードファイル
#include "public.sdk/source/vst/vstguieditor.h"
#include "pluginterfaces/vst/ivstplugview.h"

// 自作VST用のインクルードファイル
#include "myvst3def.h"

// VST3作成に必要なの名前空間を使用
namespace Steinberg {
	namespace Vst {



		using namespace VSTGUI;

		// ============================================================================================
		// VSTのGUIクラス
		// ============================================================================================
		class MyVSTGUIEditor : public VSTGUIEditor, public IControlListener
		{
		protected:
			CMovieBitmap* outputLevelMeterML;
			CMovieBitmap* outputLevelMeterSR;
			CMovieBitmap* thresholdMeterML;
			CMovieBitmap* thresholdMeterSR;
			//CVuMeter* reductionLevelMeterML;
			//CVuMeter* reductionLevelMeterSR;

			CMovieBitmap* clipLightML;
			CMovieBitmap* clipLightSR;

			CAnimKnob* compressionRatioML;
			CAnimKnob* compressionRatioSR;
			CAnimKnob* thresholdLevelML;
			CAnimKnob* thresholdLevelSR;
			CAnimKnob* makeupLevelML;
			CAnimKnob* makeupLevelSR;
			CAnimKnob* attackTimeML;
			CAnimKnob* attackTimeSR;
			CAnimKnob* releaseTimeML;
			CAnimKnob* releaseTimeSR;
			CAnimKnob* kneeWidthML;
			CAnimKnob* kneeWidthSR;
			CAnimKnob* inputFilterSmoothnessML;
			CAnimKnob* inputFilterSmoothnessSR;
			CAnimKnob* triggerSignalRatioML;
			CAnimKnob* triggerSignalRatioSR;
			CAnimKnob* wetRatioML;
			CAnimKnob* wetRatioSR;
			CAnimKnob* clippingHardnessML;
			CAnimKnob* clippingHardnessSR;
			CAnimKnob* clippingThresholdLevelML;
			CAnimKnob* clippingThresholdLevelSR;
			CHorizontalSwitch* clippingEnabledML;
			CHorizontalSwitch* clippingEnabledSR;
			CHorizontalSwitch* stateML;
			CHorizontalSwitch* stateSR;
			CHorizontalSwitch* controlLinkML;
			CHorizontalSwitch* modeSR;

			CTextLabel* compressionRatioLabelML;
			CTextLabel* compressionRatioLabelSR;
			CTextLabel* thresholdLevelLabelML;
			CTextLabel* thresholdLevelLabelSR;
			CTextLabel* makeupLevelLabelML;
			CTextLabel* makeupLevelLabelSR;
			CTextLabel* attackTimeLabelML;
			CTextLabel* attackTimeLabelSR;
			CTextLabel* releaseTimeLabelML;
			CTextLabel* releaseTimeLabelSR;
			CTextLabel* kneeWidthLabelML;
			CTextLabel* kneeWidthLabelSR;
			CTextLabel* inputFilterSmoothnessLabelML;
			CTextLabel* inputFilterSmoothnessLabelSR;
			CTextLabel* triggerSignalRatioLabelML;
			CTextLabel* triggerSignalRatioLabelSR;
			CTextLabel* wetRatioLabelML;
			CTextLabel* wetRatioLabelSR;
			CTextLabel* clippingHardnessLabelML;
			CTextLabel* clippingHardnessLabelSR;
			CTextLabel* clippingThresholdLevelLabelML;
			CTextLabel* clippingThresholdLevelLabelSR;

			//bool clippingEnabled
			//bool midSideModeEnabled
			//bool bypassed
			//bool muted

			//CTextLabel* smoothnessLabel;
			//CHorizontalSwitch* smoothnessSwitch;

		public:
			// コンストラクタ(必須)
			MyVSTGUIEditor(void* controller);

			// GUIウィンドウを開いたときに呼び出される関数(必須)
			virtual bool PLUGIN_API open(void* parent, const PlatformType& platformType = kDefaultNative);

			// GUIウィンドウを閉じたときに呼び出される関数(必須)
			virtual void PLUGIN_API close();

			// GUIウィンドウのコントローラを操作したときに呼び出される関数(必須)
			void valueChanged(CControl* pControl);

			// タイマーの動作などのメッセージを受け取った時に呼び出される関数
			CMessageResult notify(CBaseObject *sender, const char *message);

			// 自作関数
			// ノブコントロールを作成する関数
			CAnimKnob* createAnimKnobFromPath(ParamID tag, int x, int y, UTF8StringPtr path, int numImages);
			CKnob* createKnob(ParamID tag, int x, int y);
			void createSlider(ParamID tag, int x, int y);
			void createCombobox(ParamID tag, int x, int y);
			CTextLabel* createTextLabel(int x, int y, int w, int h, UTF8StringPtr text);
			CVuMeter* createVuMeter(ParamID tag, int x, int y);
			CRockerSwitch* createRockerSwitch(ParamID tag, int x, int y);
			CHorizontalSwitch* createHorizontalSwitch(ParamID tag, int x, int y, UTF8StringPtr, int numStates);
			CMovieBitmap* createMovieBitmap(ParamID tag, int x, int y, UTF8StringPtr path, int numImages);

			// ラベルアップデート用関数
			void updateLabel(CTextLabel* label, EditController* controller, ParamID tag, const char* baseText);

			// VSTGUIEditorクラスの各種設定を自作GUIクラス置き換えるマクロ(必須)
			DELEGATE_REFCOUNT(VSTGUIEditor)
		};



	}
} // namespace SteinbergとVstの終わり

#endif