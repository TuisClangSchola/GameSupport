#include "DxLib.h"

// EffekseerForDXLib.hをインクルードします。
#include "EffekseerForDXLib.h"


/// 2D用と3D用のEffekseerの切り替え-----------------------------------------------
//#define _USE_2DEffekseer
#define _USE_3DEffekseer


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(true);

	SetDrawScreen(DX_SCREEN_BACK);


	// DirectX11を使用するようにする。(DirectX9(DX_DIRECT3D_9)も可)
	SetUseDirect3DVersion(DX_DIRECT3D_11);


	if (DxLib_Init() == -1)
	{
		return -1;
	}


	// Effekseerを初期化する。
	// 引数には画面に表示する最大パーティクル数を設定する。
	if (Effkseer_Init(8000) == -1)
	{
		DxLib_End();
		return -1;
	}


	// フルスクリーンウインドウの切り替えでリソースが消えるのを防ぐ。
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);


	// DXライブラリのデバイスロストした時のコールバックを設定する。
	// ウインドウとフルスクリーンの切り替えが発生する場合は必ず実行する。
	// ただし、DirectX11を使用する場合は実行する必要はない。
	// Effekseer_SetGraphicsDeviceLostCallbackFunctions();


#ifdef _USE_2DEffekseer

	// Effekseerに2D描画の設定をする。
	Effekseer_Set2DSetting(640, 480);

#endif // _USE_2DEffekseer


	// エフェクトリソースを読み込む。
	int effectResourceHandle = LoadEffekseerEffect("laser.efk");

	
	// 時間を初期化する(定期的にエフェクトを再生するため)
	int time = 0;


#ifdef _USE_2DEffekseer

	// エフェクトの表示する位置を設定する。
	float position_x = 100.0f;
	float position_y = 250.0f;

#elif defined _USE_3DEffekseer

	// エフェクトの表示する位置を設定する。
	float position_x = 0.0f;
	float position_y = 0.0f;

#endif

	// 再生中のエフェクトのハンドルを初期化する。
	int playingEffectHandle = -1;


	while (!ProcessMessage() && !ClearDrawScreen() && !CheckHitKey(KEY_INPUT_ESCAPE))
	{

#ifdef _USE_2DEffekseer


		// 定期的にエフェクトを再生する
		if (time++ % 60 == 0)
		{
			// エフェクトを再生する。
			playingEffectHandle = PlayEffekseer2DEffect(effectResourceHandle);


			// エフェクトの拡大率を設定する。
			// Effekseerで作成したエフェクトは2D表示の場合、小さすぎることが殆どなので必ず拡大する。
			SetScalePlayingEffekseer2DEffect(playingEffectHandle, 25.0f, 25.0f, 25.0f);


			// エフェクトの位置をリセットする。
			position_x = 100.0f;
		}

		DrawBox(0, 0, 640, 480, GetColor(0, 0, 0), true);


		// 再生中のエフェクトを移動する。
		SetPosPlayingEffekseer2DEffect(playingEffectHandle, position_x, position_y, 0);
		position_x += 2.0f;


		// Effekseerにより再生中のエフェクトを更新する。(Processとかで行う
		UpdateEffekseer2D();


		// Effekseerにより再生中のエフェクトを描画する。(Drawとかで行う
		DrawEffekseer2D();

#elif defined _USE_3DEffekseer


		// DXライブラリのカメラを設定する。
		SetCameraPositionAndTarget_UpVecY(VGet(10, 10, -20), VGet(0, 0, 0));
		SetupCamera_Perspective(60.0f * DX_PI_F / 180.0f);
		SetCameraNearFar(1.0f, 150.0f);


		// DXライブラリのカメラとEffekseerのカメラを同期する。
		Effekseer_Sync3DSetting();


		// 定期的にエフェクトを再生する
		if (time++ % 60 == 0)
		{
			// エフェクトを再生する。
			playingEffectHandle = PlayEffekseer3DEffect(effectResourceHandle);


			// エフェクトの位置をリセットする。
			position_x = 0.0f;
		}

		DrawBox(0, 0, 640, 480, GetColor(0, 0, 0), true);


		// 再生中のエフェクトを移動する。
		SetPosPlayingEffekseer3DEffect(playingEffectHandle, position_x, position_y, 0);
		position_x += 0.2f;


		// Effekseerにより再生中のエフェクトを更新する。
		UpdateEffekseer3D();


		// 3Dを表示する。
		DrawCapsule3D(VGet(0.0f, 100.0f, 0.0f), VGet(0.0f, -100.0f, 0.0f), 6.0f, 16, GetColor(100, 100, 100), GetColor(255, 255, 255), TRUE);


		// Effekseerにより再生中のエフェクトを描画する。
		DrawEffekseer3D();

#endif

		DrawBox(64, 300, 576, 440, GetColor(125, 125, 125), true);

		ScreenFlip();

	}

	// エフェクトリソースを削除する。(Effekseer終了時に破棄されるので削除しなくてもいい)
	DeleteEffekseerEffect(effectResourceHandle);

	// Effekseerを終了する。
	Effkseer_End();


	DxLib_End();

	return 0; 
}