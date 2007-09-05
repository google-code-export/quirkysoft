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
#ifndef Link_h_seen
#define Link_h_seen
#include <string>
#include <list>
#include "Rectangle.h"

class Link
{
  public:
    /** Creates a link/anchor to the given address.
     * @param a the document address that the link goes to
     */
    Link(const std::string & a);

    /** Creates a color change
     * @param a the document address that the link goes to
     */
    Link(unsigned short color);

    /** Get the address of the document that this Link points to.
     * @return the document address.
     */
    std::string href() const;

    /** Set if the user has clicked here or not.
     * @param clicked true if clicked, false otherwise.
     */
    inline void setClicked(bool clicked=true);

    /** Check if the user has clicked here or not.
     * @return true if this Link has been clicked, false otherwise.
     */
    inline bool clicked() const;

    /** Set the start position in the text for this Link.
     * @param startPosition the position in the document that marks the start of the link text.
     */
    inline void setTextStart(unsigned int startPosition);

    /** Set the end position in the text for this Link.
     * @param endPosition the position in the document that marks the end of the link text.
     */
    inline void setTextEnd(unsigned int endPosition);

    /** Fetch the position in the text that the Link starts at.
     * @return the position in the document that marks the start of the text.
     */
    inline unsigned int textStart() const;
    /** Fetch the position in the text that the Link ends at.
     * @return the position in the document that marks the end of the text.
     */
    inline unsigned int textEnd() const;

    enum EventType {
      STATE_PLAIN,
      STATE_LINK,
      STATE_COLOR,
    };
    EventType eventType() const;

    inline unsigned short color() const;
    inline void setColor(unsigned short color);
  private:
    unsigned int m_textStart;
    unsigned int m_textEnd;
    unsigned short m_color;
    bool m_clicked;
    std::string m_anchor;
};

void Link::setTextStart(unsigned int startPosition)
{
  m_textStart = startPosition;
}
void Link::setTextEnd(unsigned int endPosition)
{
  m_textEnd = endPosition;
}
unsigned int Link::textStart() const
{
  return m_textStart;
}
unsigned int Link::textEnd() const
{
  return m_textEnd;
}

inline void Link::setClicked(bool clicked)
{
  m_clicked = clicked;
}

inline bool Link::clicked() const
{
  return m_clicked;
}
inline unsigned short Link::color() const
{
  return m_color;
}
void Link::setColor(unsigned short color)
{
  m_color = color;
}
#endif
