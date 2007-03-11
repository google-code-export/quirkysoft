#include "WizardCPU.h"
#include "Wizard.h"
#include "Arena.h"
#include "SpellData.h"
#include "Misc.h"
#include "Casting.h"
#include "Movement.h"
#include "GameState.h"

// create the priority table - stored at d3f2
// will contain {priority, index} for all living enemy creatures
static unsigned char s_priorityTable[320];

WizardCPU::WizardCPU(Wizard & theWizard) :
  m_tableIndex(0),
  m_wizard(theWizard),
  m_targetCount(0)
{}

void WizardCPU::doAiSpell()
{
  /*
    set spell 0 priorty = 0x0c + GetRand(10) (I imagine this is in case Disbelieve was top priority?)
    order the spell list by priority
    best spell = 0
    while (best spell < spell list length) {
      select the best spell 
      select the best square for this spell
      if no square is good
        best spell ++
      else
        break
    }
    
    if a spell has been chosen...
      cast spell at chosen square
      remove casted spell from list
    
    move on to next player...
  */
  
  if (m_wizard.isDead()) { 
    return;
  }
  
  m_wizard.setCastAmount(0);
  unsigned char * spells(m_wizard.spellArray());
  int spellCount(m_wizard.spellCount());
  spells[0] = 12 + Misc::rand(10);
  Misc::orderTable(spellCount, spells);

  int bestSpell(0);
  while (bestSpell < spellCount) {
    m_wizard.setSelectedSpell(bestSpell);
    // "cast" the spell... each casting routine has the CPU AI code for that spell built into it.
    // if no good square was found, then go to the next spell
    m_targetSquareFound = false;
    int currentSpellId(m_wizard.selectedSpellId());
#if 0
    if (currentSpellId != 0)   // Correct way to do it
#endif
    if (currentSpellId > SPELL_DISBELIEVE and currentSpellId < SPELL_MAGIC_CASTLE)
    {
      m_targetSquareFound = true;
      s_spellData[currentSpellId].spellFunction();
    }
    if (m_targetSquareFound) {
      // spell was cast succesfully
      m_wizard.removeSelectedSpell();
      return;
    }
    else {
      bestSpell++;
    }
  }
}
void WizardCPU::aiCast(int castType) 
{
  switch (castType)
  {
    case CREATURE:
      aiCastCreature();
      break;
    case DISBELIEVE:
      aiCastDisbelieve();
      break;
    default:
      break;
  }
}

void WizardCPU::aiCastCreature() 
{
  // the computer tries to cast a creature..
  // code based on 9984
  
  // get the square closest to the best target...
  int strongestIndex(strongestWizard(0xFF));
  
  /* TODO - this may not be needed
  m_startIndex = m_wizardIndex;
  */
  m_targetSquareFound = false;
  int range = 3;
  int currentSpellId(m_wizard.selectedSpellId());
  if (currentSpellId >= SPELL_GOOEY_BLOB)
    range = 13;
  
  int inRange = createRangeTable(strongestIndex, range);
  
  // have a table, and an index. uses m_targetSquareFound to do stuff
  m_tableIndex = inRange*2+1;
  setFurthestInrange();
  
  if (m_targetSquareFound) {
    m_wizard.printNameSpell();
    Misc::delay(80);
    Casting::spellAnimation();
    m_wizard.setIllusionCast(false);
    
    if (currentSpellId < SPELL_GOOEY_BLOB) {
      // randomly cast illusions, but more chance of trying if the spell is tricky to cast
      int currentSpellChance(s_spellData[currentSpellId].realCastChance());
      if ( (currentSpellChance < 4 and Misc::rand(10) > 5) 
          or (Misc::rand(10) > 7) )
      {
        m_wizard.setIllusionCast(true);
      }
    }
    
    if (Casting::calculateSpellSuccess())
      Arena::instance().creatureSpellSucceeds();
    Casting::printSuccessStatus();
    
    Arena::instance().drawCreatures();
    Misc::delay(10);
  } // else no square found!
  
  m_wizard.setCastAmount(0);
}

// based on code at c78d
// additionally, pass in the attakcer - or 0xff if no attacker ready 
// and check "current spell" value
int WizardCPU::strongestWizard(int attackerIndex) {
  if (m_wizard.timid() < 10) {
    // if we have less than 10 in this value, 
    // go for the wizard who's creature is closest to us
    createTableEnemies();
    
    // the best value will now be the index of the creature closest to us
    // or the wizard closest to us, as they are treated the same in the create_table_enemies routine
    int enemy_creature = s_priorityTable[0];
    
    if (enemy_creature == 0) {
      
      // no dangerous creature found..
      // create the priority table based on "strongest wizard" - one with most/best creatures
      createTableWizards();
      Arena::instance().setTargetIndex(s_priorityTable[1]);
      
      // in the actual game it does a pointless thing here...
      return s_priorityTable[1];
      
    } else {
      // first of all, get the most dangerous creature... regardless of whether we can kill it or not
      // if we can't kill it, later we will attack its owner to do away with it
      Misc::orderTable(m_targetCount, s_priorityTable);
      
      enemy_creature = s_priorityTable[1];
      
      int enemy_wizard = Arena::instance().owner(enemy_creature);
      
      enemy_wizard = Arena::instance().wizardIndex(enemy_wizard);

      // enemy wizard = index to the enemy wizard attacking us
      // enemy creature = index to the enemy creature attacking us
      // now see which is closer and attack them
      int creature_range = Arena::distance(enemy_creature, Arena::instance().startIndex());
      int wizard_range   = Arena::distance(enemy_wizard, Arena::instance().startIndex());
      
      /* 
        Magic Fire close to a wizard confuses the enemy creatures
        Try this out, so that fire is not attractive
        Also, undead creatures should not be attractive to living ones
      */
      bool attacker_undead(false);
      if (attackerIndex == 0xff) {
        // a creature spell to cast... so use the spellid
        int current_spell = m_wizard.selectedSpellId();
        attacker_undead = (current_spell >= SPELL_VAMPIRE and current_spell <= SPELL_ZOMBIE );
      } else {
        // is a valid square
        // see if the attacking square is undead
        
        attacker_undead = Arena::instance().isUndead(attackerIndex);
      }
      
      u8 defender_undead = 0;
      
      int tmp_prio_index(1);
      while (enemy_creature != 0xFF) {
        enemy_creature = s_priorityTable[tmp_prio_index];
        if (Arena::instance().at(0,enemy_creature) != SPELL_MAGIC_FIRE) {
          // not magic fire so check undeadness
          // remember that we are still scared of magic fire and undead creatures
          // just that we are not worried about attacking it
          defender_undead = Arena::instance().isUndead(enemy_creature);
          if ( not defender_undead or attacker_undead) { 
            break;
          } 
        }
        
        tmp_prio_index += 2;
      }
      
      if (enemy_creature == 0xFF) {
        return enemy_wizard;
      }
      
      if (creature_range < wizard_range) {
        return enemy_creature;
      }
      return enemy_wizard;
    }
    
  } else {
    // create the priority table based on "strongest wizard" - one with most/best creatures
    createTableWizards();
    Arena::instance().setTargetIndex(s_priorityTable[1]);
    
    // in the actual game it does a pointless thing here...
    return s_priorityTable[1];
  }
  
}

// based on code at cdaa
void WizardCPU::resetPriorityTable() {
  for (int i = 0, index = 0; i < 0x9E; i++, index += 2) 
  {
    s_priorityTable[index]   = 0x00;
    s_priorityTable[index+1] = 0xFF;
  }
}


// based on code at c955 
// returns squares in range...
//s16 create_range_table(u8 target, u8 range) 

int WizardCPU::createRangeTable(int target, int range)
{
  
  // fill the priority table with all squares in range 
  // distance is from target index
  resetPriorityTable();
  
  int pi(0);
  int range_count(1);
  for (int i = 0; i < 0x9f; i++) {
    int tmprange = Arena::distance(i, Arena::instance().startIndex());
    if (range >= tmprange) {
      // square is in range
      tmprange = Arena::distance(i, target);
      s_priorityTable[pi] = tmprange;
      pi++;
      s_priorityTable[pi] = i;
      pi++;
      range_count++;
    }
  }
  
  Misc::orderTable(range_count, s_priorityTable);
  return range_count;
}

// c9dc
void WizardCPU::setFurthestInrange() 
{
  // get the furthest square away still in range...
  do {
    Arena::instance().setTargetIndex(s_priorityTable[m_tableIndex]);
    
    Misc::delay(5);
    // check xpos < 10  - code at c63d
    int x,y;
    Arena::getXY(Arena::instance().targetIndex(), x, y);
    if (x < 0x10) {
      // in range
      // isBlockedLOS()
      if (!Arena::instance().isBlockedLOS()) {
        
        if (Arena::instance().at(0,Arena::instance().targetIndex()) == 0) {
          // nothing here
          m_targetSquareFound = true;
        } 
        else if (Arena::instance().at(2,Arena::instance().targetIndex()) == 4)
        {
          // is the thing here dead?
          m_targetSquareFound = true;
        }
      }
    }
    if (m_tableIndex - 2 < 0)
      m_tableIndex = 0;
    else
      m_tableIndex -=2;
  } while (m_tableIndex > 0 and (m_targetSquareFound == false));
  
}

// based on code at c7bc
void WizardCPU::createTableEnemies() 
{
  // start_index and wizard_index are in Arena...
  Arena::instance().setStartIndex(Arena::instance().wizardIndex());
  // target_index is also in Arena :-(
  Arena::instance().setTargetIndex(0); // the current "target" square
  m_targetCount = 0;  // the number of targets
  resetPriorityTable();
  
  u16 index = 0; // index to prio table
  for (int i = 0; i < 0x9f; i++) {
    int valid = Arena::instance().containsEnemy(Arena::instance().targetIndex());
    if (valid > 0) {
      m_targetCount++;
      int range = Arena::distance(Arena::instance().targetIndex(), 
          Arena::instance().startIndex());
      range = range >> 1;
      valid += 0x14;
      valid -= range;
      s_priorityTable[index] = valid;
      index++;
      s_priorityTable[index] = Arena::instance().targetIndex();
      index++;
    }
    int targetIndex(Arena::instance().targetIndex());
    Arena::instance().setTargetIndex(targetIndex+1);
  }
}

void WizardCPU::createTableWizards() 
{
  // code is from c825 
  resetPriorityTable();
  //  char str[20];
  
  m_tableIndex = 0;
  for (int i = 0; i < 0x9f; i++) {
    Arena::instance().setTargetIndex(i);
    int wizardId = Arena::instance().wizardId(i);
    if (wizardId != -1) {
      createEnemyTableEntry(wizardId);
    }
  }
  
  Misc::orderTable(7, s_priorityTable);
  
}

// create an enemy table entry for this wizard
// code at c859
void WizardCPU::createEnemyTableEntry(int wizardid) 
{
  if (wizardid == m_wizard.id()) 
    return;
  
  s_priorityTable[m_tableIndex+1] = Arena::instance().targetIndex();
  for (int i = 0; i < 0x9f; i++) {
    // Check that this is right..
    int creature(Arena::instance().at(0,i));
    if (creature > SPELL_DISBELIEVE and creature < SPELL_MAGIC_CASTLE) 
    {
      if (Arena::instance().owner(i) != wizardid) {
        s_priorityTable[m_tableIndex] += (Arena::instance().attackPref(creature) / 4) + 
          m_wizard.priorityOffset();
      }
    }
    
  }
  m_tableIndex +=2;
    
}

// code based on c8c7
// get the priorities of squares surrounding "index" based on distance from "strongest"
void WizardCPU::getSurroundingSquarePrios(int index, int strongest)
{
  resetPriorityTable();
  int surroundIndex = 0;
  m_tableIndex = 0;
  for (int i = 0; i < 8; i++) {
    int lookat_i = Arena::applyPositionModifier(index, surroundIndex);
    surroundIndex++;
    if (lookat_i == 0) {
      // out of bounds
      continue;
    }
    lookat_i--;
    
    int range = Arena::distance(strongest, lookat_i);
    // add a check in here to make sure we don't
    // tread on a magic wood. This table is ordered largest
    // first. But the potential squares are read starting 
    // at the end (LUT_index, then LUT_index-2, etc)
    if (Arena::instance().at(0,lookat_i) == SPELL_MAGIC_WOOD) {
      range += 1;  // "increase" the distance so it isn't as appealing
                   // careful not to increase too much, may go backwards!
    }
    s_priorityTable[m_tableIndex++] = range;
    s_priorityTable[m_tableIndex++] = i;
    
  }
  Misc::orderTable(7, s_priorityTable);
  m_tableIndex = 0xF;
  
}

void WizardCPU::doAiMovement()
{
  /*
   if wiz has creatures to do his bidding...

     get current position, set up relative
     movement advantages for squares we can go to

     if we are in a wood or castle, stay there  
     else ...
     select the wizard
     do the movement
   
   end if
   
   for all squares on the board...
    if we have a creature here
    then try selecting it
      if we selected it successfully (it hadn't moved already)
        then do the movement for this creature
   end for

   so if the wizard is creatureless, he himself acts like a normal creature
   */
  
  // the computer movement stuff...
  if (not m_wizard.timid()) 
  {
    //  ca92...
    setupMove();
    // if we are in a safe place, don't move
    int arena0 = Arena::instance().at(0, Arena::instance().startIndex());
    if (   arena0 == SPELL_MAGIC_WOOD 
        or arena0 == SPELL_MAGIC_CASTLE 
        or arena0 == SPELL_DARK_CITADEL)
    { 
      // set has moved...
      m_hasMoved = true;
#if 0
      arena[3][start_index] = arena[3][start_index] | 0x80;
#endif
    }
    else {
#if 0
      // move wizard...
      // select the wizard...
      m_hasMoved = false;
      // and we have the move table in memory..
      m_moveTableCreated = true;
      
      movement_a(); // select the wizard...
      delay(30);
      // now try moving it to the target square...
      do_this_movement();
      
      delay(10);
#endif
    }
  }
  // here? move creatures
  // we have already dealt with the wizard...
#if 0
  has_wizard_moved = 1;
  
  u8 i;
  for (i = 0; i < 0x9f; i++) {
    target_index = i;
    start_index = i;
    movement_a(); // select the creature...
    if (selected_creature != 0) {
      delay(30);
      do_this_movement();
    }
    
  }
  delay(20);
#endif
}

// called at the start of lightning casting amongst other stuff...
// creates the priority table based on all enemy creatures and wizards
// wizards are rated higher than creatures and table is ordered and 
// the first value in the table is pointed to by value in cd86
void WizardCPU::createAllEnemiesTable()
{
  // based on cc56
  m_wizard.setPriorityOffset(0x3c);
  // create the priority table based on "strongest wizard"
  createTableWizards();
  m_wizard.setPriorityOffset(0x20);
  m_tableIndex = 14;
  int tableSize = 9;
  // now create the rest of the table, for the enemy creatures
  for (int i = 0; i < 0x9e; i++) {
    // call c67a
    int tmp(Arena::instance().containsEnemy(i));
    
    if (tmp != 0) {
      tableSize++;
      s_priorityTable[m_tableIndex++] = tmp;
      s_priorityTable[m_tableIndex++] = i;
    }
  }
  Misc::orderTable(tableSize,s_priorityTable);
  m_tableIndex = 1;
  
  m_wizard.setPriorityOffset(0);
}

void WizardCPU::aiCastDisbelieve() {
  int currentIndex = Arena::instance().startIndex();
  createAllEnemiesTable();
  Arena::instance().setStartIndex(currentIndex);

  while (s_priorityTable[m_tableIndex] != 0xFF) {
    int target_index = s_priorityTable[m_tableIndex];
    

    if (Arena::instance().at(0,target_index) < SPELL_GOOEY_BLOB 
        and not Arena::instance().hasDisbelieveCast(target_index)  ) {
      // is a creature and has not had disbeleive cast on it yet...
      m_targetSquareFound = true;
      m_wizard.printNameSpell();
      Arena::instance().setTargetIndex(target_index);
      Misc::delay(80);
      return;
    }
    
    m_tableIndex += 2;
  }
  // got to here? must have run out of decent targets
  Arena::instance().setTargetIndex(currentIndex);
  m_targetSquareFound = false;
  
}

bool WizardCPU::hasTargetSquare() const
{
  return m_targetSquareFound;
}


// defines for the special moves the wizard can make, with wings or on a mount
#define SPECIAL_MOVE_MOUNT 1
#define SPECIAL_MOVE_WINGS 2

// ca92.
void WizardCPU::setupMove() {
  resetPriorityTable();
  // get the arena square with the wizard in it...
  int currentPlayer(Arena::instance().currentPlayer());
  int tmpWiz(Arena::instance().wizardIndex(currentPlayer));
  Arena::instance().setTargetIndex(tmpWiz);
  int targetIndex(tmpWiz);
  int targetCreature(Arena::instance().at(0,targetIndex));
  // check the value in arena 0 at the wizard's location
  if ( targetCreature >= SPELL_PEGASUS 
      and targetCreature <= SPELL_MANTICORE) 
  {
    // wizard on a flying mount...
    // cade...
    // call cbb9
    flyingMove(SPECIAL_MOVE_MOUNT);
    m_hasMoved = true;
    return;
  } else {
    Movement * movement(dynamic_cast<Movement*>(GameState::instance().currentScreen()));
    bool tmp_is_flying(false);
    if (movement)
    {
      tmp_is_flying = movement->isFlying();
    }

    // not on a flying mount (may be on a regular one though)
    if (targetCreature >= Arena::WIZARD_INDEX 
	and Wizard::player(currentPlayer).hasMagicWings() 
      /* bug fix 26/12/04 
       * If wiz is flying and engaged to undead, causes problems
       */
      && tmp_is_flying)
    {
      // has magic wings and is in the open
      // cafb....
      // call cbc7
      flyingMove(SPECIAL_MOVE_WINGS);
      m_hasMoved = true;
      return;
    } else {
      // cb06...
      int surroundIndex = 0;
      m_tableIndex = 0;
      int targetIndex(Arena::instance().targetIndex());
      for (int i = 0; i < 8; i++) {
        int lookat_i = Arena::applyPositionModifier(targetIndex, surroundIndex);
        surroundIndex++;
        if (lookat_i == 0) {
          // out of bounds
          continue;
        }
        lookat_i--;
        int creatureLookedAt(Arena::instance().at(0, lookat_i));
        if (creatureLookedAt >= SPELL_MAGIC_CASTLE 
	    and creatureLookedAt <= SPELL_DARK_CITADEL) 
	{
          s_priorityTable[m_tableIndex++] = 0;
          s_priorityTable[m_tableIndex++] = i;
          m_targetSquareFound = true;
        } else
        if (creatureLookedAt == SPELL_MAGIC_WOOD) {
          s_priorityTable[m_tableIndex++] = 1;
          s_priorityTable[m_tableIndex++] = i;
          m_targetSquareFound = true;
        } else 
        if (creatureLookedAt >= SPELL_HORSE 
            and creatureLookedAt <= SPELL_MANTICORE
            and Arena::instance().owner(lookat_i) == currentPlayer 
            and Arena::instance().at(4,lookat_i) == 0) // is not mounted...
        {
          // is a mount and is ours and no one is on it 
          s_priorityTable[m_tableIndex++] = 2;
          s_priorityTable[m_tableIndex++] = i;
          m_targetSquareFound = true;
        } else {
          // is none of the above
          s_priorityTable[m_tableIndex++] = getPriorityVal(lookat_i)+3;
          s_priorityTable[m_tableIndex++] = i;
        }
          
      }
      // cb8b
      Misc::orderTable(7, s_priorityTable);
      Arena::instance().setStartIndex(tmpWiz);
      Arena::instance().setTargetIndex(tmpWiz);
      m_tableIndex = 0xF;
    } // end if has magic wings/out in open
  
  } // end if wizard on mount
  // cba2
  m_moveTableCreated = true;
}


// used for wizards on flying mounts or with wings
void WizardCPU::flyingMove(int type)
{
  Movement * movement(dynamic_cast<Movement*>(GameState::instance().currentScreen()));
  // based on code at cbb9/cbc7
  // default to wings
  int movementAllowance = 13;
  if (type == SPECIAL_MOVE_MOUNT) {
    movementAllowance = 1 + 
      (s_spellData[Arena::instance().at(0,Arena::instance().targetIndex())].movement*2); 
  }
  movement->setMovementAllowance(movementAllowance);
  
  resetPriorityTable();
  
  //cbcf
  m_targetCount = 1;
  m_tableIndex = 0;
  int tmpWiz(Arena::instance().targetIndex());
  // loop over arena to get priority of all squares in range
  for (int i = 0; i < Arena::ARENA_SIZE; i++) {
    int x, y;
    Arena::getXY(i, x, y);
    if (x < 16) {
      // in bounds
      // get the distance from wizard start square to this square (i)
      int dist = Arena::distance(tmpWiz, i);
      if (movementAllowance >= dist) {
        // is in range of the wiz start square...
        // find the distance/danger of all enemies relative to this square  (call cd92)
        // store as priority // code to here is at cc0d
        s_priorityTable[m_tableIndex++] = getPriorityVal(i);
        s_priorityTable[m_tableIndex++] = i;
        m_targetCount++;
      }
    }  
  }
  // cc31
  Misc::orderTable(m_targetCount, s_priorityTable);
  Arena::instance().setStartIndex(tmpWiz);
  m_tableIndex--;
}



// taken from cd92 - gets the "danger" of the square at i
// this is calculated based on enemy creature distance and how dangerous that creature is
int WizardCPU::getPriorityVal(int index)
{
  int total = 0;
  Arena::instance().setStartIndex(index);
  for (int i = 0; i < Arena::ARENA_SIZE; i++)
  {
    // gets the priority val for this square from start_index
    total += Arena::instance().containsEnemy(i);
  }
  return total;
}

void WizardCPU::endMove()
{
  // whatever happens, the wizard has definitely moved...
  m_hasMoved = true;
  // and we definitely need to remake the movement table
  m_moveTableCreated = 0;
}


// check if the cpu could dismount (traditionally, the cpu never dismounts)
// return true to dismount
// return false to stay mounted
bool WizardCPU::dismount() const
{
  Arena & arena(Arena::instance());
  int targetIndex(arena.targetIndex());
  int creature(arena.at(0,targetIndex));
  // which type of mount are we on?
  if (creature >= SPELL_PEGASUS 
      and creature <= SPELL_MANTICORE )
  {
    // too complex to work out...
    return 0;
  } 

  // "type" records type of thing that attracts us
  int type(0xff);
  int surroundIndex = 0;
  for (int i = 0; i < 8; i++) {
    int lookat_i = Arena::applyPositionModifier(targetIndex, surroundIndex);
    surroundIndex++;
    if (lookat_i == 0) {
      // out of bounds
      continue;
    }
    lookat_i--;

    // rules for leaving our mount are consistent with the
    // movement patterns when not mounted, i.e. prefer
    // to move to a castle or tree before getting on a horse.
    int creature(arena.at(0,lookat_i));
    if (creature >= SPELL_MAGIC_CASTLE 
	and creature <= SPELL_DARK_CITADEL
//	&& (players[current_player].timid == 0)  // is this right?
	) 
    {
      type = 0;
    } 
    else if (creature == SPELL_MAGIC_WOOD) 
    {
      if (type > 1) {  
	type = 1;
      }
    } 
    else if ( creature >= SPELL_PEGASUS
          and creature <= SPELL_MANTICORE 
          and arena.owner(lookat_i) == arena.currentPlayer()
          and arena.at(4, lookat_i) == 0 )
    {
      /* Next to us stands a winged mount, we are on a walking mount so swap.
       * Really we should change to any better mount, but that is quite a
       * complex operation - the cpu move table would need resetting and extra
       * checks put in all over the place. Not really worth it.
       */ 
      if (type > 2) {
	type = 2;
      }
    }
  }
  return type != 0xff;
}

void WizardCPU::setMoveTableCreated(bool created)
{
  m_moveTableCreated = created;
}