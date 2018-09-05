#include "Network.h"

namespace napoleon {

  Vector2Data::Vector2Data(const Vector2& fromVector) :
    x(fromVector.x()),
    y(fromVector.y())
  {}

  bool Vector2Data::operator==(const Vector2& v) {
    return v.x() == x && v.y() == y;
  }
  bool Vector2Data::operator!=(const Vector2& v) {
    return v.x() != x || v.y() != y;
  }
  Vector2 Vector2Data::vector() {
    return Vector2(x, y);
  }

  PrefVelData::PrefVelData(const Menge::Agents::PrefVelocity& fromVel) :
    speed(fromVel.getSpeed()),
    preferred(fromVel.getPreferred()),
    target(fromVel.getTarget()),
    left(fromVel.getLeft()),
    right(fromVel.getRight())

  {}
  bool PrefVelData::operator!=(const Menge::Agents::PrefVelocity& v) {
    return speed != v.getSpeed() || preferred != v.getPreferred() ||
           target != v.getTarget() || left != v.getLeft() ||
           right != v.getRight();
  }
  PrefVelocity PrefVelData::prefVel() {
    PrefVelocity v(
      left.vector(),
      right.vector(),
      preferred.vector(),
      speed,
      target.vector()
    );
    return v;
  }

  AgentData::AgentData(const BaseAgent& fromAgt) :
    id(fromAgt._id),
    pos(fromAgt._pos),
    orient(fromAgt._orient),
    vel(fromAgt._vel),
    prefVel(fromAgt._velPref)

    // velPref_x(fromAgt._velPref.x()),
    // velPref_y(fromAgt._velPref.y())
   {}

   bool AgentData::updateAgent(BaseAgent& toAgent) {
    bool valid = true;
    if (pos != toAgent._pos) {
      valid = false;
      toAgent._pos = pos.vector();
    }
    if (orient != toAgent._orient) {
      valid = false;
      toAgent._orient = orient.vector();
    }
    if (vel != toAgent._vel) {
      valid = false;
      toAgent._vel = vel.vector();
    }
    if (prefVel != toAgent._velPref) {
      valid = false;
      toAgent._velPref = prefVel.prefVel();
    }
    return valid;
   }

}