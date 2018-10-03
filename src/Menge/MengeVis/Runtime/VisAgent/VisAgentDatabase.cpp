#include "MengeVis/Runtime/VisAgent/VisAgentDatabase.h"

#include "MengeVis/Runtime/VisAgent/VisAgent.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace MengeVis {

	namespace {
		// TODO: Make it so this works despite the fact that I can't use "delete" on this global
		//	member.
		Runtime::VisAgent* _defaulRenderer = 0x0;
	}	// namespace

	// Specialization
	template<> Runtime::VisAgent *
		Runtime::VisElementDB<Runtime::VisAgent, Menge::Agents::BaseAgent>::getDefaultElement() {
		if (!_defaulRenderer) {
			// kinda hacky, but the issue is that i don't want visAgent constructed when
			// i'm not using the visualizer since it was giving a memory leak at select.h and
			// i couldn't figure it out. Plus I don't want it being instantiated when using
			// just hte ismulator itself.
			_defaulRenderer = new Runtime::VisAgent();
		}
		return _defaulRenderer;
	}

	template<> void
		Runtime::VisElementDB<Runtime::VisAgent, Menge::Agents::BaseAgent>::addBuiltins() {
		//addVisElement( new Runtime::GoalVis::AABBGoalRenderer() );
	}

	template<> std::string
		Runtime::VisElementDB<Runtime::VisAgent, Menge::Agents::BaseAgent>::getElementName() {
		return "vis_agent";
	}
}
#endif	// DOXYGEN_SHOULD_SKIP_THIS
