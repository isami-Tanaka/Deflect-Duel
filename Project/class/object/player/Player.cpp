#include "Player.h"
#include "../../scene/SceneManager.h"
#include "../../input/KeyInput.h"
#include "../../input/PadInput.h"
#include "../../tmx/TmxObj.h"
#include"../../../_debug/_DebugDispOut.h"

constexpr int MOVE_SPEED = 20.0f;		// 移動速度
constexpr int JUMP_POW = 15.0f;		// ジャンプ力
//constexpr float FALL_SPEED = 1.0f;	// 落下速度
constexpr float FALL_ACCEL = 1.0f;	// 重力加速度

Player::Player(ControllerType type, std::shared_ptr<Ball>& ball)
{
	//コントローラーの生成
	if (type == ControllerType::Pad)
	{
		controller_ = std::make_unique<PadInput>();
	}
	else
	{
		controller_ = std::make_unique<KeyInput>();
	}
	
	ball_ = ball;

	Init();

}

Player::~Player()
{
	Release();
}

void Player::Init()
{


	//プレイヤー座標
	pos_ = {100,300};

	//プレイヤーサイズ
	size_ = {48,96};
	attacksize_ = {96,96};

	ballpos_ = {0,0};
	ballsize_ = { 0,0 };

	//状態
	state_ = State::Idel;

	//方向
	dir_ = Dir::Max;

	//重力
	gravity_ = 0.1;

	//補正差分
	offset_ = { 0.0f ,0.0f };

	jumpDeltaTime_ = 0.0;

	//Idel
	playerImage_ = LoadGraph("resource/image/character/player_idle.png", true);
	playerImage2_ = LoadGraph("resource/image/character/player_move.png", true);
	playerImage3_ = LoadGraph("resource/image/character/Player_Crouch.png", true);
	playerImage4_ = LoadGraph("resource/image/character/player_up.png", true);
	playerImage5_ = LoadGraph("resource/image/character/player_down.png", true);
	playerImage6_ = LoadGraph("resource/image/character/player_attack.png", true);

	//tmxの読み込み
	tmxObj_.LoadTmx("resource/tmx/Stage.tmx", false);

}

void Player::Update(void)
{
	controller_->Update();

	switch (state_)
	{
	case State::Idel:
	{
		gravity_ = 0;

		if (!CheckHitKeyAll(DX_CHECKINPUT_KEY))
		{
			dir_ = Dir::Max;
		}

		//プレイヤー移動
		if (controller_->ChaeckInputKey(KeyID::Up))
		{
			//ジャンプ
			gravity_ = 0;
			jumpDeltaTime_ = 0.0;
			//ジャンプ
			state_ = State::JumpUp;
			dir_ = Dir::Up;

			break;
		}
		if (!IsStageHit(Line({ pos_.x + size_.x / 2, pos_.y + size_.y / 2 }, { pos_.x + size_.x / 2,pos_.y + size_.y })))
		{
			jumpDeltaTime_ = 1.3;
			gravity_ = 7.8;
			state_ = State::Fall;
			break;
		}
		if (controller_->ChaeckLongInputKey(KeyID::Down))
		{
			//しゃがみ
			//state_ = State::Crouching;
			dir_ = Dir::Down;
		}

		if (controller_->ChaeckLongInputKey(KeyID::Left))
		{
			//左移動
			state_ = State::MoveLeft;
			dir_ = Dir::Left;
		}
		else if (controller_->ChaeckLongInputKey(KeyID::Right))
		{
			//右移動
			state_ = State::MoveRight;
			dir_ = Dir::Right;
		}
		if (controller_->ChaeckLongInputKey(KeyID::Attack))
		{
			state_ = State::Attack;
		}
	}
	break;
	case State::JumpUp:
	{
		auto YVel = -JUMP_POW + (2.0f * gravity_ * std::pow(jumpDeltaTime_, 2.0));
		if (YVel > 0)
		{
			state_ = State::Idel;

		}

		gravity_ += FALL_ACCEL;
		jumpDeltaTime_ += IpSceneMng.GetDeltaTime();
		

		pos_.y += YVel;

		if (IsStageHit(Line({ pos_.x + size_.x / 2,pos_.y + size_.y / 2 }, { pos_.x + size_.x / 2,pos_.y })))
		{
			//当たってたら補正
			pos_ -= offset_;
		}



		if (controller_->ChaeckLongInputKey(KeyID::Right))
		{
			dir_ = Dir::Right;

			//右移動
			pos_.x += MOVE_SPEED;
			//state_ = State::MoveRight;
			if (IsStageHit(Line({ pos_.x + size_.x / 2,pos_.y + size_.y / 2 }, { pos_.x + size_.x ,pos_.y + size_.y / 2 })))
			{
				//当たってたら補正
				pos_ -= offset_;
			}
			

		}
		if (controller_->ChaeckLongInputKey(KeyID::Left))
		{
			dir_ = Dir::Left;
			//左移動
			pos_.x -= MOVE_SPEED;
			//state_ = State::MoveLeft;
			if (IsStageHit(Line({ {pos_.x + size_.x / 2,pos_.y + size_.y / 2},{pos_.x,pos_.y + size_.y / 2} })))
			{
				//当たってたら補正
				pos_ -= offset_;
			}
		}
	}
	break;
	case State::Fall:
	{
		dir_ = Dir::Down;

		jumpDeltaTime_ += IpSceneMng.GetDeltaTime();
		gravity_ += FALL_ACCEL;

		auto YVel = -JUMP_POW + (gravity_ * std::pow(jumpDeltaTime_, 2.0));
		pos_.y += YVel;

		if (IsStageHit(Line({ pos_.x + size_.x / 2, pos_.y + size_.y / 2 }, { pos_.x + size_.x / 2,pos_.y + size_.y })))
		{
			//当たってたら補正
			pos_ -= offset_;
			
			state_ = State::Idel;
		}

		if (controller_->ChaeckLongInputKey(KeyID::Right))
		{
			//右移動
			pos_.x += MOVE_SPEED;
			dir_ = Dir::Right;
			if (IsStageHit(Line({ pos_.x + size_.x / 2,pos_.y + size_.y / 2 }, { pos_.x + size_.x ,pos_.y + size_.y / 2 })))
			{
				//当たってたら補正
				pos_ -= offset_;
			}
		}
		if (controller_->ChaeckLongInputKey(KeyID::Left))
		{
			//左移動
			pos_.x -= MOVE_SPEED;
			dir_ = Dir::Left;
			if (IsStageHit(Line({ {pos_.x + size_.x / 2,pos_.y + size_.y / 2},{pos_.x,pos_.y + size_.y / 2} })))
			{
				//当たってたら補正
				pos_ -= offset_;
			}
		}


	}
	break;
	case State::MoveLeft:
	{
		//左移動
		pos_.x -= MOVE_SPEED;

		if (controller_->ChaeckInputKey(KeyID::Up))
		{
			//ジャンプ
			gravity_ = 0;
			jumpDeltaTime_ = 0.0;
			state_ = State::JumpUp;
		}

		if (IsStageHit(Line({ {pos_.x + size_.x / 2,pos_.y + size_.y / 2},{pos_.x,pos_.y + size_.y / 2} })))
		{
			//当たってたら補正
			pos_ -= offset_;
		}


		if (!controller_->ChaeckLongInputKey(KeyID::Left))
		{
			//移動キーを放したらIdel
			state_ = State::Idel;
		}
	}
		break;
	case State::MoveRight:
	{
		//右移動
		pos_.x += MOVE_SPEED;

		if (controller_->ChaeckInputKey(KeyID::Up))
		{
			//ジャンプ
			gravity_ = 0;
			jumpDeltaTime_ = 0.0;
			state_ = State::JumpUp;
		}

		if (IsStageHit(Line({ pos_.x + size_.x / 2,pos_.y + size_.y / 2 }, { pos_.x + size_.x ,pos_.y + size_.y / 2 })))
		{
			//当たってたら補正
			pos_ -= offset_;
		}

		if (!controller_->ChaeckLongInputKey(KeyID::Right))
		{
			//移動キーを放したらIdel
			state_ = State::Idel;
		}
	}
		break;

	case State::Crouching:

		if (!controller_->ChaeckLongInputKey(KeyID::Down))
		{
			state_ = State::Idel;
		}
		break;

	case State::Attack:
		if (!controller_->ChaeckLongInputKey(KeyID::Attack))
		{
			state_ = State::Idel;
		}
		break;
	case State::Max:
		break;
	[[likery]]default:
		break;
	}



	
}

void Player::Draw(void)
{
	//プレイヤー描画
#ifdef _DEBUG	//デバック時のみ
	switch (state_)
	{
	case State::Idel:
		DrawFormatString(300, 30, 0xffffff, "State:Idel");
		DrawExtendGraph(pos_.x, pos_.y, pos_.x + size_.x, pos_.y + size_.y, playerImage_, true);
		break;
	case State::JumpUp:
		DrawFormatString(300, 30, 0xffffff, "State:JumpUp");
		DrawExtendGraph(pos_.x, pos_.y, pos_.x + size_.x, pos_.y + size_.y, playerImage4_, true);
		break;
	case State::Fall:
		DrawFormatString(300, 30, 0xffffff, "State:Fall");
		DrawExtendGraph(pos_.x, pos_.y, pos_.x + size_.x, pos_.y + size_.y, playerImage5_, true);
		break;
	case State::MoveLeft:
		DrawFormatString(300, 30, 0xffffff, "State:Left");
		DrawExtendGraph(pos_.x +size_.x, pos_.y, pos_.x, pos_.y + size_.y, playerImage2_, true);
		break;
	case State::MoveRight:
		DrawFormatString(300, 30, 0xffffff, "State:Right");
		DrawExtendGraph(pos_.x, pos_.y, pos_.x + size_.x, pos_.y + size_.y, playerImage2_, true);
		break;
	case State::Crouching:
		DrawFormatString(300, 30, 0xffffff, "State:Crouching");
		break;
	case State::Attack:
		DrawFormatString(300, 30, 0xffffff, "State:Attack");
		DrawExtendGraph(pos_.x, pos_.y, pos_.x + attacksize_.x, pos_.y + attacksize_.y, playerImage6_, true);
		break;
	case State::Max:
		break;
	default:
		break;
	}

	switch (dir_)
	{
	case Dir::Up:
		DrawFormatString(450, 30, 0xffffff, "Dir:Up");
		break;
	case Dir::Down:
		DrawFormatString(450, 30, 0xffffff, "Dir:Down");
		break;
	case Dir::Right:
		DrawFormatString(450, 30, 0xffffff, "Dir:Right");
		break;
	case Dir::Left:
		DrawFormatString(450, 30, 0xffffff, "Dir:Left");
		break;
	case Dir::Max:
		DrawFormatString(450, 30, 0xffffff, "Dir:MAX");
		break;
	default:
		break;
	}

	//判定
	//DrawBox(pos_.x, pos_.y, pos_.x + size_.x, pos_.y + size_.y, 0xffffff, false);

	//DrawBox(ball_.pos_.x, ball_.pos_.y, ball_.pos_.x + ball_.size_.x, ball_.pos_.y + ball_.size_.y, 0xffff00, false);

#endif // _DEBUG
}

void Player::Release(void)
{
}

//ステージとのあたり判定
bool Player::IsStageHit(Line collRay)
{
	//レイのデバック表示
	//_dbgDrawLine(collRay.p.x, collRay.p.y, collRay.end.x, collRay.end.y, 0xff0000);
	raycast_.setPlayerRay(collRay);
	raycast_.setBallRay(ball_->pos_, ball_->size_);

	//tmxのCollLiset取得
	for (auto& coll : tmxObj_.GetStageCollList())
	{	
		if (raycast_.PlayerToBallChackColl(offset_))
		{
			return true;
		}
		if (raycast_.StageToPlayerCheckColl(coll, offset_))
		{

			return true;
		}
	}

	return false;
}




