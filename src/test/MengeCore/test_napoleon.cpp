/*

License

Menge
Copyright © and trademark ™ 2012-14 University of North Carolina at Chapel Hill.
All rights reserved.

Permission to use, copy, modify, and distribute this software and its documentation
for educational, research, and non-profit purposes, without fee, and without a
written agreement is hereby granted, provided that the above copyright notice,
this paragraph, and the following four paragraphs appear in all copies.

This software program and documentation are copyrighted by the University of North
Carolina at Chapel Hill. The software program and documentation are supplied "as is,"
without any accompanying services from the University of North Carolina at Chapel
Hill or the authors. The University of North Carolina at Chapel Hill and the
authors do not warrant that the operation of the program will be uninterrupted
or error-free. The end-user understands that the program was developed for research
purposes and is advised not to rely exclusively on the program for any reason.

IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE AUTHORS
BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE
AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS SPECIFICALLY
DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND ANY STATUTORY WARRANTY
OF NON-INFRINGEMENT. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND
THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS HAVE NO OBLIGATIONS
TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

Any questions or comments should be sent to the authors {menge,geom}@cs.unc.edu

*/

#include "MengeCore/Agents/SimulatorInterface.h"
#include "MengeCore/Math/RandGenerator.h"
#include "MengeCore/PluginEngine/CorePluginEngine.h"
#include "MengeCore/ProjectSpec.h"
#include "MengeCore/Runtime/Logger.h"
#include "MengeCore/Runtime/os.h"
#include "MengeCore/Runtime/SimulatorDB.h"
#include "MengeCore/Agents/BaseAgent.h"
#include "Plugins/napoleon/Network.h"
#include "MengeCore/Agents/Events/EventSystem.h"


#include "MengeVis/PluginEngine/VisPluginEngine.h"
#include "MengeVis/Runtime/AgentContext/BaseAgentContext.h"
#include "MengeVis/Runtime/EventInjectContext.h"
#include "MengeVis/Runtime/MengeContext.h"
#include "MengeVis/Runtime/SimSystem.h"
#include "MengeVis/SceneGraph/ContextSwitcher.h"
#include "MengeVis/SceneGraph/GLScene.h"
#include "MengeVis/SceneGraph/TextWriter.h"
#include "MengeVis/Viewer/GLViewer.h"
#include "MengeVis/Viewer/ViewConfig.h"

#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
#include <stdio.h>

#include "gtest/gtest.h"
#include <gmock/gmock.h>


#if _MSC_VER >= 1900
FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void) { return _iob; }
#endif  // _MSC_VER >= 1900

using namespace Menge;
using Menge::PluginEngine::CorePluginEngine;
using Menge::ProjectSpec;
using MengeVis::PluginEngine::VisPluginEngine;

// Time step (in seconds)
float TIME_STEP = 0.2f;
// The number of uniform simulation steps to take between logical time steps
size_t SUB_STEPS = 0;
// Maximum duration of simulation (in seconds)
float SIM_DURATION = 800.f;
// Controls whether the simulation is verbose or not
bool VERBOSE = false;
// The location of the executable - for basic executable resources
std::string ROOT;


/*!
 *  @brief    Utility function for defining the plugin directory.
 *
 *  @param    The path to the plugins.
 */
std::string getPluginPath() {
#ifdef _WIN32
#ifdef NDEBUG
  return os::path::join( 2, ROOT.c_str(), "plugins" );
#else // NDEBUG
  return os::path::join( 3, ROOT.c_str(), "plugins", "debug" );
#endif  // NDEBUG
#else // _WIN32
  return os::path::join( 2, ROOT.c_str(), "plugins" );
#endif  // _WIN32
}

/*!
 *  @brief    Initialize and start the simulation.
 *
 *  @param    dbEntry     The SimulatorDBEntry that describes the simulator
 *                to be instantiated.
 *  @param    behaveFile    The path to a valid behavior specification file.
 *  @param    sceneFile   The path to a valid scene specification file.
 *  @param    outFile     The path to the output file to write.  If it is the
 *                empty string, no output file will be written.
 *  @param    scbVersion    The string indicating the version of scb file to write.
 *  @param    visualize   Determines if the simulation should be visualized.
 *                If true, an OpenGL visualizer is spawned, if false
 *                the simulation runs offline.
 *  @param    viewCfgFile   If visualizing, a path to an optional view configuration
 *                specification.  If none is provided, defaults are used.
 *  @param    dumpPath    The path to write screen grabs.  Only used in windows.
 *  @returns  0 for a successful run, non-zero otherwise.
 */
int simMain( SimulatorDBEntry * dbEntry, const std::string & behaveFile,
       const std::string & sceneFile, const std::string & outFile,
       const std::string & scbVersion, bool visualize, const std::string & viewCfgFile,
       const std::string & dumpPath ) {
  size_t agentCount;
  if ( outFile != "" ) {
    logger << Logger::INFO_MSG << "Attempting to write scb file: " << outFile << "\n";
  }

  using Menge::Agents::SimulatorInterface;
  using MengeVis::Runtime::BaseAgentContext;
  using MengeVis::Runtime::EventInjectionContext;
  using MengeVis::Runtime::SimSystem;
  using MengeVis::SceneGraph::Context;
  using MengeVis::SceneGraph::ContextSwitcher;
  using MengeVis::SceneGraph::GLScene;
  using MengeVis::SceneGraph::TextWriter;
  using MengeVis::Viewer::GLViewer;
  using MengeVis::Viewer::ViewConfig;

  SimulatorInterface * sim = dbEntry->getSimulator( agentCount, TIME_STEP, SUB_STEPS,
                            SIM_DURATION, behaveFile, sceneFile, outFile,
                            scbVersion, VERBOSE );


  if ( sim == 0x0 ) {
    return 1;
  }

  std::cout << "Starting...\n";

  if ( visualize ) {
    logger.line();
    logger << Logger::INFO_MSG << "Initializing visualization...";
    VisPluginEngine visPlugins;
    visPlugins.loadPlugins( getPluginPath() );

    TextWriter::setDefaultFont( os::path::join( 2, ROOT.c_str(), "arial.ttf" ) );

    ViewConfig viewCfg;
    if ( VERBOSE ) {
      logger << Logger::INFO_MSG << "Using visualizer!";
    }
    if ( viewCfgFile == "" ) {
      if ( VERBOSE ) {
        logger << Logger::INFO_MSG << "\tUsing default visualization settings.";
      }
    } else {
      // TODO: Error handling
      if ( viewCfg.readXML( viewCfgFile ) ) {
        if ( VERBOSE ) {
          logger << Logger::INFO_MSG << "\tUsing visualization from: " << viewCfgFile << "\n";
          logger << Logger::INFO_MSG << viewCfg << "\n";
        }
      } else {
        logger << Logger::ERR_MSG << "Unable to read the specified view configuration (" << viewCfgFile << "). Terminating.";
        return 1;
      }
    }
    GLViewer view( viewCfg );

    view.setDumpPath( dumpPath );

    #ifdef NDEBUG
      std::string viewTitle = "Pedestrian Simulation - " + dbEntry->viewerName();
    #else
      std::string viewTitle = "(DEBUG) Pedestrian Simulation - " + dbEntry->viewerName();
    #endif

    if ( !view.initViewer( viewTitle ) ) {
      std::cerr << "Unable to initialize the viewer\n\n";
      visualize = false;
    } else {
      GLScene * scene = new GLScene();
      SimSystem * system = new SimSystem( sim );
      system->populateScene( scene );
      std::cout << "POPULATE SCENE " << std::endl;
      scene->addSystem( system );
      std::cout << "FINISH SCENE " << std::endl;
      view.setScene( scene );
      std::cout << "SET SCENE " << std::endl;
      view.setFixedStep( TIME_STEP );
      view.setBGColor( 0.1f, 0.1f, 0.1f );
      MengeVis::Runtime::MengeContext * ctx = new MengeVis::Runtime::MengeContext( sim );
      scene->setContext( new EventInjectionContext( ctx ) );
      std::cout << "SET CONTEXT " << std::endl;
      view.newGLContext();
      logger.line();
      std::cout << "START RUN " << std::endl;
      view.run();

      delete ctx;
      delete scene; // scene will delete the system.
    }
  } else {
    bool running = true;
    while ( running ) {
      std::cout << " STEP !! " << Menge::SIM_TIME << std::endl;
      running = sim->step();
    }
  }

  std::cout << "...Finished\n";
  std::cout << "Simulation time: " << dbEntry->simDuration() << "\n";
  logger << Logger::INFO_MSG << "Simulation time: " << dbEntry->simDuration() << "\n";

  return 0;
}




void copyAgents(std::vector<Napoleon::AgentData>& result) {
  const Menge::Agents::SimulatorInterface* sim = Menge::SIMULATOR;
  for (size_t i = 0; i < sim->getNumAgents(); ++i) {
    const Menge::Agents::BaseAgent* agt = sim->getAgent(i);
    result.push_back(sim->getAgent(i));
  }
}


void cleanUp() {

  delete Menge::SIMULATOR;
  Menge::SIMULATOR = 0x0;
  Menge::SIM_TIME = 0.f;
  Menge::Math::resetGlobalSeed();

  Menge::ACTIVE_FSM = 0x0;
  Menge::SIM_TIME_STEP = 0.f;
  // delete Menge::SPATIAL_QUERY; // should be cleaned by simulator automatically.
  Menge::SPATIAL_QUERY = 0x0;
  // delete Menge::ELEVATION; // should be cleaned by simulator automatically.
  Menge::ELEVATION = 0x0;
  delete Menge::EVENT_SYSTEM;
}

int testMain( ProjectSpec& projSpec, std::vector<Napoleon::AgentData>& agentsResult ) {
  logger.setFile( "log.html" );
  // logger << Logger::INFO_MSG << "initialized logger";

  // std::string exePath( argv[ 0 ] );
  // std::string exePath;
  // std::string absExePath;
  // os::path::absPath( exePath, absExePath );
  // std::string tail;
  // os::path::split( absExePath, ROOT, tail );
  ROOT = "Exe";
  Menge::EVENT_SYSTEM = new Menge::EventSystem();
  SimulatorDB simDB;
  CorePluginEngine plugins( &simDB );
  // logger.line();
  std::string pluginPath = getPluginPath();
  logger << Logger::INFO_MSG << "Plugin path: " << pluginPath;
  plugins.loadPlugins( pluginPath );
  if ( simDB.modelCount() == 0 ) {
    logger << Logger::INFO_MSG << "There were no pedestrian models in the plugins folder\n";
    return 1;
  }

  // if ( !parseCommandParameters( argc, argv, &projSpec, simDB ) ) {
  //   return 0;
  // }

  if ( !projSpec.fullySpecified() ) {
    return 1;
  }

  VERBOSE = projSpec.getVerbosity();
  TIME_STEP = projSpec.getTimeStep();
  SUB_STEPS = projSpec.getSubSteps();
  SIM_DURATION = projSpec.getDuration();
  std::string dumpPath = projSpec.getDumpPath();
  Menge::Math::setDefaultGeneratorSeed( projSpec.getRandomSeed() );
  std::string outFile = projSpec.getOutputName();

  std::string viewCfgFile = projSpec.getView();
  // bool useVis = viewCfgFile != "";
  bool useVis = false; // testing!
  std::string model( projSpec.getModel() );

  SimulatorDBEntry * simDBEntry = simDB.getDBEntry( model );
  if ( simDBEntry == 0x0 ) {
    std::cerr << "!!!  The specified model is not recognized: " << model << "\n";
    // logger.close();
    return 1;
  }

  int result = simMain( simDBEntry, projSpec.getBehavior(), projSpec.getScene(),
              projSpec.getOutputName(), projSpec.getSCBVersion(), useVis,
              viewCfgFile, dumpPath );

  if ( result ) {
    std::cerr << "Simulation terminated through error.  See error log for details.\n";
  }
  simDB.clearEntries();
  // logger.close();
  copyAgents(agentsResult);
  cleanUp();
  return result;
}

// napoleon tests...

TEST(NapoleonTests, pikeCombat) {
  return;
  ProjectSpec projSpec;

  projSpec.loadFromXML("./examples/plugin/pikeCombat/pikeCombat.xml");
  projSpec.setDuration(400);

  std::vector<Napoleon::AgentData> agents1 = std::vector<Napoleon::AgentData>();
  std::vector<Napoleon::AgentData> agents2 = std::vector<Napoleon::AgentData>();

  int result = testMain(projSpec, agents1);
  ASSERT_EQ(result, 0);

  projSpec.loadFromXML("./examples/plugin/pikeCombat/pikeCombat.xml");
  projSpec.setDuration(400);
  result = testMain(projSpec, agents2);

  bool sim_agent_list_equal = analyzeAgentData(agents1, agents2);
  ASSERT_TRUE(sim_agent_list_equal);
  // logger.close();
}

TEST(NapoleonTests, pikeFlankCombat) {
  // return;
  ProjectSpec projSpec;

  projSpec.loadFromXML("./examples/plugin/pikeCombat/pikeCombatFlank.xml");
  projSpec.setDuration(400);

  std::vector<Napoleon::AgentData> agents1;
  std::vector<Napoleon::AgentData> agents2;

  int result = testMain(projSpec, agents1);
  ASSERT_EQ(result, 0);


  projSpec.loadFromXML("./examples/plugin/pikeCombat/pikeCombatFlank.xml");
  projSpec.setDuration(400);
  result = testMain(projSpec, agents2);
  ASSERT_EQ(result, 0);


  bool sim_agent_list_equal = analyzeAgentData(agents1, agents2);
  ASSERT_TRUE(sim_agent_list_equal);
  // logger.close();
}

TEST(NapoleonTests, formationCombat) {
  return;
  ProjectSpec projSpec;

  projSpec.loadFromXML("./examples/plugin/formationCombat.xml");
  projSpec.setDuration(400);

  std::vector<Napoleon::AgentData> agents1 = std::vector<Napoleon::AgentData>();
  std::vector<Napoleon::AgentData> agents2 = std::vector<Napoleon::AgentData>();

  int result = testMain(projSpec, agents1);
  ASSERT_EQ(result, 0);

  projSpec.loadFromXML("./examples/plugin/formationCombat.xml");
  projSpec.setDuration(400);
  result = testMain(projSpec, agents2);
  ASSERT_EQ(result, 0);


  bool sim_agent_list_equal = analyzeAgentData(agents1, agents2);
  ASSERT_TRUE(sim_agent_list_equal);
  logger.close();
}
