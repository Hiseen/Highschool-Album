#pragma once
#include<vector>
#include<afxwin.h>
#include"Music.h"

using namespace std;

class MusicPlay
{
public:
	enum Mode{ Shuffle, Order, Loop };
};

class MusicManager
{
public:
	static MusicManager& GetMusicManager()
	{
		static MusicManager music;
		music.InitFinish();
		return music;
	}
	~MusicManager();
	void Update();
	void SwitchMode();
	void PlayMenuBGM();
	void Exit();
	bool EndOfSong();
	void ChangeMode(MusicPlay::Mode newmode)
	{
		playmode = newmode;
	}
	MusicPlay::Mode GetMode()const
	{
		return playmode;
	}
private:
	MusicManager();
	void FindFiles(CString str);
	void InitFinish()
	{
		while (!initfinish)
			Sleep(0);
	}
	MusicPlay::Mode playmode;
	bool innermode;
	bool switchmode;
	int musiccount;
	Music *musicnow;
	vector<CString> musics;
	bool initfinish;
};

