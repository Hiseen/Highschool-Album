/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hge_tut07 - Thousand of Hares
*/


// Copy the files "font2.fnt", "font2.png", "bg2.png"
// and "zazaka.png" from the folder "precompiled" to
// the folder with executable file. Also copy hge.dll
// to the same folder.

#include "MusicManager.h"
#include <math.h>
#include "..\..\include\hge.h"
#include "..\..\include\hgefont.h"
#include "..\..\include\hgeparticle.h"

using namespace std;

#define SCREEN_WIDTH  1000
#define SCREEN_HEIGHT 600

#define BUTTON1Y 350
#define BUTTON2Y 450
#define BUTTON3Y 550

#define BACKGROUNDNUM 240
#define BGPARTSIZE 50

#define CLOCKPARTICLENUM 40
#define PARTICLENUM 144

#define MIN_OBJECTS	100
#define MAX_OBJECTS 2000

struct sprObject
{
	float x,y;
	float dx,dy;
	float scale,rot;
	float dscale,drot;
	DWORD color;
};


struct PosWithTarget
{
	float nowx, nowy;
	float targetx, targety;
	int alpha;
	float size;
};

PosWithTarget arrays[PARTICLENUM];
vector<hgeSprite*> imgparticles;
vector<CString>files;
float alphas[BACKGROUNDNUM];

struct ParticleInfo
{
	float posx, posy;
	int direction;
	int percent;
	int alpha;
	float pssize;
	bool alive;
};
ParticleInfo particles[CLOCKPARTICLENUM];

class Exit
{
public:
	enum Mode{Contract,Swell,SpeedMove,Particle};
};

class Entry
{
public:
	enum Mode{SpeedMove,Swell,Particle,Turn};
};

class ImgParticle
{
public:
	enum Mode{SpeedMove,TransParent,Swell};
};

class AlbumStats
{
public:
	enum Mode{Menu,Photos,Option};
};
int nextcount = 0;
AlbumStats::Mode Album;
sprObject*	pObjects;
int			nObjects;
int			nBlend;
HEFFECT music1;
hgeFont *font;
float angle=0;
int         Count = 0;
float size = 1.0f;
float renderx = SCREEN_WIDTH*3 / 2, rendery = SCREEN_HEIGHT / 2;
// Pointer to the HGE interface (helper classes require this to work)
float mousex, mousey;
bool transition = true, entry = true, exit1 = false, swellflag1 = false, entrystandby = true, speedmoveflag1 = false, speedmoveflag2 = false;
bool direction = true; 
bool autonext = false;
bool flashL = false, flashR = false;
bool psstart = false;
float trackmousex=SCREEN_WIDTH/2, trackmousey=SCREEN_HEIGHT/2;
int countL = 0, countR = 0;
float selectmodex = 500, selectmodey =SCREEN_HEIGHT / 4 + 50;
int alphaforrim = 0;
int countformove = 0;
int delay = 5;
int alpha = 255;
bool phototooption = false;
int framealpha = 0, optionalpha = 0;
float pointx = SCREEN_WIDTH / 2;
const float pointy = SCREEN_HEIGHT / 4;
int arrx, arry;
float presize = 0.0f;
bool mouseExit = false;
bool ExitParticleStart = false;
HGE *hge=0;
Exit::Mode ExitMode;
Entry::Mode EntryMode;
ImgParticle::Mode ParticleMode;
// Resource handles
int globalvol=50;
HTEXTURE			tex, bgtex,shadowtex,LBtex,RBtex,Frametex,mousetex,titletex,button1tex,button2tex,bgparttex,particletex,button3tex,optiontex,bartex,barpointtex,musicmodetex;
hgeSprite			*spr, *bgspr,*shadow,*rim,*LB,*RB,*Frame,*mouse,*title,*button1,*button2,*particle,*button3,*optionbutton,*bar,*barpoint,*shuffle,*order,*loop,*selectmode;
hgeSprite *bgpart;
hgeFont				*fnt;

// Set up blending mode for the scene

void SetBlend(int blend)
{
	static int sprBlend[5]=
	{
		BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE,
		BLEND_COLORADD | BLEND_ALPHABLEND | BLEND_NOZWRITE,
		BLEND_COLORADD | BLEND_ALPHAADD | BLEND_NOZWRITE,
		BLEND_COLORMUL | BLEND_ALPHAADD   | BLEND_NOZWRITE,
		BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE
	};

	static DWORD fntColor[5]=
	{
		0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF
	};

	static DWORD sprColors[5][5]=
	{
		{ 0xFFFFFFFF, 0xFFFFE080, 0xFF80A0FF, 0xFFA0FF80, 0xFFFF80A0 },
		{ 0xFF000000, 0xFF303000, 0xFF000060, 0xFF006000, 0xFF600000 },
		{ 0x80FFFFFF, 0x80FFE080, 0x8080A0FF, 0x80A0FF80, 0x80FF80A0 },
		{ 0x80FFFFFF, 0x80FFE080, 0x8080A0FF, 0x80A0FF80, 0x80FF80A0 },
		{ 0x40202020, 0x40302010, 0x40102030, 0x40203010, 0x40102030 }
	};

	if(blend>4) blend=0;
	nBlend=blend;

	spr->SetBlendMode(sprBlend[blend]);
	fnt->SetColor(fntColor[blend]);
	//for(int i=0;i<MAX_OBJECTS;i++)
		//pObjects[i].color=sprColors[blend][hge->Random_Int(0,4)];
}

void ResetClockParticle()
{
	for (int i = 1; i<CLOCKPARTICLENUM; i++)
	{
		particles[i].posx = -1000;
		particles[i].posy = -1000;
	}
}


bool Reset()
{
	delay = 5;
	alphaforrim = 0;
	countformove = 0;
	size = 1.0f;
	nextcount = 0;
	swellflag1 = false;
	entrystandby = true;
	speedmoveflag1 = false;
	speedmoveflag2 = false;
	ExitParticleStart = false;
	angle = 0;
	if (direction)
		renderx = SCREEN_WIDTH * 3 / 2;
	else
		renderx = -SCREEN_WIDTH / 2;
	rendery = SCREEN_HEIGHT / 2;
	return true;
}

bool ReStart()
{
	Album = AlbumStats::Menu;
	Count = 0;
    size = 1.0f;
    renderx = SCREEN_WIDTH * 3 / 2, rendery = SCREEN_HEIGHT / 2;
	transition = true;
	entry = true;
	exit1 = false;
	swellflag1 = false;
	entrystandby = true;
	speedmoveflag1 = false;
	speedmoveflag2 = false;
    direction = true;
    autonext = false;
	flashL = false;
	flashR = false;
	psstart = false;
	countL = 0;
	countR = 0;
    alphaforrim = 0;
    countformove = 0;
	delay = 5;
    alpha = 255;
	presize = 0.0f;
	ExitParticleStart = false;
	MusicManager::GetMusicManager().SwitchMode();
	for (int i = 0; i < CLOCKPARTICLENUM; i++)
	{
		particles[i].alive = false;
		particles[i].percent = 0;
	}
	Frame->SetColor(ARGB(0, 255, 255, 255));
	RB->SetColor(ARGB(0, 0, 0, 0));
	LB->SetColor(ARGB(0, 0, 0, 0));
	return true;
}

void FindFiles(CString str)
{
	HANDLE hFind;
	WIN32_FIND_DATA dataFind;
	BOOL bMoreFiles = TRUE;
	TCHAR exeFullPath[MAX_PATH]; // MAX_PATH
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);//得到程序模块名称，全路径
	*_tcsrchr(exeFullPath, _T('\\')) = 0;
	CString str2 = exeFullPath, fullstr="";
	//m_strDir就是你要指定的路径
	hFind = FindFirstFile(str2 + str+"\\*.*", &dataFind);//找到路径中所有文件
	//遍历路径中所有文件
	while (hFind != INVALID_HANDLE_VALUE && bMoreFiles == TRUE)
	{
		if (dataFind.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)//判断是否是文件
		{
			CString finalstr = str + "\\" + dataFind.cFileName;
			files.push_back(finalstr);
		}
		else
		{
			if (strcmp(dataFind.cFileName, ".") && strcmp(dataFind.cFileName, ".."))
			{
				//CString a = fullstr + dataFind.cFileName;
				FindFiles(str + "\\" + fullstr + dataFind.cFileName);
			}
		}
		bMoreFiles = FindNextFile(hFind, &dataFind);
	}
	FindClose(hFind);
}



bool LoadPicture()
{
	if (Count < files.size() && Count>=0)
	{
		if (spr)
		{
			delete spr;
			spr = NULL;
			hge->Texture_Free(tex);
		}
		TCHAR exeFullPath[MAX_PATH]; // MAX_PATH
		GetModuleFileName(NULL, exeFullPath, MAX_PATH);//得到程序模块名称，全路径
		*_tcsrchr(exeFullPath, _T('\\')) = 0;
		CString str2 = exeFullPath;
		tex = hge->Texture_Load(str2+files[Count]);
		// Load font, create sprites
		int width = hge->Texture_GetWidth(tex, true);
		int height = hge->Texture_GetHeight(tex, true);
		spr = new hgeSprite(tex, 0, 0, width, height);
		spr->SetHotSpot(spr->GetWidth() / 2, spr->GetHeight() / 2);
		if (spr->GetHeight() > SCREEN_HEIGHT)
		{
			size *= SCREEN_HEIGHT / spr->GetHeight();
			//if (spr->GetWidth()*size > SCREEN_WIDTH)
			//{
			//	size *= SCREEN_WIDTH / spr->GetWidth()*size;
			//	goto ChangeSizeFinish;
		//	}
		//	else
				goto ChangeSizeFinish;
		}
		if (spr->GetWidth() > SCREEN_WIDTH)
		{
			size *= SCREEN_WIDTH / spr->GetWidth();
			//if (spr->GetHeight()*size > SCREEN_HEIGHT)
			//{
			//	size *= SCREEN_HEIGHT / spr->GetHeight()*size;
			//	goto ChangeSizeFinish;
			//}
			//else
				goto ChangeSizeFinish;
		}
	ChangeSizeFinish:
		presize = size;
		psstart = false;
		/*
		if (shadow)
		delete shadow;
		shadow = new hgeSprite(tex, 0, 0, width, height);
		shadow->SetHotSpot(spr->GetWidth() / 2, spr->GetHeight() / 2);
		shadow->SetColor(ARGB(150, hge->Random_Int(100, 255), hge->Random_Int(100, 255), hge->Random_Int(100, 255)));
		*/
		if (rim)
		{
			delete rim;
		}
		rim = new hgeSprite(tex, 0, 0, width, height);
		rim->SetBlendMode(BLEND_COLORADD | BLEND_ALPHABLEND);
		rim->SetHotSpot(spr->GetWidth() / 2, spr->GetHeight() / 2);
		rim->SetColor(ARGB(0, hge->Random_Int(50, 120), hge->Random_Int(50, 120), hge->Random_Int(100, 200)));
	}
	else
	{
		ReStart();
	}
	return true;
}

bool CreateImgParticles()
{
	for (int i = 0; i < imgparticles.size();)
	{
		if (imgparticles[i])
		{
			delete imgparticles[i];
			imgparticles[i] = NULL;
			imgparticles.erase(imgparticles.begin() + i);
		}
		else
			i++;
	}
	int width = hge->Texture_GetWidth(tex, true);
	int height = hge->Texture_GetHeight(tex, true);
	for (int i = 0; i < PARTICLENUM; i++)
	{
		float posx = i % (int)(sqrtf(PARTICLENUM));
		float posy = i / (int)(sqrtf(PARTICLENUM));
		float width2 = width / (sqrtf(PARTICLENUM));
		float height2 = height /(sqrtf(PARTICLENUM));
		hgeSprite *a = new hgeSprite(tex, posx*width2, posy*height2, width2, height2);
		a->SetHotSpot(width2 / 2, height2 / 2);
		switch (ParticleMode)
		{
		case ImgParticle::SpeedMove:
			arrays[i].targetx = (SCREEN_WIDTH / 2 - width*size / 2 + posx*width2*size + width2 / 2 * size);
			arrays[i].targety = SCREEN_HEIGHT / 2 - height*size / 2 + posy*height2*size + height2 / 2 * size;
			arrays[i].nowx = arrays[i].targetx +(direction ? 1 : -1)*(SCREEN_WIDTH + hge->Random_Int(0, 125)*i*5);
			arrays[i].nowy = arrays[i].targety;
			arrays[i].alpha = 255;
			arrays[i].size = size;
			break;
		case ImgParticle::Swell:
			arrays[i].targetx = (SCREEN_WIDTH / 2 - width*size / 2 + posx*width2*size + width2 / 2 * size);
			arrays[i].targety = SCREEN_HEIGHT / 2 - height*size / 2 + posy*height2*size + height2 / 2 * size;
			arrays[i].nowx = arrays[i].targetx;
			arrays[i].nowy = arrays[i].targety;
			arrays[i].alpha = 255;
			arrays[i].size = 0.0f;
			break;
		case ImgParticle::TransParent:
			arrays[i].targetx = (SCREEN_WIDTH / 2 - width*size / 2 + posx*width2*size + width2 / 2 * size);
			arrays[i].targety = SCREEN_HEIGHT / 2 - height*size / 2 + posy*height2*size + height2 / 2 * size;
			arrays[i].nowx = arrays[i].targetx;
			arrays[i].nowy = arrays[i].targety;
			arrays[i].alpha = 0;
			arrays[i].size = size;
			a->SetColor(ARGB(0, 255, 255, 255));
			break;
		}
		imgparticles.push_back(a);
	}
	return true;
}
bool ImgParticlesRe_use()
{
	int width = hge->Texture_GetWidth(tex, true);
	int height = hge->Texture_GetHeight(tex, true);
	for (int i = 0; i < PARTICLENUM; i++)
	{
		int posx = i % (int)(sqrtf(PARTICLENUM));
		int posy = i / (int)(sqrtf(PARTICLENUM));
		float width2 = width / (sqrtf(PARTICLENUM));
		float height2 = height / (sqrtf(PARTICLENUM));
		arrays[i].targetx = (SCREEN_WIDTH / 2 - width*size / 2 + posx*width2*size + width2 / 2 * size);
		arrays[i].targety = SCREEN_HEIGHT / 2 - height*size / 2 + posy*height2*size + height2 / 2 * size;
		arrays[i].nowx = arrays[i].targetx;
		arrays[i].nowy = arrays[i].targety;
		arrays[i].alpha = 255;
		arrays[i].size = size;
		imgparticles[i]->SetColor(ARGB(255, 255, 255, 255));
	}
	return true;
}



bool FrameFunc()
{
	switch (Album)
	{
	case AlbumStats::Photos:
		if (framealpha==255)
		if (!transition)
		{
			if (Count != 0 && Count != files.size() - 1)
			{
				if (!phototooption && alphaforrim != 240)
				{
					alphaforrim += 5;
					rim->SetColor(ARGB(alphaforrim, GETR(rim->GetColor()), GETG(rim->GetColor()), GETB(rim->GetColor())));
				}
				if (!psstart)
				{
					particles[0].alive = true;
					particles[0].alpha = 255;
					particles[0].direction = hge->Random_Int(0, 3);
					psstart = true;
				}
				else
				{
					switch (particles[0].direction)
					{
					case 0:
						particles[0].percent++;
						particles[0].pssize = (size*0.015*hge->Texture_GetHeight(tex, true)) / 32;
						particles[0].posx = renderx - hge->Texture_GetWidth(tex, true)*size*0.5075 + ((float)particles[0].percent / 100)*size*1.015*hge->Texture_GetWidth(tex, true);
						particles[0].posy = rendery - hge->Texture_GetHeight(tex, true)*size*0.5075;
						if (particles[0].percent == 100)
						{
							particles[0].direction++;
							particles[0].percent = 0;
						}
						break;
					case 1:
						particles[0].percent++;
						particles[0].pssize = (size*0.015*hge->Texture_GetWidth(tex, true)) / 32;
						particles[0].posx = renderx + hge->Texture_GetWidth(tex, true)*size*0.5075;
						particles[0].posy = rendery - hge->Texture_GetHeight(tex, true)*size*0.5075 + ((float)particles[0].percent / 100)*size*1.015*hge->Texture_GetHeight(tex, true);
						if (particles[0].percent == 100)
						{
							particles[0].direction++;
							particles[0].percent = 0;
						}
						break;
					case 2:
						particles[0].percent++;
						particles[0].pssize = (size*0.015*hge->Texture_GetHeight(tex, true)) / 32;
						particles[0].posx = renderx + hge->Texture_GetWidth(tex, true)*size*0.5075 - ((float)particles[0].percent / 100)*size*1.015*hge->Texture_GetWidth(tex, true);
						particles[0].posy = rendery + hge->Texture_GetHeight(tex, true)*size*0.5075;
						if (particles[0].percent == 100)
						{
							particles[0].direction++;
							particles[0].percent = 0;
						}
						break;
					case 3:
						particles[0].percent++;
						particles[0].pssize = (size*0.015*hge->Texture_GetWidth(tex, true)) / 32;
						particles[0].posx = renderx - hge->Texture_GetWidth(tex, true)*size*0.5075;
						particles[0].posy = rendery + hge->Texture_GetHeight(tex, true)*size*0.5075 - ((float)particles[0].percent / 100)*size*1.015*hge->Texture_GetHeight(tex, true);
						if (particles[0].percent == 100)
						{
							particles[0].direction = 0;
							particles[0].percent = 0;
						}
						break;
					}
					for (int i = CLOCKPARTICLENUM - 1; i >= 1; i--)
					{
						particles[i] = particles[i - 1];
						particles[i].alpha = 255 - i*((float)255 / CLOCKPARTICLENUM);
					}
				}
			}
			if (!autonext)
			{
				if (hge->Input_GetMouseWheel() > 0)
				{
					size *= 0.9;
					ResetClockParticle();
				}
				if (hge->Input_GetMouseWheel() < 0)
				{
					size /= 0.9; 
					ResetClockParticle();
				}
				if (hge->Input_GetKeyState(HGEK_LBUTTON))
				{
					if (!mouseExit)
					if (delay)
						delay--;
					else
					{
						float x, y;
						hge->Input_GetMousePos(&x, &y);
						if (x > renderx - hge->Texture_GetWidth(tex, true)*size*0.5 && x<renderx + hge->Texture_GetWidth(tex, true)*size*0.5
							&& y>rendery - hge->Texture_GetHeight(tex, true)*size*0.5 && y<rendery + hge->Texture_GetHeight(tex, true)*size*0.5)
						{
							if (!countformove)
							{
								hge->Input_GetMousePos(&mousex, &mousey);
								countformove = 1;
							}
							else
							{
								countformove--;
								if (x > mousex)
									renderx += (x - mousex) * 2;
								if (x < mousex)
									renderx -= (mousex - x) * 2;
								if (y > mousey)
									rendery += (y - mousey) * 2;
								if (y < mousey)
									rendery -= (mousey - y) * 2;
							}
						}
						else
							mouseExit = true;
					}
					ResetClockParticle();
				}
				if (hge->Input_KeyUp(HGEK_LBUTTON))
				{
					countformove = 0;
					delay = 5;
					mouseExit = false;
				}
				if (hge->Input_KeyDown(HGEK_LBUTTON))
				{
					float x, y;
					hge->Input_GetMousePos(&x, &y);
					if (x > SCREEN_WIDTH - 50 && x < SCREEN_WIDTH)
					{
						transition = true;
						exit1 = true;
						Count++;
						direction = true;
						flashR = true;
					}
					if (x<50 && x>0)
					{
						transition = true;
						exit1 = true;
						Count--;
						direction = false;
						flashL = true;
					}
				}
			}
			else
			{
				if (Count == files.size()-1)
				{
					if (MusicManager::GetMusicManager().EndOfSong())
					{
						transition = true;
						exit1 = true;
						Count++;
						direction = true;
						flashR = true;
					}
				}
				else
				{
					if (nextcount < 400)
						nextcount++;
					else
					{
						transition = true;
						exit1 = true;
						Count++;
						direction = true;
						flashR = true;
					}
				}
			}
			if (exit1)
			{
				ExitMode = (Exit::Mode)hge->Random_Int(Exit::Contract, Exit::Particle);
				switch (ExitMode)
				{
				case Exit::Particle:
					ParticleMode =(ImgParticle::Mode)hge->Random_Int(ImgParticle::SpeedMove, ImgParticle::Swell);
					break;
				}

			}
		}
		else
		{
			for (int i = 0; i < CLOCKPARTICLENUM; i++)
			{
				particles[i].posx = -1000;
				particles[i].posy = -1000;
			}


			if (entry)
			{
				switch (EntryMode)
				{
				case Entry::SpeedMove:
					if (abs((int)(renderx - SCREEN_WIDTH / 2)) > 3)
						renderx += (SCREEN_WIDTH / 2 - renderx) / 10 + 1 * (direction ? -1 : 1);
					else
					{
						renderx = SCREEN_WIDTH / 2;
						entry = false;
						transition = false;
					}
					break;
				case Entry::Swell:
					if (!swellflag1)
					{
						if (size < presize + 0.08)
						{
							if (!size)
								size = 0.1;
							else
								size *= 1.1;
						}
						else
							swellflag1 = true;
					}
					else
					{
						if (size > presize)
							size -= 0.01;
						else
						{
							size = presize;
							entry = false;
							transition = false;
							swellflag1 = false;
						}
					}
					break;
				case Entry::Turn:
					if (size < presize)
					{
						if (!size)
							size = 0.1;
						else
							size +=0.01;
						angle = (size / presize) * 2 * M_PI;
					}
					else
					{
						angle = 0;
						entry = false;
						transition = false;
						size = presize;
					}
					break;
				case Entry::Particle:
					int count = 0;
					for (int i = 0; i <PARTICLENUM; i++)
					{
						switch (ParticleMode)
						{
						case ImgParticle::SpeedMove:
							if (abs(arrays[i].nowx - arrays[i].targetx)>3)
								arrays[i].nowx += (arrays[i].targetx - arrays[i].nowx) / 10 + 1 * (direction ? -1 : 1);
							else
							{
								arrays[i].nowx = arrays[i].targetx;
								count++;
							}
							break;
						case ImgParticle::Swell:
							if (arrays[i].size < presize)
								arrays[i].size += hge->Random_Float(0, 0.05)*hge->Random_Int(0, 3);
							else
							{
								arrays[i].size = presize;
								count++;
							}
							break;
						case ImgParticle::TransParent:
							if (arrays[i].alpha != 255)
							{
								arrays[i].alpha += hge->Random_Int(0, 5)*hge->Random_Int(0, 3);
								if (arrays[i].alpha > 255)
									arrays[i].alpha = 255;
								imgparticles[i]->SetColor(ARGB(arrays[i].alpha, 255, 255, 255));
							}
							else
							{
								imgparticles[i]->SetColor(ARGB(255, 255, 255, 255));
								count++;
							}
							break;
						}
					}
					if (count == PARTICLENUM)
					{
						EntryMode = Entry::SpeedMove;
						renderx = SCREEN_WIDTH / 2;
						entry = false;
						transition = false;
					}
					break;

				}
			}
			if (entrystandby)
			{
				EntryMode =(Entry::Mode)hge->Random_Int(Entry::SpeedMove, Entry::Turn);
				switch (EntryMode)
				{
				case Entry::Swell:
					size = 0;
					renderx = SCREEN_WIDTH / 2;
					break;
				case Entry::Particle:
					ParticleMode = (ImgParticle::Mode)hge->Random_Int(ImgParticle::SpeedMove, ImgParticle::Swell);
					break;
				case Entry::Turn:
					size = 0;
					renderx = SCREEN_WIDTH / 2;
					break;
				}
				CreateImgParticles();
				entry = true;
				entrystandby = false;
			}
			if (exit1)
			{
				switch (ExitMode)
				{
				case Exit::Contract:
					if (abs((int)renderx - SCREEN_WIDTH / 2) > 10 || abs((int)rendery != SCREEN_HEIGHT / 2) > 10)
					{
						renderx -= (renderx - SCREEN_WIDTH / 2) / 10;
						rendery -= (rendery - SCREEN_HEIGHT / 2) / 10;
						angle += 0.1;
					}
					else
					{
						renderx = SCREEN_WIDTH / 2;
						rendery = SCREEN_HEIGHT / 2;
					}
					if (size > 0.00001)
						size *= 0.7;
					else
					{
						size = 0;
						exit1 = false;
						Reset();
						LoadPicture();
					}
					break;
				case Exit::Swell:
					if (size < 10)
						size *= 1.005;
					if (alpha >= 0)
					{
						alpha -= alpha / 10 + 5;
						spr->SetColor(ARGB(alpha, 255, 255, 255));
						rim->SetColor(ARGB(alpha, 255, 255, 255));
						if (alpha < 0)
						{
							spr->SetColor(ARGB(0, 255, 255, 255));
							rim->SetColor(ARGB(0, 255, 255, 255));
						}
					}
					else
					{
						alpha = 255;
						exit1 = false;
						Reset();
						LoadPicture();
					}
					break;
				case Exit::SpeedMove:
					if (!speedmoveflag1)
					if (fabs(size - presize) > 0.01)
						size += (presize - size) / 10;
					else
					{
						size = presize;
						speedmoveflag1 = true;
					}
					if (!speedmoveflag2)
					if (abs((int)renderx - SCREEN_WIDTH / 2) > 5 || abs((int)rendery - SCREEN_HEIGHT / 2) > 5)
					{
						renderx -= (renderx - SCREEN_WIDTH / 2) / 3;
						rendery -= (rendery - SCREEN_HEIGHT / 2) / 3;
					}
					else
					{
						renderx = SCREEN_WIDTH / 2;
						rendery = SCREEN_HEIGHT / 2;
						speedmoveflag2 = true;
					}
					if (speedmoveflag1 && speedmoveflag2)
					if (!direction)
					{
						if (abs((int)renderx - SCREEN_WIDTH * 3 / 2) > 3)
							renderx += (SCREEN_WIDTH * 3 / 2 - renderx) / 10 + 1;
						else
						{
							exit1 = false;
							Reset();
							LoadPicture();
						}
					}
					else
					{
						if (abs((int)renderx + SCREEN_WIDTH / 2) > 3)
							renderx -= (renderx + SCREEN_WIDTH / 2) / 10 + 1;
						else
						{
							exit1 = false;
							Reset();
							LoadPicture();
						}
					}
					break;
				case Exit::Particle:
					if (!speedmoveflag1)
					if (fabs(size - presize) > 0.01)
						size += (presize - size) / 10;
					else
					{
						size = presize;
						speedmoveflag1 = true;
					}
					if (!speedmoveflag2)
					if (abs((int)renderx - SCREEN_WIDTH / 2) > 5 || abs((int)rendery - SCREEN_HEIGHT / 2) > 5)
					{
						renderx -= (renderx - SCREEN_WIDTH / 2) / 3;
						rendery -= (rendery - SCREEN_HEIGHT / 2) / 3;
					}
					else
					{
						renderx = SCREEN_WIDTH / 2;
						rendery = SCREEN_HEIGHT / 2;
						speedmoveflag2 = true;
					}
					if (speedmoveflag1 && speedmoveflag2)
					{
						if (!ExitParticleStart)
						{
							ExitParticleStart = true;
							ImgParticlesRe_use();
						}
						int count = 0;
						for (int i = 0; i < PARTICLENUM; i++)
						{
							switch (ParticleMode)
							{
							case ImgParticle::SpeedMove:
								arrays[i].nowy -= abs(arrays[i].nowy - arrays[i].targety) / 300 + 0.08*(PARTICLENUM - i + hge->Random_Int(0, 15));
								if (arrays[i].size>0)
								{
									arrays[i].size -= abs(arrays[i].size - presize) / 300 + 0.0000005*(PARTICLENUM - i) + 0.01 + hge->Random_Float(0, 0.02);
									if (arrays[i].size < 0)
										arrays[i].size = 0;
								}
								else
								{
									arrays[i].size = 0;
									count++;
								}
								break;
							case ImgParticle::Swell:
								if (arrays[i].alpha == 255)
								{
									if (arrays[i].size < presize + 0.2)
									{
										arrays[i].size += hge->Random_Float(0, 0.03);
										arrays[i].nowy += hge->Random_Int(1, abs(arrays[i].size - presize - 0.2) * 50 + 1)*(i % 2 ? -1 : 1);
										arrays[i].nowx += hge->Random_Int(1, abs(arrays[i].size - presize - 0.2) * 50 + 1)*(i % 2 ? -1 : 1);
									}
									else
									{
										arrays[i].alpha = 0;
										arrays[i].size = presize + 0.2;
									}
								}
								else
								{
									arrays[i].size = (presize + 0.2)*(abs(arrays[i].nowx - SCREEN_WIDTH / 2) / abs(arrays[i].targetx - SCREEN_WIDTH / 2));
									if (abs((int)arrays[i].nowx - SCREEN_WIDTH / 2) > 5 || abs((int)arrays[i].nowy != SCREEN_HEIGHT / 2) > 5)
									{
										arrays[i].nowx -= (arrays[i].nowx - SCREEN_WIDTH / 2) / 3;
										arrays[i].nowy -= (arrays[i].nowy - SCREEN_HEIGHT / 2) / 3;
									}
									else
									{
										count++;
										arrays[i].size = 0;
									}
								}
								break;
							case ImgParticle::TransParent:
								if (arrays[i].alpha != 0)
								{
									arrays[i].alpha -= hge->Random_Int(0, 5)*hge->Random_Int(0, 3);
									if (arrays[i].alpha < 0)
										arrays[i].alpha = 0;
									imgparticles[i]->SetColor(ARGB(arrays[i].alpha, 255, 255, 255));
								}
								else
								{
									imgparticles[i]->SetColor(ARGB(0, 255, 255, 255));
									count++;
								}
								break;
							}
						}
						if (count == PARTICLENUM)
						{
							exit1 = false;
							Reset();
							LoadPicture();
						}
					}
					break;

				}
			}
		}


		if (hge->Input_KeyDown(HGEK_LBUTTON))
		{
			float x, y;
			hge->Input_GetMousePos(&x, &y);
			if (x>SCREEN_WIDTH - 100 && x < SCREEN_WIDTH - 50 && y<SCREEN_HEIGHT && y>SCREEN_HEIGHT - 50)
			{
				Album = AlbumStats::Option;
				phototooption = true;
			}
		}



		if (flashL)
		{
			if (countL < 15)
			{
				countL++;
				if (countL % 6 == 0)
					LB->SetColor(ARGB(255, 255, 255, 255));
				else
					LB->SetColor(ARGB(255, 0, 0, 0));
			}
			else
			{
				countL = 0;
				LB->SetColor(ARGB(255, 0, 0, 0));
				flashL = false;
			}
		}
		if (flashR)
		{
			if (countR < 15)
			{
				countR++;
				if (countR % 6 == 0)
					RB->SetColor(ARGB(255, 255, 255, 255));
				else
					RB->SetColor(ARGB(255, 0, 0, 0));
			}
			else
			{
				countR = 0;
				RB->SetColor(ARGB(255, 0, 0, 0));
				flashR = false;
			}
		}
		break;
		case AlbumStats::Menu:
			if (!optionalpha && !framealpha)
			if (hge->Input_KeyDown(HGEK_LBUTTON))
			{
				float x, y;
				hge->Input_GetMousePos(&x, &y);
				if (x > SCREEN_WIDTH / 2 - 50 && x < SCREEN_WIDTH/2 + 50)
				{
					if (y>BUTTON1Y - 25 && y < BUTTON1Y + 25)
					{
						autonext = true;
						Album = AlbumStats::Photos;
						MusicManager::GetMusicManager().SwitchMode();
						LoadPicture();
					}
					if (y>BUTTON2Y - 25 && y < BUTTON2Y + 25)
					{
						Album = AlbumStats::Photos;
						MusicManager::GetMusicManager().SwitchMode();
						LoadPicture();
					}
					if (y>BUTTON3Y - 25 && y < BUTTON3Y + 25)
					{
						return true;
					}
				}
				if (x>SCREEN_WIDTH - 100 && x < SCREEN_WIDTH-50 && y<SCREEN_HEIGHT && y>SCREEN_HEIGHT - 50)
				{
						Album = AlbumStats::Option;
				}
			}
			break;
		case AlbumStats::Option:
			if (optionalpha == 255)
			{
				if (hge->Input_KeyDown(HGEK_LBUTTON))
				{
					float x, y;
					hge->Input_GetMousePos(&x, &y);
					if (x > SCREEN_WIDTH - 100 && x < SCREEN_WIDTH-50 && y<SCREEN_HEIGHT && y>SCREEN_HEIGHT - 50)
					{
						if (!phototooption)
						Album = AlbumStats::Menu;
						else
						{
							Album = AlbumStats::Photos;
						}
					}
					if (y<SCREEN_HEIGHT / 4 + 75 && y>SCREEN_HEIGHT / 4 + 25)
					{
						if (x>275 && x < 325)
						{
							MusicManager::GetMusicManager().ChangeMode(MusicPlay::Shuffle);
						}
						if (x>475 && x < 525)
						{
							MusicManager::GetMusicManager().ChangeMode(MusicPlay::Order);
						}
						if (x>675 && x < 725)
						{
							MusicManager::GetMusicManager().ChangeMode(MusicPlay::Loop);
						}
					}
				}
				if (hge->Input_GetKeyState(HGEK_LBUTTON))
				{
					if (!mouseExit)
					{
						float x, y;
						hge->Input_GetMousePos(&x, &y);
						if (x > SCREEN_WIDTH / 2 - 350 && x<SCREEN_WIDTH / 2 + 350 && y>SCREEN_HEIGHT / 4 - 65 && y<SCREEN_HEIGHT / 4 + 65)
						{
							if (!countformove)
							{
								hge->Input_GetMousePos(&mousex, &mousey);
								countformove = 1;
							}
							else
							{
								countformove--;
								if (x > mousex && pointx < SCREEN_WIDTH / 2 + 284)
								{
									pointx += (x - mousex) * 2;
									if (pointx > SCREEN_WIDTH / 2 + 284)
										pointx = SCREEN_WIDTH / 2 + 284;
								}
								if (x < mousex && pointx>SCREEN_WIDTH / 2 - 284)
								{
									pointx -= (mousex - x) * 2;
									if (pointx < SCREEN_WIDTH / 2 - 284)
										pointx = SCREEN_WIDTH / 2 - 284;
								}
							}
						}
						else
							mouseExit = true;
					}
				}
				if (hge->Input_KeyUp(HGEK_LBUTTON))
				{
					countformove = 0;
					mouseExit = false;
				}
			}
			globalvol = ((pointx - SCREEN_WIDTH / 2 + 284) / 284) * 50;
			hge->System_SetState(HGE_FXVOLUME, globalvol);
			break;
	}

	switch (MusicManager::GetMusicManager().GetMode())
	{
	case MusicPlay::Order:
		if (abs(selectmodex - 500) > 3)
			selectmodex += (500 - selectmodex) / 10 + (500 > selectmodex ? 1 : -1);
		else
			selectmodex = 500;
		break;
	case MusicPlay::Loop:
		if (abs(selectmodex - 700) > 3)
			selectmodex += (700 - selectmodex) / 10 + (700 > selectmodex ? 1 : -1);
		else
			selectmodex = 700;
		break;
	case MusicPlay::Shuffle:
		if (abs(selectmodex - 300) > 3)
			selectmodex += (300 - selectmodex) / 10 + (300 > selectmodex ? 1 : -1);
		else
			selectmodex = 300;
		break;
	}
		float mx, my;
		hge->Input_GetMousePos(&mx, &my);
		if (abs(trackmousex - mx) > 3)
			trackmousex += (mx - trackmousex) / 20 + (mx > trackmousex ? -1 : 1);
		else
			trackmousex = mx;
		if (abs(trackmousey - my) > 3)
			trackmousey += (my - trackmousey) / 20+(my>trackmousey ? -1 : 1);
		else
			trackmousey = my;
		arrx = (trackmousex - BGPARTSIZE/2) / BGPARTSIZE;
		arry = (trackmousey - BGPARTSIZE/2) / BGPARTSIZE;
		for (int i = 0; i < BACKGROUNDNUM; i++)
		{
			int arrx2 = i % (SCREEN_WIDTH / BGPARTSIZE);
			int arry2 = i / (SCREEN_WIDTH / BGPARTSIZE);
			if (alphas[i]>5)
				alphas[i] -= 5;
			else
				alphas[i] = 0;
			float setvalue = sqrt((arrx2 - arrx)*(arrx2 - arrx) + (arry2 - arry)*(arry2 - arry));
			if (setvalue<12)
			{
				if (alphas[i]<(12 - setvalue) * 255 / 12)
					alphas[i] = (12 - setvalue) * 255 / 12;
			}
		}
		MusicManager::GetMusicManager().Update();
	return false;
}


bool RenderFunc()
{
	// Render the scene
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0x000000);
	bgspr->Render(0, 0);
	for (int i = 0; i < BACKGROUNDNUM; i++)
	{
		if (alphas[i])
		{
			int rex, rey;
			rex = i % (SCREEN_WIDTH / BGPARTSIZE);
			rey = i / (SCREEN_WIDTH / BGPARTSIZE);
			bgpart->SetColor(ARGB(alphas[i], 255, 255, 255));
			bgpart->Render(BGPARTSIZE / 2 + rex * BGPARTSIZE, BGPARTSIZE / 2 + rey *BGPARTSIZE);
		}
	}
	switch (Album)
	{
	case AlbumStats::Photos:
		title->Render(SCREEN_WIDTH / 2, 150);
		button1->Render(SCREEN_WIDTH / 2, BUTTON1Y);
		button2->Render(SCREEN_WIDTH / 2, BUTTON2Y);
		button3->Render(SCREEN_WIDTH / 2, BUTTON3Y);
		optionbutton->Render(SCREEN_WIDTH - 75, SCREEN_HEIGHT - 25);
		bar->Render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4);
		barpoint->Render(pointx, pointy);
		shuffle->Render(300, SCREEN_HEIGHT / 4 + 50);
		order->Render(500, SCREEN_HEIGHT / 4 + 50);
		loop->Render(700, SCREEN_HEIGHT / 4 + 50);
		selectmode->Render(selectmodex, selectmodey);
		font->printf(SCREEN_WIDTH / 2 + 350, SCREEN_HEIGHT / 4 - 15, HGETEXT_CENTER, "%d", globalvol);
		font->printf(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50, HGETEXT_CENTER, "Coded by Fuwa.");
		if (EntryMode != Entry::Particle && !ExitParticleStart)
		{
			rim->RenderEx(renderx, rendery, 0, size*1.03, size*1.03);
			spr->RenderEx(renderx, rendery, angle, size, size);
			for (int i = 0; i<CLOCKPARTICLENUM; i++)
			{
				particle->SetColor(ARGB(particles[i].alpha, GETR(rim->GetColor()), GETG(rim->GetColor()), GETB(rim->GetColor())));
				particle->RenderEx(particles[i].posx, particles[i].posy, 0, particles[i].pssize, particles[i].pssize);
			}

		}
		else
		{
			if (imgparticles.size() > 0)
			for (int i = 0; i < PARTICLENUM; i++)
				imgparticles[i]->RenderEx(arrays[i].nowx, arrays[i].nowy, 0, arrays[i].size, arrays[i].size);
		}
		if (phototooption)
		{
			if (optionalpha != 0)
			{
				optionalpha -= 5;
				bar->SetColor(ARGB(optionalpha, 255, 255, 255));
				barpoint->SetColor(ARGB(optionalpha, 255, 255, 255));
				loop->SetColor(ARGB(optionalpha, 255, 255, 255));
				shuffle->SetColor(ARGB(optionalpha, 255, 255, 255));
				order->SetColor(ARGB(optionalpha, 255, 255, 255));
				selectmode->SetColor(ARGB(optionalpha, 255, 255, 255));
				font->SetColor(ARGB(optionalpha, 255, 255, 255));
				Frame->SetColor(ARGB(255 - optionalpha, 255, 255, 255));
				LB->SetColor(ARGB(255 - optionalpha, 0, 0, 0));
				RB->SetColor(ARGB(255 - optionalpha, 0, 0, 0));
				if (alpha == 255)
				{
					if (alphaforrim > optionalpha)
						rim->SetColor(ARGB(alphaforrim - optionalpha, GETR(rim->GetColor()), GETG(rim->GetColor()), GETB(rim->GetColor())));
				}
				else
				{
					if (alpha>optionalpha)
						rim->SetColor(ARGB(alpha - optionalpha, GETR(rim->GetColor()), GETG(rim->GetColor()), GETB(rim->GetColor())));
				}
				if (alpha>optionalpha)
				spr->SetColor(ARGB(alpha - optionalpha, 255, 255, 255));
				for (int i = 0; i<CLOCKPARTICLENUM; i++)
					particle->SetColor(ARGB(255 - optionalpha, GETR(rim->GetColor()), GETG(rim->GetColor()), GETB(rim->GetColor())));
				if (imgparticles.size() > 0)
				for (int i = 0; i < PARTICLENUM; i++)
					imgparticles[i]->SetColor(ARGB(255 - optionalpha, 255, 255, 255));
			}
			else
				phototooption = false;

		}




		if (framealpha != 255)
		{
			framealpha += 5;
			Frame->SetColor(ARGB(framealpha, 255, 255, 255));
			LB->SetColor(ARGB(framealpha, 0, 0, 0));
			RB->SetColor(ARGB(framealpha, 0, 0, 0));
			title->SetColor(ARGB(255 - framealpha, 255, 255, 255));
			button1->SetColor(ARGB(255 - framealpha, 255, 255, 255));
			button2->SetColor(ARGB(255 - framealpha, 255, 255, 255));
			button3->SetColor(ARGB(255 - framealpha, 255, 255, 255));
		}
		Frame->Render(0, 0);
		LB->Render(25, SCREEN_HEIGHT / 2);
		RB->Render(SCREEN_WIDTH - 25, SCREEN_HEIGHT / 2);
		break;
	case AlbumStats::Menu:
		title->Render(SCREEN_WIDTH / 2,150);
		button1->Render(SCREEN_WIDTH / 2, BUTTON1Y);
		button2->Render(SCREEN_WIDTH / 2, BUTTON2Y);
		button3->Render(SCREEN_WIDTH / 2, BUTTON3Y);
		optionbutton->Render(SCREEN_WIDTH - 75, SCREEN_HEIGHT - 25);
		bar->Render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4);
		barpoint->Render(pointx, pointy);
		shuffle->Render(300, SCREEN_HEIGHT / 4 + 50);
		order->Render(500, SCREEN_HEIGHT / 4 + 50);
		loop->Render(700, SCREEN_HEIGHT / 4 + 50);
		font->printf(SCREEN_WIDTH / 2 + 350, SCREEN_HEIGHT / 4 - 15, HGETEXT_CENTER, "%d", globalvol);
		font->printf(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50, HGETEXT_CENTER, "Coded by Fuwa.");
		selectmode->Render(selectmodex, selectmodey);
		if (framealpha != 0)
		{
			framealpha -= 5;
			Frame->SetColor(ARGB(framealpha, 255, 255, 255));
			LB->SetColor(ARGB(framealpha, 0, 0, 0));
			RB->SetColor(ARGB(framealpha, 0, 0, 0));
			title->SetColor(ARGB(255-framealpha, 255, 255, 255));
			button1->SetColor(ARGB(255-framealpha, 255, 255, 255));
			button2->SetColor(ARGB(255-framealpha, 255, 255, 255));
			button3->SetColor(ARGB(255 - framealpha, 255, 255, 255));
		}
		else
		if (optionalpha != 0)
		{
			optionalpha -= 5;
			title->SetColor(ARGB(255 - optionalpha, 255, 255, 255));
			button1->SetColor(ARGB(255 - optionalpha, 255, 255, 255));
			button2->SetColor(ARGB(255 - optionalpha, 255, 255, 255));
			button3->SetColor(ARGB(255 - optionalpha, 255, 255, 255));
			bar->SetColor(ARGB(optionalpha, 255, 255, 255));
			barpoint->SetColor(ARGB(optionalpha, 255, 255, 255));
			loop->SetColor(ARGB(optionalpha, 255, 255, 255));
			shuffle->SetColor(ARGB(optionalpha, 255, 255, 255));
			order->SetColor(ARGB(optionalpha, 255, 255, 255));
			selectmode->SetColor(ARGB(optionalpha, 255, 255, 255));
			font->SetColor(ARGB(optionalpha, 255, 255, 255));
		}
		Frame->Render(0, 0);
		LB->Render(25, SCREEN_HEIGHT / 2);
		RB->Render(SCREEN_WIDTH - 25, SCREEN_HEIGHT / 2);
		break;
	case AlbumStats::Option:
		optionbutton->Render(SCREEN_WIDTH - 75, SCREEN_HEIGHT - 25);
		bar->Render(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4);
		barpoint->Render(pointx, pointy);
		shuffle->Render(300, SCREEN_HEIGHT / 4 + 50);
		order->Render(500, SCREEN_HEIGHT / 4 + 50);
		loop->Render(700, SCREEN_HEIGHT / 4 + 50);
		selectmode->Render(selectmodex, selectmodey);
		font->printf(SCREEN_WIDTH / 2 + 350, SCREEN_HEIGHT / 4-15, HGETEXT_CENTER, "%d", globalvol);
		font->printf(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50, HGETEXT_CENTER, "Coded by Fuwa.");
		if (phototooption)
		{
			if (EntryMode != Entry::Particle && !ExitParticleStart)
			{
				rim->RenderEx(renderx, rendery, 0, size*1.03, size*1.03);
				spr->RenderEx(renderx, rendery, 0, size, size);
			}
			else
			{
				if (imgparticles.size() > 0)
				for (int i = 0; i < PARTICLENUM; i++)
					imgparticles[i]->RenderEx(arrays[i].nowx, arrays[i].nowy, 0, arrays[i].size, arrays[i].size);
			}
		}
		else
		{
			title->Render(SCREEN_WIDTH / 2, 150);
			button1->Render(SCREEN_WIDTH / 2, BUTTON1Y);
			button2->Render(SCREEN_WIDTH / 2, BUTTON2Y);
			button3->Render(SCREEN_WIDTH / 2, BUTTON3Y);
		}
		if (optionalpha != 255)
		{
			optionalpha += 5;
			if (phototooption)
			{
				Frame->SetColor(ARGB(255 - optionalpha, 255, 255, 255));
				LB->SetColor(ARGB(255 - optionalpha, 0, 0, 0));
				RB->SetColor(ARGB(255 - optionalpha, 0, 0, 0));
				rim->SetColor(ARGB(((255 - (float)optionalpha)/255)*alphaforrim, GETR(rim->GetColor()), GETG(rim->GetColor()), GETB(rim->GetColor())));
				spr->SetColor(ARGB(255 - optionalpha, 255, 255, 255));
				for (int i = 0; i<CLOCKPARTICLENUM; i++)
					particle->SetColor(ARGB(255 - optionalpha, GETR(rim->GetColor()), GETG(rim->GetColor()), GETB(rim->GetColor())));
				if (imgparticles.size() > 0)
				for (int i = 0; i < PARTICLENUM; i++)
					imgparticles[i]->SetColor(ARGB(255 - optionalpha, 255, 255, 255));
			}
			else
			{
				title->SetColor(ARGB(255 - optionalpha, 255, 255, 255));
				button1->SetColor(ARGB(255 - optionalpha, 255, 255, 255));
				button2->SetColor(ARGB(255 - optionalpha, 255, 255, 255));
				button3->SetColor(ARGB(255 - optionalpha, 255, 255, 255));
			}
			bar->SetColor(ARGB(optionalpha, 255, 255, 255));
			barpoint->SetColor(ARGB(optionalpha, 255, 255, 255));
			loop->SetColor(ARGB(optionalpha, 255, 255, 255));
			shuffle->SetColor(ARGB(optionalpha, 255, 255, 255));
			order->SetColor(ARGB(optionalpha, 255, 255, 255));
			selectmode->SetColor(ARGB(optionalpha, 255, 255, 255));
			font->SetColor(ARGB(optionalpha, 255, 255, 255));
		}
		Frame->Render(0, 0);
		LB->Render(25, SCREEN_HEIGHT / 2);
		RB->Render(SCREEN_WIDTH - 25, SCREEN_HEIGHT / 2);
		break;
	}
	float x, y;
	hge->Input_GetMousePos(&x, &y);
	mouse->Render(x, y);
	hge->Gfx_EndScene();
	return false;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	hge = hgeCreate(HGE_VERSION);

	// Set desired system states and initialize HGE
	//hge->System_SetState(HGE_LOGFILE, "hge_tut07.log");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "纪念相册——Commemorative Album");
	hge->System_SetState(HGE_USESOUND, true);
	hge->System_SetState(HGE_FXVOLUME, 50);
	hge->System_SetState(HGE_FPS, 130);
	//焦点失去时继续运行：
	hge->System_SetState(HGE_DONTSUSPEND, true);
	hge->System_SetState(HGE_SHOWSPLASH, false);
	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_SCREENWIDTH, SCREEN_WIDTH);
	hge->System_SetState(HGE_SCREENHEIGHT, SCREEN_HEIGHT);
	hge->System_SetState(HGE_SCREENBPP, 32);
	if(hge->System_Initiate())
	{
		// Let's rock now!
		FindFiles("\\picture");
		bgtex = hge->Texture_Load("bg.png");
		bgspr = new hgeSprite(bgtex, 0, 0, hge->Texture_GetWidth(bgtex, true), hge->Texture_GetHeight(bgtex, true));
		LBtex = hge->Texture_Load("L.png");
		LB = new hgeSprite(LBtex, 0, 0, hge->Texture_GetWidth(LBtex, true), hge->Texture_GetHeight(LBtex, true));
		LB->SetHotSpot(hge->Texture_GetWidth(LBtex, true) / 2, hge->Texture_GetHeight(LBtex, true) / 2);
		LB->SetBlendMode(BLEND_COLORADD | BLEND_ALPHABLEND);
		LB->SetColor(ARGB(255, 0, 0, 0));
		RBtex = hge->Texture_Load("R.png");
		RB = new hgeSprite(RBtex, 0, 0, hge->Texture_GetWidth(RBtex, true), hge->Texture_GetHeight(RBtex, true));
		RB->SetHotSpot(hge->Texture_GetWidth(RBtex, true) / 2, hge->Texture_GetHeight(RBtex, true) / 2);
		RB->SetBlendMode(BLEND_COLORADD | BLEND_ALPHABLEND);
		RB->SetColor(ARGB(255, 0, 0, 0));
		Frametex = hge->Texture_Load("Frame.png");
		Frame = new hgeSprite(Frametex, 0, 0, hge->Texture_GetWidth(Frametex, true), hge->Texture_GetHeight(Frametex, true));
		mousetex = hge->Texture_Load("mouse.png");
		mouse = new hgeSprite(mousetex, 0, 0, hge->Texture_GetWidth(mousetex, true), hge->Texture_GetHeight(mousetex, true));
		mouse->SetHotSpot(hge->Texture_GetWidth(mousetex, true)/2, hge->Texture_GetHeight(mousetex, true)/2);
		titletex = hge->Texture_Load("title.png");
		button1tex = hge->Texture_Load("PPTMode.png");
		button2tex = hge->Texture_Load("LIUMode.png");
		title = new hgeSprite(titletex, 0, 0, 600, 200);
		title->SetHotSpot(300, 100);
		button1 = new hgeSprite(button1tex, 0, 0, 100, 50);
		button1->SetHotSpot(50, 25);
		button2 = new hgeSprite(button2tex, 0, 0, 100, 50);
		button2->SetHotSpot(50, 25);
		bgparttex = hge->Texture_Load("bgpart.png");
		bgpart = new hgeSprite(bgparttex, 0, 0, BGPARTSIZE, BGPARTSIZE);
		bgpart->SetHotSpot(BGPARTSIZE / 2, BGPARTSIZE/2);
		particletex = hge->Texture_Load("particle.png");
		particle = new hgeSprite(particletex, 0, 0, 32, 32);
		particle->SetHotSpot(16, 16);
		button3tex = hge->Texture_Load("exit.png");
		button3 = new hgeSprite(button3tex, 0, 0, 100, 50);
		button3->SetHotSpot(50, 25);
		optiontex = hge->Texture_Load("option.png");
		optionbutton = new hgeSprite(optiontex, 0, 0, 42, 42);
		optionbutton->SetHotSpot(21, 21);
		bartex = hge->Texture_Load("volbar.png");
		bar = new hgeSprite(bartex, 0, 0, 700, 30);
		bar->SetHotSpot(400, 15);
		barpointtex = hge->Texture_Load("volpoint.png");
		barpoint = new hgeSprite(barpointtex, 0, 0, 25, 25);
		barpoint->SetHotSpot(12.5, 12.5);
		bar->SetColor(ARGB(0, 255, 255, 255));
		barpoint->SetColor(ARGB(0, 255, 255, 255));
		musicmodetex = hge->Texture_Load("musicmode.png");
		loop = new hgeSprite(musicmodetex, 0, 0, 50, 50);
		loop->SetHotSpot(25, 25);
		order = new hgeSprite(musicmodetex, 50, 0, 50, 50);
		order->SetHotSpot(25, 25);
		shuffle = new hgeSprite(musicmodetex, 100, 0, 50, 50);
		shuffle->SetHotSpot(25, 25);
		loop->SetColor(ARGB(0, 255, 255, 255));
		shuffle->SetColor(ARGB(0, 255, 255, 255));
		order->SetColor(ARGB(0, 255, 255, 255));
		selectmode = new hgeSprite(musicmodetex, 150, 0, 50, 50);
		selectmode->SetHotSpot(25, 25);
		selectmode->SetColor(ARGB(0, 255, 255, 255));
		font = new hgeFont("FONT.fnt");
		font->SetColor(ARGB(0, 255, 255, 255));
		arrx = (trackmousex - BGPARTSIZE / 2) / BGPARTSIZE;
		arry = (trackmousey - BGPARTSIZE / 2) / BGPARTSIZE;
		for (int i = 0; i < BACKGROUNDNUM; i++)
		{
			int arrx2 = i % (SCREEN_WIDTH / BGPARTSIZE);
			int arry2 = i / (SCREEN_WIDTH / BGPARTSIZE);
			if (alphas[i]>5)
				alphas[i] -= 5;
			else
				alphas[i] = 0;
			float setvalue = sqrt((arrx2 - arrx)*(arrx2 - arrx) + (arry2 - arry)*(arry2 - arry));
			if (setvalue<12)
			{
				if (alphas[i]<(12 - setvalue) * 255 / 12)
					alphas[i] = (12 - setvalue) * 255 / 12;
			}
		}
		MusicManager::GetMusicManager();
		ReStart();
		hge->System_Start();

		// Delete created objects and free loaded resources
	}

	// Clean up and shutdown
	for (int i = 0; i < imgparticles.size();)
	{
		if (imgparticles[i])
		{
			delete imgparticles[i];
			imgparticles[i] = NULL;
			imgparticles.erase(imgparticles.begin() + i);
		}
		else
			i++;
	}
	hge->Texture_Free(tex);
	hge->Texture_Free(bgtex);
	hge->Texture_Free(shadowtex);
	hge->Texture_Free(LBtex);
	hge->Texture_Free(RBtex);
	hge->Texture_Free(Frametex);
	hge->Texture_Free(mousetex);
	hge->Texture_Free(titletex);
	hge->Texture_Free(button1tex);
	hge->Texture_Free(button2tex);
	hge->Texture_Free(bgparttex);
	hge->Texture_Free(particletex);
	hge->Texture_Free(button3tex);
	hge->Texture_Free(optiontex);
	hge->Texture_Free(bartex);
	hge->Texture_Free(barpointtex);
	hge->Texture_Free(musicmodetex);
	delete bar;
	delete barpoint;
	delete shuffle;
	delete loop;
	delete order;
	delete optionbutton;
	delete particle;
	delete bgpart;
	delete spr;
	delete bgspr;
	delete shadow;
	delete rim;
	delete LB;
	delete RB;
	delete Frame;
	delete mouse;
	delete title;
	delete button1;
	delete button2;
	delete button3;
	MusicManager::GetMusicManager().Exit();
	hge->System_Shutdown();
	hge->Release();
	return 0;
}
