/*
 * Copyright (С) since 2019+ AzerothCore <www.azerothcore.org>
 * Licence MIT https://opensource.org/MIT
 */

#include "Chat.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "CFBG.h"

using namespace Acore::ChatCommands;

class cfbg_commandscript : public CommandScript
{
public:
    cfbg_commandscript() : CommandScript("cfbg_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        // For soap / admins
        static ChatCommandTable cfbgGmCommands =
        {
            { "on", HandleCFBG_GmOn, SEC_ADMINISTRATOR, Console::Yes },
            { "off", HandleCFBG_GmOff, SEC_ADMINISTRATOR, Console::Yes }
        };

        static ChatCommandTable cfbgCommands =
        {
            { "race", HandleCFBGChooseRace, SEC_PLAYER, Console::No },
            { "gm", cfbgGmCommands },
            { "on", HandleCFBG_On, SEC_PLAYER, Console::No },
            { "off", HandleCFBG_Off, SEC_PLAYER, Console::No },
        };

        static ChatCommandTable commandTable =
        {
            { "cfbg",  cfbgCommands },
        };

        return commandTable;
    }

    static bool HandleCFBGChooseRace(ChatHandler* handler, std::string raceInput)
    {
        Player* player = handler->GetPlayer();

        uint8 raceId = 0;

        if (sCFBG->RandomizeRaces())
        {
            handler->SendSysMessage("Race selection is currently disabled.");
            handler->SetSentErrorMessage(true);
            return true;
        }

        for (auto const& raceVariable : *sCFBG->GetRaceInfo())
        {
            if (raceInput == raceVariable.RaceName)
            {
                if (player->GetTeamId(true) == raceVariable.TeamId)
                {
                    raceId = raceVariable.RaceId;
                }
                else
                {
                    handler->SendSysMessage("Race not available to your faction.");
                    handler->SetSentErrorMessage(true);
                    return true;
                }
                
                if (!IsRaceValidForClass(player, raceId))
                {
                    handler->SendSysMessage("Race not available to your class.");
                    handler->SetSentErrorMessage(true);
                    return true;
                }

                if (raceId == RACE_NIGHTELF)
                {
                    handler->SendSysMessage("Night elf models are not available as the female model is missing and the male one causes client crashes.");
                    handler->SetSentErrorMessage(true);
                    return true;
                }

                if (player->getGender() == GENDER_FEMALE && (raceId == RACE_TROLL || raceId == RACE_DWARF))
                {
                    handler->SendSysMessage("Female models are not available for the following races: troll, dwarf.");
                    handler->SetSentErrorMessage(true);
                    return true;
                }
            }
        }

        player->UpdatePlayerSetting("mod-cfbg", SETTING_CFBG_RACE, raceId);

        if (!raceId)
        {
            handler->SendSysMessage("Race unavailable. CFBG selected race set to random. You will be morphed into a random race when you enter a battleground on the opposite team.");
        }
        else
        {
            handler->PSendSysMessage("CFBG selected race set to %s", raceInput);
        }

        return true;
    }

    static bool HandleCFBG_GmOn(ChatHandler* handler, Optional<PlayerIdentifier> target)
    {
        if (!target)
        {
            handler->SendErrorMessage(LANG_NO_PLAYER_FOUND);
            return false;
        }

        const uint32 guid = target->GetGUID().GetCounter();

        if (sCFBG->IsCrossFactionEnabled(guid))
        {
            handler->SendErrorMessage(LANG_CROSS_FACTION_ALREADY_ENABLED);
            return false;
        }

        sCFBG->EnableCrossFaction(guid);
        handler->PSendSysMessage(LANG_CROSS_FACTION_ENABLED);
        return true;
    }

    static bool HandleCFBG_GmOff(ChatHandler* handler, Optional<PlayerIdentifier> target)
    {
        if (!target)
        {
            handler->SendErrorMessage(LANG_NO_PLAYER_FOUND);
            return false;
        }

        const uint32 guid = target->GetGUID().GetCounter();

        if (!sCFBG->IsCrossFactionEnabled(guid))
        {
            handler->SendErrorMessage(LANG_CROSS_FACTION_ALREADY_DISABLED);
            return false;
        }

        sCFBG->DisableCrossFaction(guid);
        handler->PSendSysMessage(LANG_CROSS_FACTION_DISABLED);
        return true;
    }

    static bool HandleCFBG_On(ChatHandler* handler)
    {
        if (auto player = handler->GetPlayer())
        {
            const uint32 guid = player->GetGUID().GetCounter();
            if (sCFBG->IsCrossFactionEnabled(guid))
            {
                handler->SendErrorMessage(LANG_CROSS_FACTION_ALREADY_ENABLED);
                return false;
            }

            sCFBG->EnableCrossFaction(guid);
            handler->PSendSysMessage(LANG_CROSS_FACTION_ENABLED);
            return true;
        }

        handler->SendErrorMessage(LANG_NO_PLAYER_FOUND);
        return false;
    }

    static bool HandleCFBG_Off(ChatHandler* handler)
    {
        if (auto player = handler->GetPlayer())
        {
            const uint32 guid = player->GetGUID().GetCounter();
            if (!sCFBG->IsCrossFactionEnabled(guid))
            {
                handler->SendErrorMessage(LANG_CROSS_FACTION_ALREADY_DISABLED);
                return false;
            }

            sCFBG->DisableCrossFaction(guid);
            handler->PSendSysMessage(LANG_CROSS_FACTION_DISABLED);
            return true;
        }

        handler->SendErrorMessage(LANG_NO_PLAYER_FOUND);
        return false;
    }

    static bool IsRaceValidForClass(Player* player, uint8 fakeRace)
    {
        auto raceData{ *sCFBG->GetRaceData() };

        std::vector<uint8> availableRacesForClass = player->GetTeamId(true) == TEAM_HORDE ?
            raceData[player->getClass()].availableRacesA : raceData[player->getClass()].availableRacesH;

        for (auto const& races : availableRacesForClass)
        {
            if (races == fakeRace)
            {
                return true;
            }
        }

        return false;
    }
};

void AddSC_cfbg_commandscript()
{
    new cfbg_commandscript();
}
