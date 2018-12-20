/*
 * File: cleanflight_cli.c
 *
 * Written by duvallee.lee in 2018
 *
 */
#include "main.h"
#if (defined(USE_USB_CDC_DEVICE) || defined(USE_USB_BULK_DEVICE))
#include "usb_device.h"
#endif

// --------------------------------------------------------------------------
#define CLI_IN_BUFFER_SIZE                               256
static char cliBuffer[CLI_IN_BUFFER_SIZE];

// --------------------------------------------------------------------------
typedef void (*CLI_FUNCTION)(char*);

typedef struct
{
   const char *name;
   const char *description;
   const char *args;
   CLI_FUNCTION func;
} clicmd_t;

#define CLI_COMMAND_DEF(name, description, args, method)          \
{                                                                 \
   name,                                                          \
   description,                                                   \
   args,                                                          \
   method                                                         \
}

// --------------------------------------------------------------------------

/* --------------------------------------------------------------------------
 * Name : cliPrint()
 *
 *
 * -------------------------------------------------------------------------- */
static void cliPrint(const char *str)
{
   usb_write(str, strlen(str));
}

/* --------------------------------------------------------------------------
 * Name : cliPrintLinefeed()
 *
 *
 * -------------------------------------------------------------------------- */
static void cliPrintLinefeed(void)
{
   cliPrint("\r\n");
}

/* --------------------------------------------------------------------------
 * Name : cliPrintHashLine()
 *
 *
 * -------------------------------------------------------------------------- */
static void cliPrintHashLine(const char *str)
{
   cliPrint("\r\n# ");
   cliPrintLine(str);
}

/* --------------------------------------------------------------------------
 * Name : cliPutp()
 *
 *
 * -------------------------------------------------------------------------- */
static void cliPutp(void *p, char ch)
{
//   bufWriterAppend(p, ch);
}

/* --------------------------------------------------------------------------
 * Name : cliPutp()
 *
 *
 * -------------------------------------------------------------------------- */
static void cliPrintfva(const char *format, va_list va)
{
static char buffer[100];

   vsnprintf(buffer, 100, format, va);
   cliPrint(buffer);
}

/* --------------------------------------------------------------------------
 * Name : isEmpty()
 *  Check if a string's length is zero
 *
 * -------------------------------------------------------------------------- */
static int isEmpty(const char *string)
{
   return (string == NULL || *string == '\0') ? 1 : 0;
}

/* --------------------------------------------------------------------------
 * Name : nextArg()
 *
 *
 * -------------------------------------------------------------------------- */
static const char *nextArg(const char *currentArg)
{
   const char *ptr                                       = strchr(currentArg, ' ');
   while (ptr && *ptr == ' ')
   {
      ptr++;
   }

   return ptr;
}

// --------------------------------------------------------------------------
void cliAdjustmentRange(char *cmdline);
void cliAux(char *cmdline);
void cliBootloader(char *cmdline);
void cliBoardName(char *cmdline);
void cliDefaults(char *cmdline);
void cliDiff(char *cmdline);
void cliBeacon(char *cmdline);
void cliBeeper(char *cmdline);
void cliDshotProg(char *cmdline);
void cliDump(char *cmdline);
void cliExit(char *cmdline);
void cliGet(char *cmdline);
void cliFeature(char *cmdline);
void cliDumpGyroRegisters(char *cmdline);
void cliHelp(char *cmdline);
void cliMap(char *cmdline);
void cliMcuId(char *cmdline);
void cliMixer(char *cmdline);
void cliMotorMix(char *cmdline);
void cliName(char *cmdline);
void cliProfile(char *cmdline);
void cliRateProfile(char *cmdline);
void cliLed(char *cmdline);
void cliManufacturerId(char *cmdline);
void cliMotor(char *cmdline);
void cliRxFailsafe(char *cmdline);
void cliRxRange(char *cmdline);
void cliSave(char *cmdline);
void cliSerial(char *cmdline);
void cliSet(char *cmdline);
void cliStatus(char *cmdline);
void cliVersion(char *cmdline);


// --------------------------------------------------------------------------
const clicmd_t cmdTable[]                                =
{
   CLI_COMMAND_DEF("adjrange",         "configure adjustment ranges",                  NULL,                                                 cliAdjustmentRange),
   CLI_COMMAND_DEF("aux",              "configure modes",                              "<index> <mode> <aux> <start> <end> <logic>",         cliAux),
   CLI_COMMAND_DEF("bl",               "reboot into bootloader",                       NULL,                                                 cliBootloader),
   CLI_COMMAND_DEF("board_name",       "get / set the name of the board model",        "[board name]",                                       cliBoardName),
   CLI_COMMAND_DEF("defaults",         "reset to defaults and reboot",                 "[nosave]",                                           cliDefaults),
   CLI_COMMAND_DEF("diff",             "list configuration changes from default",      "[master|profile|rates|all] {defaults}",              cliDiff),
   CLI_COMMAND_DEF("beacon",           "enable/disable Dshot beacon for a condition",  "list\r\n\t<->[name]",                                cliBeacon),
   CLI_COMMAND_DEF("beeper",           "enable/disable beeper for a condition",        "list\r\n\t<->[name]",                                cliBeeper),
   CLI_COMMAND_DEF("dshotprog",        "program DShot ESC(s)",                         "<index> <command>+",                                 cliDshotProg),
   CLI_COMMAND_DEF("dump",             "dump configuration",                           "[master|profile|rates|all] {defaults}",              cliDump),
   CLI_COMMAND_DEF("exit",             NULL,                                           NULL,                                                 cliExit),
   CLI_COMMAND_DEF("get",              "get variable value",                           "[name]",                                             cliGet),
   CLI_COMMAND_DEF("feature",          "configure features",                           "list\r\n\t<+|->[name]",                              cliFeature),
   CLI_COMMAND_DEF("gyroregisters",    "dump gyro config registers contents",          NULL,                                                 cliDumpGyroRegisters),
   CLI_COMMAND_DEF("help",             NULL,                                           NULL,                                                 cliHelp),
   CLI_COMMAND_DEF("map",              "configure rc channel order",                   "[<map>]",                                            cliMap),
   CLI_COMMAND_DEF("mcu_id",           "id of the microcontroller",                    NULL,                                                 cliMcuId),
   CLI_COMMAND_DEF("mixer",            "configure mixer",                              "list\r\n\t<name>",                                   cliMixer),
   CLI_COMMAND_DEF("mmix",             "custom motor mixer",                           NULL,                                                 cliMotorMix),
   CLI_COMMAND_DEF("name",             "name of craft",                                NULL,                                                 cliName),
   CLI_COMMAND_DEF("profile",          "change profile",                               "[<index>]",                                          cliProfile),
   CLI_COMMAND_DEF("rateprofile",      "change rate profile",                          "[<index>]",                                          cliRateProfile),
   CLI_COMMAND_DEF("led",              "configure leds",                               NULL,                                                 cliLed),
   CLI_COMMAND_DEF("manufacturer_id",  "get / set the id of the board manufacturer",   "[manufacturer id]",                                  cliManufacturerId),
   CLI_COMMAND_DEF("motor",            "get/set motor",                                "<index> [<value>]",                                  cliMotor),
   CLI_COMMAND_DEF("rxfail",           "show/set rx failsafe settings",                NULL,                                                 cliRxFailsafe),
   CLI_COMMAND_DEF("rxrange",          "configure rx channel ranges",                  NULL,                                                 cliRxRange),
   CLI_COMMAND_DEF("save",             "save and reboot",                              NULL,                                                 cliSave),
   CLI_COMMAND_DEF("serial",           "configure serial ports",                       NULL,                                                 cliSerial),
   CLI_COMMAND_DEF("set",              "change setting",                               "[<name>=<value>]",                                   cliSet),
   CLI_COMMAND_DEF("status",           "show status",                                  NULL,                                                 cliStatus),
   CLI_COMMAND_DEF("version",          "show version",                                 NULL,                                                 cliVersion),
};


/* --------------------------------------------------------------------------
 * Name : cliAdjustmentRange()
 *
 *
 * -------------------------------------------------------------------------- */
void cliAdjustmentRange(char *cmdline)
{
#if 0
   const char *format                                    = "adjrange %u %u %u %u %u %u %u %u %u";
   int i, val                                            = 0;
   const char *ptr;
   if (isEmpty(cmdline))
   {
//      printAdjustmentRange(DUMP_MASTER, adjustmentRanges(0), NULL);
   }
   else
   {
      ptr                                                = cmdline;
      i                                                  = atoi(ptr++);
      if (i < MAX_ADJUSTMENT_RANGE_COUNT)
      {
         adjustmentRange_t *ar                           = adjustmentRangesMutable(i);
         uint8_t validArgumentCount                      = 0;

         ptr                                             = nextArg(ptr);
         if (ptr)
         {
            val                                          = atoi(ptr);
            if (val >= 0 && val < MAX_SIMULTANEOUS_ADJUSTMENT_COUNT)
            {
               ar->adjustmentIndex                       = val;
               validArgumentCount++;
            }
         }
         ptr                                             = nextArg(ptr);
         if (ptr)
         {
            val                                          = atoi(ptr);
            if (val >= 0 && val < MAX_AUX_CHANNEL_COUNT)
            {
               ar->auxChannelIndex                       = val;
               validArgumentCount++;
            }
         }

         ptr                                             = processChannelRangeArgs(ptr, &ar->range, &validArgumentCount);

         ptr                                             = nextArg(ptr);
         if (ptr)
         {
            val                                          = atoi(ptr);
            if (val >= 0 && val < ADJUSTMENT_FUNCTION_COUNT)
            {
               ar->adjustmentFunction                    = val;
               validArgumentCount++;
            }
         }
         ptr                                             = nextArg(ptr);
         if (ptr)
         {
            val                                          = atoi(ptr);
            if (val >= 0 && val < MAX_AUX_CHANNEL_COUNT)
            {
               ar->auxSwitchChannelIndex                 = val;
               validArgumentCount++;
            }
         }

         if (validArgumentCount != 6)
         {
            memset(ar, 0, sizeof(adjustmentRange_t));
            cliShowParseError();
            return;
         }

         // Optional arguments
         ar->adjustmentCenter                            = 0;
         ar->adjustmentScale                             = 0;

         ptr                                             = nextArg(ptr);
         if (ptr)
         {
            val                                          = atoi(ptr);
            ar->adjustmentCenter                         = val;
            validArgumentCount++;
         }
         ptr                                             = nextArg(ptr);
         if (ptr)
         {
            val                                          = atoi(ptr);
            ar->adjustmentScale                          = val;
            validArgumentCount++;
         }
         cliDumpPrintLinef(0, false, format, i, ar->adjustmentIndex, ar->auxChannelIndex,
                                                MODE_STEP_TO_CHANNEL_VALUE(ar->range.startStep),
                                                MODE_STEP_TO_CHANNEL_VALUE(ar->range.endStep),
                                                ar->adjustmentFunction,
                                                ar->auxSwitchChannelIndex,
                                                ar->adjustmentCenter,
                                                ar->adjustmentScale);

      }
      else
      {
         cliShowArgumentRangeError("index", 0, MAX_ADJUSTMENT_RANGE_COUNT - 1);
      }
   }
#endif
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliAux(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliBootloader(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliBoardName(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliDefaults(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliDiff(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliBeacon(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliBeeper(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliDshotProg(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliDump(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliExit(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliGet(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliFeature(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliDumpGyroRegisters(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliHelp(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliMap(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliMcuId(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliMixer(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliMotorMix(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliName(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliProfile(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliRateProfile(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliLed(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliManufacturerId(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliMotor(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliRxFailsafe(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliRxRange(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliSave(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliSerial(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliSet(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliStatus(char *cmdline)
{
}

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cliVersion(char *cmdline)
{
}




#if defined(RTOS_FREERTOS)
#if (defined(USE_USB_CDC_DEVICE) || defined(USE_USB_BULK_DEVICE))
static SemaphoreHandle_t g_usb_event_semaphore           = NULL;
#endif

/* --------------------------------------------------------------------------
 * Name : cleanflight_cli_task()
 *
 *
 * -------------------------------------------------------------------------- */
void cleanflight_cli_task(void const* argument)
{
   int receive_data                                      = 0;
   while (1)
   {
#if (defined(USE_USB_CDC_DEVICE) || defined(USE_USB_BULK_DEVICE))
      if (xSemaphoreTake(g_usb_event_semaphore, portMAX_DELAY) == pdPASS)
      {
         taskENTER_CRITICAL();
         receive_data                                    = usb_get_data((byte*) cliBuffer, sizeof(cliBuffer));
         if (receive_data < 0)
         {
            debug_output_warn("... \r\n");
            osDelay(5);
            taskEXIT_CRITICAL();
            continue;
         }
         taskEXIT_CRITICAL();
      }
      else
      {
         debug_output_warn("time-out in cli \r\n");
      }
#endif
      osDelay(5);
   }
}
#endif


/* --------------------------------------------------------------------------
 * Name : cleanflight_cliInit()
 *
 *
 * -------------------------------------------------------------------------- */
void cleanflight_cliInit()
{
#if defined(RTOS_FREERTOS)
#if (defined(USE_USB_CDC_DEVICE) || defined(USE_USB_BULK_DEVICE))
    g_usb_event_semaphore                                = get_usb_device_semaphore();
#endif

   // --------------------------------------------------------------------------
   // Thread definition for tcp server
   osThreadDef(cleanflight_cli_task, cleanflight_cli_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
   if (osThreadCreate(osThread(cleanflight_cli_task), (void *) NULL) == NULL)
   {
      debug_output_error("Can't create thread : cleanflight_cli_task !!!");
   }
#else

#endif
}


