#include "Music.h"


Music::Music(const char* filename)
{
	HGE *hge = hgeCreate(HGE_VERSION);
	music = hge->Effect_Load(filename);
	channel = 0;
	vol = 0;
}


Music::~Music()
{
	HGE *hge = hgeCreate(HGE_VERSION);
	hge->Effect_Free(music);
}

bool Music::Play()
{
	HGE *hge = hgeCreate(HGE_VERSION);
	if (!vol)
	{
		channel = hge->Effect_PlayEx(music,vol);
		vol = 1;
	}
	else
	{
		float length = hge->Channel_GetLength(channel);
		float pos = hge->Channel_GetPos(channel);
		if (length - pos < 3)
		{
			if (vol>0)
				this->vol--;
		}
		else
		{
			if (this->vol < 100)
				this->vol++;
		}
		if (!hge->Channel_IsPlaying(channel))
		{
			this->vol = 0;
		}
		hge->Channel_SetVolume(channel, this->vol);
		if (!vol)
     	return false;
	}
	return true;
}

bool Music::Play2()
{
	HGE *hge = hgeCreate(HGE_VERSION);
	if (!vol)
	{
		channel = hge->Effect_PlayEx(music, vol,0,1.0f,true);
		vol = 1;
	}
	else
	{
		if (this->vol < 100)
		{
			this->vol++;
			hge->Channel_SetVolume(channel, vol);
		}
	}
	return true;
}
bool Music::Stop()
{
	HGE *hge = hgeCreate(HGE_VERSION);
	if (vol)
	{
		vol--;
		hge->Channel_SetVolume(channel, vol);
		return false;
	}
	else
	{
		hge->Channel_Stop(channel);
		return true;
	}
}