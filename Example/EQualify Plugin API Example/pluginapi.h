/*
Equalify Plugin API header
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

#ifndef INCLUDE_PLUGINS_H
#define INCLUDE_PLUGINS_H
#define ApiVersionNumber 7

#define F_NOCHANGE -100

enum FilterNames {
	LOWPASS,
	HIPASS,
	BANDPASS_CSG,
	BANDPASS_CZPG,
	NOTCH,
	ALLPASS,
	PEAKING,
	LOWSHELF,
	HISHELF
};

//v6
enum PluginStatus {
	P_ERR = -1,
	P_OK = 0x0001,
	P_RUNNING = 0x0002,
	P_LOADED = 0x0004,
	P_UNLOADABLE = 0x0008,
};

#ifndef TRACKINFO_LOADED
#define TRACKINFO_LOADED
struct SpotifyInfo {
	int			LocApiVersion;
	char		SpVersion[50];
	int			Playing;
	int			Shuffle;
	int			Repeat;
	int			Online;
	int			Running;
};
struct TRACKINFO {
	char		name[500];			//trackname
	char		URI[500];
	int			length;				//sec lenght of track
	char		artist[20][500];
	char		artistURI[500];
	int			coartists;
	char		album[500];
	char		albumyear[50];
	char		albumURI[500];
	char		redirect[15][50];	//not really used much anymore
	int			redirects;
	int			currplay;			//sec played
	double		vol;				//0.0 ->1.0
	int			tracktype;			//0=normal, 1=local, 3=ad
	int			IsPrivate;
	int			IsExplicit;
	char		Popularity[50];
	int			TrackNr;
	SpotifyInfo SpInfo;
};
#endif

struct s_EQ_functions {
	int			(*GetTrackInfo)(TRACKINFO *);   // This function sets in motion a large ammount of events,
	// can take some time but will return all available info about the current playing track
	void		(*Play_Pause)();
	void		(*Next_Track)();
	void		(*Prev_Track)();
	void		(*Volume_Up)();
	void		(*Volume_down)();
	//v5 below
	int			(*SetFilter)(int band, int FilterType, int freq,float q,double gain,bool QisBW); //added in api 5
	int			(*Plugin_getFilter)(int band);//added in api 5
	void		(*PlayURI)(char *URI);
};

/////////?????????????????????vv
struct BandInfo{
	int FilterType;
	int Freq;
	float Q;
	double Gain;
	bool QisBW;
};

struct FilterInfo {
	BandInfo band1;
	BandInfo band2;
	BandInfo band3;
	BandInfo band4;
	BandInfo band5;
	BandInfo band6;
	BandInfo band7;
	BandInfo band8;
	BandInfo band9;
	BandInfo band10;
};
/////////?????????????????????^^

struct s_EQinfo {
	bool EqEnabled;					//Is the eq turned on?
	bool LimiterEnabled;			//Is the eq limiter turned on?
	BOOL BassBoostEnabled;			//Is the eq bass boost turned on?

	int SampleRate;					//currently hardcoded
	int Channels;					//currently hardcoded
	int NumberOfAnalyzedSamples;
	TRACKINFO ti;
	char *ModulePath;				//full path to the plugin
	float *FloatData;				//An array of floats represending the analyzed FFT data. ( floatdata[NumberOfAnalyzedSamples] )
	HINSTANCE ghInstance;			//Handle to the Spotify instance
	HWND mHwnd;						//Handle to the Spotify window
	s_EQ_functions Funcs;			//look above ^^
	//v5 below
	FilterInfo EqFilter;
	bool isAutoLoaded;
};

//Version 4 of the api (current)
struct PluginsInfo {
	wchar_t name[255];				//Name of plugin, Will also be used as the windowclass and menu name
	wchar_t desc[1024];				//Not really shown anywhere but the console at the moment
	wchar_t author[255];			//You!
	int type;						//Not in use in this version
	int ApiVersion;
	int status;						//v6
};

class EQPlugin
{
public:
	virtual const int Get_Unloadable () const = 0;
	virtual void Process_Data () = 0;
	virtual int Initialize_plugin(const s_EQinfo *info) = 0;
	virtual const PluginsInfo *PluginData(void) const = 0;
};

#endif