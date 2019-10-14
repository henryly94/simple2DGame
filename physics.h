#ifndef PHYSICS_H_
#define PHYSICS_H_

#include "circle.h"
#include "game_item.h"
#include "line.h"
#include <cmath>
#include <iostream>

template <typename T1, typename T2> bool Collision(T1 *t1, T2 *t2);

template <> bool Collision(Circle *c1, Circle *c2) {
  float dx = c1->x_ - c2->x_;
  float dy = c1->y_ - c2->y_;
  float distance = std::sqrt(dx * dx + dy * dy);
  if (distance > c1->radius_ + c2->radius_)
    return false;
  float norm = std::sqrt(dx * dx + dy * dy);
  dx /= norm;
  dy /= norm;
  float v1p = c1->vx_ * dx + c1->vy_ * dy;
  float v1p_x = v1p * dx;
  float v1p_y = v1p * dy;
  float v1v_x = c1->vx_ - v1p_x;
  float v1v_y = c1->vy_ - v1p_y;

  float v2p = c2->vx_ * dx + c2->vy_ * dy;
  float v2p_x = v2p * dx;
  float v2p_y = v2p * dy;
  float v2v_x = c2->vx_ - v2p_x;
  float v2v_y = c2->vy_ - v2p_y;

  if (!(v2p > v1p))
    return true;

  float m1_m2 = c1->m_ + c2->m_;
  float v1p_t = v1p - ((1.8 * c2->m_) / (m1_m2)) * (v1p - v2p);
  float v2p_t = v2p - ((1.8 * c1->m_) / (m1_m2)) * (v2p - v1p);

  float v1p_t_x = v1p_t * dx;
  float v1p_t_y = v1p_t * dy;
  float v2p_t_x = v2p_t * dx;
  float v2p_t_y = v2p_t * dy;

  c1->vx_ = v1p_t_x + v1v_x;
  c1->vy_ = v1p_t_y + v1v_y;
  c2->vx_ = v2p_t_x + v2v_x;
  c2->vy_ = v2p_t_y + v2v_y;
  return true;
}

template <> bool Collision(Circle *c, Line *l) {
  float c_1_x = c->x_ - l->x1_;
  float c_1_y = c->y_ - l->y1_;

  float dx_l = l->x2_ - l->x1_;
  float dy_l = l->y2_ - l->y1_;
  float norm_l = std::sqrt(dx_l * dx_l + dy_l * dy_l);
  dx_l /= norm_l;
  dy_l /= norm_l;

  float c_1_p = c_1_x * dx_l + c_1_y * dy_l;
  float foot_x = c_1_p * dx_l + l->x1_;
  float foot_y = c_1_p * dy_l + l->y1_;

  float a_x = foot_x - l->x1_;
  float a_y = foot_y - l->y1_;
  float b_x = foot_x - l->x2_;
  float b_y = foot_y - l->y2_;
  bool between = (a_x * b_x + a_y * b_y) <= 0;

  float dx_v = c->x_ - foot_x;
  float dy_v = c->y_ - foot_y;
  float distance = std::sqrt(dx_v * dx_v + dy_v * dy_v);
  if (!(distance <= c->radius_ && between))
    return false;
  if (dx_v * c->vx_ + dy_v * c->vy_ > 0)
    return true;

  dx_v /= distance;
  dy_v /= distance;
  float v_p = c->vx_ * dx_v + c->vy_ * dy_v;
  float v_p_x = v_p * dx_v;
  float v_p_y = v_p * dy_v;

  float v_v_x = c->vx_ - v_p_x;
  float v_v_y = c->vy_ - v_p_y;

  float v_p_x_t = -0.9 * v_p_x;
  float v_p_y_t = -0.9 * v_p_y;
  c->vx_ = v_p_x_t + v_v_x;
  c->vy_ = v_p_y_t + v_v_y;
  return true;
}

template <> bool Collision(Line *l, Circle *c) { return Collision(c, l); }

#endif // PHYSICS_H_
