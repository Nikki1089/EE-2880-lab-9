/*
 * movement.h
 *
 *  Created on: Feb 4, 2026
 *      Author: luceng
 */

#ifndef LUCENG_MOVEMENT_H_
#define LUCENG_MOVEMENT_H_

#include "open_interface.h"

#define MAX_SPEED 500

double move_forward(oi_t *sensor_data, double distance_mm, int16_t speed, int8_t ignore_obstacles);
#define move_fwd move_forward
double move_backward(oi_t *sensor_data, double distance_mm, int16_t speed, int8_t ignore_obstacles);
double move_forward_accel(oi_t *sensor_data, double distance_mm);
double turn_counterclockwise(oi_t *sensor_data, double angle, int16_t speed);
#define turn_ccw turn_counterclockwise
double turn_clockwise(oi_t *sensor_data, double angle, int16_t speed);
#define turn_cw turn_clockwise

#endif
