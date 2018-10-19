/*************************************************************************/
/*  curve.h                                                              */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2018 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2018 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef CURVE_H
#define CURVE_H

#include <map>
#include <vector>
#include "MengeCore/Math/Vector2.h"
#include "MengeCore/resources/Resource.h"

namespace Napoleon {
using Menge::Math::Vector2;

typedef float real_t;
typedef std::vector<Vector2> PoolVector2Array;

class Curve2D : public Menge::Resource {
  struct Point {
    Vector2 in;
    Vector2 out;
    Vector2 pos;
  };

  std::vector<Point> points;

  struct BakedPoint {
    float ofs;
    Vector2 point;
  };

  mutable bool baked_cache_dirty;
  mutable PoolVector2Array baked_point_cache;
  mutable float baked_max_ofs;

  void _bake() const;

  float bake_interval;

  // Dictionary _get_data() const;
  // void _set_data(const Dictionary &p_data);

 protected:
  static void _bind_methods();

 public:
  int get_point_count() const;
  void addPoint(Vector2 v);
  static const std::string LABEL;
  const std::string& getLabel() const override { return LABEL; };
  // Vector2 interpolate(int p_index, float p_offset) const;
  // Vector2 interpolatef(real_t p_findex) const;

  void set_bake_interval(float p_tolerance);
  float get_bake_interval() const;

  float get_baked_length() const;
  Vector2 interpolate_baked(float p_offset, bool p_cubic = false) const;

  Curve2D(const std::string& fileName);
  static Menge::Resource* load(const std::string& fileName);
};

typedef Menge::ResourcePtr<Curve2D> CurvePtr;
CurvePtr loadFormation(const std::string& fileName);

}  // namespace Napoleon.

#endif  // CURVE_H
