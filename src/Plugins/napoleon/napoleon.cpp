#include "selectStateByClass.h"
#include "MengeCore/PluginEngine/CorePluginEngine.h"
#include "napoleonConfig.h"
#include "NearestEnemyComponent.h"
#include "EnemyNearCondition.h"
#include "NumEnemCloseCondition.h"
#include "CanPerformCondition.h"
#include "MengeVis/PluginEngine/VisPluginEngine.h"
#include "AttackAction.h"
#include "DeadCondition.h"
#include "TurnModifier.h"
#include "AimingComponent.h"
// #include "NearestEnemCompContext.h"
#include "IdleComponent.h"
#include "FacingTargetEnem.h"
#include "PikeModifier.h"
#include "PikeAction.h"
#include "PikeCollisionModifier.h"
#include "StaggerCondition.h"
#include "StaggerModifier.h"
#include "PikeCollisionCondition.h"
#include "UserCommandCondition.h"
#include "UserCommandTask.h"
// #include "VisAttackElement.h"

// using MengeVis::PluginEngine::VisPluginEngine;
using Menge::PluginEngine::CorePluginEngine;

extern "C" {
  /*!
   *  @brief    Retrieves the name of the plug-in.
   *
   *  @returns  The name of the plug in.
   */
  EXPORT_API const char * getName() {
    return "Napoleon_plugin";
  }

  /*!
   *  @brief    Description of the plug-in.
   *
   *  @returns  A description of the plugin.
   */
  EXPORT_API const char * getDescription() {
    return  "napoleon game";
  }

  /*!
   *  @brief    Registers the plug-in with the PluginEngine
   *
   *  @param    engine    A pointer to the plugin engine.
   */
  EXPORT_API void registerCorePlugin( CorePluginEngine * engine ) {
    // engine->registerActionFactory( new Menge::BFSM::PropertyXActFactory() );
    engine->registerTargetFactory( new Menge::BFSM::SelectStateByClassTargetFactory() );
    engine->registerVelCompFactory( new Napoleon::NearestEnemComponentFactory() );
    engine->registerConditionFactory( new Napoleon::EnemyNearCondFactory());
    engine->registerConditionFactory( new Napoleon::NumEnemyCloseCondFactory());
    engine->registerConditionFactory( new Napoleon::CanPerformCondFactory());
    engine->registerActionFactory( new Napoleon::AttackActionFactory());
    engine->registerConditionFactory ( new Napoleon::DeadCondFactory() );
    engine->registerConditionFactory ( new Napoleon::FacingTargetEnemCondFactory() );
    engine->registerVelModFactory ( new Napoleon::TurnModifierFactory() );
    engine->registerVelCompFactory( new Napoleon::IdleComponentFactory() );
    engine->registerVelCompFactory( new Napoleon::AimingComponentFactory() );
    engine->registerVelModFactory ( new Napoleon::PikeModifierFactory() );
    engine->registerVelModFactory ( new Napoleon::PikeCollisionModifierFactory() );
    engine->registerActionFactory( new Napoleon::PikeActionFactory() );
    engine->registerConditionFactory( new Napoleon::StaggerCondFactory() );
    engine->registerVelModFactory ( new Napoleon::StaggerModifierFactory() );
    engine->registerConditionFactory( new Napoleon::PikeCollisionCondFactory() );
    engine->registerConditionFactory( new Napoleon::UserCommandConditionFactory() );
    engine->registerTaskFactory( new Napoleon::UserCommandTaskFactory() );
  }

  // EXPORT_API void registerVisPlugin( VisPluginEngine * engine ) {
  //   engine->registerVCContext( new Napoleon::TargetVelContext() );
  // }
}
