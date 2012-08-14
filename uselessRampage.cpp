/*
Copyright (c) 2012 Vladimir Jimenez
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
     derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


*** Useless Rampage Details ***
Author:
Vladimir Jimenez (allejo)

Description:
Go on a rampage with useless and get rewarded

Slash Commands:
N/A

License:
BSD

Version:
1.0
*/

#include "bzfsAPI.h"
#include "plugin_utils.h"

class uselessRampage : public bz_Plugin
{
    virtual const char* Name (){return "Useless Rampage";}
    virtual void Init (const char* config);
    virtual void Event (bz_EventData* eventData);
    virtual void Cleanup ();
};

BZ_PLUGIN(uselessRampage);

int playerKills[256] = {0};
int consecutiveUSKils[256] = {0};

void uselessRampage::Init(const char* /*commandLine*/ )
{
    bz_debugMessage(4,"uselessRampage plugin loaded");
    
    //Register the events...
    Register(bz_ePlayerDieEvent);
    Register(bz_ePlayerPartEvent);
}

void uselessRampage::Cleanup(void)
{
    bz_debugMessage(4,"uselessRampage plugin unloaded");
    Flush();
}

void uselessRampage::Event(bz_EventData* eventData)
{
    switch (eventData->eventType)
    {
        case bz_ePlayerDieEvent:
        {
            bz_PlayerDieEventData_V1* diedata = (bz_PlayerDieEventData_V1*)eventData;
            playerKills[diedata->killerID]++; //Give the player credit and increment their number of kills
                
            if (diedata->flagKilledWith == "US") //A player is killed with the Useless flag
            {
                consecutiveUSKils[diedata->killerID]++; //Give the player credit and increment their number of consecutive kills with useless
                
                int killerScore = bz_getPlayerWins(diedata->killerID); //Get their normal score
                if (playerKills[diedata->killerID] < 20)
                {
                    killerScore = killerScore + playerKills[diedata->killerID]; //Increment by whatever range of multiplication they are at based on the array position
                }
                else
                {
                    killerScore = killerScore + 19; //Limit the max to 20 points per kill
                }
                
                bz_setPlayerWins(diedata->killerID, killerScore); //Give the player their new score
                
                if (consecutiveUSKils[diedata->killerID] == 20)
                {
                    bz_BasePlayerRecord *pr = bz_getPlayerByIndex(diedata->killerID);
                    bz_sendTextMessagef(BZ_SERVER, BZ_ALLUSERS, "Holy shit! %s is kicking ass with the useless flag!", pr->callsign.c_str());
                    bz_freePlayerRecord(pr);
                }
            }
            
            if (playerKills[diedata->playerID] > 0) //They died so let's reset their number of kills with Useless
            {
                playerKills[diedata->playerID] = 0;
                consecutiveUSKils[diedata->playerID] = 0;
            }
        }
        break;
        
        case bz_ePlayerPartEvent:
        {
            bz_PlayerJoinPartEventData_V1* partdata = (bz_PlayerJoinPartEventData_V1*)eventData;
            
            playerKills[partdata->playerID] = 0; //Reset the player slots number of kills with Useless when they leave
            consecutiveUSKils[partdata->playerID] = 0;
        }
        break;
        
        default:break; //So pointless
    }
}