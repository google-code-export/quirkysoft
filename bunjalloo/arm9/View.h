/*
  Copyright (C) 2007,2008 Richard Quirk

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
#ifndef View_h_seen
#define View_h_seen

#include <string>
#include "ViewI.h"
#include "ButtonListener.h"
#include "LinkListener.h"

class Document;
class Controller;

class FormControl;
class Keyboard;
class KeyState;
class LinkHandler;
class ScrollPane;
class SearchEntry;
class Stylus;
class TextField;
class Toolbar;
class ViewRender;

/** Handle the displaying of HTML data.*/
class View : public ViewI, public ButtonListener, public LinkListener
{
  friend class ViewRender;
  public:
    /** Create a View for the given document and controller.
     * @param doc the model.
     * @param controller the controller.
     */
    View(Document & doc, Controller & controller);

    ~View();

    void notify();

    /** Show the view, handle key events for one frame.
     */
    void tick();

    /** Implement the ButtonListener interface.
     * @param button the button that has been pressed.
     */
    virtual void pressed(ButtonI * button);

    /** Implement the LinkListener interface.
     * @param link the link that has been clicked.
     */
    virtual void linkClicked(Link * link);

    /** The user wants to enter a URL. Set up URL entering mode.
     */
    void enterUrl();

    /** User wants to bookmark a URL or show the bookmark page.
     */
    void bookmarkUrl();

    /** Add the current page to the bookmarks.
     */
    void bookmarkCurrentPage();

    /** Save the current page as something.  */
    void saveAs();

    /** Exit bookmark mode. */
    void endBookmark();

    /** Exit bookmark mode. */
    void preferences();

    /** Get the Controller.
     * @return a reference to the Controller.
     */
    inline Controller & controller() const;

    /** Get the Document.
     * @return a reference to the Document.
     */
    inline Document & document() const;
  private:

    enum InputState
    {
      BROWSE,
      ENTER_URL,
      BOOKMARK,
      SAVE_AS
    };

    Document & m_document;
    Controller & m_controller;
    Keyboard * m_keyboard;
    ViewRender * m_renderer;
    TextField * m_addressBar;
    Toolbar * m_browseToolbar;
    Toolbar * m_bookmarkToolbar;
    Toolbar * m_prefsToolbar;
    Toolbar * m_toolbar;
    ScrollPane * m_scrollPane;
    InputState m_state;
    FormControl * m_form;
    LinkHandler * m_linkHandler;
    SearchEntry * m_search;
    KeyState * m_keyState;
    int m_preInputStartLine;
    bool m_dirty;
    std::string m_linkHref;
    int m_refreshing;
    // store the title of the page to bookmark..
    std::string m_bookmarkTitleUtf8;

    void updateInput();
    void browse();
    void keyboard();

    void doEnterUrl();
    void doSaveAs();

    void showBookmarkPage();
    // extract the page title for bookmark hack
    void extractTitle();

    void setToolbar(Toolbar * toolbar);
};

Controller & View::controller() const
{
  return m_controller;
}

Document & View::document() const
{
  return m_document;
}
#endif
