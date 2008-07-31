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
#ifndef ImageComponent_h_seen
#define ImageComponent_h_seen

#include "Component.h"
#include "ViewI.h"
#include <string>
namespace nds {
class Image;
}
class Document;
class ImageComponent: public Component, public ViewI
{
  public:
    ImageComponent(nds::Image * image, Document * doc=0);
    void reload();
    virtual void paint(const nds::Rectangle & clip);
    virtual ~ImageComponent();
    virtual void notify();

    virtual bool stylusUp(const Stylus * stylus) { return false; }
    virtual bool stylusDownFirst(const Stylus * stylus) { return false; }
    virtual bool stylusDownRepeat(const Stylus * stylus) { return false; }
    virtual bool stylusDown(const Stylus * stylus) { return false; }
  private:
    nds::Image * m_image;
    Document * m_document;

};
#endif
