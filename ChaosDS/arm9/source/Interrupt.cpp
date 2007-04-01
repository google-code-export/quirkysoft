#include "libnds.h"
#include "Interrupt.h"
#include "GameState.h"

static bool s_processFrames(true);
void Interrupt::processVblank()
{
  GameState::instance().incrFrame();
  if (s_processFrames) {
    GameState::instance().checkVblankAnimation();
  }
}
void Interrupt::enable()
{
  s_processFrames = true;
}
void Interrupt::disable()
{
  s_processFrames = false;
}
