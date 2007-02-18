#include <nds.h>
#include <stdio.h>
#include <algorithm>
#include <functional>
#include "ndspp.h"
#include "images.h"
#include "Arena.h"
#include "Text16.h"
#include "ChaosData.h"
#include "SpellData.h"
#include "Graphics.h"
#include "Wizard.h"


// external data
extern const unsigned short _binary_bg_raw_start[];

// static defines
static const int ARENA_SIZE(0x9f);
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
// the second bank of 256*256 start at tile 32*32...
static const int SECOND_TILEBANK_0(32*32);

static const int X_LIMIT(31);
const int Arena::HEIGHT(23);
const int Arena::WIZARD_INDEX(0x2A);

// namespace usage
using namespace nds;

Arena::Arena():m_bg(new Background(ARENA_SCREEN,0,0,28)),
  m_cursor(new Sprite(0, 16, 16, 0, 256)),
  m_playerCount(0)
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
  int i;   
  // FIXME - hardcoded...
  for (i = 0; i < ARENA_SIZE; i++) {
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
void Arena::players(int p)
{
  m_playerCount = p;
}
int Arena::currentPlayer() const
{
  return m_currentPlayer;
}
void Arena::currentPlayer(int p)
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
#if 0
  u16 * mapData = m_bg->mapData();
  u16 * tileData = m_bg->tileData();
  // draws the game border, which is tiles in bg
  // place the tiles at the end of the creature data, i.e. 150 creatures, 4 tiles each = 600
  // use tile 600 to 607 = 0x4b00 19200 to 19392 (= 32 bytes per tile
  dmaCopy(_binary_bg_raw_start, &tileData[16 + (19200/2)], 128);
  
  // top left corner
  mapData[0] = BG_CORNER_TILE|BORDER_PALETTE;
  // top right corner
  mapData[SECOND_TILEBANK_0+1] = BG_CORNER_TILE|TILE_FLIP_HORZ|BORDER_PALETTE;
  // bottom left corner
  mapData[23*32] = BG_CORNER_TILE|TILE_FLIP_VERT|BORDER_PALETTE;
  // bottom right corner
  mapData[SECOND_TILEBANK_0+1+23*32] = BG_CORNER_TILE|TILE_FLIP_HORZ|TILE_FLIP_VERT|BORDER_PALETTE;
  
  // now fill in the sides
  // top / bottom edges
  for (int x=1; x < 32; x++) {
    // top
    mapData[x] = BG_HORZ_EDGE_TILE|BORDER_PALETTE;
    mapData[x+32] = BG_SOLID_TILE|BORDER_PALETTE;
    // bottom edge
    mapData[x+23*32] = BG_HORZ_EDGE_TILE|TILE_FLIP_VERT|BORDER_PALETTE;
    mapData[x+22*32] = BG_SOLID_TILE|BORDER_PALETTE;
  }
  // left over top edge tile...
  mapData[SECOND_TILEBANK_0+0] = BG_HORZ_EDGE_TILE|BORDER_PALETTE;
  // left over bottom edge tile...
  mapData[SECOND_TILEBANK_0+23*32] = BG_HORZ_EDGE_TILE|TILE_FLIP_VERT|BORDER_PALETTE;
  
  // left and right edges
  for (int y=1; y < 23; y++) {
    // left
    mapData[y*32] = BG_VERT_EDGE_TILE|BORDER_PALETTE;
    mapData[1+y*32] = BG_SOLID_TILE|BORDER_PALETTE;
    
    // right edge
    mapData[SECOND_TILEBANK_0+1+y*32] = BG_VERT_EDGE_TILE|TILE_FLIP_HORZ|BORDER_PALETTE;
    mapData[SECOND_TILEBANK_0+y*32] = BG_SOLID_TILE|BORDER_PALETTE;
    
  }
#endif
}

void Arena::setBorderColour(unsigned char playerid) {
  // set the border colour based on the player colour
  if (playerid < 8) 
  {
    Color playerColour(Wizard::getPlayers()[playerid].getColour());
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

void Arena::drawCreature(int x, int y, int creature, int frame)
{
  const SpellData & spell(s_spellData[creature]);
  setPalette8(x*2,y*2,spell.palette);
  drawGfx8(spell.gfxData, spell.mapData, x*2, y*2, frame);
}

void Arena::drawCreatures(void) {
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
        iprintf("Draw %d @ %d %d\n", playerIndex, x, y);
        Wizard::getPlayers()[playerIndex].draw(x-1,y-1, frame);
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
  Wizard * players = Wizard::getPlayers();
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
    m_cursor->loadTileMapData(s_cursorGfx[type][0],
        s_cursorGfx[type][1],
        4);
    m_cursor->enable();
  } else {
    m_cursor->enable(false);
  }
  setCursor(x, y);
}

void Arena::setCursor(int x, int y)
{
  m_cursorPosition.x = x;
  m_cursorPosition.y = y;
  m_cursor->setXY(8+m_cursorPosition.x*16, 8+m_cursorPosition.y*16);
  m_cursor->update();
}

void Arena::removeCursor()
{
  m_cursor->enable(false);
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

int Arena::getCursorContents() const
{
  return m_arena[0][m_cursorPosition.x+m_cursorPosition.y*16];
}
void Arena::getCursorContents(int & theCreature, int & theOneUnderneath, int & theFlags) const
{
  int index(m_cursorPosition.x+m_cursorPosition.y*16);
  theCreature      = m_arena[0][index];
  theOneUnderneath = m_arena[4][index];
  theFlags         = m_arena[3][index];
}
