/*
 * File: scheduler.h
 *
 * Written by duvallee in 2018
 *
 */

#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__
#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

// ---------------------------------------------------------------------------
#define INFINIT_TIMER_COUNT                              0xFF
#define NO_EVENT_ID                                      0xFF

// ---------------------------------------------------------------------------
#define WAIT_EVENT_NAME_LEN                              8

#if defined(USE_USB)
#define USB_CABLE_DETECT_EVENT                           "USB_DET"
#define USB_RX_EVENT                                     "USB_RX"
#endif

// ---------------------------------------------------------------------------
typedef void (*TIMER_FN)(uint32_t system_ms);
typedef void (*WAIT_EVENT_FN)(uint8_t status);

// ---------------------------------------------------------------------------
void scheduler_init();
void scheduler_run();

// ---------------------------------------------------------------------------
int add_timer(uint32_t timer_ms, uint8_t count, TIMER_FN fn);
uint8_t add_event(uint16_t wait_ms, uint8_t status, WAIT_EVENT_FN fn, char* event_name);

uint8_t set_event(uint8_t event, uint8_t status);
uint8_t set_event_name(char* event_name, uint8_t status);

uint8_t get_event_status(uint8_t event);
uint8_t get_event_name_status(char* event_name);

#ifdef __cplusplus
}
#endif      // __cplusplus
#endif      // __SCHEDULER_H__


