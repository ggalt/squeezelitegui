/***************************************************************************
 *   Copyright (C) 2010 by George Galt                                     *
 *   george at galtfamily dot net                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *
 *  Many thanks to Richard Titmuss who developed the SlimProtoLib library  *
 *  SoftSqueeze and squeezeslave, from which much of the code here is      *
 *  stolen.  For more information on SoftSqueeze and squeezeslave, see     *
 *  Richard's site at: http://sourceforge.net/projects/softsqueeze         *
 *                                                                         *
 ***************************************************************************/

#ifndef SQUEEZEDEFINES_H
#define SQUEEZEDEFINES_H

// Qt
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QPixmap>
#include <QHash>
#include <QList>
#include <QByteArray>
#include <QNetworkReply>
#include <QtGlobal>
#include <QtDebug>

// C++
#include <iostream>

// uncomment the following to turn on debugging for a particular file
// #define SQUEEZEMAINWINDOW_DEBUG
// #define SLIMCLI_DEBUG
// #define SLIMDEVICE_DEBUG
// #define SLIMDATABASE_DEBUG
// #define SLIMSERVERINFO_DEBUG
// #define SQUEEZEDISPLAY_DEBUG
// #define SQUEEZEPICFLOW_DEBUG
// #define SLIMIMAGECACHE_DEBUG

// forward declaration of classes
class SlimCLI;
class SlimDevice;
class SlimServerInfo;
class SlimDatabaseFetch;

// Path to directories
#define DATAPATH "/.squeezelitegui/info/"
#define DATAFILE "squeezelitegui.dat"

#define DATAVERSION 1       // simply a check to change when we alter the data stored

#define LOC QString( "SqueezeLiteGUI: ")
#define LOC_WARN QString( "SqueezeLiteGUI, Warning: ")
#define LOC_ERR QString( "SqueezeLiteGUI, Error: ")

typedef enum { CLI_DISCONNECTED = 0, CLI_CONNECTED, SETUP_SERVER, SETUP_DEVICES, SETUP_IMAGES, CLI_READY } cliState;

typedef enum { NOLOGIN=101,
               CONNECTION_ERROR,
               CONNECTION_TIMEOUT,
               SETUP_NOLOGIN,
               SETUP_DATABASEERROR,
               SETUP_NODEVICES,
               SETUP_NOARTISTS,
               SETUP_NOALBUMS,
               SETUP_NOSONGS,
               SETUP_NOYEARS,
               SETUP_NOGENRES,
               SETUP_NOPLAYLISTS
           } cli_error_msg;

enum RepeatMode
{ REPEAT_OFF = 0,
  REPEAT_TRACK,
  REPEAT_ALL,
  MAX_REPEAT_MODES
};
enum ShuffleMode
{ SHUFFLE_OFF = 0,
  SHUFFLE_RANDOM,
  SHUFFLE_INTELLIGENT,
  SHUFFLE_ALBUM,
  SHUFFLE_ARTIST,
  MAX_SHUFFLE_MODES
};

enum ResumeMode
{ RESUME_OFF,
  RESUME_TRACK,
  RESUME_EXACT,
  MAX_RESUME_MODES
};


QDataStream & operator<< (QDataStream& stream, const Album& al);
QDataStream & operator>> (QDataStream& stream, Album& al);

QDataStream & operator<< (QDataStream& stream, const Artist& art);
QDataStream & operator>> (QDataStream& stream, Artist& art);

// This mapping is for the custom remote

// Additionally, some new codes may be defined for use with programmable
// remotes

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Button names to IR code mappings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define IR_0         0x76899867
#define IR_1         0x7689f00f
#define IR_2         0x768908f7
#define IR_3         0x76898877
#define IR_4         0x768948b7
#define IR_5         0x7689c837
#define IR_6         0x768928d7
#define IR_7         0x7689a857
#define IR_8         0x76896897
#define IR_9         0x7689e817
#define IR_arrow_down         0x7689b04f
#define IR_arrow_left         0x7689906f
#define IR_arrow_right         0x7689d02f
#define IR_arrow_up         0x7689e01f
#define IR_voldown         0x768900ff
#define IR_volup         0x7689807f
#define IR_power         0x768940bf
#define IR_rew         0x7689c03f
#define IR_pause         0x768920df
#define IR_fwd         0x7689a05f
#define IR_add         0x7689609f
#define IR_play         0x768910ef
#define IR_search         0x768958a7
#define IR_shuffle         0x7689d827
#define IR_repeat         0x768938c7
#define IR_sleep         0x7689b847
#define IR_now_playing         0x76897887
#define IR_size         0x7689f807
#define IR_brightness         0x768904fb
#define IR_favorites         0x768918e7
#define IR_browse         0x7689708f
#define IR_power_on         0x76898f70
#define IR_power_off         0x76898778
#define IR_home         0x768922dd

#define IR_now_playing_2                0x7689a25d
#define IR_search_2                     0x7689629d
#define IR_favorites_2                  0x7689e21d

#define IR_menu_browse_album            0x76897c83
#define IR_menu_browse_artist           0x7689748b
#define IR_menu_browse_playlists        0x76897a85
#define IR_menu_browse_music            0x7689728d

#define IR_menu_search_artist           0x768954ab
#define IR_menu_search_album            0x76895ca3
#define IR_menu_search_song             0x768952ad

#define IR_digital_input_aes_ebu        0x768906f9
#define IR_digital_input_bnc_spdif      0x76898679
#define IR_digital_input_rca_spdif      0x768946b9
#define IR_digital_input_toslink        0x7689c639

#define IR_analog_input_line_in         0x76890ef1

#define IR_muting         0x7689c43b


#endif // SQUEEZEDEFINES_H
