#include <DxLib.h>
#include "TitelScene.h"
#include "GameScene.h"
#include "SelectScene.h"
#include "SceneManager.h"
#include "../../_debug/_DebugDispOut.h"



//画面サイズ(フルスクリーン予定)
constexpr int SCREEN_SIZE_X =1280;
constexpr int SCREEN_SIZE_Y = 720;
bool SceneManager::SystemInit(void)
{
	SetOutApplicationLogValidFlag(false);
	SetGraphMode(static_cast<int>(SCREEN_SIZE_X), static_cast<int>(SCREEN_SIZE_Y), 32);
	ChangeWindowMode(true);
	SetWindowText("Bouncer");

	if (DxLib_Init() == -1)
	{
		return false;
	}

	_dbgSetup(static_cast<int>(SCREEN_SIZE_X), static_cast<int>(SCREEN_SIZE_Y), 255);

	return true;
}

SceneManager::SceneManager()
{

}

SceneManager::~SceneManager()
{

}

void SceneManager::Init(void)
{
	//起動時シーン設定
	scene_ = std::make_unique<TitelScene>();

	//デルタタイム系の初期化
	deltaTime_ = 0.0f;
	tickCount_ = std::chrono::system_clock::now();
}

void SceneManager::Run(void)
{
	if (!SystemInit())
	{
		return;
	}

	Init();

	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
		
		
		//デバック開始
		_dbgStartDraw();

		// 更新
		Update();

		//描画
		Draw();

		ScreenFlip();
	}
	// DXLIB終了
	DxLib_End();

}

void SceneManager::Update()
{
	//更新
	scene_ = scene_->Update(std::move(scene_));

	//デルタタイムの作成
	auto tick = std::chrono::system_clock::now();
	deltaTime_ = std::chrono::duration_cast<std::chrono::microseconds>(tick - tickCount_).count() / 1000000.0f;
	tickCount_ = tick;
}

double SceneManager::GetDeltaTime(void)
{
	return deltaTime_;
}

void SceneManager::Draw(void)
{
	SetDrawScreen(DX_SCREEN_BACK);
	ClsDrawScreen();
	scene_->Draw();
	_dbgDraw();
}

void SceneManager::Relese(void)
{
}

