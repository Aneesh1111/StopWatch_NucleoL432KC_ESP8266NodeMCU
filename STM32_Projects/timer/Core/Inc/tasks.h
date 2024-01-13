/*
 * tasks.hpp
 *
 *  Created on: 29 Jan 2021
 *      Author: harbanse
 */

#ifndef INC_TASKS_H_
#define INC_TASKS_H_

#ifdef __cplusplus
extern "C" {
#endif

void LD3_task(void *pvParameters);
void LD4_task(void *pvParameters);
void PWM_task(void *pvParameters);
//void OLED_task(void *pvParameters);
void Timer_task(void *pvParameters);

#ifdef __cplusplus
}
#endif


#endif /* INC_TASKS_H_ */
