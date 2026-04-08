/*
 * movement.c
 *
 *  Created on: Feb 4, 2026
 *      Author: luceng
 */

#include "movement.h"
#include "uart-interrupt.h"

double move_forward(oi_t *sensor_data, double distance_mm, int16_t speed, int8_t ignore_obstacles)
{
    double dist_elapsed = 0;

    oi_setWheels(speed, speed);
    // continue moving until full distance elapsed
    while (dist_elapsed < distance_mm && !(
            sensor_data->bumpLeft
            || sensor_data->bumpRight))
    {
        oi_update(sensor_data);
//        uart_printf("%f\n", sensor_data->distance);
        dist_elapsed += sensor_data->distance;
    }

    // stop moving
    oi_setWheels(0, 0);
    return distance_mm - dist_elapsed;
}

double move_backward(oi_t *sensor_data, double distance_mm, int16_t speed, int8_t ignore_obstacles)
{
    double dist_elapsed = 0;

    oi_setWheels(-speed, -speed);
    // continue moving until full distance elapsed
    while (dist_elapsed < distance_mm && (ignore_obstacles || !(
            sensor_data->bumpLeft
            || sensor_data->bumpRight
    )))
    {
        oi_update(sensor_data);
//        uart_printf("%f\n", sensor_data->distance);
        dist_elapsed -= sensor_data->distance;
    }

    // stop moving
    oi_setWheels(0, 0);
    return distance_mm - dist_elapsed;
}

double move_forward_accel(oi_t *sensor_data, double distance_mm)
{
    double dist_elapsed = 0;

    // gradually accelerate to avoid slipping
    int i;
    for (i = 0; i < MAX_SPEED && dist_elapsed < distance_mm; ++i)
    {
        oi_setWheels(i, i);
        oi_update(sensor_data);
//        uart_printf("%f\n", sensor_data->distance);
        dist_elapsed += sensor_data->distance;
    }

    // continue moving until full distance elapsed
    while (dist_elapsed < distance_mm)
    {
        oi_update(sensor_data);
//        uart_printf("%f\n", sensor_data->distance);
        dist_elapsed += sensor_data->distance;
    }

    // stop moving
    oi_setWheels(0, 0);
    return 0;
}

double turn_counterclockwise(oi_t *sensor_data, double angle, int16_t speed)
{
    double angle_elapsed = 0;

    oi_setWheels(speed, -speed);
    while (angle_elapsed < angle) {
        oi_update(sensor_data);
//        uart_printf("%f\n", sensor_data->distance);
        angle_elapsed += sensor_data->angle;
    }

    // stop moving
    oi_setWheels(0, 0);
    return angle - angle_elapsed;
}

double turn_clockwise(oi_t *sensor_data, double angle, int16_t speed) {
    double angle_elapsed = 0;

    oi_setWheels(-speed, speed);
    while (angle_elapsed < angle) {
        oi_update(sensor_data);
//        uart_printf("%f\n", sensor_data->distance);
        angle_elapsed -= sensor_data->angle;
    }

    // stop moving
    oi_setWheels(0, 0);
    return angle - angle_elapsed;
}

