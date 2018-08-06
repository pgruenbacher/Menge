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
// #include "VisAttackElement.h"

using MengeVis::PluginEngine::VisPluginEngine;
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
  }

  // EXPORT_API void registerVisPlugin( VisPluginEngine * engine ) {
  //   engine->registerVisElement( new Napoleon::VisAttackElement() );
  // }
}
