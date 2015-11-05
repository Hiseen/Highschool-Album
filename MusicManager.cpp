#include "MusicManager.h"


void MusicManager::FindFiles(CString str)
{
	HANDLE hFind;
	WIN32_FIND_DATA dataFind;
	BOOL bMoreFiles = TRUE;
	TCHAR exeFullPath[MAX_PATH]; // MAX_PATH
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);//得到程序模块名称，全路径
	*_tcsrchr(exeFullPath, _T('\\')) = 0;
	CString str2 = exeFullPath, fullstr = "";
	//m_strDir就是你要指定的路径
	hFind = FindFirstFile(str2 + str + "\\*.*", &dataFind);//找到路径中所有文件
	//遍历路径中所有文件
	while (hFind != INVALID_HANDLE_VALUE && bMoreFiles == TRUE)
	{
		if (dataFind.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)//判断是否是文件
		{
			CString finalstr = str + "\\" + dataFind.cFileName;
			//Music *a = new Music(exeFullPath+finalstr);
			CString a = exeFullPath + finalstr;
			musics.push_back(a);
		}
		else
		{
			if (strcmp(dataFind.cFileName, ".") && strcmp(dataFind.cFileName, ".."))
			{
				fullstr += dataFind.cFileName;
				FindFiles(str + "\\" + fullstr);
			}
		}
		bMoreFiles = FindNextFile(hFind, &dataFind);
	}
	FindClose(hFind);
}

MusicManager::MusicManager()
{
	FindFiles("\\music");
	musiccount = 0;
	playmode = MusicPlay::Order;
	musicnow = NULL;
	innermode=false;
	switchmode = false;
	initfinish = true;
}

bool MusicManager::EndOfSong()
{
	return !musicnow;
}


void MusicManager::Update()
{
	if (!innermode)
	{
		if (!switchmode)
		{
			if (musicnow)
			{
				if (musicnow->Stop())
				{
					delete musicnow;
					musicnow = NULL;
					switchmode = true;
				}
			}
			else
				switchmode = true;
		}
		else
		if (musics.size() > 0)
		{
			if (!musicnow)
			{
				if (playmode == MusicPlay::Shuffle)
				{
					if (musiccount != 0 && musiccount != musics.size() - 1)
					{
						HGE *hge = hgeCreate(HGE_VERSION);
						if (hge->Random_Int(0, 1))
							musiccount = hge->Random_Int(0, musiccount - 1);
						else
							musiccount = hge->Random_Int(musiccount + 1, musics.size() - 1);
					}
					else
					{
						if (musics.size() == 1)
							return;
						HGE *hge = hgeCreate(HGE_VERSION);
						if (musiccount == 0)
						{
							musiccount = hge->Random_Int(1, musics.size() - 1);
						}
						else
						{
							musiccount = hge->Random_Int(0, musics.size() - 2);
						}
					}
				}
				musicnow = new Music(musics[musiccount]);
			}
			else
			if (!musicnow->Play())
			{
				delete musicnow;
				musicnow = NULL;
				switch (playmode)
				{
				case MusicPlay::Order:
					if (musiccount < musics.size() - 1)
						musiccount++;
					else
						musiccount = 0;
					break;
				case MusicPlay::Loop:
					break;
				case MusicPlay::Shuffle:
					if (musiccount != 0 && musiccount != musics.size() - 1)
					{
						HGE *hge = hgeCreate(HGE_VERSION);
						if (hge->Random_Int(0, 1))
							musiccount = hge->Random_Int(0, musiccount - 1);
						else
							musiccount = hge->Random_Int(musiccount + 1, musics.size() - 1);
					}
					else
					{
						if (musics.size() == 1)
							return;
						HGE *hge = hgeCreate(HGE_VERSION);
						if (musiccount == 0)
						{
							musiccount = hge->Random_Int(1, musics.size() - 1);
						}
						else
						{
							musiccount = hge->Random_Int(0, musics.size() - 2);
						}
					}
					break;
				}
			}
		}
	}
	else
	{
		if (!switchmode)
		{
			if (musicnow)
			{
				if (musicnow->Stop())
				{
					delete musicnow;
					musicnow = NULL;
					switchmode = true;
				}
			}
			else
				switchmode = true;
		}
		else
		{
			if (!musicnow)
				musicnow = new Music("bgm.mp3");
			else
				musicnow->Play2();
		}
	}
}

void MusicManager::SwitchMode()
{
	innermode = !innermode;
	switchmode = false;
}



MusicManager::~MusicManager()
{
}

void MusicManager::Exit()
{

	if (musicnow)
		delete musicnow;

	/*
	for (int i = 0; i < musics.size();)
	{
		if (musics[i])
		{
			delete musics[i];
			musics[i] = NULL;
			musics.erase(musics.begin() + i);
		}
		else
			i++;
	}
	musics.shrink_to_fit();
	*/
}