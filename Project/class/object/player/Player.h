#pragma once
#include <memory>	//ゆにーくポインタに必要
#include "../../common/Geometry.h"
#include "../Object.h"
#include "../../tmx/TmxObj.h"
#include "../../common/Raycast.h"


class Controller;
enum class ControllerType;

//状態
enum class State
{
    Idel,       //立つ
    JumpUp,     //ジャンプ上昇
    Fall,       //下降
    MoveLeft,   //左移動
    MoveRight,  //右移動
    Crouching,  //しゃがみ
    Attack,     //攻撃
    Max
};

//方向
enum class Dir
{
    Up,     //上
    Down,   //下
    Right,  //右
    Left,   //左   
    Max
};

class Player :
    public Object
{
public:
    Player(ControllerType type);
    ~Player();

    //初期化
    void Init() override;

    //更新
    void Update(void) override;

    //描画
    void Draw(void) override;

    //解放
    void Release(void) override;

private:
    //コントローラー
    std::unique_ptr<Controller> controller_;

    //色
    int color ;

    //座標
    Vector2 pos_; 
    
    //大きさ
    Vector2 size_;

    //攻撃時の画像の大きさ
    Vector2 attacksize_;

    //状態
    State state_;

    //方向
    Dir dir_;

    //重力
    float gravity_;

    //補正差分
    Vector2 offset_;

    //始点、終点
    Line stageLine_;
    Line playerLine_;

    //判定
    Raycast raycast_;

    //ステージとの判定処理
    bool IsStageHit(Line collRay);

    bool IsBallHit();

    //経過時間
    double jumpDeltaTime_;

    int playerImage_;
    int playerImage2_;
    int playerImage3_;
    int playerImage4_;
    int playerImage5_;
    int playerImage6_;

protected:
    //tmx
    TmxObj tmxObj_;
};

