/*
  Copyright 2007 Richard Quirk
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
#ifndef Keyboard_h_seen
#define Keyboard_h_seen

#include "Component.h"
#include "ButtonListener.h"
#include "TextListener.h"
#include "UnicodeString.h"

class Button;
class EditableTextArea;
class ScrollPane;
/** Show a virtual keyboard. Accepts touch input and keypad input. */
class Keyboard : public Component, public TextListener, public ButtonListener
{
  public:

    /** Construct a keyboard.  */
    Keyboard();

    /** Call this each frame to handle keyboard input.*/
    //void handleInput();

    /** Get the result of the last string entered (after Enter is "pressed").
     * @return the last string entered.
     */
    UnicodeString result() const;

    /** Set the Component that paints the top level.
     * @param topLevel the top level component that will be hidden when the
     * keyboards is shown.
     */
    void setTopLevel(Component * topLevel);

    /** Update timer to deactivate current key selection.
     * @return true if the activation means a repaint is required.
     */
    bool tick();


    // Interface implementation.
    virtual bool touch(int x, int y);
    virtual void paint(const nds::Rectangle & clip);
    virtual void editText(TextEntryI * entry);
    virtual void pressed(ButtonI * button);

  private:
    bool m_extra;
    bool m_shift;
    bool m_capsLock;
    int  m_ticks;

    ScrollPane * m_scrollPane;
    EditableTextArea * m_textArea;
    Button * m_shiftKey;
    Button * m_capsLockKey;
    // Button * m_tabKey;
    Button * m_enterKey;
    Button * m_backspaceKey;
    // Button * m_deleteKey;
    Button * m_spaceKey;
    Button * m_extraKey;
    Button * m_ok;
    Button * m_cancel;
    Button * m_clearKey;

    enum SpecialKey
    {
      SPKY_SHIFT,
      SPKY_CAPS,
      SPKY_ENTER,
      SPKY_BACKSPACE,
      SPKY_SPACE,
      SPKY_EXTRA,
      SPKY_OK,
      SPKY_CANCEL,
      SPKY_CLEAR,
      SPKY_UNKNOWN
    };
    SpecialKey buttonToSpecialKey(const ButtonI * button);

    UnicodeString m_initialText;
    Component * m_topLevel;
    TextEntryI * m_entry;

    void initUI();

    void createRow(int x, int y, const char * text, int keys);
    void createSpecialKey(int x, int y, int w, int h,
                          const UnicodeString & text, Button * button);
    void updateRow(const char * newText, int keys, int offset);
    void updateModifierKeys();
    void updateLayout(const char * text, const char * numbers);
    void updateTicksForUI(ButtonI * button);
    void appendText(const UnicodeString & text);
    void layoutViewer();
    void applyResult();

    /** Get the current multi line state.
     * @return true if multi line entries are allowed, false otherwise.
     */
    inline bool multiLine() const;

};
#endif
