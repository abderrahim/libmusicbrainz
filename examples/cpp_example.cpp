/* ----------------------------------------------------------------------

   MusicBrainz -- The Internet music metadatabase

   Copyright (C) 2000 Robert Kaye
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

     $Id$

----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "musicbrainz/musicbrainz.h"
using namespace std;

int main(void)
{
    MusicBrainz o;
    string      error, data;
    bool        ret;
    int         numTracks, trackNum;
 
    // Set the proper server to use. Defaults to mm.musicbrainz.org:80
    if (getenv("MB_SERVER"))
    {
        string server(getenv("MB_SERVER"));
        o.SetServer(server, 80);
    }

    // If you need to use a proxy, uncomment/edit the following lines
    // as appropriate
    //o.SetProxy("proxy.server.goes.here", 80);
    //o.SetProxyCreds("username", "password");
    
    int numEncodings = o.GetNumAvailableEncodings();
    string encoding;
    if (o.GetCurrentEncoding(encoding) == false) {
        o.GetQueryError(error);
        printf(error.c_str());
        return 0;
    }
    printf("Number of Encodings Available: %d\n", numEncodings);
    printf("Current Encoding: %s\n", encoding.c_str());

    // Execute the GetCDInfo query, which pulls the TOC from the
    // audio CD in the cd-rom drive, calculates the disk id and
    // requests the data from the server
    ret = o.Query(string(MBQ_GetCDInfo));
    if (!ret)
    {
         o.GetQueryError(error);
         printf("Query failed: %s\n", error.c_str());
         return 0;
    }

    // Check to see how many items were returned from the server
    if (o.DataInt(MBE_GetNumAlbums) < 1)
    {
        printf("This CD was not found.\n");
        return 0;
    }

    // Select the first album
    o.Select(MBS_SelectAlbum, 1);

    // Get the number of tracks
    numTracks = o.DataInt(MBE_AlbumGetNumTracks);
    printf(" NumTracks: %d\n", numTracks);

    // Now get and print the title of the cd
    printf("Album Name: '%s'\n", o.Data(MBE_AlbumGetAlbumName).c_str());
    o.GetIDFromURL(o.Data(MBE_AlbumGetAlbumId), data);
    printf("   AlbumId: '%s'\n\n", data.c_str());

    for(int i = 1; i <= numTracks; i++)
    {
        // Print out the artist and then print the title of the tracks
        printf("    Artist: '%s'\n", o.Data(MBE_AlbumGetArtistName, i).c_str());

        trackNum = o.DataInt(MBE_AlbumGetTrackNum, i);
        printf("  Track %2d: '%s'\n", 
            trackNum, o.Data(MBE_AlbumGetTrackName, i).c_str());
        o.GetIDFromURL(o.Data(MBE_AlbumGetTrackId, i), data);
        printf("   TrackId: '%s'\n\n", data.c_str());
    }
    return 0;
}

