#include "Player.h"
#include "../../scene/SceneManager.h"
#include "../../common/SoundManager.h"
#include "../../common/ImageManager.h"
#include "../../common/AnimController.h"
#include "../../input/KeyInput.h"
#include "../../input/PadInput.h"
#include "../../input/PadInput2.h"
#include "../../tmx/TmxObj.h"
#include"../../../_debug/_DebugDispOut.h"

constexpr int MOVE_SPEED = 15.0f;		// 移動速度
constexpr int JUMP_POW = 16.0f;		// ジャンプ力
constexpr float FALL_ACCEL = 2.0f;	// 重力加速度

constexpr int DRAW_OFFSET = 12;	//描画補正
constexpr float DRAW_EXRATE = 2.25f;//拡大率

Player::Player(ControllerType type, PlayerType pType, std::shared_ptr<Ball>& ball)
{
	//コントローラーの生成
	if (type == ControllerType::Pad1)
	{
		controller_ = std::make_unique<PadInput>();
	}
	else if(type == ControllerType::Pad2)
	{
		
		controller_ = std::make_unique<PadInput2>();
	}
	else if (type == ControllerType::Key)
	{
		controller_ = std::make_unique<KeyInput>();
	}


	//プレイヤーの種類情報
	playertype_ = pType;

	//ボール情報
	ball_ = ball;

	//初期化
	Init();

}

Player::~Player()
{
	Release();
}

void Player::Init()
{

	animController_ = std::make_unique<AnimController>();

	//プレイヤー座標
	if (playertype_ == PlayerType::One)
	{
		//1P
		pos_ = { 100,450 };

		//プレイヤーサイズ
		collSize_ = { 64,96 };

		DrawSize_ = { 18,66 };

	}
	else if (playertype_ == PlayerType::Two)
	{
		//2P
		pos_ = { 900,450 };
		//プレイヤーサイズ
		collSize_ = {80,96 };
	} 

	//攻撃時のサイズ
	attacksize_ = {48,96};

	GetGraphSizeF(lpImageMng.GetID("knight_attack")[0],&imageSize_.x, &imageSize_.y);

	//ボール情報
	ballpos_ = {0,0};
	ballsize_ = { 0,0 };

	//状態
	state_ = State::Idle;
	animController_->SetAnim(Anim::Fall);

	//方向
	dir_ = Dir::Max;

	//重力
	gravity_ = 0.1;

	//補正差分
	offset_ = { 0.0f ,0.0f };

	//反射方向
	refDir_ = { 0.0f ,0.0f };

	//経過時間
	jumpDeltaTime_ = 0.0;

	//tmxの読み込み
	tmxObj_.LoadTmx("resource/tmx/Stage.tmx", false);
	movePos_ = { MOVE_SPEED , MOVE_SPEED };


	reverse_ = 0;

	animEnd_ = false;

	isGround = false;
}

void Player::Update(void)
{
	imagePos_ = { pos_.x + (imageSize_.x - collSize_.x) / 2 - 23,pos_.y };
	
	controller_->Update();

	switch (state_)
	{
	case State::Idle:
	{
		gravity_ = 0;

		if (!IsStageHit(Line({ pos_.x + collSize_.x / 2, pos_.y + collSize_.y / 2 }, { pos_.x + collSize_.x / 2,pos_.y + collSize_.y })))
		{
			//ステージに当たっていないなら
			jumpDeltaTime_ = 1.3;
			gravity_ = 7.8;
			state_ = State::Fall;
			break;
		}

		//プレイヤー移動
		if (controller_->ChaeckInputKey(KeyID::Up))
		{
			//ジャンプ
			gravity_ = 0;
			jumpDeltaTime_ = 0.0;

			PlaySoundMem(lpSoundMng.GetID("jumpSe"), DX_PLAYTYPE_BACK);
			state_ = State::JumpUp;
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
			//左
			state_ = State::MoveLeft;
		}
		else if (controller_->ChaeckLongInputKey(KeyID::Right))
		{
			//右
			state_ = State::MoveRight;
		}
		if (controller_->ChaeckInputKey(KeyID::Attack))
		{
			//攻撃
			ChangeVolumeSoundMem(150, lpSoundMng.GetID("attackSe"));
			PlaySoundMem(lpSoundMng.GetID("attackSe"), DX_PLAYTYPE_BACK);
			state_ = State::Attack;
		}

	}
	break;
	case State::JumpUp:
	{
		jumpDeltaTime_ += lpSceneMng.GetDeltaTime();
		gravity_ += FALL_ACCEL;

		yVel_ = -JUMP_POW + (gravity_ * std::pow(jumpDeltaTime_, 2.0));
		pos_.y += yVel_;

		if (yVel_ > 0&&state_==State::JumpUp)
		{
			//jumpDeltaTime_ = 1.3;
			//gravity_ = 7.8;
			state_ = State::Fall;
			break;
		}

		if (IsStageHit(Line({ pos_.x + collSize_.x / 2,pos_.y + collSize_.y / 2 }, { pos_.x + collSize_.x / 2,pos_.y })))
		{
			//当たってたら補正
			pos_ -= offset_;
		}

		if (controller_->ChaeckLongInputKey(KeyID::Right))
		{
			//右移動
			MovePosition(Dir::Right);
		}
		if (controller_->ChaeckLongInputKey(KeyID::Left))
		{
			//左移動
			MovePosition(Dir::Left);
		}

 		if (controller_->ChaeckInputKey(KeyID::Attack))
		{
			//攻撃
			ChangeVolumeSoundMem(150, lpSoundMng.GetID("attackSe"));
			PlaySoundMem(lpSoundMng.GetID("attackSe"), DX_PLAYTYPE_BACK);


			animEnd_ = false;

			if (animController_->SetAnimEnd(animEnd_) == true)
			{
				//キーを放したら
				state_ = State::Idle;
			}

			state_ = State::AirAttack;
		}

	}
	break;
	case State::Fall:
	{
		jumpDeltaTime_ += lpSceneMng.GetDeltaTime();
		gravity_ += FALL_ACCEL;

		yVel_ = -JUMP_POW + (gravity_ * std::pow(jumpDeltaTime_, 2.0));
		pos_.y += yVel_;

		if (IsStageHit(Line({ pos_.x + collSize_.x / 2, pos_.y + collSize_.y / 2 }, { pos_.x + collSize_.x / 2,pos_.y + collSize_.y })))
		{
			//当たってたら補正
			pos_ -= offset_;
			state_ = State::Idle;
		}

		if (controller_->ChaeckLongInputKey(KeyID::Right))
		{
			//右移動
			MovePosition(Dir::Right);
		}
		if (controller_->ChaeckLongInputKey(KeyID::Left))
		{
			//左移動
			MovePosition(Dir::Left);
		}

		if (controller_->ChaeckInputKey(KeyID::Attack))
		{
			//攻撃
			ChangeVolumeSoundMem(150, lpSoundMng.GetID("attackSe"));
			PlaySoundMem(lpSoundMng.GetID("attackSe"), DX_PLAYTYPE_BACK);

			if (IsAttackHit())
			{
				ball_->SetAttackRef(refDir_);
			}

			animEnd_ = false;

			if (animController_->SetAnimEnd(animEnd_) == true)
			{
				//キーを放したら
				state_ = State::Idle;
			}

			state_ = State::AirAttack;

		}


	}
	break;
	case State::MoveLeft:
	{
		//左移動
		MovePosition(Dir::Left);

		if (controller_->ChaeckInputKey(KeyID::Up))
		{
			//ジャンプ
			gravity_ = 0;
			jumpDeltaTime_ = 0.0;
			PlaySoundMem(lpSoundMng.GetID("jumpSe"), DX_PLAYTYPE_BACK);
			state_ = State::JumpUp;
		}

		if (!controller_->ChaeckLongInputKey(KeyID::Left))
		{
			//キーを放したらIdel
			state_ = State::Idle;
		}

		if (controller_->ChaeckInputKey(KeyID::Attack))
		{
			//攻撃
			ChangeVolumeSoundMem(150, lpSoundMng.GetID("attackSe"));
			PlaySoundMem(lpSoundMng.GetID("attackSe"), DX_PLAYTYPE_BACK);
			state_ = State::Attack;
		}
	}
		break;
	case State::MoveRight:
	{
		//右移動
		MovePosition(Dir::Right);

		if (controller_->ChaeckInputKey(KeyID::Up))
		{
			//ジャンプ
			gravity_ = 0;
			jumpDeltaTime_ = 0.0;
			PlaySoundMem(lpSoundMng.GetID("jumpSe"), DX_PLAYTYPE_BACK);
			state_ = State::JumpUp;
		}

		if (!controller_->ChaeckLongInputKey(KeyID::Right))
		{
			//キーを放したらIdel
			state_ = State::Idle;
		}

		if (controller_->ChaeckInputKey(KeyID::Attack))
		{
			//攻撃
			ChangeVolumeSoundMem(150, lpSoundMng.GetID("attackSe"));
			PlaySoundMem(lpSoundMng.GetID("attackSe"), DX_PLAYTYPE_BACK);
			state_ = State::Attack;
		}
	}
		break;

	case State::Crouching:

		//しゃがみ
		if (!controller_->ChaeckLongInputKey(KeyID::Down))
		{
			//キーを離したら
			//state_ = State::Idel;
		}
		break;

	case State::Attack:

		//アニメーションが終わったら
		if (animController_->SetAnimEnd(animEnd_) == true)
		{
			state_ = State::Idle;
		}
		else
		{
			if (IsAttackHit())
			{
				ball_->SetAttackRef(refDir_);
			}
		}

		break;

	case State::AirAttack:

		if (!isGround)
		{
			jumpDeltaTime_ += lpSceneMng.GetDeltaTime();
			gravity_ += FALL_ACCEL;

			yVel_ = -JUMP_POW + (gravity_ * std::pow(jumpDeltaTime_, 2.0));
			pos_.y += yVel_;
			if (IsStageHit(Line({ pos_.x + collSize_.x / 2, pos_.y + collSize_.y / 2 }, { pos_.x + collSize_.x / 2,pos_.y + collSize_.y })))
			{
				//当たってたら補正
				pos_ -= offset_;
				state_ = State::Idle;
			}

		}

		if (controller_->ChaeckLongInputKey(KeyID::Right))
		{
			MovePosition(Dir::Right);
		}
		if (controller_->ChaeckLongInputKey(KeyID::Left))
		{
			MovePosition(Dir::Left);
		}

		//アニメーションが終わったら
		if (animController_->SetAnimEnd(animEnd_) == true)
		{
			if (IsStageHit(Line({ pos_.x + collSize_.x / 2, pos_.y + collSize_.y / 2 }, { pos_.x + collSize_.x / 2,pos_.y + collSize_.y })))
			{
				//当たってたら補正
				pos_ -= offset_;
				state_ = State::Idle;
			}
			else
			{
				state_ = State::Fall;
			}

		}
		else
		{
			if (IsAttackHit())
			{
				ball_->SetAttackRef(refDir_);
			}
		}

		break;
	case State::Death:

		break;
	case State::Max:
		break;
	[[likery]]default:
		break;
	}

	//ボールとの判定
	if (IsBallHit())
	{
		ChangeVolumeSoundMem(180, lpSoundMng.GetID("daethSe"));
		PlaySoundMem(lpSoundMng.GetID("daethSe"), DX_PLAYTYPE_BACK);
		state_ = State::Death;
	}

	if (dir_ == Dir::Left)
	{
		//左向いてたら
		reverse_ = -1;
		attackpos_ = { pos_.x ,pos_.y };
	}
	else if(dir_ == Dir::Right)
	{
		//右向いてたら
		reverse_ = 1;
		attackpos_ = { pos_.x + collSize_.x,pos_.y };
	}

	//地面にいるか
	if (IsStageHit(Line({ pos_.x + collSize_.x / 2, pos_.y + collSize_.y / 2 }, { pos_.x + collSize_.x / 2,pos_.y + collSize_.y })))
	{
		//当たってたら補正
		pos_ -= offset_;
		isGround = true;
	}
	else
	{
		isGround = false;
	}

}

void Player::Draw(void)
{

	//プレイヤーの描画
	switch (state_)
	{
	case State::Idle:	//立ち
		
		animController_->SetAnim(Anim::Idle);

		if (playertype_ == PlayerType::One)
		{
			DrawRotaGraph(
				pos_.x + collSize_.x / 2 - DRAW_OFFSET * reverse_, pos_.y + collSize_.y / 2 + DRAW_OFFSET, 
				DRAW_EXRATE,
				0,
				lpImageMng.GetID("knight_idle")[animController_->Update()],
				true, -1 * reverse_);
		}
		if (playertype_ == PlayerType::Two)
		{
			DrawExtendGraph(
				pos_.x, pos_.y,
				pos_.x + collSize_.x, pos_.y + collSize_.y,
				lpImageMng.GetID("rogue_idle")[animController_->Update()], true);
		}
		_dbgDrawFormatString(pos_.x, pos_.y-40, 0xffffff, "Idle", true);
		break;
	case State::JumpUp:	//ジャンプ上昇
		animController_->SetAnim(Anim::JumpUp);

		if (playertype_ == PlayerType::One)
		{
			DrawRotaGraph(
				pos_.x + collSize_.x / 2 - DRAW_OFFSET * reverse_, pos_.y + collSize_.y / 2 + DRAW_OFFSET,
				DRAW_EXRATE,
				0,
				lpImageMng.GetID("knight_jumpUp")[animController_->Update()],
				true, -1 * reverse_);
		}

		_dbgDrawFormatString(pos_.x, pos_.y - 40, 0xffffff, "JumpUp", true);
		break;
	case State::Fall:	//落下
		animController_->SetAnim(Anim::Fall);
		if (playertype_ == PlayerType::One)
		{
			DrawRotaGraph(
				pos_.x + collSize_.x / 2 - DRAW_OFFSET * reverse_, pos_.y + collSize_.y / 2 + DRAW_OFFSET,
				DRAW_EXRATE,
				0,
				lpImageMng.GetID("knight_fall")[animController_->Update()],
				true, -1 * reverse_);
		}
		_dbgDrawFormatString(pos_.x, pos_.y - 40, 0xffffff, "Fall", true);
		break;
	case State::MoveLeft://左移動
		animController_->SetAnim(Anim::Run);
		if (playertype_ == PlayerType::One)
		{
			DrawRotaGraph(
				pos_.x + collSize_.x / 2 - DRAW_OFFSET * reverse_, pos_.y + collSize_.y / 2 + DRAW_OFFSET,
				DRAW_EXRATE,
				0,
				lpImageMng.GetID("knight_run")[animController_->Update()],
				true, -1 * reverse_);
		}
		_dbgDrawFormatString(pos_.x, pos_.y - 40, 0xffffff, "Left", true);
		break;
	case State::MoveRight://右移動
		animController_->SetAnim(Anim::Run);
		if (playertype_ == PlayerType::One)
		{
			DrawRotaGraph(
				pos_.x + collSize_.x / 2 - DRAW_OFFSET * reverse_, pos_.y + collSize_.y / 2 + DRAW_OFFSET,
				DRAW_EXRATE,
				0,
				lpImageMng.GetID("knight_run")[animController_->Update()],
				true, -1 * reverse_);
		}
		_dbgDrawFormatString(pos_.x, pos_.y - 40, 0xffffff, "Right", true);
		break;
	case State::Crouching:
		_dbgDrawFormatString(pos_.x, pos_.y - 40, 0xffffff, "Crouching", true);
		break;

	case State::Attack://攻撃
		animController_->SetAnim(Anim::Attack);

		if (playertype_ == PlayerType::One)
		{
			DrawRotaGraph(
				pos_.x + collSize_.x / 2 - DRAW_OFFSET * reverse_, pos_.y + collSize_.y / 2 + DRAW_OFFSET,
				DRAW_EXRATE,
				0,
				lpImageMng.GetID("knight_attack")[animController_->Update()],
				true, -1 * reverse_);
		}
		_dbgDrawFormatString(pos_.x, pos_.y - 40, 0xffffff, "Attack", true);
		break;

	case State::AirAttack://空中攻撃
		animController_->SetAnim(Anim::AirAttack);
		if (playertype_ == PlayerType::One)
		{
			DrawRotaGraph(
				pos_.x + collSize_.x / 2 - DRAW_OFFSET * reverse_, pos_.y + collSize_.y / 2 + DRAW_OFFSET,
				DRAW_EXRATE,
				0,
				lpImageMng.GetID("knight_airAttack")[animController_->Update()],
				true, -1 * reverse_);
		}

		_dbgDrawFormatString(pos_.x, pos_.y - 40, 0xffffff, "AirAttack", true);
		break;
	case State::Death://死
		animController_->SetAnim(Anim::Death);
		if (playertype_ == PlayerType::One)
		{
			DrawRotaGraph(
				pos_.x + collSize_.x / 2 - DRAW_OFFSET * reverse_, pos_.y + collSize_.y / 2 + DRAW_OFFSET,
				DRAW_EXRATE,
				0,
				lpImageMng.GetID("knight_death")[animController_->Update()],
				true, -1 * reverse_);
		}

		_dbgDrawFormatString(pos_.x, pos_.y - 40, 0xffffff, "Death", true);
		break;
	case State::Max:
		break;
	default:
		break;
	}

	//プレイヤーの名前
	if (playertype_ == PlayerType::One)
	{
		DrawFormatString(pos_.x + collSize_.x / 2-10, pos_.y - 20, 0xffff00, "1P", true);
	}
	else if (playertype_ == PlayerType::Two)
	{
		DrawFormatString(pos_.x + collSize_.x / 2-10, pos_.y - 20, 0xff0000, "2P", true);
	}

	//操作説明
	DrawString(50, 625, "Player1\n操作\nA/Dで左右移動\nWでジャンプ\nSPACEで攻撃", 0xfff00f, true);
	DrawString(1100, 625, "Player2\n操作\n右/左で左右移動\nBでジャンプ\nXで攻撃", 0xff0000, true);
	

#ifdef _DEBUG	//デバック時のみ

	//プレイヤー
	if (playertype_ == PlayerType::One)
	{
		DrawFormatString(48, 600, 0xffff00, "player1PosX%f,player1PosY%f", pos_.x, pos_.y);
		DrawFormatString(50, 625 + 16 * 5, 0xff0000, "yvel:%f", yVel_);
	}
	else if (playertype_ == PlayerType::Two)
	{
		DrawFormatString(800, 600, 0xff0000, "player2PosX%f,player2PosY%f", pos_.x, pos_.y);
	}
#endif //_DEBUG

}

void Player::Release(void)
{

}

State Player::GetState(void)
{
	return state_;
}

PlayerType Player::GetPlayerType(void)
{
	return playertype_;
}

//ステージとのあたり判定
bool Player::IsStageHit(Line collRay)
{
	//レイのデバック表示
	_dbgDrawLine(collRay.p.x, collRay.p.y, collRay.end.x, collRay.end.y, 0xff0000);

	//プレイヤーのレイをセット
	raycast_.setPlayerRay(collRay);

	//tmxのCollLiset取得
	for (auto& coll : tmxObj_.GetStageCollList())
	{	
		if (raycast_.StageToPlayerCheckColl(coll, offset_))
		{
			return true;
		}	
	}
	return false;
}

bool Player::IsBallHit()
{
	//矩形レイのセット
	raycast_.setPlayerSquareRay(pos_, collSize_, movePos_);
	raycast_.setBallRay(ball_->pos_+ ball_->movePos_, ball_->collSize_);

	//プレイヤーとボールの接触判定
	if (raycast_.PlayerToBallChackColl(offset_))
	{
		return true;
	}

	return false;
}

bool Player::IsAttackHit()
{
	//矩形レイのセット
	raycast_.setPlayerAttackRay(attackpos_, attacksize_,reverse_);
	raycast_.setBallRay(ball_->pos_+ ball_->movePos_, ball_->collSize_);

	//攻撃とボールの接触判定
	if (raycast_.AttackToBallCheckColl(refDir_))
	{
    		return true;
	}

	return false;
}

void Player::MovePosition(Dir dir)
{


	if (dir == Dir::Right)
	{
		//右移動
		dir_ = dir;
		pos_.x += MOVE_SPEED;
		if (IsStageHit(Line({ pos_.x + collSize_.x / 2,pos_.y + collSize_.y / 2 }, { pos_.x + collSize_.x ,pos_.y + collSize_.y / 2 })))
		{
			//当たってたら補正
			pos_ -= offset_;
		}
	}
	else if(dir == Dir::Left)
	{
		//左移動
		dir_ = dir;
		pos_.x -= MOVE_SPEED;
		if (IsStageHit(Line({ {pos_.x + collSize_.x / 2,pos_.y + collSize_.y / 2},{pos_.x,pos_.y + collSize_.y / 2} })))
		{
			//当たってたら補正
			pos_ -= offset_;
		}
	}

}




