// VST3 SDKのインクルードファイル
#include "public.sdk/source/main/pluginfactory.h"

// 自作VSTのヘッダファイルをインクルード
#include "myvst3fuid.h"
#include "processor.h"
#include "controller.h"


// ============================================================================================
// 製作者の情報を入力
// ============================================================================================
// 製作者(製作会社)の名前。終端文字「\0」含めて64文字まで。
#define MYVST_VENDOR   "Kazzix"
// 製作者(製作会社)のWebサイトのURL。終端文字「\0」含めて256文字まで。
#define MYVST_URL      ""
// 製作者(製作会社)の連絡先メールアドレス。終端文字「\0」含めて128文字まで。
#define MYVST_EMAIL    "kazzix14@gmail.com"


// ============================================================================================
// 自作VSTプラグインの情報を入力
// ============================================================================================
// 自作するVSTの名前。終端文字「\0」含めて64文字まで。
#define MYVST_VSTNAME  "CMP2005"
// 自作するVSTのバージョン。終端文字「\0」含めて64文字まで。
#define MYVST_VERSION  "0" 
// 自作するVSTのカテゴリ。終端文字「\0」含めて64文字まで。
#define MYVST_SUBCATEGORIES Vst::PlugType::kFxDynamics


// ============================================================================================
// DLLファイルの初期化、終了処理関数
// ============================================================================================

// DLLファイルが読み込まれたときに初期化するための関数
// 初期化に成功した場合、trueを返す。
// 基本的に何もする必要はない。
bool InitModule() { return true; }


// DLLファイルが破棄されたときに終了処理をするための関数
// 初期化に成功した場合、trueを返す。
// 基本的に何もする必要はない。
bool DeinitModule() { return true; }


// ============================================================================================
// 自作VSTプラグインの生成
// ============================================================================================
BEGIN_FACTORY_DEF(MYVST_VENDOR, MYVST_URL, MYVST_EMAIL)

// MyVSTProcessorクラスの作成を行う
DEF_CLASS2(INLINE_UID_FROM_FUID(Steinberg::Vst::ProcessorUID),
	PClassInfo::kManyInstances,
	kVstAudioEffectClass,
	MYVST_VSTNAME,
	Vst::kDistributable,
	MYVST_SUBCATEGORIES,
	MYVST_VERSION,
	kVstVersionString,
	Steinberg::Vst::MyVSTProcessor::createInstance)

	// MyVSTControllerクラスの作成を行う
	DEF_CLASS2(INLINE_UID_FROM_FUID(Steinberg::Vst::ControllerUID),
		PClassInfo::kManyInstances,
		kVstComponentControllerClass,
		MYVST_VSTNAME " Controller",	// 自作VSTの名前に"Controller"を付与したもの。
		0,						    // 使わないので必ず0にする
		"",						    // 使わないので必ず""にする
		MYVST_VERSION,
		kVstVersionString,
		Steinberg::Vst::MyVSTController::createInstance)

	END_FACTORY


