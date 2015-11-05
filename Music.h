
#pragma once
#include "..\..\include\hge.h"
class Music
{
public:
	Music(const char* filename);
	bool Play();
	bool Play2();
	bool Stop();
	/*
	bool Force_SetVolume(int vol)
	{
		if (channel)
		{
			if (this->vol == settedvol)
			{
				settedvol = vol;
				this->vol = vol;
			}
			else
				settedvol = vol;
		}
	}
	*/
	~Music();
private:
	HEFFECT music;
	HCHANNEL channel;
	int vol;
};

