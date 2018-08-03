#include "ActionLimiter"
#include "MengeCore/mengeCommon.h"


ActionLimiter::ActionLimiter() {
  _defaultDur = 0.3;
}
ActionLimiter::ActionLimiter(float dur) {
  _defaultDur = 0.3;
}

ActionLimiter::addAgentID(size_t id) {
  // by default the agent is available for action when first added.
  _map[id] = 0.0;
}
ActionLimiter::addAgentID(size_t id, float dur) {
  _map[id] = dur + Menge::SIM_TIME;
}
ActionLimiter::removeAgentID(size_t id) {
  _map.erase(id);
}