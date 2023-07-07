#pragma once
#include "BaseScene.h"
#include "../common/Geometry.h"
#include "../object/Object.h"
#include "../tmx/TmxObj.h"

class Controller;
enum class ControllerType;

class TitelScene :
    public BaseScene
{
public:
    TitelScene();
    ~TitelScene();

    //初期化
    void Init(void) override;

    // 更新ステップ
    UniqueScene Update(UniqueScene scene) override;

    //描画処理
    void DrawScreen(void) override;

    //解放処理
    void Release(void) override;

    //シーンID返却
    SceneID GetSceneID(void) override
    {
        return SceneID::Title;
    };



private :
    //シーン切り替え関数
    UniqueScene UpdateScene(UniqueScene& scene);

    //背景座標
    Vector2 bgPos;		
    Vector2 bgPosEnd;	

    //ロゴ座標
    Vector2 logoPos;
    Vector2 logoPosEnd;


    int bgImageH_;      //背景画像
    int logoImageH_;    //タイトルロゴ画像

protected:
    //tmx
    TmxObj tmxObj_;

    //コントローラー
    std::unique_ptr<Controller> controller_;


};

