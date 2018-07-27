#include "selectStateByClass.h"
#include "MengeCore/PluginEngine/CorePluginEngine.h"
#include "napoleonConfig.h"

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
  }
}
