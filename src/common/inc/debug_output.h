/*
 * File: debug_output.h
 *
 * Written by duvallee.lee in 2018
 *
 */

#ifndef __DEBUG_OUTPUT_H__
#define __DEBUG_OUTPUT_H__

#ifdef __cplusplus
extern "C" {
#endif 

#if defined(DEBUG_STRING_LEVEL_ERROR)
#define debug_output_error(fmt, ...)                                             \
         printf("[%06d.%03d:%s-%s-%d-ERROR] " fmt,                               \
                                             (int) g_System_Start_Second,        \
                                             (int) g_System_Start_Milli_Second,  \
                                             __FILE__,                           \
                                             __func__,                           \
                                             (int) __LINE__,                     \
                                             ##__VA_ARGS__);                     \
                                             /* end of ... */
#else
#define debug_output_error(fmt, ...)         asm("nop;")
#endif

#if defined(DEBUG_STRING_LEVEL_WARN)
#define debug_output_warn(fmt, ...)                                              \
         printf("[%06d.%03d:%s-%s-%d-WARN ] " fmt,                               \
                                             (int) g_System_Start_Second,        \
                                             (int) g_System_Start_Milli_Second,  \
                                             __FILE__,                           \
                                             __func__,                           \
                                             (int) __LINE__,                     \
                                             ##__VA_ARGS__);                     \
                                             /* end of ... */
#else
#define debug_output_warn(fmt, ...)          asm("nop;")
#endif

#if defined(DEBUG_STRING_LEVEL_DEBUG)
#define debug_output_debug(fmt, ...)                                             \
         printf("[%06d.%03d:%s-%s-%d-DEBUG] " fmt,                               \
                                             (int) g_System_Start_Second,        \
                                             (int) g_System_Start_Milli_Second,  \
                                             __FILE__,                           \
                                             __func__,                           \
                                             (int) __LINE__,                     \
                                             ##__VA_ARGS__);                     \
                                             /* end of ... */
#else
#define debug_output_debug(fmt, ...)         asm("nop;")
#endif

#if defined(DEBUG_STRING_LEVEL_FN_TRACE)
#define debug_output_fn(fmt, ...)                                                \
         printf("[%06d.%03d:%s-%s-%d-FN   ] " fmt,                               \
                                             (int) g_System_Start_Second,        \
                                             (int) g_System_Start_Milli_Second,  \
                                             __FILE__,                           \
                                             __func__,                           \
                                             (int) __LINE__,                     \
                                             ##__VA_ARGS__);                     \
                                             /* end of ... */
#else
#define debug_output_fn(fmt, ...)            asm("nop;")
#endif

#if defined(DEBUG_STRING_LEVEL_INFO)
#define debug_output_info(fmt, ...)                                              \
         printf("[%06d.%03d:%s-%s-%d-INFO ] " fmt,                               \
                                             (int) g_System_Start_Second,        \
                                             (int) g_System_Start_Milli_Second,  \
                                             __FILE__,                           \
                                             __func__,                           \
                                             (int) __LINE__,                     \
                                             ##__VA_ARGS__);                     \
                                             /* end of ... */
#else
#define debug_output_info(fmt, ...)          asm("nop;")
#endif


#if defined(DEBUG_STRING_LEVEL_DUMP)
#define debug_output_dump(fmt, ...)          printf(fmt, ##__VA_ARGS__)
#else
#define debug_output_dump(fmt, ...)          asm("nop")
#endif

#if defined(SUPPORT_DEBUG_OUTPUT)
extern volatile unsigned int g_System_Start_Second;
extern volatile unsigned int g_System_Start_Milli_Second;
#endif

#ifdef __cplusplus
}
#endif

#endif      // __DEBUG_OUTPUT_H__

