// 自作VST用のインクルードファイル
#include "guieditor.h"

#include "converter.h"

// VST3作成に必要なの名前空間を使用
namespace Steinberg {
	namespace Vst {

		const static int frameWidth = 950;
		const static int frameHeight = 350;

		// ============================================================================================
		// コンストラクタ
		// ============================================================================================
		MyVSTGUIEditor::MyVSTGUIEditor(void* controller)
			: VSTGUIEditor(controller)
		{
			// コンストラクタでウィンドウサイズを設定する
			// 設定しなければ、ウィンドウが開かない
			ViewRect viewRect(0, 0, frameWidth, frameHeight);
			setRect(viewRect);

			// 30 fps
			setIdleRate(10);
		}

		// ============================================================================================
		// GUIウィンドウを開いたとき、閉じたときに呼び出される関数
		// ============================================================================================
		bool PLUGIN_API MyVSTGUIEditor::open(void* parent, const PlatformType& platformType)
		{
			// GUIウィンドウが開かれたときに、UIを作成する

			// まずはフレーム(配置領域)がすでに作成・設定されているか確認。
			// すでに作成・設定されている場合(frameがNULLでない場合)は終了
			// frameは継承元クラスで定義されている。
			if (frame) { return false; }

			// 作成するフレームのサイズを設定
			CRect size(0, 0, frameWidth, frameHeight);

			// フレームを作成。作成に失敗したら(NULLなら)終了。
			// 引数には、フレームサイズ、自作GUIクラスのポインタを指定する
			frame = new CFrame(size, this);
			if (frame == NULL) { return false; }
			frame->enableTooltips(true);
			frame->kDirtyCallAlwaysOnMainThread = true;

			// 作成したフレームに背景画像を設定
			CBitmap* cbmp = new CBitmap("panel.png"); // リソースから背景画像を読み込む
			frame->setBackground(cbmp); // フレームに背景画像を設定
			cbmp->forget();	// フレームに設定後は背景画像はforgetで解放しておく

			// 作成したフレームを開く
			frame->open(parent);

			//　---------------------------------------------
			// ここから各コントローラの作成
			// !!! UIが重なると更新できない !!!

			constexpr int main_x_space = 119;
			constexpr int main_y_space = 73;
			constexpr int general_y_space = 170;

			constexpr int main_upper_right_x = 656;
			constexpr int main_upper_mid_x = main_upper_right_x - main_x_space;
			constexpr int main_upper_left_x = main_upper_mid_x - main_x_space;

			constexpr int main_lower_right_x = 596;
			constexpr int main_lower_mid_x = main_lower_right_x - main_x_space;
			constexpr int main_lower_left_x = main_lower_mid_x - main_x_space;

			constexpr int upper_upper_y = 25;
			constexpr int upper_lower_y = upper_upper_y + main_y_space;

			constexpr int lower_upper_y = upper_upper_y + general_y_space;
			constexpr int lower_lower_y = lower_upper_y + main_y_space;

			constexpr int clipper_x_space = 102;
			constexpr int clipper_right_x = main_lower_left_x - clipper_x_space;
			constexpr int clipper_left_x = clipper_right_x - 50;

			constexpr int lower_clipper_right_lower_y = lower_lower_y + 7;
			constexpr int lower_clipper_right_upper_y = lower_clipper_right_lower_y - 70;
			constexpr int lower_clipper_left_lower_y = lower_clipper_right_lower_y + 5;
			constexpr int lower_clipper_left_upper_y = lower_clipper_right_lower_y - 50;

			constexpr int upper_clipper_right_lower_y = lower_clipper_right_lower_y - general_y_space;
			constexpr int upper_clipper_right_upper_y = lower_clipper_right_upper_y - general_y_space;
			constexpr int upper_clipper_left_lower_y = lower_clipper_left_lower_y - general_y_space;
			constexpr int upper_clipper_left_upper_y = lower_clipper_left_upper_y - general_y_space;

			constexpr int lower_threshold_bar_x = main_lower_right_x + 254;
			constexpr int upper_threshold_bar_x = lower_threshold_bar_x;
			constexpr int lower_output_bar_x = main_lower_right_x + 252;
			constexpr int upper_output_bar_x = lower_output_bar_x + 1;

			constexpr int lower_threshold_bar_y = lower_upper_y + 1;
			constexpr int upper_threshold_bar_y = lower_threshold_bar_y - general_y_space;
			constexpr int lower_output_bar_y = lower_upper_y + 2;
			constexpr int upper_output_bar_y = lower_output_bar_y - general_y_space + 2;

			constexpr int lower_main_threshold_x = main_upper_right_x + 90;
			constexpr int upper_main_threshold_x = lower_main_threshold_x;

			constexpr int lower_main_threshold_y = lower_upper_y - 00;
			constexpr int upper_main_threshold_y = lower_main_threshold_y - general_y_space;

			constexpr int general_right_x = clipper_left_x - 95;
			constexpr int general_left_x = general_right_x - 53;

			constexpr int lower_general_lower_right_y = lower_clipper_right_lower_y;
			constexpr int lower_general_upper_right_y = lower_general_lower_right_y - 70;
			constexpr int lower_general_lower_left_y = lower_general_lower_right_y + 3;
			constexpr int lower_general_upper_left_y = lower_general_lower_right_y - 70;
			constexpr int upper_general_lower_right_y = lower_general_lower_right_y - general_y_space;
			constexpr int upper_general_upper_right_y = lower_general_upper_right_y - general_y_space;
			constexpr int upper_general_lower_left_y = lower_general_lower_left_y - general_y_space;
			constexpr int upper_general_upper_left_y = lower_general_upper_left_y - general_y_space;

			//////////////////

			//this->meterUpdaterSR = new CViewContainer(CRect(lower_threshold_bar_x, lower_threshold_bar_y, frameWidth, frameHeight));
			//////////////////


			this->thresholdMeterML = createMovieBitmap(PARAMETER_THRESHOLD_ML, upper_threshold_bar_x, upper_threshold_bar_y, u8"u_threshold_bar.png", 260);
			this->outputLevelMeterML = createMovieBitmap(PARAMETER_OUTPUT_LEVEL_ML, upper_output_bar_x, upper_output_bar_y, u8"u_output_bar.png", 260);

			this->thresholdMeterSR = createMovieBitmap(PARAMETER_THRESHOLD_SR, lower_threshold_bar_x, lower_threshold_bar_y, u8"l_threshold_bar.png", 260);
			this->outputLevelMeterSR = createMovieBitmap(PARAMETER_OUTPUT_LEVEL_SR, lower_output_bar_x, lower_output_bar_y, u8"l_output_bar.png", 260);

			this->clipLightML = createMovieBitmap(PARAMETER_CLIPPING_LEVEL_ML, clipper_left_x, upper_clipper_left_lower_y, u8"u_clip_light.png", 32);
			this->clipLightSR = createMovieBitmap(PARAMETER_CLIPPING_LEVEL_SR, clipper_left_x, lower_clipper_left_lower_y, u8"l_clip_light.png", 32);

			//this->reductionLevelMeterML = createVuMeter(PARAMETER_REDUCTION_LEVEL_ML, 1040, 10);
			//this->reductionLevelMeterSR = createVuMeter(PARAMETER_REDUCTION_LEVEL_SR, 1040, 140);

			//this->thresholdMeterSR->setDirty();
			//this->outputLevelMeterML = createVuMeter(PARAMETER_OUTPUT_LEVEL_ML, 1010, 10);



			this->compressionRatioML = createAnimKnobFromPath(PARAMETER_COMPRESSION_RATIO_ML, main_upper_right_x, upper_upper_y, u8"u_ratio.png", 270);
			//this->compressionRatioLabelML = createTextLabel(10, 10, 240, 10, u8"");
			//updateLabel(this->compressionRatioLabelML, controller, PARAMETER_COMPRESSION_RATIO_ML, u8"Compression Ratio Left/Mid: %s");

			this->compressionRatioSR = createAnimKnobFromPath(PARAMETER_COMPRESSION_RATIO_SR, main_upper_right_x, lower_upper_y, u8"l_ratio.png", 270);
			//this->compressionRatioLabelSR = createTextLabel(260, 10, 240, 10, u8"");
			//updateLabel(this->compressionRatioLabelSR, controller, PARAMETER_COMPRESSION_RATIO_SR, u8"Compression Ratio Right/Side: %s");

			// u m
			this->thresholdLevelML = createAnimKnobFromPath(PARAMETER_THRESHOLD_ML, lower_main_threshold_x, upper_main_threshold_y, u8"u_main_threshold.png", 200);
			//this->thresholdLevelML = createAnimKnobFromPath(PARAMETER_COMPRESSION_RATIO_SR, 656, 95, u8"u_threshold.png", 270);
			//this->thresholdLevelLabelML = createTextLabel(510, 10, 240, 10, u8"");
			//updateLabel(this->thresholdLevelLabelML, controller, PARAMETER_THRESHOLD_ML, u8"Threshold Left/Mid: %s");

			this->thresholdLevelSR = createAnimKnobFromPath(PARAMETER_THRESHOLD_SR, lower_main_threshold_x, lower_main_threshold_y, u8"l_main_threshold.png", 200);
			//this->thresholdLevelLabelSR = createTextLabel(760, 10, 240, 10, u8"");
			//updateLabel(this->thresholdLevelLabelSR, controller, PARAMETER_THRESHOLD_SR, u8"Threshold Right/Side: %s");

			this->makeupLevelML = createAnimKnobFromPath(PARAMETER_MAKEUP_ML, main_upper_mid_x, upper_upper_y, u8"u_makeup.png", 270);
			//this->makeupLevelLabelML = createTextLabel(10, 80, 240, 10, u8"");
			//updateLabel(this->makeupLevelLabelML, controller, PARAMETER_MAKEUP_ML, u8"Makeup Left/Mid: %s");

			this->makeupLevelSR = createAnimKnobFromPath(PARAMETER_MAKEUP_SR, main_upper_mid_x, lower_upper_y, u8"l_makeup.png", 270);
			//this->makeupLevelLabelSR = createTextLabel(260, 80, 240, 10, u8"");
			//updateLabel(this->makeupLevelLabelSR, controller, PARAMETER_MAKEUP_SR, u8"Makeup Right/Side: %s");

			this->attackTimeML = createAnimKnobFromPath(PARAMETER_ATTACK_ML, main_lower_right_x, upper_lower_y, u8"u_attack.png", 270);
			//this->attackTimeLabelML = createTextLabel(510, 80, 240, 10, u8"");
			//updateLabel(this->attackTimeLabelML, controller, PARAMETER_ATTACK_ML, u8"Attack Left/Mid: %s");

			this->attackTimeSR = createAnimKnobFromPath(PARAMETER_ATTACK_SR, main_lower_right_x, lower_lower_y, u8"l_attack.png", 270);
			//this->attackTimeLabelSR = createTextLabel(760, 80, 240, 10, u8"");
			//updateLabel(this->attackTimeLabelSR, controller, PARAMETER_ATTACK_SR, u8"Attack Right/Side: %s");

			this->releaseTimeML = createAnimKnobFromPath(PARAMETER_RELEASE_ML, main_lower_mid_x, upper_lower_y, u8"u_release.png", 270);
			//this->releaseTimeLabelML = createTextLabel(10, 150, 240, 10, u8"");
			//updateLabel(this->releaseTimeLabelML, controller, PARAMETER_RELEASE_ML, u8"Release Left/Mid: %s");

			this->releaseTimeSR = createAnimKnobFromPath(PARAMETER_RELEASE_SR, main_lower_mid_x, lower_lower_y, u8"l_release.png", 270);
			//this->releaseTimeLabelSR = createTextLabel(260, 150, 240, 10, u8"");
			//updateLabel(this->releaseTimeLabelSR, controller, PARAMETER_RELEASE_SR, u8"Release Right/Side: %s");

			this->kneeWidthML = createAnimKnobFromPath(PARAMETER_KNEE_WIDTH_ML, main_lower_left_x, upper_lower_y, u8"u_knee.png", 270);
			//this->kneeWidthML = createKnob(PARAMETER_KNEE_WIDTH_ML, 510, 170);
			//this->kneeWidthLabelML = createTextLabel(510, 150, 240, 10, u8"");
			//updateLabel(this->kneeWidthLabelML, controller, PARAMETER_KNEE_WIDTH_ML, u8"Knee Width Left/Mid: %s");

			this->kneeWidthSR = createAnimKnobFromPath(PARAMETER_KNEE_WIDTH_SR, main_lower_left_x, lower_lower_y, u8"l_knee.png", 270);
			//this->kneeWidthLabelSR = createTextLabel(760, 150, 240, 10, u8"");
			//updateLabel(this->kneeWidthLabelSR, controller, PARAMETER_KNEE_WIDTH_SR, u8"Knee Width Right/Side: %s");

			this->inputFilterSmoothnessML = createAnimKnobFromPath(PARAMETER_SMOOTHNESS_ML, main_upper_left_x, upper_upper_y, u8"u_smooth.png", 270);
			//this->inputFilterSmoothnessML = createKnob(PARAMETER_SMOOTHNESS_ML, 10, 240);
			//this->inputFilterSmoothnessLabelML = createTextLabel(10, 220, 240, 10, u8"");
			//updateLabel(this->inputFilterSmoothnessLabelML, controller, PARAMETER_SMOOTHNESS_ML, u8"Smoothness Left/Mid: %s");

			this->inputFilterSmoothnessSR = createAnimKnobFromPath(PARAMETER_SMOOTHNESS_SR, main_upper_left_x, lower_upper_y, u8"l_smooth.png", 270);
			//this->inputFilterSmoothnessLabelSR = createTextLabel(260, 220, 240, 10, u8"");
			//updateLabel(this->inputFilterSmoothnessLabelSR, controller, PARAMETER_SMOOTHNESS_SR, u8"Smoothness Right/Side: %s");

			this->triggerSignalRatioML = createAnimKnobFromPath(PARAMETER_TRIGGER_SIGNAL_RATIO_ML, general_right_x, upper_general_lower_right_y, u8"u_trigger.png", 270);
			//this->triggerSignalRatioLabelML = createTextLabel(510, 220, 240, 10, u8"");
			//updateLabel(this->triggerSignalRatioLabelML, controller, PARAMETER_TRIGGER_SIGNAL_RATIO_ML, u8"Trigger Signal Left/Mid: %s");

			this->triggerSignalRatioSR = createAnimKnobFromPath(PARAMETER_TRIGGER_SIGNAL_RATIO_SR, general_right_x, lower_general_lower_right_y, u8"l_trigger.png", 270);
			//this->triggerSignalRatioLabelSR = createTextLabel(760, 220, 240, 10, u8"");
			//updateLabel(this->triggerSignalRatioLabelSR, controller, PARAMETER_TRIGGER_SIGNAL_RATIO_SR, u8"Trigger Signal Right/Side: %s");

			this->wetRatioML = createAnimKnobFromPath(PARAMETER_WET_RATIO_ML, general_right_x, upper_general_upper_right_y, u8"u_mix.png", 270);
			//this->wetRatioML = createKnob(PARAMETER_WET_RATIO_ML, 10, 310);
			//this->wetRatioLabelML = createTextLabel(10, 290, 240, 10, u8"");
			//updateLabel(this->wetRatioLabelML, controller, PARAMETER_WET_RATIO_ML, u8"Wet Ratio Left/Mid: %s");

			this->wetRatioSR = createAnimKnobFromPath(PARAMETER_WET_RATIO_SR, general_right_x, lower_general_upper_right_y, u8"l_mix.png", 270);
			//this->wetRatioSR = createKnob(PARAMETER_WET_RATIO_SR, 260, 310);
			//this->wetRatioLabelSR = createTextLabel(260, 290, 240, 10, u8"");
			//updateLabel(this->wetRatioLabelSR, controller, PARAMETER_WET_RATIO_SR, u8"Wet Ratio Right/Side: %s");

			this->clippingHardnessML = createAnimKnobFromPath(PARAMETER_CLIPPING_HARDNESS_ML, clipper_right_x, upper_clipper_right_lower_y, u8"u_slope.png", 270);
			//this->clippingHardnessLabelML = createTextLabel(510, 290, 240, 10, u8"");
			//updateLabel(this->clippingHardnessLabelML, controller, PARAMETER_CLIPPING_HARDNESS_ML, u8"Clipping Hardness Left/Mid: %s");

			this->clippingHardnessSR = createAnimKnobFromPath(PARAMETER_CLIPPING_HARDNESS_SR, clipper_right_x, lower_clipper_right_lower_y, u8"l_slope.png", 270);
			//this->clippingHardnessLabelSR = createTextLabel(760, 290, 240, 10, u8"");
			//updateLabel(this->clippingHardnessLabelSR, controller, PARAMETER_CLIPPING_HARDNESS_SR, u8"Clipping Hardness Right/Side: %s");


			this->clippingThresholdLevelML = createAnimKnobFromPath(PARAMETER_CLIPPING_THRESHOLD_ML, clipper_right_x, upper_clipper_right_upper_y, u8"u_clip_threshold.png", 270);
			//this->clippingThresholdLevelLabelML = createTextLabel(10, 360, 240, 10, u8"");
			//updateLabel(this->clippingThresholdLevelLabelML, controller, PARAMETER_CLIPPING_THRESHOLD_ML, u8"Clipping Threshold Left/Mid: %s");

			this->clippingThresholdLevelSR = createAnimKnobFromPath(PARAMETER_CLIPPING_THRESHOLD_SR, clipper_right_x, lower_clipper_right_upper_y, u8"l_clip_threshold.png", 270);
			//this->clippingThresholdLevelLabelSR = createTextLabel(260, 360, 240, 10, u8"");
			//updateLabel(this->clippingThresholdLevelLabelSR, controller, PARAMETER_CLIPPING_THRESHOLD_SR, u8"Clipping Threshold Right/Side: %s");

			this->clippingEnabledML = createHorizontalSwitch(PARAMETER_CLIPPING_ENABLED_ML, clipper_left_x, upper_clipper_left_upper_y, u8"u_clip.png", 2);
			this->clippingEnabledSR = createHorizontalSwitch(PARAMETER_CLIPPING_ENABLED_SR, clipper_left_x, lower_clipper_left_upper_y, u8"l_clip.png", 2);

			// TODO bypassed じゃなくて state みたいの作る
			this->stateML = createHorizontalSwitch(PARAMETER_BYPASSED_ML, general_left_x, upper_general_upper_left_y, u8"u_state.png", 3);
			this->stateSR = createHorizontalSwitch(PARAMETER_BYPASSED_SR, general_left_x, lower_general_upper_left_y, u8"l_state.png", 3);

			this->controlLinkML = createHorizontalSwitch(PARAMETER_CONTROL_LINK_ENABLED, general_left_x, upper_general_lower_left_y, u8"u_control_link.png", 2);
			this->modeSR = createHorizontalSwitch(PARAMETER_MID_SIDE_MODE_ENABLED, general_left_x, lower_general_lower_left_y, u8"l_mode.png", 2);
			//this->= createHorizontalSwitch(PARAMETER_CLIPPING_ENABLED_SR, clipper_left_x, lower_clipper_left_upper_y, u8"l_clip.png", 2);


			// GUIウィンドウのオープンに成功した場合はtrueを返す
			return true;
		}


		// GUIウィンドウを閉じたときに呼び出される関数
		void PLUGIN_API MyVSTGUIEditor::close()
		{
			// GUIウィンドウが閉じたときに、UIを削除する

			// フレームを解放
			// 背景画像や追加したつまみ(ノブ）やスライダーなどもあわせて解放される
			// (個別で解放する必要はない)
			if (frame)
			{
				frame->forget();
				frame = 0;
			}
		}

		// ============================================================================================
		// GUIウィンドウのコントローラを操作したときに呼び出される関数
		// ============================================================================================
		void MyVSTGUIEditor::valueChanged(CControl* pControl)
		{
			// どのパラメーターが操作されたかを取得する。
			int32 tag = pControl->getTag();

			// パラメーターの値を取得する。
			float value = pControl->getValueNormalized();

			// 非線形ノブの補正
			//switch (tag) {
			//case PARAM_RATIO_TAG:
			//	value = Converter::mySlopeMapper1(value, 0.04f, 6.0f);
			//	break;
			//case PARAM_ATTACK_TIME_TAG:
			//	value = Converter::mySlopeMapper1(value, 0.0f, 3.2f);
			//	break;
			//case PARAM_RELEASE_TIME_TAG:
			//	value = Converter::mySlopeMapper1(value, 0.0f, 3.2f);
			//	break;
			//}

			//switch (tag) {
			//case PARAMETER_THRESHOLD_ML:
			//case PARAMETER_THRESHOLD_SR:
			//}

			// 取得した値をパラメーターに反映させる
			controller->setParamNormalized(tag, value);
			// 音声処理クラスに反映した値を通知する
			controller->performEdit(tag, value);

			// 値を表示したいコントロールが操作されたかどうかを確認
			switch (tag) {
			case PARAMETER_THRESHOLD_ML:
				//updateLabel(thresholdLevelLabelML, controller, tag, u8"Threshold Left/Mid: %s");
				this->thresholdMeterML->setValue(value);
				if(0.1f < this->controller->getParamNormalized(PARAMETER_CONTROL_LINK_ENABLED))
					this->thresholdMeterSR->setValue(value);
				break;
			case PARAMETER_THRESHOLD_SR:
				//updateLabel(thresholdLevelLabelSR, controller, tag, u8"Threshold Right/Side: %s");
				this->thresholdMeterSR->setValue(value);
				if(0.1f < this->controller->getParamNormalized(PARAMETER_CONTROL_LINK_ENABLED))
					this->thresholdMeterML->setValue(value);
				//this->thresholdMeterSR->setDirty(true);
				break;
			}
			if (0.1f < this->controller->getParamNormalized(PARAMETER_CONTROL_LINK_ENABLED))
			{
				switch (tag) {
				case PARAMETER_COMPRESSION_RATIO_ML:
					this->compressionRatioSR->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_COMPRESSION_RATIO_SR, value);
					controller->performEdit(PARAMETER_COMPRESSION_RATIO_SR, value);
					break;
				case PARAMETER_COMPRESSION_RATIO_SR:
					this->compressionRatioML->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_COMPRESSION_RATIO_ML, value);
					controller->performEdit(PARAMETER_COMPRESSION_RATIO_ML, value);
					break;
				case PARAMETER_THRESHOLD_ML:
					this->thresholdLevelSR->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_THRESHOLD_SR, value);
					controller->performEdit(PARAMETER_THRESHOLD_SR, value);
					break;
				case PARAMETER_THRESHOLD_SR:
					this->thresholdLevelML->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_THRESHOLD_ML, value);
					controller->performEdit(PARAMETER_THRESHOLD_ML, value);
					break;
				case PARAMETER_MAKEUP_ML:
					this->makeupLevelSR->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_MAKEUP_SR, value);
					controller->performEdit(PARAMETER_MAKEUP_SR, value);
					break;
				case PARAMETER_MAKEUP_SR:
					this->makeupLevelML->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_MAKEUP_ML, value);
					controller->performEdit(PARAMETER_MAKEUP_ML, value);
					break;
				case PARAMETER_ATTACK_ML:
					this->attackTimeSR->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_ATTACK_SR, value);
					controller->performEdit(PARAMETER_ATTACK_SR, value);
					break;
				case PARAMETER_ATTACK_SR:
					this->attackTimeML->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_ATTACK_ML, value);
					controller->performEdit(PARAMETER_ATTACK_ML, value);
					break;
				case PARAMETER_RELEASE_ML:
					this->releaseTimeSR->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_RELEASE_SR, value);
					controller->performEdit(PARAMETER_RELEASE_SR, value);
					break;
				case PARAMETER_RELEASE_SR:
					this->releaseTimeML->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_RELEASE_ML, value);
					controller->performEdit(PARAMETER_RELEASE_ML, value);
					break;
				case PARAMETER_KNEE_WIDTH_ML:
					this->kneeWidthSR->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_KNEE_WIDTH_SR, value);
					controller->performEdit(PARAMETER_KNEE_WIDTH_SR, value);
					break;
				case PARAMETER_KNEE_WIDTH_SR:
					this->kneeWidthML->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_KNEE_WIDTH_ML, value);
					controller->performEdit(PARAMETER_KNEE_WIDTH_ML, value);
					break;
				case PARAMETER_SMOOTHNESS_ML:
					this->inputFilterSmoothnessSR->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_SMOOTHNESS_SR, value);
					controller->performEdit(PARAMETER_SMOOTHNESS_SR, value);
					break;
				case PARAMETER_SMOOTHNESS_SR:
					this->inputFilterSmoothnessML->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_SMOOTHNESS_ML, value);
					controller->performEdit(PARAMETER_SMOOTHNESS_ML, value);
					break;
				case PARAMETER_WET_RATIO_ML:
					this->wetRatioSR->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_WET_RATIO_SR, value);
					controller->performEdit(PARAMETER_WET_RATIO_SR, value);
					break;
				case PARAMETER_WET_RATIO_SR:
					this->wetRatioML->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_WET_RATIO_ML, value);
					controller->performEdit(PARAMETER_WET_RATIO_ML, value);
					break;
				case PARAMETER_CLIPPING_HARDNESS_ML:
					this->clippingHardnessSR->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_CLIPPING_HARDNESS_SR, value);
					controller->performEdit(PARAMETER_CLIPPING_HARDNESS_SR, value);
					break;
				case PARAMETER_CLIPPING_HARDNESS_SR:
					this->clippingHardnessML->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_CLIPPING_HARDNESS_ML, value);
					controller->performEdit(PARAMETER_CLIPPING_HARDNESS_ML, value);
					break;
				case PARAMETER_CLIPPING_THRESHOLD_ML:
					this->clippingThresholdLevelSR->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_CLIPPING_THRESHOLD_SR, value);
					controller->performEdit(PARAMETER_CLIPPING_THRESHOLD_SR, value);
					break;
				case PARAMETER_CLIPPING_THRESHOLD_SR:
					this->clippingThresholdLevelML->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_CLIPPING_THRESHOLD_ML, value);
					controller->performEdit(PARAMETER_CLIPPING_THRESHOLD_ML, value);
					break;
				case PARAMETER_CLIPPING_ENABLED_ML:
					this->clippingEnabledSR->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_CLIPPING_ENABLED_SR, value);
					controller->performEdit(PARAMETER_CLIPPING_ENABLED_SR, value);
					break;
				case PARAMETER_CLIPPING_ENABLED_SR:
					this->clippingEnabledML->setValueNormalized(value);
					controller->setParamNormalized(PARAMETER_CLIPPING_ENABLED_ML, value);
					controller->performEdit(PARAMETER_CLIPPING_ENABLED_ML, value);
					break;
				}
			}
		}

		// ============================================================================================
		// タイマーの動作などのメッセージを受け取った時に呼び出される関数
		// ============================================================================================
		CMessageResult MyVSTGUIEditor::notify(CBaseObject* sender, const char* message)
		{
			if (message == CVSTGUITimer::kMsgTimer)
			{
				ParamValue value;
				this->frame->setDirty(true);
				//if (inputLevelMeter != nullptr)
				//{
				//	// パラメータ操作クラスから指定したIDのパラメータ値を取得する
				//	ParamValue value = controller->getParamNormalized(PARAM_INPUT_LEVEL_TAG);
				//	inputLevelMeter->setValue(value);
				//	inputLevelMeter->setDirty(true);
				//}
				//if (outputLevelMeter != nullptr)
				//{
				//	// パラメータ操作クラスから指定したIDのパラメータ値を取得する
				//	ParamValue value = controller->getParamNormalized(PARAM_OUTPUT_LEVEL_TAG);
				//	outputLevelMeter->setValue(value);
				//	outputLevelMeter->setDirty(true);
				//}
				//if (reductionLevelMeter != nullptr)
				//{
				//	// パラメータ操作クラスから指定したIDのパラメータ値を取得する
				//	ParamValue value = controller->getParamNormalized(PARAM_REDUCTION_LEVEL_TAG);
				//	reductionLevelMeter->setValue(value);
				//	reductionLevelMeter->setDirty(true);
				//}
				//if (this->thresholdMeterSR != nullptr) {
				//	ParamValue value = controller->getParamNormalized(PARAMETER_THRESHOLD_SR);
				//	this->thresholdMeterSR->setValue(value);
				//	this->thresholdMeterSR->setDirty();
				//	//this->thresholdMeterSR->setDirty(true);
				//	//outputLevelMeterSR->setDirty(true);
				//}
				value = controller->getParamNormalized(PARAMETER_OUTPUT_LEVEL_ML);
				outputLevelMeterML->setValue(value);
				controller->setParamNormalized(PARAMETER_OUTPUT_LEVEL_ML, value);
				value = controller->getParamNormalized(PARAMETER_OUTPUT_LEVEL_SR);
				outputLevelMeterSR->setValue(value);
				controller->setParamNormalized(PARAMETER_OUTPUT_LEVEL_SR, value);
				value = controller->getParamNormalized(PARAMETER_CLIPPING_LEVEL_ML);
				clipLightML->setValue(value);
				controller->setParamNormalized(PARAMETER_CLIPPING_LEVEL_ML, value);
				value = controller->getParamNormalized(PARAMETER_CLIPPING_LEVEL_SR);
				clipLightSR->setValue(value);
				controller->setParamNormalized(PARAMETER_CLIPPING_LEVEL_SR, value);
				//if (reductionLevelMeterML != nullptr) {
				//	ParamValue value = controller->getParamNormalized(PARAMETER_REDUCTION_LEVEL_ML);
				//	reductionLevelMeterML->setValue(value);
				//	//reductionLevelMeterML->setDirty(true);
				//}
				//if (reductionLevelMeterSR != nullptr) {
				//	ParamValue value = controller->getParamNormalized(PARAMETER_REDUCTION_LEVEL_SR);
				//	reductionLevelMeterSR->setValue(value);
				//	//reductionLevelMeterSR->setDirty(true);
				//}
			}

			return VSTGUIEditor::notify(sender, message);
		}

		// ============================================================================================
		// 各コントローラを作成する自作関数
		// ============================================================================================
		CAnimKnob* MyVSTGUIEditor::createAnimKnobFromPath(ParamID tag, int x, int y, UTF8StringPtr path, int numImages)
		{
			// ノブコントロールを作成する

			// まずは画像ファイルを読み込む
			CBitmap* bmp = new CBitmap(path);

			// ノブコントロールのサイズを画像ファイルから取得する
			CRect size(0, 0, bmp->getWidth(), bmp->getHeight() / numImages);

			// ノブコントロールの位置を指定
			// 位置を設定(frameの左上が0,0となる)
			size.offset(x, y);

			// ノブコントロールの作成
			//CVerticalSlider* control = new CVerticalSlider(size, this, tag,
			//	y + bmpmargin,
			//	y + backbmp->getHeight() - (handlebmp->getHeight() + bmpmargin),
			//	handlebmp, backbmp);
			CAnimKnob* control = new CAnimKnob(size, this, tag, numImages, bmp->getHeight() / numImages, bmp);
			UTF8StringPtr tooltipText = "This is a tooltip";
			control->setAttribute(kCViewTooltipAttribute, strlen(tooltipText) + 1, tooltipText);

			// パラメータの現在の値を取得し、コントローラに反映
			// controllerは継承元クラスで定義されており、
			// GUIEditorクラスを呼び出したコントローラ操作クラスを示す。
			ParamValue value = controller->getParamNormalized(tag);
			control->setValueNormalized(value);

			// ノブコントロールをフレームに登録
			frame->addView(control);

			// 読み込んだ画像は忘れず解放
			bmp->forget();

			return control;
		}

		CKnob* MyVSTGUIEditor::createKnob(ParamID tag, int x, int y)
		{
			// ノブコントロールを作成する

			// まずは画像ファイルを読み込む
			CBitmap* backbmp = new CBitmap("knob.png");
			CBitmap* handlebmp = new CBitmap("knob_handle.png");

			// ノブコントロールのサイズを画像ファイルから取得する
			CRect size(0, 0, backbmp->getWidth(), backbmp->getHeight());

			// ノブコントロールの位置を指定
			// 位置を設定(frameの左上が0,0となる)
			size.offset(x, y);

			// ノブコントロールの作成
			CKnob* control = new CKnob(size, this, tag, backbmp, handlebmp);
			UTF8StringPtr tooltipText = "This is a tooltip";
			control->setAttribute(kCViewTooltipAttribute, strlen(tooltipText) + 1, tooltipText);

			// パラメータの現在の値を取得し、コントローラに反映
			// controllerは継承元クラスで定義されており、
			// GUIEditorクラスを呼び出したコントローラ操作クラスを示す。
			ParamValue value = controller->getParamNormalized(tag);
			control->setValueNormalized(value);

			// ノブコントロールをフレームに登録
			frame->addView(control);

			// 読み込んだ画像は忘れず解放
			backbmp->forget();
			handlebmp->forget();

			return control;
		}

		// スライダーを作成するメンバー関数
		void MyVSTGUIEditor::createSlider(ParamID tag, int x, int y)
		{
			// スライダーの作成
			// まずは画像ファイルを読み込む
			CBitmap* backbmp = new CBitmap("slider.png");
			CBitmap* handlebmp = new CBitmap("slider_handle.png");

			// 画像ファイルからサイズを取得する
			CRect  size;
			size(0, 0, backbmp->getWidth(), backbmp->getHeight());
			size.offset(x, y);   // 位置を設定(tergetframeの左上が0,0となる)

			// スライダーの作成
			// スライダーハンドルはスライダー背景画像の上から下まで動かないよう、
			// 若干のマージンを持たせる。このマージンは画像によって変わる
			int bmpmargin = 1;
			CVerticalSlider* control = new CVerticalSlider(size, this, tag,
				y + bmpmargin,
				y + backbmp->getHeight() - (handlebmp->getHeight() + bmpmargin),
				handlebmp, backbmp);

			// パラメータの現在の値を取得し、コントローラに反映
			ParamValue value = controller->getParamNormalized(tag);
			control->setValueNormalized(value);

			// スライダーをフレームに登録
			frame->addView(control);

			// 読み込んだ画像は忘れず解放
			backbmp->forget();
			handlebmp->forget();

		}

		// コンボボックスを作成するメンバー関数
		void MyVSTGUIEditor::createCombobox(ParamID tag, int x, int y)
		{
			// コンボボックスのサイズを設定(サイズは適当)
			CRect  size;
			size(0, 0, 60, 16);
			size.offset(x, y);

			// コンボボックス作成
			COptionMenu* control = new COptionMenu(size, this, tag);

			// コンボボックスに選択肢を追加する
			control->addEntry("Volume");
			control->addEntry("Tremolo");
			control->addEntry("Panning");


			// パラメータの現在の値を取得し、コンボボックス反映
			ParamValue value = controller->getParamNormalized(tag);
			control->setValueNormalized(value);

			// 小型フォントを指定
			control->setFont(kNormalFontSmaller);

			// 文字色・背景色を設定
			control->setFontColor(kWhiteCColor);
			control->setBackColor(kBlackCColor);
			control->setFrameColor(kRedCColor);

			// コンボボックスをフレームに登録
			frame->addView(control);

		}

		// テキストラベルを作成するメンバ関数
		CTextLabel* MyVSTGUIEditor::createTextLabel(int x, int y, int w, int h, UTF8StringPtr text)
		{
			// テキストラベルコントロールを作成する

			// コントロールのサイズを設定(サイズは適当)
			CRect  size;
			size(0, 0, w, h);
			size.offset(x, y);   // 位置を設定(frameの左上が0,0となる)

			// コントロール作成
			CTextLabel* control = new CTextLabel(size, text);

			// 小型フォントを指定
			control->setFont(kNormalFont);

			// 文字色・背景色を設定
			control->setFontColor(kWhiteCColor);
			control->setBackColor(kTransparentCColor);
			control->setFrameColor(kTransparentCColor);

			// コントロールをフレームに登録
			frame->addView(control);

			// 作成したコントロールのポインタを返す
			return control;
		}

		// VuMeterを作成するメンバー関数
		CVuMeter* MyVSTGUIEditor::createVuMeter(ParamID tag, int x, int y)
		{
			// まずは画像ファイルを読み込む
			// インジケーター
			CBitmap* onbmp = new CBitmap("meter_front.png");
			// 背景
			CBitmap* offbmp = new CBitmap("meter_back.png");

			// 画像ファイルからサイズを取得する
			CRect  size;
			size(0, 0, onbmp->getWidth(), onbmp->getHeight());
			size.offset(x, y);   // 位置を設定(tergetframeの左上が0,0となる)

			// VuMeterの作成
			CVuMeter* control = new CVuMeter(size, onbmp, offbmp, 128);

			// パラメータの現在の値を取得し、コントローラに反映
			ParamValue value = controller->getParamNormalized(tag);
			control->setValueNormalized(value);

			// スライダーをフレームに登録
			frame->addView(control);

			// 読み込んだ画像は忘れず解放
			onbmp->forget();
			offbmp->forget();

			return control;
		}

		// rocker switch を作成するメンバー関数
		CRockerSwitch* MyVSTGUIEditor::createRockerSwitch(ParamID tag, int x, int y)
		{
			// まずは画像ファイルを読み込む
			CBitmap* bmp = new CBitmap("rocker_switch.png");

			// 画像ファイルからサイズを取得する
			CRect  size;
			size(0, 0, bmp->getWidth(), bmp->getHeight() / 3);
			size.offset(x, y);   // 位置を設定(tergetframeの左上が0,0となる)

			// rocker switch の作成
			CRockerSwitch* control = new CRockerSwitch(size, this, tag, bmp);

			// パラメータの現在の値を取得し、コントローラに反映
			ParamValue value = controller->getParamNormalized(tag);
			control->setValueNormalized(value);

			// rocker switch をフレームに登録
			frame->addView(control);

			// 読み込んだ画像は忘れず解放
			bmp->forget();

			return control;
		}

		// horizontal switch を作成するメンバー関数
		CHorizontalSwitch* MyVSTGUIEditor::createHorizontalSwitch(ParamID tag, int x, int y, UTF8StringPtr path, int numStates)
		{
			// まずは画像ファイルを読み込む
			CBitmap* bmp = new CBitmap(path);

			// 画像ファイルからサイズを取得する
			CRect  size;
			size(0, 0, bmp->getWidth(), bmp->getHeight() / numStates);
			size.offset(x, y);   // 位置を設定(tergetframeの左上が0,0となる)

			// horizontal switch の作成
			CHorizontalSwitch* control = new CHorizontalSwitch(size, this, tag, numStates, size.getHeight(), 0, bmp);

			// パラメータの現在の値を取得し、コントローラに反映
			ParamValue value = controller->getParamNormalized(tag);
			control->setValueNormalized(value);

			// horizontal switch をフレームに登録
			frame->addView(control);

			// 読み込んだ画像は忘れず解放
			bmp->forget();

			return control;
		}

		CMovieBitmap* MyVSTGUIEditor::createMovieBitmap(ParamID tag, int x, int y, UTF8StringPtr path, int numImages)
		{
			// まずは画像ファイルを読み込む
			CBitmap* bmp = new CBitmap(path);

			// 画像ファイルからサイズを取得する
			CRect  size;
			size(0, 0, bmp->getWidth(), bmp->getHeight() / numImages);
			size.offset(x, y);   // 位置を設定(tergetframeの左上が0,0となる)

			CMovieBitmap* control = new CMovieBitmap(size, this, tag, numImages, bmp->getHeight() / numImages, bmp);

			// パラメータの現在の値を取得し、コントローラに反映
			ParamValue value = controller->getParamNormalized(tag);
			control->setValueNormalized(value);

			// horizontal switch をフレームに登録
			frame->addView(control);

			// 読み込んだ画像は忘れず解放
			bmp->forget();

			return control;
		}

		// ラベルアップデート用関数
		void MyVSTGUIEditor::updateLabel(CTextLabel* label, EditController* controller, ParamID tag, const char* baseText) {
			float value = controller->getParamNormalized(tag);

			// パラメーターから文字列を取得
			String128 value_u16;
			controller->getParamStringByValue(tag, value, value_u16);

			//// 取得した文字列はUNICODEなのでUTF8に変換
			char value_u8[256];
			wcstombs(value_u8, value_u16, 128);

			char text[256];
			snprintf(text, 256, baseText, value_u8);

			// 取得した文字列をテキストラベルに設定
			label->setText(text);
			// テキストラベルコントロールを再描画する
			//label->setDirty();
		}
	}
} // namespace SteinbergとVstの終わり