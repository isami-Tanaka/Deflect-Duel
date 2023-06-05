#pragma once
#include "../Object.h"
#include "../../common/Raycast.h"
#include "../../common/Geometry.h"
#include "../../tmx/TmxObj.h"


enum class Dir;

class Ball :
    public Object
{
public:
    Ball();
    ~Ball();

    //初期化
    void Init() override;
    //更新
    void Update() override;
    //描画
    void Draw() override;
    //解放
    void Release() override;

    void SetBallform(Vector2& pos,Vector2&size);


    //座標
    Vector2 pos_;

    //大きさ
    Vector2 size_;

    int rad_;

    bool flg;

    Vector2 movepow;
    
private:
    //ボール画像
    int ballImage_;

    //重力
    float gravity_;

    Vector2 offset_;

    //あたり判定処理
    bool IsStageHit();

    //ステージ
    TmxObj tmxObj_;

    //判定
    Raycast raycast_;
};

