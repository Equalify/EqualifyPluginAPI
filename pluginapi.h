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

	$Id: plugins.h 159 2013-08-25 23:46:14Z zs@dev0.net $
*/

#ifndef INCLUDE_PLUGINS_H
#define INCLUDE_PLUGINS_H	 
#define ApiVersionNumber 4

typedef struct {
	char		name[500];			// Track Name
	char		URI[500];
	int			length;				// Track length in sec.
	char		artist[20][500];
	char		artistURI[500];
	int			coartists;
	char		album[500];
	char		albumyear[50];
	char		albumURI[500];
	char		redirect[15][50];	// Alternate uri's
	int			redirects;			// How many alternate uri's for the track
	int			currplay;			// How many seconds into a song
	double		vol;				// 0.0 -> 1.0
	int 		tracktype;			// local or "spotify"
} TRACKINFO;

struct s_EQ_functions {
	int			(*GetTrackInfo)(TRACKINFO *);	// This function sets in motion a large ammount of events, 
												// can take some time but will return all available info about the current playing track
	void		(*Play_Pause)();
	void		(*Next_Track)();
	void		(*Prev_Track)();
	void		(*Volume_Up)();
	void		(*Volume_down)();
};

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
};

struct PluginsInfo {
	wchar_t name[255];				//Name of plugin, Will also be used as the windowclass and menu name
	wchar_t desc[1024];				//Not really shown anywhere but the console at the moment
	wchar_t author[255];			//You!
	int type;						//Not in use in this version
	int ApiVersion;					
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