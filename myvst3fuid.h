// VST3 SDKのインクルードファイル
#include "pluginterfaces\base\funknown.h"

// 自作VST用のインクルードファイル
#include "myvst3def.h"


// VST3作成に必要なの名前空間を使用
namespace Steinberg{
namespace Vst {


// ============================================================================================
// FUIDの生成
// ============================================================================================
static const FUID ProcessorUID (0x1E5620F0, 0xB0C54434, 0x93A6D2E6, 0x793DF84E);
static const FUID ControllerUID(0xDC821BA2, 0x3DCA437F, 0x85046563, 0x196B2BFD);


} } // namespace SteinbergとVstの終わり