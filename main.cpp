/*
Equalify Plugin API 
Copyright (C) 2013  Kenneth Leonardsen / Equalify.me

SKELETON PLUGIN / TEMPLATE

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

#define _CRT_SECURE_NO_WARNINGS
#include "stdio.h"
#include <Windows.h>
#include <Windowsx.h>
#include "pluginapi.h" //kinda important if you want it to work!

#define AUTHOR L"Equalify"
#define PLUGIN_DESC L"EQUALIFY API SKELETON APP"
#define PLUGIN_NAME L"API SKELETON"

#define Window_Width 800
#define Window_Height 600

const s_EQinfo *EQinfo;  //	will be set by equalify so you can access the data it shares
PluginsInfo MyInfo={0};  // set by plugin and shared to equalify

bool threadrunning =false;
int plugin_main();

class Plugin : public EQPlugin   
{
public:

	virtual int Initialize_plugin(const s_EQinfo *info)
	{
		if (info){
			EQinfo=info;
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
				//should probably not ignore that the thread could not be started... but here we are, in a comment explaining why i ignored it...
			}

		}
	}

};

extern "C"
{
	//Called before loading anything else, to check if Equalify and the plugin are on the same API version!
	__declspec(dllexport) int API_VERSION ()
	{
		return ApiVersionNumber;
	}

	__declspec(dllexport) EQPlugin* Create_Plugin (char* ModuleFilePath)
	{
		//if you start spotify with "spotify.exe -console" you will be able to see console output!
		printf("----\n%s\n----\n",ModuleFilePath);

		lstrcpynW(MyInfo.author,AUTHOR,255); //please dont change the limits, they are there for a reason :)
		lstrcpynW(MyInfo.desc,PLUGIN_DESC,1024);
		lstrcpynW(MyInfo.name,PLUGIN_NAME,255);
		MyInfo.ApiVersion=ApiVersionNumber;
		MyInfo.type=1;
		return new Plugin ();
	}

	//Called on plugin unload
	__declspec(dllexport) void Release_Plugin (EQPlugin* p_plugin)
	{
		//add cleanupcode here
		delete p_plugin;
	}

}

LRESULT CALLBACK Default_window_proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static LPARAM lParamMouse;

	switch(Message) {

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
		AppendMenu( menu, MF_STRING, 1004, L"Trackinfo" );
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


		if (menusel==1004) {
			//if you start spotify with "spotify.exe -console" you will be able to see console output!

			TRACKINFO test= {0};
			EQinfo->Funcs.GetTrackInfo(&test);
			printf("TRACKINFO:\n========================\n%s\n%s\n%i\n%s\n%s\n%i\n%s\n%s\n%s\n%s\n%i\n%i\n%f\n%i\n",
				test.name,
				test.URI,
				test.length,
				test.artist,
				test.artistURI,
				test.coartists,
				test.album,
				test.albumyear,
				test.albumURI,
				test.redirect,
				test.redirects,
				test.currplay,
				test.vol,
				test.tracktype
				);
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
	}
	return DefWindowProc(hwnd, Message, wParam, lParam);;
}

int plugin_main()
{
	HWND hwnd;
	MSG Msg;

	WNDCLASSEX WndClass = {
		sizeof(WNDCLASSEX), CS_VREDRAW | CS_HREDRAW, Default_window_proc, 0, 0, EQinfo->ghInstance,
		LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)GetStockObject(BLACK_BRUSH), NULL, PLUGIN_NAME , NULL,
	};

	UnregisterClass(PLUGIN_NAME,EQinfo->ghInstance);

	if(!RegisterClassEx(&WndClass)) {
		MessageBox(0, L"Could not register window class...", L"Error!", MB_ICONSTOP | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_COMPOSITED,
		PLUGIN_NAME, PLUGIN_NAME, WS_OVERLAPPEDWINDOW,
		100,
		100,
		Window_Width, Window_Height, NULL, NULL, EQinfo->ghInstance, NULL);

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