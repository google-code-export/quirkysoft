/*
  Copyright (C) 2007 Richard Quirk

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
*/
#ifndef Toolbar_h_seen
#define Toolbar_h_seen

#include <vector>
#include "Sprite.h"
#include "ViewI.h"

class Document;
class Controller;
class View;

class Toolbar: public ViewI
{
  public:
    enum Position
    {
      TOP,
      BOTTOM,
      LEFT,
      RIGHT
    };

    /** Icon positions in the toolbar image. */
    enum ToolbarIcon
    {
      ICON_BACK = 0,
      ICON_FORWARD,
      ICON_STOP,
      ICON_REFRESH,

      ICON_BACK_DISABLE,
      ICON_FORWARD_DISABLE,
      ICON_PREFS,
      ICON_SPINNER,

      ICON_BOOKMARK,
      ICON_GO_URL,
      ICON_SEARCH,
      ICON_SAVE_AS,

      ICON_NOT_CONNECTED,
      ICON_CONNECTED,
      ICON_CONNECT_ERROR,
      ICON_HIDE_LEFT,
      ICON_ADD_BOOKMARK,

    };
    static const int TILES_PER_ICON;
    static const int TOOLBAR_X;
    static const int TOOLBAR_X_RIGHT;
    static const int TOOLBAR_X_LEFT;
    static const int TOOLBAR_SEP;

    Toolbar(Document & doc, Controller & cont, View & view, int entries);
    virtual ~Toolbar();

    bool visible() const;
    virtual void setVisible(bool visible);

    /** Callback for each frame animation. */
    virtual void tick() = 0;
    virtual void updateIcons() = 0;

    bool touch(int x, int y);


    void cyclePosition();


    void showCursor(int x, int y, int cursorid);
    void hideCursor();

    //! Implement ViewI
    virtual void notify();

    Position position() const;

  protected:
    Document & m_document;
    Controller & m_controller;
    View & m_view;
    typedef std::vector<nds::Sprite * > SpriteVector;
    SpriteVector m_sprites;

    virtual void handlePress(int i) = 0;
    virtual void layout();

  private:
    //VRAM initialisation - done just the once.
    static bool s_haveInitialised;
    static void initSpriteData(unsigned short * oamData);

    // visible - ie. is showing
    bool m_visible;
    nds::Sprite *  m_cursorSprite;

    Position m_position;


};
#endif
