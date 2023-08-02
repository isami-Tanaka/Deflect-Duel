#include<DxLib.h>
#include "SoundManager.h"

const int& SoundManager::GetID(std::string key, std::string path)
{
	//音を読み込んでるか
	if (soundMap_.count(key)==0)
	{
		//読み込んでない場合は読み込む
		soundMap_[key] = LoadSoundMem(path.c_str());
		count_[key] = 0;
	}
	return soundMap_[key];
}

const int& SoundManager::GetID(std::string key)
{
	//音を読み込んでるか
	if (soundMap_.count(key) == 0)
	{
		//読み込んでない場合は読み込む
		soundMap_[key] = LoadSoundMem(key.c_str());
		count_[key] = 0;

	}
	return soundMap_[key];
}

void SoundManager::PlaySoundOneTime(std::string name)
{
	count_[name]++;
	if (count_[name]==1)
	{
		if (CheckSoundMem(lpSoundMng.GetID(name)) == 0)
		{
			PlaySoundMem(lpSoundMng.GetID(name),DX_PLAYTYPE_BACK);
			count_[name] = 0;
		}
	}
}


void SoundManager::ResetCount(std::string name)
{
	StopSoundMem(lpSoundMng.GetID(name));
	count_[name] = 0;
}

void SoundManager::AllStopSound()
{
	for (auto& sound : soundMap_)
	{
		StopSoundMem(lpSoundMng.GetID(sound.first));
	}
}

SoundManager::SoundManager()
{
	//サウンド読み込み
	GetID("gameBgm", "resource/sound/gamebgm.mp3");
	GetID("bgm", "resource/sound/titlebgm.mp3");

	GetID("decideSe", "resource/sound/decide.mp3");
	GetID("winSe", "resource/sound/win.mp3");
	GetID("jumpSe","resource/sound/jump.mp3");

	GetID("attackSe", "resource/sound/attackSe.mp3");

	GetID("airAttackSe", "resource/sound/airAttackSe.mp3");

	GetID("attackHitSe","resource/sound/attackHitSe.wav");

	GetID("walkSe", "resource/sound/walkSe.wav");

	GetID("daethSe","resource/sound/daeth.wav");
	GetID("ballSe","resource/sound/ballSe.mp3");
	GetID("runSe","resource/sound/runSe.mp3");
}

SoundManager::~SoundManager()
{
}

