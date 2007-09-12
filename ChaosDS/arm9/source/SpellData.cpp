/*
  Copyright (C) 2007 Richard Quirk

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
#include "libnds.h"
#include "images.h"
#include "magic.h"
#include "SpellData.h"
const SpellData s_spellData[] = {
  {"None", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, cast_disbelieve, 0, 0, 0, },
  {"Disbelieve", 0, 10, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, cast_disbelieve, 0, 0, 0, },
  {"Meditate", 0, 1, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, cast_meditate, NEW_FEATURE, 0, 0, },
  {"King Cobra", 1, 10, 3, 18, 0, 4, 0, 0, 1, 1, 6, 1, cast_creature, 0, _binary_king_cobra_raw_start, _binary_king_cobra_map_start, },
  {"Dire Wolf", -1, 9, 3, 18, 1, 3, 0, 0, 2, 3, 7, 2, cast_creature, 0, _binary_dire_wolf_raw_start, _binary_dire_wolf_map_start, },
  {"Goblin", -1, 9, 3, 18, 6, 2, 0, 0, 4, 1, 4, 4, cast_creature, 0, _binary_goblin_raw_start, _binary_goblin_map_start, },
  {"Crocodile", 0, 8, 3, 18, 0, 5, 0, 0, 6, 1, 2, 2, cast_creature, 0, _binary_crocodile_raw_start, _binary_crocodile_map_start, },
  {"Troll", -1, 6, 3, 18, 1, 4, 0, 0, 8, 1, 1, 4, cast_creature, NEW_FEATURE, _binary_troll_raw_start, _binary_troll_map_start, },
  {"Faun", -1, 8, 3, 18, 3, 3, 0, 0, 2, 1, 7, 8, cast_creature, 0, _binary_faun_raw_start, _binary_faun_map_start, },
  {"Lion", 1, 6, 3, 18, 2, 6, 0, 0, 4, 4, 8, 3, cast_creature, 0, _binary_lion_raw_start, _binary_lion_map_start, },
  {"Elf", 2, 7, 3, 18, 0, 1, 2, 6, 2, 1, 5, 7, cast_creature, 0, _binary_elf_raw_start, _binary_elf_map_start, },
  {"Orc", -1, 10, 3, 18, 3, 2, 0, 0, 1, 1, 4, 4, cast_creature, 0, _binary_orc_raw_start, _binary_orc_map_start, },
  {"Bear", 1, 6, 3, 18, 3, 6, 0, 0, 7, 2, 6, 2, cast_creature, 0, _binary_bear_raw_start, _binary_bear_map_start, },
  {"Gorilla", 0, 7, 3, 18, 7, 6, 0, 0, 5, 1, 4, 2, cast_creature, 0, _binary_gorilla_raw_start, _binary_gorilla_map_start, },
  {"Ogre", -1, 7, 3, 18, 4, 4, 0, 0, 7, 1, 3, 6, cast_creature, 0, _binary_ogre_raw_start, _binary_ogre_map_start, },
  {"Hydra", -1, 5, 3, 18, 0, 7, 0, 0, 8, 1, 4, 6, cast_creature, 0, _binary_hydra_raw_start, _binary_hydra_map_start, },
  {"Giant Rat", 0, 10, 3, 18, 1, 1, 0, 0, 1, 3, 8, 2, cast_creature, 0, _binary_giant_rat_raw_start, _binary_giant_rat_map_start, },
  {"Giant", 1, 4, 3, 18, 6, 9, 0, 0, 7, 2, 6, 5, cast_creature, 0, _binary_giant_raw_start, _binary_giant_map_start, },
  {"Horse", 1, 9, 3, 18, 3, 1, 0, 0, 3, 4, 8, 1, cast_creature, 0, _binary_horse_raw_start, _binary_horse_map_start, },
  {"Unicorn", 2, 6, 3, 18, 7, 5, 0, 0, 4, 4, 9, 7, cast_creature, 0, _binary_unicorn_raw_start, _binary_unicorn_map_start, },
  {"Centaur", 1, 7, 3, 18, 3, 1, 2, 4, 3, 4, 5, 5, cast_creature, 0, _binary_centaur_raw_start, _binary_centaur_map_start, },
  {"Pegasus", 2, 7, 3, 18, 1, 2, 0, 0, 4, 5, 6, 7, cast_creature, 0, _binary_pegasus_raw_start, _binary_pegasus_map_start, },
  {"Gryphon", 1, 6, 3, 18, 1, 3, 0, 0, 5, 5, 5, 6, cast_creature, 0, _binary_gryphon_raw_start, _binary_gryphon_map_start, },
  {"Manticore", -1, 4, 3, 18, 2, 3, 1, 3, 6, 5, 6, 8, cast_creature, 0, _binary_manticore_raw_start, _binary_manticore_map_start, },
  {"Bat", -1, 8, 3, 18, 1, 1, 0, 0, 1, 5, 9, 4, cast_creature, 0, _binary_bat_raw_start, _binary_bat_map_start, },
  {"Green Dragon", -1, 2, 3, 18, 0, 5, 4, 6, 8, 3, 4, 4, cast_creature, 0, _binary_green_dragon_raw_start, _binary_green_dragon_map_start, },
  {"Red Dragon", -2, 1, 3, 18, 4, 7, 3, 5, 9, 3, 4, 5, cast_creature, 0, _binary_red_dragon_raw_start, _binary_red_dragon_map_start, },
  {"Golden Dragon", 2, 2, 3, 18, 2, 9, 5, 4, 9, 3, 5, 5, cast_creature, 0, _binary_golden_dragon_raw_start, _binary_golden_dragon_map_start, },
  {"Harpy", -1, 7, 3, 18, 1, 4, 0, 0, 2, 5, 8, 5, cast_creature, 0, _binary_harpy_raw_start, _binary_harpy_map_start, },
  {"Eagle", 1, 7, 3, 18, 2, 3, 0, 0, 3, 6, 8, 2, cast_creature, 0, _binary_eagle_raw_start, _binary_eagle_map_start, },
  {"Vampire", -2, 2, 3, 18, 1, 6, 0, 0, 8, 4, 6, 5, cast_creature, 0, _binary_vampire_raw_start, _binary_vampire_map_start, },
  {"Ghost", -1, 5, 3, 18, 4, 1, 0, 0, 3, 2, 9, 6, cast_creature, 0, _binary_ghost_raw_start, _binary_ghost_map_start, },
  {"Spectre", -1, 6, 3, 18, 7, 4, 0, 0, 2, 1, 6, 4, cast_creature, 0, _binary_spectre_raw_start, _binary_spectre_map_start, },
  {"Wraith", -1, 6, 3, 18, 5, 5, 0, 0, 5, 2, 4, 5, cast_creature, 0, _binary_wraith_raw_start, _binary_wraith_map_start, },
  {"Skeleton", -1, 7, 3, 18, 4, 3, 0, 0, 2, 1, 3, 4, cast_creature, 0, _binary_skeleton_raw_start, _binary_skeleton_map_start, },
  {"Zombie", -1, 9, 3, 18, 7, 1, 0, 0, 1, 1, 2, 3, cast_creature, 0, _binary_zombie_raw_start, _binary_zombie_map_start, },
  {"Gooey Blob", -1, 9, 13, 18, 0, 0, 0, 0, 0, 0, 0, 0, cast_fire_goo, 0, _binary_gooey_blob_raw_start, _binary_gooey_blob_map_start, },
  {"Magic Fire", -1, 8, 13, 18, 4, 0, 0, 0, 0, 0, 0, 0, cast_fire_goo, 0, _binary_magic_fire_raw_start, _binary_magic_fire_map_start, },
  {"Magic Wood", 1, 8, 17, 17, 8, 0, 0, 0, 0, 0, 0, 0, cast_trees_castles, 0, _binary_magic_wood_raw_start, _binary_magic_wood_map_start, },
  {"Shadow Wood", -1, 4, 17, 18, 5, 0, 0, 0, 0, 0, 0, 0, cast_trees_castles, 0, _binary_shadow_wood_raw_start, _binary_shadow_wood_map_start, },
  {"Magic Castle", 1, 6, 17, 23, 5, 0, 0, 0, 0, 0, 0, 0, cast_trees_castles, 0, _binary_magic_castle_raw_start, _binary_magic_castle_map_start, },
  {"Dark Citadel", -1, 6, 17, 23, 5, 0, 0, 0, 0, 0, 0, 0, cast_trees_castles, 0, _binary_dark_citadel_raw_start, _binary_dark_citadel_map_start, },
  {"Wall", 0, 8, 13, 12, 3, 0, 0, 0, 0, 0, 0, 0, cast_wall, 0, _binary_wall_raw_start, _binary_wall_map_start, },
  {"Magic Bolt", 0, 10, 13, 18, 0, 0, 0, 0, 0, 0, 0, 0, cast_magic_missile, 0, 0, 0, },
  {"Blind", -1, 10, 13, 18, 0, 0, 0, 0, 0, 0, 0, 0, cast_magic_missile, NEW_FEATURE, 0, 0, },
  {"Lightning", 0, 10, 9, 18, 0, 0, 0, 0, 0, 0, 0, 0, cast_magic_missile, 0, 0, 0, },
  {"Magic Sleep", 1, 10, 13, 18, 0, 0, 0, 0, 0, 0, 0, 0, cast_magic_missile, NEW_FEATURE, 0, 0, },
  {"Vengeance", -1, 8, 41, 12, 0, 0, 0, 0, 0, 0, 0, 0, cast_justice, 0, 0, 0, },
  {"Decree", 1, 8, 41, 12, 0, 0, 0, 0, 0, 0, 0, 0, cast_justice, 0, 0, 0, },
  {"Dark Power", -2, 5, 41, 11, 0, 0, 0, 0, 0, 0, 0, 0, cast_justice, 0, 0, 0, },
  {"Justice", 2, 5, 41, 11, 0, 0, 0, 0, 0, 0, 0, 0, cast_justice, 0, 0, 0, },
  {"Magic Shield", 1, 7, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, cast_magic_shield, 0, 0, 0, },
  {"Magic Armour", 1, 4, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, cast_magic_armour, 0, 0, 0, },
  {"Magic Sword", 1, 4, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, cast_magic_sword, 0, 0, 0, },
  {"Magic Knife", 1, 7, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, cast_magic_knife, 0, 0, 0, },
  {"Magic Bow", 1, 5, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, cast_magic_bow, 0, 0, 0, },
  {"Magic Wings", 0, 5, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, cast_magic_wings, 0, 0, 0, },
  {"Law 1", 2, 8, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, cast_chaos_law, 0, 0, 0, },
  {"Law 2", 4, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, cast_chaos_law, 0, 0, 0, },
  {"Chaos 1", -2, 8, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, cast_chaos_law, 0, 0, 0, },
  {"Chaos 2", -4, 6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, cast_chaos_law, 0, 0, 0, },
  {"Shadow Form", 0, 8, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, cast_shadow_form, 0, 0, 0, },
  {"Subversion", 0, 10, 15, 18, 0, 0, 0, 0, 0, 0, 0, 0, cast_subversion, 0, 0, 0, },
  {"Mutation", 0, 10, 15, 18, 0, 0, 0, 0, 0, 0, 0, 0, cast_subversion, NEW_FEATURE, 0, 0, },
  {"Raise Dead", -1, 5, 9, 18, 0, 0, 0, 0, 0, 0, 0, 0, cast_raise_dead, 0, 0, 0, },
  {"Turmoil", -1, 5, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, cast_turmoil, 0, 0, 0, },
};
