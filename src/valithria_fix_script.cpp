#include "ScriptMgr.h"
#include "Player.h"
#include "Creature.h"
#include "Map.h"

class valithria_evade_fix : public AllCreatureScript
{
public:
    valithria_evade_fix() : AllCreatureScript("valithria_evade_fix") {}

    void OnAllCreatureUpdate(Creature* creature, uint32 /*diff*/) override
    {
        // 36853 is NPC_GREEN_DRAGON_COMBAT_TRIGGER
        if (creature->GetEntry() == 36853)
        {
            if (creature->IsInCombat())
            {
                // In new threat system, GetUnsortedThreatList() might be empty for triggers
                auto const& threatList = creature->GetThreatMgr().GetUnsortedThreatList();
                if (threatList.begin() == threatList.end())
                {
                    Map* map = creature->GetMap();
                    if (map && map->IsDungeon())
                    {
                        // Add any alive player within 200 yards to the threat list to prevent evade
                        Map::PlayerList const &players = map->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                        {
                            if (Player* player = itr->GetSource())
                            {
                                if (player->IsAlive() && !player->IsGameMaster() && creature->GetExactDist(player) < 200.0f)
                                {
                                    creature->AddThreat(player, 1.0f);
                                    break; // Adding one player is enough to prevent the empty list check
                                }
                            }
                        }
                    }
                }
            }
        }
    }
};

void Addvalithria_evade_fixScripts()
{
    new valithria_evade_fix();
}
