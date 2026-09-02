#include "stdafx.h"
#include "system/system.h"
#include "RenderingEngine.h"
#include "ModelRender.h"

// 関数宣言
void InitRootSignature(RootSignature& rs);

///////////////////////////////////////////////////////////////////
// ウィンドウプログラムのメイン関数
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine, int nCmdShow)
{
	// ゲームの初期化
	InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

	//////////////////////////////////////
	// ここから初期化を行うコードを記述する
	//////////////////////////////////////

	// ルートシグネチャを作成
	RootSignature rootSignature;
	InitRootSignature(rootSignature);

	//レンダリングパイプラインを初期化
	myRenderer::RenderingEngine renderingEngine;
	renderingEngine.Init();

	// 背景モデルのレンダラーを初期化
	myRenderer::ModelRender bgModelRender;
	bgModelRender.InitDeferredRendering(renderingEngine, "Assets/modelData/bg/bg.tkm", true);

	// step-1 ティーポットモデルの描画処理を初期化
	myRenderer::ModelInitDataFR modelInitData;
	modelInitData.m_tkmFilePath = "Assets/modelData/teapot.tkm";
	modelInitData.m_fxFilePath = "Assets/shader/sample.fx";

	//【注目】メインレンダリングターゲットのスナップショットテクスチャを拡張SRVに指定する
	modelInitData.m_expandShaderResoruceView[0] = &renderingEngine.GetMainRenderTargetSnapshotDrawnOpacity();
	myRenderer::ModelRender teapotModelRender;

	//フォワードレンダリングの描画パスで実行されるように初期化する
	teapotModelRender.InitForwardRendering(renderingEngine, modelInitData);
	teapotModelRender.SetShadowCasterFlag(true);

	teapotModelRender.UpdateWorldMatrix({ 0.0f, 20.0f, 0.0f }, g_quatIdentity, g_vec3One);

	//////////////////////////////////////
	// 初期化を行うコードを書くのはここまで！！！
	//////////////////////////////////////
	auto& renderContext = g_graphicsEngine->GetRenderContext();

	float rotationY = 0.0f;

	//float transparencyTimer = 0.0f;
	bool isHalfTransparent = false;
	bool passed180 = false;

	// ここからゲームループ
	while (DispatchWindowMessage())
	{
		// レンダリング開始
		g_engine->BeginFrame();

		// ティーポットを回転させる
		rotationY += 1.0f;

		if (rotationY >= 360.0f)
		{
			rotationY -= 360.0f;
		}

		// 180度回転するたびに透明・半透明を切り替える
		if (rotationY >= 180.0f && !passed180)
		{
			isHalfTransparent = !isHalfTransparent;
			passed180 = true;
		}

		if (rotationY < 180.0f)
		{
			passed180 = false;
		}

		// Y軸回転のクォータニオンを作成
		Quaternion rotation;
		rotation.SetRotationDegY(rotationY);

		// ティーポットに回転を適用
		teapotModelRender.UpdateWorldMatrix(
			{ 0.0f, 20.0f, 0.0f },
			rotation,
			g_vec3One
		);

		// 5秒ごとに透明・半透明を切り替える
		//transparencyTimer += 1.0f / 60.0f;

		/*if (transparencyTimer >= 5.0f)
		{
			transparencyTimer -= 5.0f;
			isHalfTransparent = !isHalfTransparent;
		}*/

		g_camera3D->MoveForward(g_pad[0]->GetLStickYF());
		g_camera3D->MoveRight(g_pad[0]->GetLStickXF());
		g_camera3D->MoveUp(g_pad[0]->GetRStickYF());

		//////////////////////////////////////
		// ここから絵を描くコードを記述する
		//////////////////////////////////////

		bgModelRender.Draw();

		// step-2 ティーポットモデルを描画
		teapotModelRender.Draw();

		//レンダリングパイプラインを実行
		renderingEngine.Execute(renderContext);

		/////////////////////////////////////////
		// 絵を描くコードを書くのはここまで！！！
		//////////////////////////////////////
		// レンダリング終了
		g_engine->EndFrame();
	}

	return 0;
}

// ルートシグネチャの初期化
void InitRootSignature(RootSignature& rs)
{
	rs.Init(D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP);
}