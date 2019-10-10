#ifndef PHYSICS_H_
#define PHYSICS_H_

#include <cmath>
#include "game_item.h"
#include "circle.h"

template<typename T1, typename T2>
void Collision(T1* t1, T2* t2);


template<>
void Collision(Circle* c1, Circle* c2) {
  float dx = c1->x_ - c2->x_;
  float dy = c1->y_ - c2->y_;
  float norm = std::sqrt(dx*dx+dy*dy);
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

  float m1_m2 = c1->m_ + c2->m_;
  float v1p_t = v1p - ((2 * c2->m_) / (m1_m2)) * (v1p - v2p);
  float v2p_t = v2p - ((2 * c1->m_) / (m1_m2)) * (v2p - v1p);

  float v1p_t_x = v1p_t * dx;
  float v1p_t_y = v1p_t * dy;
  float v2p_t_x = v2p_t * dx;
  float v2p_t_y = v2p_t * dy;
  
  c1->vx_ = v1p_t_x + v1v_x;
  c1->vy_ = v1p_t_y + v1v_y;
  c2->vx_ = v2p_t_x + v2v_x;
  c2->vy_ = v2p_t_y + v2v_y;
}

#endif //PHYSICS_H_
