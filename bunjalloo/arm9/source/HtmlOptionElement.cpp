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
#include "HtmlOptionElement.h"

const UnicodeString * HtmlOptionElement::attributePtr(const std::string & name) const
{
  const UnicodeString * p = HtmlElement::attributePtr(name);
  if (p) {
    return p;
  }

  if (name == "value")
  {
    return &m_value;
  }
  if (name == "label")
  {
    return &m_label;
  }
  return 0;
}

void HtmlOptionElement::setAttribute(const std::string & name, const UnicodeString & value)
{
  if (name == "selected")
  {
    m_selected = true;
  }
  HtmlElement::setAttribute(name, value);
}

bool HtmlOptionElement::selected() const
{
  return m_selected;
}
