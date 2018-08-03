#include "ActionLimiter.h"
#include "MengeCore/mengeCommon.h"
#include "MengeCore/Core.h"


ActionLimiter::ActionLimiter() {
  _defaultDur = 0.3;
}
ActionLimiter::ActionLimiter(float dur) {
  _defaultDur = 0.3;
}

bool ActionLimiter::addAgentID(size_t id) {
  // by default the agent is available for action when first added.
  _map[id] = 0.0;
}
bool ActionLimiter::addAgentID(size_t id, float dur) {
  _map[id] = dur + Menge::SIM_TIME;
}
bool ActionLimiter::removeAgentID(size_t id) {
  _map.erase(id);
}

bool ActionLimiter::resetAgentID(size_t id) {
  _map[id] = Menge::SIM_TIME + _defaultDur;
}

bool ActionLimiter::resetAgentID(size_t id, float dur) {
  _map[id] =Menge::SIM_TIME + dur;
}

bool ActionLimiter::isAgentTimeout(size_t id) {
  return Menge::SIM_TIME > _map[id];
}