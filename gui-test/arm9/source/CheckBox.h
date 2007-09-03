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
#ifndef CheckBox_h_seen
#define CheckBox_h_seen

#include "ToggleButton.h"
/** A CheckBox is a button that can be selected or deselected, toggling its
 * state each time it is pressed.*/
class CheckBox : public ToggleButton
{
  public:
    /** Create an instance of this class. */
    CheckBox();

  protected:
    virtual const unsigned char * getGfxData();
    virtual const unsigned short * getPaletteData();

};
#endif
