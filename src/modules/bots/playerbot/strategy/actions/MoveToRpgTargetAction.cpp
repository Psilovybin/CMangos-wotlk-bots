#include "botpch.h"
#include "../../playerbot.h"
#include "MoveToRpgTargetAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../values/PossibleRpgTargetsValue.h"

using namespace ai;

bool MoveToRpgTargetAction::Execute(Event event)
{
    Unit* target = ai->GetUnit(AI_VALUE(ObjectGuid, "rpg target"));
    if (!target) return false;

    float distance = AI_VALUE2(float, "distance", "rpg target");
    if (distance > 180.0f)
    {
        context->GetValue<ObjectGuid>("rpg target")->Set(ObjectGuid());
        return false;
    }

    float x = target->GetPositionX();
    float y = target->GetPositionY();
    float z = target->GetPositionZ();
    float mapId = target->GetMapId();
	
	if (sPlayerbotAIConfig.RandombotsWalkingRPG)
	{
#ifdef MANGOSBOT_ONE
		bot->m_movementInfo->AddMovementFlag(MOVEFLAG_WALK_MODE);
#else
        bot->m_movementInfo.AddMovementFlag(MOVEFLAG_WALK_MODE);
#endif
	}

    if (bot->IsWithinLOS(x, y, z)) return MoveNear(target, sPlayerbotAIConfig.followDistance);

    WaitForReach(distance);

    if (bot->IsSitState())
        bot->SetStandState(UNIT_STAND_STATE_STAND);

    if (bot->IsNonMeleeSpellCasted(true))
    {
        bot->CastStop();
        ai->InterruptSpell();
    }

#ifdef MANGOSBOT_TWO
    bool generatePath = !bot->IsFlying() && !bot->IsUnderWater();
#else
    bool generatePath = !bot->IsFlying() && !bot->IsUnderwater();
#endif
    MotionMaster &mm = *bot->GetMotionMaster();
#ifdef CMANGOS
    mm.Clear();
#endif
    mm.MovePoint(mapId, x, y, z, generatePath);

    AI_VALUE(LastMovement&, "last movement").Set(x, y, z, bot->GetOrientation());
    return true;
}

bool MoveToRpgTargetAction::isUseful()
{
    return context->GetValue<ObjectGuid>("rpg target")->Get() && AI_VALUE2(float, "distance", "rpg target") > sPlayerbotAIConfig.followDistance;
}
