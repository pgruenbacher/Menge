#include "Network.h"

namespace napoleon {

  // Vector2Data::Vector2Data(const Vector2& fromVector) :
  //   x(fromVector.x()),
  //   y(fromVector.y())
  // {}

  // bool Vector2Data::operator==(const Vector2& v) {
  //   return v.x() == x && v.y() == y;
  // }
  // bool Vector2Data::operator!=(const Vector2& v) {
  //   return v.x() != x || v.y() != y;
  // }
  // Vector2 Vector2Data::vector() {
  //   return Vector2(x, y);
  // }

  // PrefVelData::PrefVelData(const Menge::Agents::PrefVelocity& fromVel) :
  //   speed(fromVel.getSpeed()),
  //   preferred(fromVel.getPreferred()),
  //   target(fromVel.getTarget()),
  //   left(fromVel.getLeft()),
  //   right(fromVel.getRight())

  // {}
  // bool PrefVelData::operator!=(const Menge::Agents::PrefVelocity& v) {
  //   return speed != v.getSpeed() || preferred != v.getPreferred() ||
  //          target != v.getTarget() || left != v.getLeft() ||
  //          right != v.getRight();
  // }
  // PrefVelocity PrefVelData::prefVel() {
  //   PrefVelocity v(
  //     left.vector(),
  //     right.vector(),
  //     preferred.vector(),
  //     speed,
  //     target.vector()
  //   );
  //   return v;
  // }

  // PrefVelData::PrefVelData

  bool AgentData::operator==(const AgentData& other) const {
    return id == other.id &&
      pos == other.pos &&
      orient == other.orient &&
      vel == other.vel &&
      prefVel == other.prefVel;
  }

  AgentData::AgentData(const BaseAgent* fromAgt) :
    id(fromAgt->_id),
    pos(fromAgt->_pos),
    orient(fromAgt->_orient),
    vel(fromAgt->_vel),
    prefVel(fromAgt->_velPref)

    // velPref_x(fromAgt->_velPref.x()),
    // velPref_y(fromAgt->_velPref.y())
   {}

   bool compare(const AgentData& d, const AgentData& d2) {
    bool valid = true;
    if (d2.pos != d.pos) {
      std::cout << "pos " << std::endl;
    }
    if (d2.orient != d.orient) {
      std::cout << "bad orient " << d2.orient << " " << d.orient << std::endl;
    }
    if (d2.vel != d.vel) {
      std::cout << "vel not valid " << std::endl;
      std::cout << "vel " << d2.vel.x() << " " << d2.vel.y() << " " << d.vel.x() << " " << d.vel.y() << std::endl;
    }
    if (d2.prefVel != d.prefVel) {
      std::cout << "pref vel not valid " << std::endl;
      // std::cout << "vel " << d2.vel.x() << " " << d2.vel.y() << " " << d.vel.x() << " " << d.vel.y() << std::endl;
    }
    return valid;
   }

   bool analyzeAgentData(const std::vector<AgentData>& d, const std::vector<AgentData>& d2) {
    if (d.size() != d2.size()) {
      std::cout << " Size doesn't match " << std::endl;
      return false;
    }

    int num_invalid = 0;
    bool already_printed = false;
    for (int i = 0; i < d.size(); ++ i) {
      if (!(d[i] == d2[i])) {
        if (!already_printed) {
          printf("%d index diverges.", i);
          compare(d2[i], d[i]);
          already_printed = true;
        }
        ++num_invalid;
      } else {

      }
    }
    if (num_invalid > 0) {
      printf("%d of %lu are invalid", num_invalid, d.size());
      return false;
    }
    return true;
   }

   bool AgentData::updateAgent(BaseAgent* toAgent) {
    bool valid = true;
    if (pos != toAgent->_pos) {
      valid = false;
      toAgent->_pos = pos;
    }
    if (orient != toAgent->_orient) {
      valid = false;
      toAgent->_orient = orient;
    }
    if (vel != toAgent->_vel) {
      valid = false;
      toAgent->_vel = vel;
    }
    if (prefVel != toAgent->_velPref) {
      valid = false;
      toAgent->_velPref = prefVel;
    }
    return valid;
   }
}
