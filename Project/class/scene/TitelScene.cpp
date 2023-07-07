#include <DxLib.h>
#include"SceneManager.h"
#include "../input/KeyInput.h"
#include "../input/PadInput.h"
#include "SelectScene.h"
#include "TitelScene.h"

TitelScene::TitelScene()
{

	//コントローラーの生成
	if (GetJoypadNum() >= 1)
	{
		controller_ = std::make_unique<PadInput>();
	}
	else
	{
		controller_ = std::make_unique<KeyInput>();
	}


	//初期化
	Init();

	//ちらつき防止
	DrawScreen();
}

TitelScene::~TitelScene()
{
	//解放
	Release();
}

void TitelScene::Init(void)
{
	//tmxの読み込み
	tmxObj_.LoadTmx("resource/tmx/titleScene.tmx", false);

	bgImageH_ = LoadGraph("resource/image/stage/titleBg.png", true);
	logoImageH_ = LoadGraph("resource/image/titlelogo.png");
}

UniqueScene TitelScene::Update(UniqueScene scene)
{
	controller_->Update();
	DrawScreen();

	return UpdateScene(scene);
}

void TitelScene::DrawScreen(void)
{
	SetDrawScreen(screenID_);
	ClsDrawScreen();

	//tmxのCollLiset取得
	for (auto& coll : tmxObj_.GetTitleBgimageList())
	{
		bgPos = coll.first;
		bgPosEnd = coll.first + coll.second;
	}
	for (auto& coll : tmxObj_.GetTitleLogoimageList())
	{
		logoPos = coll.first;
		logoPosEnd = coll.first+coll.second;
	}

	DrawExtendGraph(bgPos.x, bgPos.y, bgPosEnd.x, bgPosEnd.y, bgImageH_, true);
	//DrawRotaGraph(logoPos.x,logoPos.y,1.3,Deg2RadF(-5), logoImageH_, true);
	//DrawExtendGraph(logoPos.x, logoPos.y, logoPosEnd.x,logoPosEnd.y, logoImageH_, true);
	DrawGraph(logoPos.x, logoPos.y, logoImageH_,true);
	DrawString(550,600 - 16, "Start to Press X", 0xffffff);

	DrawFormatString(0, 0, 0xffffff, "TitleScene");


}

void TitelScene::Release(void)
{

	DeleteGraph(bgImageH_);
	DeleteGraph(logoImageH_);

}

UniqueScene TitelScene::UpdateScene(UniqueScene& scene)
{
	//デバック用
#ifdef _DEBUG
	if (controller_->ChaeckInputKey(KeyID::Transition))
	{
		return std::make_unique<SelectScene>();
	}


#endif

	//元のシーンに戻す
	return std::move(scene);
}
