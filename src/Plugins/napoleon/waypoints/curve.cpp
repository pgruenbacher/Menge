/*************************************************************************/
/*  curve.cpp                                                            */
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

#include "curve.h"
#include "MengeCore/Runtime/Logger.h"

#define CMP_EPSILON 0.00001
namespace Napoleon {
using Menge::Logger;
using Menge::logger;

const std::string Curve2D::LABEL("formation");

template <class T>
T _bezier_interp(real_t t, T start, T control_1, T control_2, T end) {
  /* Formula from Wikipedia article on Bezier curves. */
  real_t omt = (1.0 - t);
  real_t omt2 = omt * omt;
  real_t omt3 = omt2 * omt;
  real_t t2 = t * t;
  real_t t3 = t2 * t;

  return start * omt3 + control_1 * omt2 * t * 3.0 +
         control_2 * omt * t2 * 3.0 + end * t3;
}


// float lerp(float p_from, float p_to, float p_weight) { return p_from + (p_to
// - p_from) * p_weight; }

Vector2 cubic_interpolate(const Vector2& p_a, const Vector2& p_b,
                          const Vector2& p_pre_a, const Vector2& p_post_b,
                          real_t p_t) {
  Vector2 p0 = p_pre_a;
  Vector2 p1 = p_a;
  Vector2 p2 = p_b;
  Vector2 p3 = p_post_b;

  real_t t = p_t;
  real_t t2 = t * t;
  real_t t3 = t2 * t;

  Vector2 out;
  out = ((p1 * 2.0) + ((-p0 + p2) * t) +
         ((p0 * 2.0 - p1 * 5.0 + p2 * 4 - p3) * t2) +
         ((-p0 + p1 * 3.0 - p2 * 3.0 + p3) * t3)) *
        0.5;
  return out;
}

int Curve2D::get_point_count() const { return points.size(); }

void Curve2D::addPoint(Vector2 pt) {
  Point point;
  point.pos = pt;
  points.push_back(point);
  baked_cache_dirty = true;
}

void Curve2D::_bake() const {
  if (!baked_cache_dirty) return;

  baked_max_ofs = 0;
  baked_cache_dirty = false;

  if (points.size() == 0) {
    baked_point_cache.resize(0);
    return;
  }

  if (points.size() == 1) {
    baked_point_cache.resize(1);
    baked_point_cache[0] = points[0].pos;
    return;
  }

  Vector2 pos = points[0].pos;
  std::vector<Vector2> pointlist;

  pointlist.push_back(pos);  // start always from origin

  for (int i = 0; i < points.size() - 1; i++) {
    float step = 0.1;  // at least 10 substeps ought to be enough?
    float p = 0;

    int limit = 0;
    const int max_limit = 10;
    while (p < 1.0 && limit < max_limit) {
      limit++;
      if (limit == max_limit) {
        std::cout << "HIT MAX LIMIT !! " << bake_interval << std::endl;
      }
      float np = p + step;
      if (np > 1.0) np = 1.0;

      Vector2 npp = _bezier_interp(
          np, points[i].pos, points[i].pos + points[i].out,
          points[i + 1].pos + points[i + 1].in, points[i + 1].pos);
      float d = pos.distance(npp);
      if (d > bake_interval) {
        // OK! between P and NP there _has_ to be Something, let's go searching!

        int iterations = 10;  // lots of detail!

        float low = p;
        float hi = np;
        float mid = low + (hi - low) * 0.5;

        for (int j = 0; j < iterations; j++) {
          npp = _bezier_interp(
              mid, points[i].pos, points[i].pos + points[i].out,
              points[i + 1].pos + points[i + 1].in, points[i + 1].pos);
          d = pos.distance(npp);

          if (bake_interval < d)
            hi = mid;
          else
            low = mid;
          mid = low + (hi - low) * 0.5;
        }

        pos = npp;
        p = mid;
        pointlist.push_back(pos);
      } else {
        p = np;
      }
    }
  }

  Vector2 lastpos = points[points.size() - 1].pos;

  float rem = pos.distance(lastpos);
  baked_max_ofs = (pointlist.size() - 1) * bake_interval + rem;
  pointlist.push_back(lastpos);

  baked_point_cache.resize(pointlist.size());
  // PoolVector2Array::Write w = baked_point_cache.write();
  PoolVector2Array& w = baked_point_cache;
  int idx = 0;

  for (const Vector2& e : pointlist) {
    w[idx] = e;
    idx++;
  }

  std::cout << " FINISH BAKE " << std::endl;
}

float Curve2D::get_baked_length() const {
  if (baked_cache_dirty) _bake();

  return baked_max_ofs;
}
Vector2 Curve2D::interpolate_baked(float p_offset, bool p_cubic) const {
  if (baked_cache_dirty) _bake();
  // validate//
  int pc = baked_point_cache.size();
  if (pc == 0) {
    // ERR_EXPLAIN("No points in Curve2D");
    // ERR_FAIL_COND_V(pc == 0, Vector2());
  }

  if (pc == 1) return baked_point_cache[0];

  int bpc = baked_point_cache.size();
  PoolVector2Array& r = baked_point_cache;
  // PoolVector2Array::Read r = baked_point_cache.read();

  if (p_offset < 0) return r[0];
  if (p_offset >= baked_max_ofs) return r[bpc - 1];

  int idx = floor((double)p_offset / (double)bake_interval);
  float frac = fmod(p_offset, (float)bake_interval);

  if (idx >= bpc - 1) {
    return r[bpc - 1];
  } else if (idx == bpc - 2) {
    frac /= fmod(baked_max_ofs, bake_interval);
  } else {
    frac /= bake_interval;
  }

  if (p_cubic) {
    Vector2 pre = idx > 0 ? r[idx - 1] : r[idx];
    Vector2 post = (idx < (bpc - 2)) ? r[idx + 2] : r[idx + 1];
    return cubic_interpolate(r[idx], r[idx + 1], pre, post, frac);
  } else {
    return r[idx].linear_interpolate(r[idx + 1], frac);
  }
}

void Curve2D::set_bake_interval(float p_tolerance) {
  bake_interval = p_tolerance;
  baked_cache_dirty = true;
  // emit_signal(CoreStringNames::get_singleton()->changed);
}

float Curve2D::get_bake_interval() const { return bake_interval; }

Curve2D::Curve2D(const std::string& name) : Menge::Resource(name) {
  baked_cache_dirty = false;
  baked_max_ofs = 0;
  bake_interval = 5;
}

Menge::Resource* Curve2D::load(const std::string& fileName) {
  // TODO: Change this to support comments.
  std::ifstream f;
  f.open(fileName.c_str(), std::ios::in);

  if (!f.is_open()) {
    logger << Logger::ERR_MSG << "Error opening waypoint file: " << fileName
           << ".";
    return 0x0;
  }

  Curve2D* crv = new Curve2D(fileName);

  float x, y;

  // while we have points left, they are internal points
  // if ((f >> x >> y)) {
  while (!f.eof()) {
    f >> x >> y;
    crv->addPoint(Vector2(x, y));
    std::cout << " ADD POINT " << x << " " << y << std::endl;
  }
  // }
  return crv;
}

CurvePtr loadCurve(const std::string& fileName) {
  Menge::Resource* rsrc =
      Menge::ResourceManager::getResource(fileName, &Curve2D::load, Curve2D::LABEL);
  if (rsrc == 0x0) {
    logger << Logger::ERR_MSG << "No resource available.";
    throw Menge::ResourceException();
  }
  Curve2D* form = dynamic_cast<Curve2D*>(rsrc);
  if (form == 0x0) {
    logger << Logger::ERR_MSG << "Resource with name " << fileName
           << " is not a formation.";
    throw Menge::ResourceException();
  }

  return CurvePtr(form);
}

}  // namespace Napoleon