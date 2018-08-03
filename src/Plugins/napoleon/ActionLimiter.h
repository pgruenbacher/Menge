// #include <std>
#include <map>
// meant for limiting the amount of action changes an agent can
// do. E.g. advance and retreat while within the same state.
// can also be used to track how long agent been in state.
class ActionLimiter {
  std::map<size_t, float> _map;
  float _defaultDur;
public:
  ActionLimiter();
  ActionLimiter(float dur);

  addAgentID(size_t id);
  addAgentID(size_t id, float dur);
  removeAgentID(size_t id);


}