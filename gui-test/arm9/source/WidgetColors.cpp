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
#include "libnds.h"
#include "WidgetColors.h"

#define FG      RGB5(27,27,27)
#define SHAD    RGB5(21,21,21)
#define VDARK   RGB5(14,14,14)
#define PRESSED RGB5(24,24,24)
#define BRIGHT  RGB5(31,31,31)

const unsigned short WidgetColors::BUTTON_FOREGROUND = FG;
const unsigned short WidgetColors::BUTTON_PRESSED = PRESSED;
const unsigned short WidgetColors::BUTTON_SHADOW = SHAD;
const unsigned short WidgetColors::BUTTON_SHINE = BRIGHT;

const unsigned short WidgetColors::COMBOBOX_DROP_DOWN = FG; //RGB5(26,26,26);
const unsigned short WidgetColors::COMBOBOX_FOREGROUND = FG;
const unsigned short WidgetColors::COMBOBOX_SELECTED = PRESSED;

const unsigned short WidgetColors::SCROLLBAR_BACKGROUND = SHAD;
const unsigned short WidgetColors::SCROLLBAR_BORDER = VDARK;
const unsigned short WidgetColors::SCROLLBAR_ARROW = FG;
const unsigned short WidgetColors::SCROLLBAR_ARROW_HEAD = VDARK;
const unsigned short WidgetColors::SCROLLBAR_HANDLE_NOT_HELD = FG;
const unsigned short WidgetColors::SCROLLBAR_HANDLE_HELD = BRIGHT;