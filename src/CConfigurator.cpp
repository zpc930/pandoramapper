/*
 *  Pandora MUME mapper
 *
 *  Copyright (C) 2000-2009  Azazello
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */



/* configuration reader/saver and handler */
#include <QFile>
#include <QImage>
#include <QXmlDefaultHandler>
#include <QGLWidget>
#include <QMutex>
#include <QSettings>

#include "defines.h"
#include "CRoom.h"
#include "CRoomManager.h"
#include "mainwindow.h"

#include "CConfigurator.h"
#include "utils.h"
#include "CEngine.h"
#include "CGroup.h"
//#include "renderer.h"

class Cconfigurator *conf;

Cconfigurator::Cconfigurator()
{
    /* here we set the default configuration */

    print_debug(DEBUG_CONFIG, "in configurator constructor");

    setRegionsAutoReplace( false );
    setRegionsAutoSet( false );


    /* data */
    databaseModified = false;

    resetCurrentConfig();
    setConfigModified(false);
}

void Cconfigurator::resetCurrentConfig()
{
    sectors.clear();
    spells.clear();
    moveCancelPatterns.clear();
    moveForcePatterns.clear();

    struct roomSectorsData first;

    first.pattern = 0;
    first.desc = "NONE";
    first.texture = 1;
    first.gllist = 1;
    sectors.push_back(first);
}


int Cconfigurator::saveConfigAs(QByteArray path, QByteArray filename)
{
  unsigned int i;

  // try QSettings
  QSettings conf(path + filename, QSettings::IniFormat);

  conf.beginGroup("General");
  conf.setValue("mapFile", getBaseFile());
  conf.setValue("windowRect", renderer_window->geometry() );
  conf.setValue("alwaysOnTop", getAlwaysOnTop() );
  conf.setValue("startupMode", getStartupMode() );
  conf.endGroup();

  conf.beginGroup("Networking");
  conf.setValue("localPort", getLocalPort() );
  conf.setValue("remoteHost", getRemoteHost() );
  conf.setValue("remotePort", getRemotePort() );
  conf.endGroup();

  conf.beginGroup("OpenGL");
  conf.setValue("texturesVisibility", getTextureVisibility() );
  conf.setValue("detailsVisibility", getDetailsVisibility() );
  conf.setValue("visibleLayers", getVisibleLayers() );
  conf.setValue("showNotes", getShowNotesRenderer() );
  conf.setValue("showRegions", getShowRegionsInfo() );
  conf.setValue("displayRegions", getDisplayRegionsRenderer() );
  conf.setValue("multisampling", getMultisampling() );
  conf.setValue("selectOnAnyLayer", getSelectOAnyLayer());
  conf.setValue("angleX", angleX );
  conf.setValue("angleY", angleY );
  conf.setValue("angleZ", angleZ );
  conf.setValue("userX", userX );
  conf.setValue("userY", userY );
  conf.setValue("userZ", userZ );
  conf.beginWriteArray("Textures");
  conf.setValue("noteColor", getNoteColor() );
  for (unsigned int i = 0; i < sectors.size(); ++i) {
	  conf.setArrayIndex(i);
	  conf.setValue("handle", sectors[i].desc);
	  conf.setValue("file", sectors[i].filename);
	  conf.setValue("pattern", sectors[i].pattern);

  }
  conf.endArray();
  conf.endGroup();

  conf.beginGroup("Engine");
  conf.setValue("checkExits", getExitsCheck() );
  conf.setValue("checkTerrain", getTerrainCheck() );
  conf.setValue("briefmode", getBriefMode() );
  conf.setValue("autoMerge", getAutomerge() );
  conf.setValue("angryLinker", getAngrylinker() );
  conf.setValue("dualLinker", getDuallinker() );
  conf.setValue("autoRefresh", getAutorefresh() );
  conf.setValue("roomNameQuote", getNameQuote() );
  conf.setValue("descQuote", getDescQuote() );
  conf.endGroup();

  conf.beginGroup("Patterns");
  conf.setValue("exitsPattern", getExitsPattern());
  conf.setValue("spellsEffectPattern", spells_pattern);
  conf.setValue( "scorePattern", getScorePattern() );
  conf.setValue( "scorePatternShort", getShortScorePattern() );
  conf.endGroup();



  conf.beginGroup("GroupManager");
  conf.setValue("remoteHost", getGroupManagerHost() );
  conf.setValue("remotePort", getGroupManagerRemotePort() );
  conf.setValue("localServerPort", getGroupManagerLocalPort() );
  conf.setValue("charName", getGroupManagerCharName() );
  conf.setValue("charColor", getGroupManagerColor().name() );
  conf.setValue("showSelf", getGroupManagerShowSelf() );
  conf.setValue("notifyArm", getGroupManagerNotifyArmour() );
  conf.setValue("notifySanc", getGroupManagerNotifySanc() );
  conf.setValue("showGroupManager", getGroupManagerShowManager() );

  conf.setValue("windowRect", renderer_window->getGroupManagerRect() );

  conf.endGroup();


  conf.beginWriteArray("Spells");
  for (unsigned int i = 0; i < spells.size(); ++i) {
	  conf.setArrayIndex(i);
	  conf.setValue("addon", spells[i].addon);
	  conf.setValue("name", spells[i].name);
	  conf.setValue("upMessage", spells[i].up_mes);
	  conf.setValue("refreshMessage", spells[i].refresh_mes);
	  conf.setValue("downMessage", spells[i].down_mes);
  }
  conf.endArray();



  conf.beginGroup("Movement tracking");
  conf.beginWriteArray("Cancel Patterns");
  for (int i = 0; i < moveCancelPatterns.size(); ++i) {
	  conf.setArrayIndex(i);
	  conf.setValue("pattern", moveCancelPatterns.at(i));
  }
  conf.endArray();
  conf.beginWriteArray("Force Patterns");
  for (int i = 0; i < moveForcePatterns.size(); ++i) {
	  conf.setArrayIndex(i);
	  conf.setValue("pattern", moveForcePatterns.at(i));
  }
  conf.endArray();
  conf.endGroup();


  conf.beginWriteArray("Debug Settings");
  i = 0;
  while (debug_data[i].name) {
	  conf.setArrayIndex(i);
	  conf.setValue("name", debug_data[i].name);
	  conf.setValue("state", debug_data[i].state);

      i++;
  }
  conf.endArray();

  configFile = filename;
  configPath = path;

  setConfigModified(false);
  return true;
}



int Cconfigurator::loadConfig(QByteArray path, QByteArray filename)
{
	int size;
	QSettings conf(path + filename, QSettings::IniFormat);

	conf.beginGroup("General");
	setBaseFile( conf.value("mapFile", "database/mume.xml").toByteArray() );
	setWindowRect( conf.value("windowRect").toRect() );
	setAlwaysOnTop( conf.value("alwaysOnTop", true ).toBool() );
	setStartupMode( conf.value("startupMode", 1).toInt() );
	conf.endGroup();

	conf.beginGroup("Networking");
	setLocalPort( conf.value("localPort", 4242).toInt() );
	setRemoteHost( conf.value("remoteHost", "193.134.218.111").toByteArray() );
	setRemotePort( conf.value("remotePort", 443).toInt() );
	conf.endGroup();

	conf.beginGroup("OpenGL");
	setTextureVisibility( conf.value("texturesVisibility", 500).toInt() );
	setDetailsVisibility( conf.value("detailsVisibility", 300).toInt() );
	setVisibleLayers( conf.value("visibleLayers", 5).toInt() );
	setShowNotesRenderer( conf.value("showNotes", true).toBool() );
	setShowRegionsInfo( conf.value("showRegions", false). toBool() );
	setDisplayRegionsRenderer( conf.value("displayRegions", false).toBool() );
	setMultisampling( conf.value("multisampling", true).toBool() );
	setSelectOAnyLayer( conf.value("selectOnAnyLayer", true).toBool() );

    setRendererAngles(conf.value("angleX", 0).toFloat(), conf.value("angleY", 0).toFloat(), conf.value("angleZ", 0).toFloat());
    setRendererPosition(conf.value("userX", 0).toFloat(), conf.value("userY", 0).toFloat(), conf.value("userZ", 0).toFloat());
    setNoteColor( conf.value("noteColor", "#F28003").toByteArray() );

    size = conf.beginReadArray("Textures");
	for (int i = 0; i < size; ++i) {
	  conf.setArrayIndex(i);
      addTexture(conf.value("handle").toByteArray(),  conf.value("file").toByteArray(),  (char) conf.value("pattern").toInt());
	}
	conf.endArray();
	conf.endGroup();

	conf.beginGroup("Engine");
	setExitsCheck( conf.value("checkExits", false).toBool() );
	setTerrainCheck( conf.value("checkTerrain", true).toBool() );
	setBriefMode( conf.value("briefmode", true ).toBool() );
	setAutomerge( conf.value("autoMerge", true ).toBool() );
	setAngrylinker( conf.value("angryLinker", true ).toBool() );
	setDuallinker( conf.value("dualLinker", false ).toBool() );
	setAutorefresh( conf.value("autoRefresh", true ).toBool() );
	setNameQuote( conf.value("roomNameQuote", 10 ).toInt() );
	setDescQuote( conf.value("descQuote", 10 ).toInt() );

	setRegionsAutoReplace( conf.value("regionsAutoReplace", false ).toBool() );
	setRegionsAutoSet( conf.value("regionsAutoSet", false ).toBool() );
	conf.endGroup();

    conf.beginGroup("Patterns");
    setExitsPattern( conf.value("exitsPattern", "Exits: ").toByteArray() );
    spells_pattern = conf.value("spellsEffectPattern", "Affected by:").toByteArray();
    setScorePattern( conf.value("scorePattern", "[0-9]*/* hits, */* mana, and */* moves.").toByteArray()  );
    setShortScorePattern( conf.value("scorePatternShort", "[0-9]*/* hits and */* moves.").toByteArray() );
	conf.endGroup();


	conf.beginGroup("GroupManager");
	setGroupManagerHost( conf.value("remoteHost", "localhost").toByteArray() );
	setGroupManagerRemotePort( conf.value("remotePort", 4243 ).toInt() );
	setGroupManagerLocalPort( conf.value("localServerPort", 4243 ).toInt() );
	setGroupManagerCharName( conf.value("charName", "Charname" ).toByteArray() );
	setGroupManagerColor( QColor( conf.value("charColor", "#F28003").toString() ) );
	setGroupManagerShowSelf( conf.value("showSelf", false ).toBool() );
	setGroupManagerNotifyArmour( conf.value("notifyArm", true ).toBool() );
	setGroupManagerNotifySanc( conf.value("notifySanc", true ).toBool() );
	setGroupManagerShowManager( conf.value("showGroupManager", true ).toBool() );
	setGroupManagerRect( conf.value("windowRect").toRect() );
	conf.endGroup();


	size = conf.beginReadArray("Spells");
	for (int i = 0; i < size; ++i) {
	  conf.setArrayIndex(i);
	  TSpell spell;

      spell.up = false;
      spell.silently_up = false;
	  spell.addon = conf.value("addon", 0).toInt();
	  spell.name = conf.value("name").toByteArray();
	  spell.up_mes = conf.value("upMessage").toByteArray();
	  spell.refresh_mes = conf.value("refreshMessage").toByteArray();
	  spell.down_mes = conf.value("downMessage").toByteArray();

      addSpell(spell);
	}
	conf.endArray();


	conf.beginGroup("Movement tracking");
	size = conf.beginReadArray("Cancel Patterns");
	for (int i = 0; i < size; ++i) {
	  conf.setArrayIndex(i);
	  moveCancelPatterns.append( conf.value("pattern").toByteArray() );
	}
	conf.endArray();
	size = conf.beginReadArray("Force Patterns");
	for (int i = 0; i < size; ++i) {
	  conf.setArrayIndex(i);
	  moveForcePatterns.append( conf.value("pattern").toByteArray() );
	}
	conf.endArray();
	conf.endGroup();

	size = conf.beginReadArray("Debug Settings");
	for (int i = 0; i < size; ++i) {
	  conf.setArrayIndex(i);

	  QString s = conf.value("name").toString();

	  unsigned int z = 0;
      while (debug_data[z].name != NULL) {
          if (debug_data[z].name == s)
              break;
          z++;
      }
      if (debug_data[z].name == NULL) {
          print_debug(DEBUG_CONFIG, "Warning, %s is a wrong debug descriptor/name!", qPrintable(s));
          continue;
      }

      debug_data[i].state = conf.value("state", 0 ).toInt();
	}
	conf.endArray();

	configFile = filename;
	configPath = path;

	setConfigModified(false);
	return true;
}

/* ---------------- PATTERNS and REGEXPS GENERATION --------------- */
void Cconfigurator::setExitsPattern(QByteArray str)
{
    exitsPattern = str;
    exitsExpr.setPattern(QRegExp::escape(str) );

    setConfigModified(true);
}

/* --------------------------------------- spells ----------------------------------------- */
void Cconfigurator::addSpell(QByteArray spellname, QByteArray up, QByteArray down, QByteArray refresh, bool addon)
{
    TSpell spell;

    spell.name = spellname;
    spell.up_mes = up;
    spell.down_mes = down;
    spell.refresh_mes = refresh;
    spell.addon = addon;
    spell.up = false;
    spell.silently_up = false;

    spells.push_back(spell);
    setConfigModified(true);
}

void Cconfigurator::addSpell(TSpell spell)
{
    spells.push_back(spell);
    setConfigModified(true);
}

QString Cconfigurator::spellUpFor(unsigned int p)
{
    if (p > spells.size())
        return "";


    QString s;
    int min;
    int sec;

    sec = spells[p].timer.elapsed() / (1000);
    min = sec / 60;
    sec = sec % 60;

    s = QString("- %1%2:%3%4")
            .arg( min / 10 )
            .arg( min % 10 )
            .arg( sec / 10 )
            .arg( sec % 10 );

    return s;
}

void Cconfigurator::resetSpells()
{
    for (unsigned int p = 0; p < conf->spells.size(); p++) {
    	conf->spells[p].up = false;
    	conf->spells[p].silently_up = false;
    }

}



/* ----------------- REGULAR EXPRESSIONS SECTION ---------------- */
/* ------------------- DATA ------------------- */
char Cconfigurator::getPatternByRoom(CRoom *r)
{
    return sectors.at(r->getTerrain()).pattern;
}

int Cconfigurator::getSectorByDesc(QByteArray desc)
{
    unsigned int i;
    for (i = 0; i < sectors.size(); ++i) {
        if (sectors[i].desc == desc)
            return i;
    }
    return 0;
}

GLuint Cconfigurator::getTextureByDesc(QByteArray desc)
{
    int i;
    i = getSectorByDesc(desc);
    if (i == -1)
        return 0;
    return sectors[i].texture;
}


void Cconfigurator::addTexture(QByteArray desc, QByteArray filename, char pattern)
{
    struct roomSectorsData s;

    s.desc = desc;
    s.filename = filename;
    s.pattern = pattern;

    sectors.push_back(s);
}

int Cconfigurator::getSectorByPattern(char pattern)
{
    unsigned int i;
    for (i = 0; i < sectors.size(); ++i) {
        if (sectors[i].pattern == pattern)
            return i;
    }
    return 0;
}


void Cconfigurator::setBaseFile(QByteArray str)
{
    baseFile = str;
    setConfigModified(true);
}

void Cconfigurator::setDisplayRegionsRenderer(bool b)
{
    displayRegionsRenderer = b;
    setConfigModified(true);
}

void Cconfigurator::setShowRegionsInfo(bool b)
{
    showRegionsInfo = b;
    setConfigModified(true);
}

bool Cconfigurator::getDisplayRegionsRenderer()
{
    return displayRegionsRenderer;
}

bool Cconfigurator::getShowRegionsInfo()
{
    return showRegionsInfo;
}

bool Cconfigurator::getRegionsAutoSet()
{
    return regionsAutoSet;
}

bool Cconfigurator::getRegionsAutoReplace()
{
    return regionsAutoReplace;
}

void Cconfigurator::setRegionsAutoSet(bool b)
{
    regionsAutoSet = b;
}

void Cconfigurator::setRegionsAutoReplace(bool b)
{
    regionsAutoReplace = b;
    setConfigModified(true);
}

void Cconfigurator::setShowNotesRenderer(bool b)
{
    showNotesRenderer = b;
    setConfigModified(true);
}



void Cconfigurator::setRemoteHost(QByteArray str)
{
    remoteHost = str;
    setConfigModified(true);
}

void Cconfigurator::setRemotePort(int i)
{
    remotePort = i;
    setConfigModified(true);
}

void Cconfigurator::setLocalPort(int i)
{
    localPort = i;
    setConfigModified(true);
}

void Cconfigurator::setAutorefresh(bool b)
{
    autorefresh = b;
    setConfigModified(true);
}

void Cconfigurator::setAutomerge(bool b)
{
    automerge = b;
    setConfigModified(true);
}

void Cconfigurator::setDuallinker(bool b)
{
    duallinker = b;
    setConfigModified(true);
}

bool Cconfigurator::getDuallinker()
{
    return duallinker;
}


void Cconfigurator::setAngrylinker(bool b)
{
    angrylinker = b;
    setConfigModified(true);
}

void Cconfigurator::setExitsCheck(bool b)
{
    exitsCheck = b;
//    set_conf_mod(true);       /* this option changes repeatedly when you turn */
                                /* mapping on and off */
}

void Cconfigurator::setTerrainCheck(bool b)
{
    terrainCheck = b;
    setConfigModified(true);
}

void Cconfigurator::setDetailsVisibility(int i)
{
    detailsVisibilityRange = i;
    setConfigModified(true);
}

void Cconfigurator::setTextureVisibility(int i)
{
    textureVisibilityRange = i;
    setConfigModified(true);
}

void Cconfigurator::setBriefMode(bool b)
{
    briefMode = b;
    setConfigModified(true);
}

void Cconfigurator::setAlwaysOnTop(bool b)
{
    alwaysOnTop = b;
    setConfigModified(true);
}

void Cconfigurator::setNameQuote(int i)
{
    nameQuote = i;
    setConfigModified(true);
}

void Cconfigurator::setDescQuote(int i)
{
    descQuote = i;
    setConfigModified(true);
}

void Cconfigurator::setStartupMode(int i)
{
    startupMode = i;
    setConfigModified(true);
}

int Cconfigurator::getStartupMode()
{
    return startupMode;
}

// default color
void Cconfigurator::setNoteColor(QByteArray c)
{
    noteColor = c;
    setConfigModified(true);
}

QByteArray Cconfigurator::getNoteColor()
{
    return noteColor;
}

int Cconfigurator::loadTexture(struct roomSectorsData *p)
{
    QImage tex1, buf1;

    glGenTextures(1, &p->texture);
    print_debug(DEBUG_RENDERER, "loading texture %s", (const char *) p->filename);
    if (p->filename == "")
        return -1;
    if (!buf1.load( p->filename )) {
        print_debug(DEBUG_CONFIG, "Failed to load the %s!", (const char *) p->filename);
        return -1;
    }
    tex1 = QGLWidget::convertToGLFormat( buf1 );
    glGenTextures(1, &p->texture );
    glBindTexture(GL_TEXTURE_2D, p->texture );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex1.width(), tex1.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex1.bits() );



    p->gllist = glGenLists(1);
    if (p->gllist != 0) {
        glNewList(p->gllist, GL_COMPILE);

        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, p->texture);

        glBegin(GL_QUADS);
            glTexCoord2f(0.0, 1.0);
            glVertex3f(-ROOM_SIZE,  ROOM_SIZE, 0.0f);
            glTexCoord2f(0.0, 0.0);
            glVertex3f(-ROOM_SIZE, -ROOM_SIZE, 0.0f);
            glTexCoord2f(1.0, 0.0);
            glVertex3f( ROOM_SIZE, -ROOM_SIZE, 0.0f);
            glTexCoord2f(1.0, 1.0);
            glVertex3f( ROOM_SIZE,  ROOM_SIZE, 0.0f);

        glEnd();
        glDisable(GL_TEXTURE_2D);



        glEndList();
    }
    return 1;
}

