#include <nds.h>
#include <algorithm>
#include <functional>
#include "ndspp.h"
#include "images.h"
#include "Arena.h"
#include "Text16.h"
#include "ChaosData.h"
#include "SpellData.h"
#include "SoundEffect.h"
#include "Graphics.h"
#include "Wizard.h"
#include "WizardData.h"
#include "Interrupt.h"
#include "Line.h"
#include "Misc.h"
#include "Options.h"


// external data
extern const unsigned short _binary_bg_raw_start[];

// static defines
static const int ARENA_SCREEN(0);
static const int ARENA_BORDER_1(2);
static const int ARENA_BORDER_2(1);
static const int ARENA_CORNER_TILE(601);
static const int ARENA_VERT_EDGE_TILE(602);
static const int ARENA_HORZ_EDGE_TILE(603);
static const int BG_CORNER_TILE(601);
static const int BG_VERT_EDGE_TILE(602);
static const int BG_HORZ_EDGE_TILE(603);
static const int BG_SOLID_TILE(604);
static const int BORDER_PALETTE(11<<12);
static const int HIGHLIGHT_INIT(9);
// the second bank of 256*256 start at tile 32*32...
static const int SECOND_TILEBANK_0(32*32);

static const signed char s_surroundTable[8][2] = {
  {-1,-1},
  {-1, 0},
  {-1, 1},
  { 0, 1},
  { 1, 1},
  { 1, 0}, 
  { 1,-1},
  { 0,-1}
};

// attack preference array...
// taken from c6d4
// except troll, which is new
static const char s_attackPref[48] = {
  0x08/*cobra*/,    0x0b/*wolf*/,    0x07/*goblin*/,    0x09/*croc*/,
  0x09/*troll*/,    0x09/*faun*/,    0x0f/*lion*/,      0x0e/*elf*/,
  0x06/*orc*/,      0x0e/*bear*/,    0x0b/*goril*/,     0x0a/*ogre*/,
  0x0d/*hydra*/,    0x09/*rat*/,     0x10/*giant*/,     0x0b/*horse*/,
  0x0f/*unicor*/,   0x0f/*cent*/,    0x0f/*peg*/,       0x10/*gryph*/,
  0x15/*manti*/,    0x0f/*bat*/,     0x19/*green*/,     0x19/*red*/,
  0x1b/*gold*/,     0x11/*harpy*/,   0x12/*eagle*/,     0x13/*vamp*/,
  0x0d/*ghost*/,    0x09/*spectre*/, 0x0b/*wraith*/,    0x07/*skeleton*/,
  0x04/*zombie*/,   0x04/*blob*/,    0x04/*fire*/,      0x00/*wood*/,
  0x01/*shadow*/,   0x00/*castle*/,  0x00/*cit*/,       0x00/*wall*/,
  /*wizards*/
  0x09,0x09,0x09,0x09,0x09,0x09,0x09,0x09,};
  


static const int X_LIMIT(31);
const int Arena::HEIGHT(23);
const int Arena::WIZARD_INDEX(0x2A);
const int Arena::ARENA_SIZE(0x9f);

// namespace usage
using namespace nds;

Arena::Arena():m_bg(new Background(ARENA_SCREEN,0,0,28)),
  m_cursor(new Sprite(0, 16, 16, 0, 256)),
  m_playerCount(0), m_roundNumber(0),m_highlightCreations(HIGHLIGHT_INIT)
{
  reset();
  m_bg->enable();
}
Arena::~Arena()
{
  delete m_bg;
}

Arena & Arena::instance()
{
  static Arena s_instance;
  return s_instance;
}

void Arena::reset()
{
  unsigned short * mapData = m_bg->mapData();
  for (int x = 0; x < 32; x++) {
    for (int y = 0; y < 32; y++) {
      mapData[x+y*32] = 0;
    }
  }
  for (int i=1, y=1; y < 21; y++) {
    for (int x=1; x < 31; x++) {
      mapData[x+y*32] = i++;
    }
  }
}

void Arena::clear()
{
  for (int x = 0; x < 15; x++){
    for (int y = 0; y < 10; y++){
      this->clearSquare(x,y);
    }
  }
}

void Arena::clearSquare(int x, int y) {
  // clears a square in the arena
  unsigned int startOAM = 16+x*2*16+2*y*30*16;
  // FIXME: use DMA
  u16 * tileData = m_bg->tileData();
  
  for (int i = 0; i < 32; i++)
  {
    tileData[startOAM+i] = 0;
  }
  startOAM = 16+x*2*16+(2*y+1)*30*16; // x*2*16+(2*y+1)*30*16;
  for (int i = 0; i < 32; i++)
  {
    tileData[startOAM+i] = 0;
  }
  /*
  REG_DMA3SAD = (u32)&zero; // 16 bit reads into VRAM
  REG_DMA3DAD = (u32)&bg_level.tileData[startOAM]; // 16 bit reads into VRAM
  REG_DMA3CNT = 32 |DMA_SOURCE_FIXED|DMA_16NOW; // 16 bit transfers from gfx array via DMA  
  
  REG_DMA3SAD = (u32)&zero; // 16 bit reads into VRAM
  REG_DMA3DAD = (u32)&bg_level.tileData[startOAM]; // 16 bit reads into VRAM
  REG_DMA3CNT = 32 |DMA_SOURCE_FIXED|DMA_16NOW; // 16 bit transfers from gfx array via DMA  
  */
}

void Arena::drawGfx8(
    const unsigned short * gfx, 
    const unsigned short * map, 
    int x, int y, int frame) {
  // draws graphics on the 8 by 8 tile boundaries
  // ie, to draw on 16*16 tiles, multiply x, y by 2
  int index = frame*4; // 4 tiles for each 16*16 gfx 
  unsigned short * tileData = m_bg->tileData();
  // each 8*8 tile takes up 32 bytes.
  unsigned int startOAM = 16+x*16+y*30*16;
  // get start address in screen
  for (int loop = 0; loop < 2; loop++) {
    // id = <the tile id in the map file at this position>
    int id = map[index++];
    swiCopy(&(gfx[id*16]), &tileData[loop*16+startOAM],16);
  }
  startOAM = 16+x*16+(y+1)*30*16 - 32;
  for (int loop = 2; loop < 4; loop++) {
    // id = <the tile id in the map file at this position>
    int id = map[index++];
    swiCopy(&(gfx[id*16]), &tileData[loop*16+startOAM],16);
  }
}

void Arena::countdownAnim()
{
  // counts down the arena[1] values and updates the arena 2 val if needed
  // FIXME - hardcoded...
  for (int i = 0; i < ARENA_SIZE; i++) {
    if (m_arena[0][i] >= 2) {
      if ( (--m_arena[1][i]) == 0) {
        m_arena[2][i]++;
        if (m_arena[2][i] == 4) {
          m_arena[2][i] = 0;
        }
        if (m_arena[2][i] == 5) {
          m_arena[2][i]--;
        } 
        // each spell has a "speed" count
        m_arena[1][i] = anim_speed_data[m_arena[0][i]-2];
      }
    }
  }

}



void Arena::decorativeBorder(int pal, unsigned short col1,  unsigned short col2, int height) {
  
  // set the colours
  Palette palette(ARENA_SCREEN,pal);
  palette[ARENA_BORDER_1] = col1;
  palette[ARENA_BORDER_2] = col2;
  u16 * tileData = m_bg->tileData();
  u16 * mapData = m_bg->mapData();
  // And these magic numbers??
  // board size = 10*15, == 20*30 tiles
  // each tile is 32 bytes (8*8 pixels, 4 bits per pixel)
  // 20*30*32 = 19200, but have to write in u16, so divide by 2 
  dmaCopy(_binary_bg_raw_start, &tileData[16 + (19200/2)], 128);

  // top left corner
  mapData[0] = ARENA_CORNER_TILE|(pal<<12);
  // top right corner
  mapData[X_LIMIT] = ARENA_CORNER_TILE|TILE_FLIP_HORZ|(pal<<12);
  // bottom left corner
  mapData[height*32] = ARENA_CORNER_TILE|TILE_FLIP_VERT|(pal<<12);
  // bottom right corner
  mapData[X_LIMIT+height*32] = ARENA_CORNER_TILE|TILE_FLIP_HORZ|TILE_FLIP_VERT|(pal<<12);
  // now fill the sides
  // top edge
  for (int x=1; x < X_LIMIT; x++) {
    mapData[x] = ARENA_HORZ_EDGE_TILE|(pal<<12);
  }
  
  // bottom edge
  for (int x=1; x < X_LIMIT; x++) {
    mapData[x+height*32] = ARENA_HORZ_EDGE_TILE|TILE_FLIP_VERT|(pal<<12);
  }
  
  // left edge
  for (int y=1; y < height; y++) {
    mapData[y*32] = ARENA_VERT_EDGE_TILE|(pal<<12);
  }
  
  // right edge
  for (int y=1; y < height; y++) {
    mapData[X_LIMIT+y*32] = ARENA_VERT_EDGE_TILE|TILE_FLIP_HORZ|(pal<<12);
  }
  m_bg->xScroll(0);
  m_bg->yScroll(0);
  m_bg->update();
}
int Arena::players() const
{
  return m_playerCount;
}
void Arena::setPlayers(int p)
{
  m_playerCount = p;
}
int Arena::currentPlayer() const
{
  return m_currentPlayer;
}
void Arena::setCurrentPlayer(int p)
{
  m_currentPlayer = p;
}



void Arena::setPalette8(int x, int y, int palette)
{
  // 8 bit resolution, 4 square size setting...
  // mask out the old palette and set the current one
  // get the tile ids for the 4 gba tiles at the ARENA x,y
  u16 tile[4];
  // 66 comes from the fact that there are 2 rows before hand (32 *2) and 2 blocks
  // at the start of each row
  // x = 0 = 132
  int offset = 33; // 66
  tile[0] = offset+x+y*32;    tile[1] = offset+1+x+y*32;
  tile[2] = tile[0]+32;   tile[3] = tile[1]+32;
  
  u16 * mapData = m_bg->mapData();
  mapData[tile[0]] &= ~0xF000; mapData[tile[0]] |= (palette<<12)&0xF000;
  mapData[tile[1]] &= ~0xF000; mapData[tile[1]] |= (palette<<12)&0xF000;
  mapData[tile[2]] &= ~0xF000; mapData[tile[2]] |= (palette<<12)&0xF000;
  mapData[tile[3]] &= ~0xF000; mapData[tile[3]] |= (palette<<12)&0xF000;
}

void Arena::setWizardSquare(int square, int id)
{
  m_arena[0][square] = id+WIZARD_INDEX;
  m_arena[3][square] = id;
}

void Arena::gameBorder() {
  decorativeBorder(11, Color(31,0,0), Color(21,0,0), HEIGHT-2);
}

void Arena::setBorderColour(unsigned char playerid) {
  // set the border colour based on the player colour
  if (playerid < 8) 
  {
    Color playerColour(Wizard::player(playerid).colourRGB());
    Color playerColourDark( playerColour.red()-8,
                            playerColour.green()-8,
                            playerColour.blue()-8);
    Palette borderPal(0, 11);
    borderPal[1] = playerColour;
    borderPal[2] = playerColourDark;
  }
}

void Arena::getXY2(int index, int & x, int & y)
{
  x = ((index & 0xf) * 2)+1;
  y = ((index & 0xf0) / 8) + 1;
}

void Arena::getXY(int index, int & x, int & y)
{
  getXY2(index, x, y);
  x = (x/2) + 1;
  y = (y/2) + 1;
}

void Arena::currentPlayerXY(int & x, int &y)
{
  getXY(m_wizardIndex, x, y);
  x -= 1;
  y -= 1;
}

void Arena::drawCreature(int x, int y, int creature, int frame)
{
  const SpellData & spell(s_spellData[creature]);
  setPalette8(x*2,y*2,spell.palette);
  drawGfx8(spell.gfx, spell.map, x*2, y*2, frame);
}

void Arena::drawCreatures(void)
{
  if (m_highlightCreations < HIGHLIGHT_INIT) {
    m_highlightCreationsCounter--;
    if (m_highlightCreationsCounter > 0) {
      highlightCreatures(m_highlightCreations);
      return;
    }
    else {
      m_highlightCreationsCounter = HIGHLIGHT_INIT;
    }
  }
  for (int i = 0; i < ARENA_SIZE; i++) {
    int x,y;
    getXY(i, x, y);
    if (m_arena[0][i] >= 2) {
      
      if (m_arena[0][i] < WIZARD_INDEX) {
        int creature = m_arena[0][i];
        int frame = m_arena[2][i];
        drawCreature(x-1,y-1,creature,frame);
      } else {
        int playerIndex = m_arena[0][i]-WIZARD_INDEX;
        int frame = m_arena[2][i];
        Wizard::player(playerIndex).draw(x-1,y-1, frame);
      }
    } 
    else {
      if (x > 0 and x < 0x10 and y > 0 and y < 0xB) {
        clearSquare(x-1,y-1);
      }
    }
    
  }
}
void Arena::display() {
  // redraw the arena...
  Text16::instance().clear();
  clear();
  Graphics::instance().clearPalettes();
  Graphics::loadAllPalettes();
  m_bg->xScroll(0);
  m_bg->yScroll(0);
  m_bg->update();
  reset();
  gameBorder();
  // draw all the creatures in the arena array
  drawCreatures();
  Wizard * players = Wizard::players();
  using std::for_each;
  using std::mem_fun_ref;
  for_each(players, players+m_playerCount, mem_fun_ref(&Wizard::updateColour));
}

// map Cursor_t to cursor gfx
static const unsigned short * const s_cursorGfx[][2] = {
  {_binary_cursor_spell_raw_start,  _binary_cursor_spell_map_start },
  {_binary_cursor_engaged_raw_start,_binary_cursor_engaged_map_start },
  {_binary_cursor_fire_raw_start,   _binary_cursor_fire_map_start },
  {_binary_cursor_fly_raw_start,    _binary_cursor_fly_map_start },
  {_binary_cursor_ground_raw_start, _binary_cursor_ground_map_start },
  {_binary_cursor_raw_start,        _binary_cursor_map_start }
};

void Arena::initialiseCursor(int x, int y, Cursor_t type)
{
  if (type != CURSOR_NOT_DRAWN) {
    drawCursor(type);
    m_cursor->enable();
    m_cursor->update();
  } else {
    m_cursor->enable(false);
  }
  setCursor(x, y);
}

void Arena::setCursor(int x, int y)
{
  m_cursorPosition.x = x;
  m_cursorPosition.y = y;
  m_targetIndex = x+y*16;
  m_cursor->setXY(8+m_cursorPosition.x*16, 8+m_cursorPosition.y*16);
  m_cursor->update();
  displayCursorContents();
}

void Arena::displayCursorContents()
{
  // dispay the creature name, etc in the right colours...
  // taken from bd18 onwards
  Text16 & text16(Text16::instance());
  text16.clearMessage();
  int creature (m_arena[0][m_targetIndex]);
  
  if (creature == 0) 
    return;
  
  // light blue
  text16.setColour(12, Color(0,28,31)); 

  int x(Text16::MESSAGE_X);
  if (creature >= WIZARD_INDEX) {
    // wizard... write wiz name
    Wizard & player(Wizard::player(creature - WIZARD_INDEX));
    player.printNameAt(x, Text16::MESSAGE_Y, 12);
    x += strlen(player.name());

  } else {
    // creature... print name and owner
    const SpellData & spellData(s_spellData[creature]);
    text16.print(spellData.spellName, x, Text16::MESSAGE_Y, 12);
    x += strlen(spellData.spellName);
  }
  
  // check underneath
  Color c(0,0,0);
  if (m_arena[4][m_targetIndex] != 0) {
    // underneath colour 47 (white)
    c = Color(30,30,30);
  } else if (m_arena[5][m_targetIndex] != 0) {
    // purple
    c = Color(27,4,28);
  }
  text16.setColour(14, c);

  char str[30];
  // the character # is actually a \ in my character array as I don't have many entries.
  str[0] = '\\';
  str[1] = 0;
  text16.print(str, x, Text16::MESSAGE_Y, 14);
  x++;
  
  if (creature < WIZARD_INDEX) {
    // print the creature owner or status...
    // yellow text by default
    Color c(30,30,0);
    if (m_arena[2][m_targetIndex] == 4) {
      // dead - green
      c = Color(0,30,0);
      strcpy(str, "(DEAD)");
    } else {
      str[0] = '(';
      str[1] = 0;
      strcat(str, Wizard::player(owner(m_targetIndex)).name() );
      strcat(str, ")");
    }
    text16.setColour(13, c);
    text16.print(str, x, Text16::MESSAGE_Y, 13);
  }
}

void Arena::drawCursor(Cursor_t type)
{
  if (type != CURSOR_NOT_DRAWN) {
    m_cursor->loadTileMapData(
        s_cursorGfx[type][0],
        s_cursorGfx[type][1],
        4);
  }
}

void Arena::enableCursor(bool enable)
{
  m_cursor->enable(enable);
  m_cursor->update();
}

void Arena::cursorUp()
{
  if (m_cursorPosition.y)
    setCursor(m_cursorPosition.x,m_cursorPosition.y-1);
}
void Arena::cursorDown()
{
    if (m_cursorPosition.y<9)
      setCursor(m_cursorPosition.x,m_cursorPosition.y+1);
}
void Arena::cursorLeft()
{
  if (m_cursorPosition.x)
    setCursor(m_cursorPosition.x-1,m_cursorPosition.y);
}
void Arena::cursorRight()
{
  if (m_cursorPosition.x<14)
    setCursor(m_cursorPosition.x+1,m_cursorPosition.y);
}

int Arena::cursorContents() const
{
  return m_arena[0][m_cursorPosition.x+m_cursorPosition.y*16];
}
void Arena::cursorContents(int & theCreature, int & theOneUnderneath, int & theFlags) const
{
  int index(m_cursorPosition.x+m_cursorPosition.y*16);
  theCreature      = m_arena[0][index];
  theOneUnderneath = m_arena[4][index];
  theFlags         = m_arena[3][index];
}
void Arena::cursorContents(int & theCreature, int & theOneUnderneath, int & theFlags, int & theFrame) const
{

  int index(m_cursorPosition.x+m_cursorPosition.y*16);
  theCreature      = m_arena[0][index];
  theOneUnderneath = m_arena[4][index];
  theFlags         = m_arena[3][index];
  theFrame         = m_arena[2][index];

}

void Arena::resetAnimFrames(void) {
  // initialises the arena tables - code based on that at c0dd
  Interrupt::disable();
  for (int i = 0; i < 0xA0; i++) {
    if ((i + 1) & 0xF) {
      if (m_arena[0][i] == 0) {
        // arena[0][i] = 0; // set to one in the actual game...
        m_arena[2][i] = 0;        
      }      
    }
    m_arena[1][i] = 1;
  }
  Interrupt::enable();  
}

void Arena::setSpellAt(int x, int y, int spellID)
{
  m_arena[0][x+y*16] = spellID;
}

int Arena::roundNumber() const
{
  return m_roundNumber;
}


// code from 97d1 - sets the index of the current player for spell casting
void Arena::setCurrentPlayerIndex() {
  for (int i = 0; i < ARENA_SIZE; i++) {
    if (m_arena[0][i] - WIZARD_INDEX == m_currentPlayer) {
        setCurrentIndex(i);
        return;         
    }
    if (m_arena[4][i] - WIZARD_INDEX == m_currentPlayer) {
      setCurrentIndex(i);
      return;
    }
  }
}

void Arena::setCurrentIndex(int index) {
  m_wizardIndex = index;
  m_startIndex = index;
  m_targetIndex = index;
}

int Arena::distance(int square1, int square2) {
  // based on code at 9786 and 0xbeef
  int x1, y1, x2, y2;
  getXY(square1, x1, y1);
  getXY(square2, x2, y2);
  
  // calculate 2*(larger of xposdiff and yposdiff) + (the smaller of the 2)
  int xdiff = abs(x2-x1);
  int ydiff = abs(y2-y1);
  if (xdiff < ydiff) 
  {
    return xdiff + (ydiff * 2);
  } 
  return ydiff + (xdiff * 2);
}

bool Arena::isSpellInRange(int selectedSpellId) const
{
  int dist= distance(m_wizardIndex, m_targetIndex);
  int range = s_spellData[selectedSpellId].castRange;
  if (range >= dist) {
    return 1;
  }
  return 0;
}


int Arena::applyPositionModifier(int square, int index) {
  // use the look up table to convert the square to a new one 
  int y, x;
  getXY(square,x,y);
  
  y = y + s_surroundTable[index][0];
  if (y == 0 or y == 0xB) {
    return 0;
  }
  
  x = x + s_surroundTable[index][1];
  if (x == 0 or x == 0x10) {
    return 0;
  }
  
  y--;
  
  int tmp = x + y*16;
  
  // Add the "3 trees in top corner" bug...
  if (int(Options::instance().option(Options::OLD_BUGS)) != 0
      and tmp == 2
      and Wizard::currentPlayer().selectedSpellId() == SPELL_MAGIC_WOOD)
  {
    tmp = 0;
  } 
  return tmp;
  
}

bool Arena::isTreeAdjacent(int spellId) const
{
  if (spellId < SPELL_MAGIC_WOOD or spellId >= SPELL_MAGIC_CASTLE)
    return false;
  
  bool found(false);
  for (int i = 0; i < 8; i++) {
    int lookAt = applyPositionModifier(m_targetIndex, i);
    if (lookAt == 0) {
      // out of bounds
      continue;
    }
    lookAt--;
    if (m_arena[0][lookAt] < SPELL_MAGIC_WOOD 
        or m_arena[0][lookAt] >= SPELL_MAGIC_CASTLE) {
      // not a wood
      continue;
    } else {
      found = true;
    }
  }
  return found;
}

// 9c0f code - check that the casting wizard isn't next to the wall target square
bool Arena::isWallAdjacent(int spellId, int wizardId) const
{
  if (spellId != SPELL_WALL)
    return false;
  // Factorise this into isTreeAdjacent??
  bool found(false);
  for (int i = 0; i < 8; i++) {
    int lookAt = applyPositionModifier(m_targetIndex, i);
    if (lookAt == 0) {
      // out of bounds
      continue;
    }
    lookAt--;
    if (m_arena[0][lookAt] == (WIZARD_INDEX + wizardId)) {
      // casting wizard is adjacent
      found = true;
    }
  }
  return found;
}

void Arena::targetXY(int & x, int & y)
{
  getXY(m_targetIndex, x, y);
}

bool Arena::isBlockedLOS() const
{
  return Line::doLine(Line::SIGHT);
}

void Arena::drawPopFrame(int x, int y, int frame)
{
  setPalette8(x*2, y*2, 8);
  drawGfx8(_binary_disappear_raw_start, _binary_disappear_map_start, x*2, y*2, frame);
}

void Arena::drawSpellcastFrame(int x, int y, int frame)
{
  setPalette8(x*2, y*2, 0);
  drawGfx8(_binary_spell_raw_start, _binary_spell_map_start, x*2, y*2, frame);
}

void Arena::creatureSpellSucceeds() 
{
  Wizard & player(Wizard::currentPlayer());
  int currentSpellId(player.selectedSpellId());
  if (!(currentSpellId == SPELL_GOOEY_BLOB 
        or currentSpellId == SPELL_MAGIC_FIRE)) 
  {
    // whatever is in this square, place in arena 5...
    m_arena[5][m_targetIndex] = m_arena[0][m_targetIndex];
  }

  m_arena[0][m_targetIndex] = currentSpellId;
  m_arena[1][m_targetIndex] = 1;
  m_arena[2][m_targetIndex] = 0;
  m_arena[3][m_targetIndex] = currentPlayer();
  if (player.illusionCast()) {
    // set bit 4 - illusion
    m_arena[3][m_targetIndex] |= 0x10;
  }
}

int Arena::wizardId(int index) const
{
  // check i = 0 and i = 4
  for (int i = 0; i <= 4; i+= 4) {
    if (m_arena[i][index] >= WIZARD_INDEX) {
      return m_arena[i][index]-WIZARD_INDEX;
    }
  }
  return -1;
}

int Arena::wizardIndex(int id) const
{
  int wizardId(id+WIZARD_INDEX);
  for (int i = 0; i < ARENA_SIZE; i++) {
    if ( (m_arena[0][i] == wizardId) 
        or (m_arena[4][i] == wizardId) ) 
    {
      return i;
    }
  }
  return -1;
}

int Arena::owner(int index) const
{
  return m_arena[3][index] & 7; 
}

// check the square in index to see if it contains a live enemy creature
// based on code at c67a
int Arena::containsEnemy(int index)
{
  int creature = m_arena[0][index];
  if (creature < 2) {
    return 0; // jr c6d2 - was if creature == 0 jump, but that is buggy
  }
  
  if (m_arena[2][index] == 4) {
    // frame 4 showing, i.e. dead
    return 0; // jr c6d2
  }
  
  if (owner(index) == m_currentPlayer) {
    return 0; // jr c6d2
  }
  
  // got to here? contains a living enemy creature....
  // get the "creature attack preference" value
  int range = distance(index, m_startIndex);
  int pref = s_attackPref[creature-2] + 4;

  // pref += var_cc55; // no idea what is stored here... 0x60 for lightning
  // priority_offset .... a big fat global :-/
  // usually 0, but can be other values when we care about all creatures
  pref += Wizard::currentPlayer().priorityOffset();
  if ( (pref - range) < 0) {
    return 0;
  } 
  
  return (pref - range);
}
int Arena::attackPref(int c) const
{
  return s_attackPref[c];
}
bool Arena::isUndead(int index) const
{
  return ( (m_arena[3][index]&(1<<6)) or 
      (m_arena[0][index] >= SPELL_VAMPIRE and m_arena[0][index] <= SPELL_ZOMBIE ) )   ;
}
bool Arena::hasDisbelieveCast(int index) const
{
  return m_arena[3][index] & 0x20;
}

void Arena::nextRound()
{
  unsetMovedFlags();
  m_roundNumber++;
  Text16::instance().clearMessage();
  m_currentPlayer = 0;
  spreadFireBlob();
  destroyCastles();
  randomNewSpell();
}

// called at the end of the moves round
// resets the movement flags so that the creatures can move next time
void Arena::unsetMovedFlags()
{
  for (int i = 0; i < 0x9f; i++) {
    m_arena[3][i] &= 0x7F;  // unset bit 7
  }
}

void Arena::spreadFireBlob()
{}
void Arena::destroyCastles()
{}
void Arena::randomNewSpell()
{}

void Arena::highlightCreatures(int playerId)
{
  for (int i = 0; i < ARENA_SIZE; i++) {
    if (m_arena[0][i] == 0) 
      continue;
    
    if (m_arena[0][i] >= WIZARD_INDEX) {
      // wizard
      if ( (m_arena[0][i] - WIZARD_INDEX) == playerId) {
        // highlight the player
        Wizard & player(Wizard::player(playerId));
        drawSilhouetteGfx(i, 
          s_wizardData[player.image()].gfx,
          s_wizardData[player.image()].map,
          player.colourRGB(), 11, 1); 
      }
      
    }
    else if (m_arena[2][i] != 4) {
      // creature, not dead
      if (owner(i) == playerId)
      {
        const SpellData & spell(s_spellData[m_arena[0][i]]);
        int col = spell.mainColour(m_arena[2][i]);

        drawSilhouetteGfx(
          i, 
          spell.gfx, 
          spell.map, 
      	  col, -1,
      	  1);
      }
      
    }
  }
  
}

// draw a silhoutte style gfx thing at the given location
// used for the justice cast
// passing in 1 for negative draws in the reversed "hilighted" style
void Arena::drawSilhouetteGfx(
    int arenaIndex, 
    const unsigned short * gfx, 
    const unsigned short * map,
    unsigned short col, 
    int palette,
    bool negative)
{
  int x,y;
  getXY(arenaIndex, x, y);
  x--;
  y--;
  
  u8 newGfx[128];
  
  int id;
  // set the frame...
  if (palette == -1) {
    palette = s_spellData[m_arena[0][arenaIndex]].palette;
  }
  
  setPalette8(x*2,y*2,palette);
  if (palette == 11) {
    Palette p(0,11);
    p[15] = col;
    col = 15;
  }
  // need to load it in blocks of 16 for 4bpp gfx
  // copy the actual graphics into the temp array
  int index = m_arena[2][arenaIndex]*4;
  for (int loop = 0; loop < 4; loop++) {
    // id = <the tile id in the map file at this position>
    id = map[index+loop];
    swiCopy(&(gfx[id*16]), &newGfx[loop*32], 16);
  }
  
  // In newGfx, replace non zero pixel values with the silhoutte value 15
  // Use palette 11 - the border palette - 15th entry was not used there
  
  int lowMask =  col&0xf;
  int hiMask =  (col&0xf)<<4;
  
  // loop over all pixels (16*16)/2 since 2 pixels per byte
  for (int loop = 0; loop < 128; loop++) {
    u8 thisVal = newGfx[loop];
    // 4 bits per pixel...
    // mask the lower 4 bits
    if (not negative) {
      // draw solid withno pixel detail
      if ((thisVal & 0x0f) != 0) {
        thisVal |= lowMask;
      }
    } else {
      // draw inversed
      if ((thisVal & 0x0f) == 0) {
        thisVal |= lowMask;
      }else if ((thisVal & 0x0f) != 0) {
        thisVal &= 0xf0;
      }
    }
    
    
    // mask the higher 4 bits
    if (not negative) {
      // draw solid withno pixel detail
      if ((thisVal & 0xf0) != 0) {
        thisVal |= hiMask;
      }
    } else {
      // draw inversed
      if ((thisVal & 0xf0) == 0) {
        thisVal |= hiMask;
      } else if ((thisVal & 0xf0) != 0) {
        thisVal &= 0x0f;
      }
    }
    
    newGfx[loop] = thisVal;
  }
  
  drawGfx8((u16*)newGfx, Wizard::DEFAULT_MAP, x*2, y*2, 0);

}


void Arena::highlightTargetCreations() 
{
  // get the owner of the creature
  // make sure dead ones don't count...
  int arena0 = m_arena[0][m_targetIndex];
  int arena2 = m_arena[2][m_targetIndex];
  if (arena0 != 0 and arena2 != 4)
  {
    enableCursor(false);
    m_highlightCreations = HIGHLIGHT_INIT;
    int playerid(HIGHLIGHT_INIT);
    if (arena0 >= WIZARD_INDEX) {
      playerid = arena0 - WIZARD_INDEX;
    }
    else if (arena2 != 4) {
      playerid = owner(m_targetIndex);
    }
    if (playerid == HIGHLIGHT_INIT)
      return;
    
    highlightCreatures(playerid);
    m_highlightCreations = playerid;
    m_highlightCreationsCounter = HIGHLIGHT_INIT;
    char str[30];
    strcpy(str, Wizard::player(playerid).name());
    strcat(str, "'S CREATIONS");
    
    Text16::instance().clearMessage();
    // cyan
    Text16::instance().displayMessage(str, Color(30,31,0));
    
    Misc::waitForLetgo();
    m_highlightCreations = HIGHLIGHT_INIT;
    Text16::instance().clearMessage();
    enableCursor();
  }
}

bool Arena::isIllusion(int index)
{
 return m_arena[3][index]&(1<<4);
}
void Arena::setDisbelieved(int index)
{
 m_arena[3][index] |= 0x20;
}

// the nitty gritty of disbelieve
void Arena::doDisbelieve()
{
  // place the arena 4 riding wizard, or 0, in arena 0
  m_arena[0][m_targetIndex] = m_arena[4][m_targetIndex];
  m_arena[4][m_targetIndex] = 0;
  // This fixes the bug where disbelieve raises a dead creature
  if (not Options::instance().option(Options::OLD_BUGS)
      and m_arena[5][m_targetIndex] != 0
      and m_arena[0][m_targetIndex] == 0) 
  {
    // what about dead bodies?
    // but only if arena 4 was empty
    // bug fix v0.7a (disbelieve failed with old bugs turned off)
    m_arena[0][m_targetIndex] = m_arena[5][m_targetIndex]; //creature in arena 5
    m_arena[2][m_targetIndex] = 4; // dead
    m_arena[5][m_targetIndex] = 0; //clear creature in arena 5 
  }
  popAnimation();
}

void Arena::popAnimation()
{
  int x, y;
  Arena::getXY(m_targetIndex, x, y);
  SoundEffect::play(SND_SPELLSUCCESS);
  x--;
  y--;
  for (int i = 0; i < 7; i++) {
    Misc::delay(2,false);
    drawPopFrame(x, y, i);
  }
}

void Arena::setHasMoved(int index)
{
  m_arena[3][index] |= 0x80;
}
bool Arena::hasMoved(int index) const
{
  return m_arena[3][index]&0x80;
}
bool Arena::isDead(int index) const
{
  return m_arena[2][index] == 4;
}

bool Arena::doSuccessfulMove(int selectedCreature)
{
  u8 start_a3 = m_arena[3][m_startIndex];
  u8 start_a4 = m_arena[4][m_startIndex];
  
  if (selectedCreature >= WIZARD_INDEX 
      and selectedCreature != m_arena[0][m_startIndex])
  {
    // the start creature is a wizard, but the visible thing is not
    // i.e. wizard moving out of something
    start_a4 = 0;
  } 
  else 
  {
    // af88
    if (m_arena[5][m_startIndex] != 0) {
      // af93  
      m_arena[0][m_startIndex] = m_arena[5][m_startIndex]; //creature in arena 5??
      m_arena[2][m_startIndex] = 4; // dead
      m_arena[5][m_startIndex] = 0; //clear creature in arena 5 (not sure about this)
    } else {
      m_arena[0][m_startIndex] = 0; // 1 in the game...
    }
  }
  // afa3..
  m_arena[1][m_startIndex] = 1; //update anim
  m_arena[4][m_startIndex] = 0; // nothing here
  
  // sort out the target square
  
  bool wizardEnd(false);
  // continue at afc7....
  if (m_arena[0][m_targetIndex] == 0 || isDead(m_targetIndex)) {
    // afdf
    u8 old_target = m_arena[0][m_targetIndex];
    m_arena[0][m_targetIndex] = selectedCreature;
    m_arena[5][m_targetIndex] = old_target;
    m_arena[1][m_targetIndex] = 1;
    m_arena[2][m_targetIndex] = 0;
    m_arena[3][m_targetIndex] = start_a3;
    m_arena[4][m_targetIndex] = start_a4;
  } else {
    // afd7
    // something in target square and thing there is not dead
    // mustbe a wiz moving to a mount or castle
    m_arena[4][m_targetIndex] = selectedCreature;
    // jump b007...
    wizardEnd = true;
  }
  m_startIndex = m_targetIndex;
  return wizardEnd;
}


// based on be52 - get the owner of creature at x,y
void Arena::ownerAt(int x, int y, int &surround_creature) const
{
  if (y == 0 || y > 10)
    return;
  if (x == 0 || x > 15) 
    return;
  
  // based on code at bdd1
  int index = (x-1) + ( (y-1) * 16);
  
  if (m_arena[0][index] == 0) 
    return;
  if (isDead(index))
    return;
  
  int owner = 0;
  if (m_arena[0][index] >= SPELL_GOOEY_BLOB) {
    // a blob or more
    if ((m_arena[0][index] - WIZARD_INDEX) < 0)   //if it is a creature/spell from 22-28
      return;
    owner = m_arena[0][index] - WIZARD_INDEX + 1;
    if (owner == (m_currentPlayer + 1))
      return;
  } else {
    // an actual alive creature, get the owner
    owner = this->owner(index) + 1;
    if (owner == (m_currentPlayer + 1))
      return;
  }
  
  if (owner != 0) {
    surround_creature = owner;
  }
}

//! kill the creature in target_index
void Arena::killCreature()
{
  u8 arena0 = m_arena[0][m_targetIndex]; 
  u8 arena4 = m_arena[4][m_targetIndex];
  m_arena[4][m_targetIndex] = 0;
  if (arena0 == SPELL_GOOEY_BLOB) {
    // uncover whatever was underneath
    m_arena[3][m_targetIndex] = m_arena[5][m_targetIndex];
  }
  else if (Options::instance().option(Options::OLD_BUGS) == 0)
  {
    // the famous "undead wizard" bug is caused by not updating the
    // m_arena[3] flag properly
    if (arena4 >= Arena::WIZARD_INDEX) {
      m_arena[3][m_targetIndex] = arena4-Arena::WIZARD_INDEX;
    }
  }

  /* Another mini-bug here - if we kill a ridden mount, the wizard appears
   * in the square (that's correct), but the mount leaves no corpse. Needs
   * a fix?  m_arena[5] holds corpse info... */ 
  m_arena[0][m_targetIndex] = arena4;
  if ((arena0 > SPELL_MANTICORE) or isUndead(m_targetIndex) or isIllusion(m_targetIndex))
    m_arena[5][m_targetIndex] = 0;
  else
    m_arena[5][m_targetIndex] = arena0;
}

void Arena::leaveCorpse()
{
  m_arena[1][m_targetIndex] = 1;
  m_arena[2][m_targetIndex] = 4;
  Misc::delay(12);
  // do sound fx
}

void Arena::killUndead()
{
  // creature doesn't leave a corpse
  // check arena 5 val
  if (m_arena[5][m_targetIndex] == 0) {
    m_arena[0][m_targetIndex] = 0;
    m_arena[1][m_targetIndex] = 1;
  } else {
    m_arena[0][m_targetIndex] = m_arena[5][m_targetIndex];
    m_arena[1][m_targetIndex] = 1;
    m_arena[2][m_targetIndex] = 4;
    m_arena[5][m_targetIndex] = 0;
  }
  Misc::delay(6);
} 
