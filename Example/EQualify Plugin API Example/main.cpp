/*
	Equalify Plugin API Example
	Copyright (C) 2013  Kenneth Leonardsen / Equalify.me

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	
	Contact info: leo@equalify.me
*/


// ALL OF THE CODE BELOW IS MESSY, ITS JUST AN EXAMPLE THAT SHOWS HOW TO PLAY WITH THE API...

#define _CRT_SECURE_NO_WARNINGS
#include "stdio.h"
#include <Windows.h>
#include <Windowsx.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#include <math.h>
#include "pluginapi.h"


ULONG_PTR gdiplusToken=NULL;
float *floatOldMag;

const s_EQinfo *EQinfo;
PluginsInfo MyInfo={0};
bool threadrunning =false;

#define AUTHOR L"Equalify"
#define PLUGIN_DESC L"EQUALIFY API SKELETON APP"
#define PLUGIN_NAME L"API SKELETON EXAMPLE"




int AnalyzerMode=1;
int plugin_main();
int wx=800;			//window width
int wy=600;			//window height
int wxi=800;
int barcount=80;	


class DXanalyzer : public EQPlugin   
{
public:
	virtual int Initialize_plugin(const s_EQinfo *info)
	{
		if (info){
			EQinfo=info;
			floatOldMag = (float*)malloc(EQinfo->NumberOfAnalyzedSamples);
			memset(floatOldMag,0, EQinfo->NumberOfAnalyzedSamples);
			return 1;
		}
		return 0;
	}
	virtual PluginsInfo *PluginData(void) const
	{
		return &MyInfo;
	}
	const int Get_Unloadable() const
	{
		return threadrunning ? 0:1;
	}
	virtual void Process_Data ()
	{
		if ((!threadrunning)){
			threadrunning=true;
			if (CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&plugin_main,NULL,0,NULL) == NULL)
			{
			}

		}
	}

};

extern "C"
{
	__declspec(dllexport) int API_VERSION ()
	{
		return ApiVersionNumber;
	}

	__declspec(dllexport) EQPlugin* Create_Plugin (char* ModuleFilePath)
	{
		printf("----\n%s\n----\n",ModuleFilePath);

		lstrcpyn(MyInfo.author,AUTHOR,255);
		lstrcpyn(MyInfo.desc, PLUGIN_DESC ,1024);
		lstrcpyn(MyInfo.name, PLUGIN_NAME ,255);
		MyInfo.ApiVersion=ApiVersionNumber;
		MyInfo.type=1;
		Gdiplus::GdiplusStartupInput gdiplusstartupinput;
		Gdiplus::GdiplusStartup(&gdiplusToken,&gdiplusstartupinput,NULL);
		return new DXanalyzer ();
	}

	__declspec(dllexport) void Release_Plugin (EQPlugin* p_plugin)
	{
		//add cleanupcode here
		delete p_plugin;
	}

}


inline void DrawSpectrumLarge(HDC drawhdc, float* fftData,float specbands,int x,int y, int height, int width,int reverse,float decay,float multiplier)
{
	/*
	This whole function is a HUHE MESS
	i do not remember where i got the idea, and i do not rememeber when it all went wrong...
	It does show the dancing bars though.. so.. Example plugin for the win!

	This code is ripped/modified from a 2 year old source file from the Equalify project...
	*/
	
	if(fftData == NULL)
		return;	
	Gdiplus::Graphics graphics(drawhdc);

	/////vv
	if(!reverse) height =wy;
	int gradientheight=wy;
	if(reverse==1)
		gradientheight=wy;

 	Gdiplus::LinearGradientBrush blackBrush(Gdiplus::Point( 0, 0),
 		Gdiplus::Point( 0,  gradientheight),
 		Gdiplus::Color(255, 0, 0, 0),   
 		Gdiplus::Color(255, 255,255, 255)); 


 	if(reverse==1) {
 		blackBrush.SetBlendTriangularShape(.5f, 1.0f);
 	}
	///////^^

	RECT rect;

	rect.left = x-1;
	rect.top = y;
	rect.right = rect.left+width;
	rect.bottom = rect.top+height;

	float c = 0;
	float floatFrrh = 1.0;
	float floatDecay = decay;
	float floatSadFrr = (floatFrrh*floatDecay);
	float floatBandWidth = ((float)width/(float)specbands);
	float floatMultiplier = multiplier;
	RECT r;
	Gdiplus::Rect r2;
	for(int a=0, band=0; band < specbands; a+=(int)floatMultiplier, band++)
	{
		float wFs = 0;

		for (int b = 0; b < floatMultiplier; b++) {
			wFs += fftData[a + b];
		}


		wFs = (wFs * (float) log((float)(band + 2)));
		if (wFs > 1.4f) {
			wFs =1.4f;
		}


		if (wFs >= (floatOldMag[a] - floatSadFrr)) {
			floatOldMag[a] = wFs;
		} else {
			floatOldMag[a] -= floatSadFrr;
			if (floatOldMag[a] < 0) {
				floatOldMag[a] = 0;
			}
			wFs = floatOldMag[a];
		}

		r.left = rect.left + (int)c + 1;
		r.right = r.left + (int)(floatBandWidth-1);
		r.top = height - (int)(wFs*(height-23)+23);
		if(r.top < rect.top)
			r.top = rect.top + 2;

		r.top += 22;
		r.bottom = rect.bottom-2;		
		r2.X=r.left;
		r2.Width=(r.right-r.left);
		r2.Y=r.top;

		r2.Height=(r.bottom-r.top);
		graphics.FillRectangle(&blackBrush,r2);

		r2.Height=((r.bottom-r.top) + ((r.bottom-r.top)/2));
		r2.Y=r.top-((r.bottom-r.top)/2);
		graphics.FillRectangle(&blackBrush,r2);

		r2.Height=((r.bottom-r.top) + ((r.bottom-r.top)/2))/2;
		r2.Y=r.top+((r.bottom-r.top)/2)/2;

		graphics.FillRectangle(&blackBrush,r2);

		if (reverse) {
			r2.Y=r.bottom;
			r2.Height=(r.bottom-r.top);
			graphics.FillRectangle(&blackBrush,r2);

			r2.Height=((r.bottom-r.top) + ((r.bottom-r.top)/2));
			graphics.FillRectangle(&blackBrush,r2);
			r2.Height=((r.bottom-r.top) + ((r.bottom-r.top)/2))/2;
			graphics.FillRectangle(&blackBrush,r2);


		}

		c += floatBandWidth;
	}

	graphics.ReleaseHDC(drawhdc);
}

LRESULT CALLBACK analyzerwndproc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static LPARAM lParamMouse;

	switch(Message) {
	case WM_CREATE:
		{
			SendMessage(hwnd, WM_TIMER, 6643, 0);
			SetTimer(hwnd, 6643, 50, NULL );
			RECT wrect;
			GetClientRect(hwnd,&wrect);
			wx=wrect.right;
			wy=wrect.bottom;
		}
		break;
	case WM_TIMER:
		switch(wParam)
		{
		case 6643:
			InvalidateRect(hwnd, NULL, FALSE);
			break;
		}
		break;

	case WM_RBUTTONUP: {
		SetCapture(hwnd);
		int	xPos = GET_X_LPARAM(lParam); 
		int	yPos = GET_Y_LPARAM(lParam);
		HMENU   menu;
		POINT   pt;
		menu = CreatePopupMenu();
		GetCursorPos( &pt );
		AppendMenu( menu, MF_STRING, 1005, L"Play/Pause" );
		AppendMenu( menu, MF_STRING, 1006, L"Next" );
		AppendMenu( menu, MF_STRING, 1002, L"Center" );
		AppendMenu( menu, MF_STRING, 1003, L"Bottom" );
		AppendMenu( menu, MF_STRING, 1001, L"Close" );
		long menusel=0;
		menusel = TrackPopupMenuEx( menu
			, TPM_VERPOSANIMATION | TPM_RETURNCMD
			, pt.x, pt.y
			, hwnd
			, NULL );

		if (menusel==1005) {
			EQinfo->Funcs.Play_Pause();
		}
		if (menusel==1006) {
			EQinfo->Funcs.Next_Track();
		}

		if (menusel==1001) {
			threadrunning=false;
			SendMessage(hwnd,WM_CLOSE,0,0);
			ExitThread(2);
		}
		if (menusel==1002) {
			AnalyzerMode=1;
		}
		if (menusel==1003) {
			AnalyzerMode=0;
		}
		ReleaseCapture();
					   }
					   return 0;
					   break;

	case WM_DESTROY: {
		threadrunning=false;
		SendMessage(hwnd,WM_CLOSE,0,0);
		ExitThread(2);
		break;
					 }

	case WM_SIZE:
	case WM_SIZING: {
		RECT wrect;
		GetClientRect(hwnd,&wrect);
		wx=wrect.right;
		wy=wrect.bottom;

		if(wxi>1400)
			barcount=160;
		else
			barcount=80;

		wxi = (int)ceil((double)(((float)wx/(float)barcount)));
		wxi *=barcount;

		if(wxi>1400)
			barcount=160;
		else
			barcount=80;
					}
					break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			if(BeginPaint(hwnd, &ps))
			{
				HDC hdcMem = CreateCompatibleDC(ps.hdc);
				INT ndcMem = SaveDC(hdcMem);
				HBITMAP hbmMem = CreateCompatibleBitmap(ps.hdc, wx, wy);

				SelectObject(hdcMem, hbmMem);
				Gdiplus::Graphics graphics(hdcMem);
				DrawSpectrumLarge(hdcMem, EQinfo->FloatData,(float)barcount,0,5,wy/2,wxi,AnalyzerMode,0.03f,3.0f);

				BitBlt(ps.hdc, 0, 0, wx, wy, hdcMem, 0, 0, SRCCOPY);
				graphics.Clear(Gdiplus::Color::Transparent);
				graphics.ReleaseHDC(hdcMem);

				RestoreDC(hdcMem, ndcMem);
				DeleteObject(hbmMem);
				DeleteDC(hdcMem);
				EndPaint(hwnd, &ps);
			}
		}
		break;
	}
	return DefWindowProc(hwnd, Message, wParam, lParam);;
}



int plugin_main()
{
	HWND hwnd;
	MSG Msg;

	WNDCLASSEX WndClass = {
		sizeof(WNDCLASSEX), CS_VREDRAW | CS_HREDRAW, analyzerwndproc, 0, 0, EQinfo->ghInstance,
		LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)GetStockObject(BLACK_BRUSH), NULL, PLUGIN_NAME, NULL,
	};

	UnregisterClass(PLUGIN_NAME,EQinfo->ghInstance);

	if(!RegisterClassEx(&WndClass)) {
		MessageBox(0, L"Could not register window class...", L"Error!", MB_ICONSTOP | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(/*WS_EX_TOPMOST |*/ WS_EX_COMPOSITED,
		PLUGIN_NAME,PLUGIN_NAME, WS_OVERLAPPEDWINDOW /*WS_CHILD*/,
		100,
		100,
		wx, wy, NULL, NULL, EQinfo->ghInstance, NULL);

	SetWindowPos(hwnd,NULL, 100, 100, wx, wy,NULL);
	if(hwnd == NULL) {
		MessageBox(0, L"Window creation failed!", L"Error!", MB_ICONSTOP | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, 1);
	SetForegroundWindow(hwnd);
	UpdateWindow(hwnd);


	while(GetMessage(&Msg, NULL, 0, 0)) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return Msg.wParam;
	threadrunning=false;
	return 1;
}