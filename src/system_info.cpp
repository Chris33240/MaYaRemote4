#include "system_info.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

/// @brief Affiche des informations système de base.
/// Utilise les fonctions ESP-IDF et FreeRTOS pour récupérer ces données.
void printSystemInfo()
{
        // ===== RAM =====
    Serial.println(F("===== System Information ====="));
    Serial.printf("Free Heap             : %u bytes\n", ESP.getFreeHeap());
    Serial.printf("Total Heap            : %u bytes\n", ESP.getHeapSize());
    Serial.printf("Stack High Water Mark : %u words\n", uxTaskGetStackHighWaterMark(nullptr));

    // ===== Flash =====
    Serial.printf("Flash Size            : %u MB\n", ESP.getFlashChipSize() / (1024 * 1024));
    Serial.printf("Flash Speed           : %u MHz\n", ESP.getFlashChipSpeed() / 1000000);
    Serial.printf("Flash Mode            : %d\n", ESP.getFlashChipMode());
    Serial.printf("Flash Sector Size     : %u bytes\n", SPI_FLASH_SEC_SIZE);

    // ===== CPU =====
    Serial.printf("CPU Frequency         : %u MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("Chip Revision         : %u\n", ESP.getChipRevision());

    // ===== Firmware =====
    Serial.printf("Sketch Size           : %u bytes\n", ESP.getSketchSize());
    Serial.printf("Free Sketch Space     : %u bytes\n", ESP.getFreeSketchSpace());

    Serial.println();

    // Débogage avancé (à activer si nécessaire)
    // heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);
    // Serial.printf("configMINIMAL_STACK_SIZE : %u\n", configMINIMAL_STACK_SIZE);
}

// #define MAX_TASKS 10

// void printTaskInfo(TaskStatus_t *pxTaskStatusArray, UBaseType_t uxArraySize, uint32_t ulTotalRunTime) {
//    for (UBaseType_t x = 0; x < uxArraySize; x++) {
//      Serial.print("Task name: ");
//      Serial.print(pxTaskStatusArray[x].pcTaskName);
//      Serial.print(", Task number: ");
//      Serial.print(pxTaskStatusArray[x].xTaskNumber);
//      Serial.print(", Task state: ");
//      Serial.print(pxTaskStatusArray[x].eCurrentState);
//      Serial.print(", Stack high water mark: ");
//      Serial.print(pxTaskStatusArray[x].usStackHighWaterMark);
//      Serial.print(", Runtime: ");
//      Serial.print(pxTaskStatusArray[x].ulRunTimeCounter);
//      Serial.println(" ticks");
//    }

//   if (ulTotalRunTime > 0) {
//     Serial.print("Total runtime: ");
//     Serial.print(ulTotalRunTime);
//     Serial.println(" ticks");
//   }
// }

// void taskSystemInfo(void *pvParameters) {
//   TaskStatus_t xTaskStatusArray[MAX_TASKS];
//   UBaseType_t uxArraySize;
//   uint32_t ulTotalRunTime;

//   while (1) {
//     uxArraySize = uxTaskGetNumberOfTasks();

//     if (uxArraySize > MAX_TASKS) {
//       uxArraySize = MAX_TASKS;
//     }

//     uxArraySize = uxTaskGetSystemState(xTaskStatusArray, uxArraySize, &ulTotalRunTime);

//     printTaskInfo(xTaskStatusArray, uxArraySize, ulTotalRunTime);

//     vTaskDelay(pdMS_TO_TICKS(5000));
//   }
// }

/* Utilisation :
void setup() {
  Serial.begin(115200);
  delay(1000);

  xTaskCreate(taskSystemInfo, "SystemInfo", 2048, NULL, 1, NULL);
}

void loop() {
  // La boucle principale reste vide car nous utilisons des tâches FreeRTOS
}
*/

/*
 // This example demonstrates how a human readable table of run time stats
 // information is generated from raw data provided by uxTaskGetSystemState().
 // The human readable table is written to pcWriteBuffer
 void vTaskGetRunTimeStats( char *pcWriteBuffer )
 {
 TaskStatus_t *pxTaskStatusArray;
 volatile UBaseType_t uxArraySize, x;
 configRUN_TIME_COUNTER_TYPE ulTotalRunTime, ulStatsAsPercentage;

// Make sure the write buffer does not contain a string.
pcWriteBuffer = 0x00;

// Take a snapshot of the number of tasks in case it changes while this
// function is executing.
     uxArraySize = uxTaskGetNumberOfTasks();

// Allocate a TaskStatus_t structure for each task.  An array could be
// allocated statically at compile time.
     pxTaskStatusArray = pvPortMalloc( uxArraySize * sizeof( TaskStatus_t ) );

if( pxTaskStatusArray != NULL )
     {
// Generate raw status information about each task.
         uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, &ulTotalRunTime );

// For percentage calculations.
         ulTotalRunTime /= 100UL;

// Avoid divide by zero errors.
if( ulTotalRunTime > 0 )
         {
// For each populated position in the pxTaskStatusArray array,
// format the raw data as human readable ASCII data
for( x = 0; x < uxArraySize; x++ )
             {
// What percentage of the total run time has the task used?
// This will always be rounded down to the nearest integer.
// ulTotalRunTimeDiv100 has already been divided by 100.
                 ulStatsAsPercentage = pxTaskStatusArray[ x ].ulRunTimeCounter / ulTotalRunTime;

if( ulStatsAsPercentage > 0UL )
                 {
                     sprintf( pcWriteBuffer, "%s\t\t%lu\t\t%lu%%\r\n", pxTaskStatusArray[ x ].pcTaskName, pxTaskStatusArray[ x ].ulRunTimeCounter, ulStatsAsPercentage );
                 }
else
                 {
// If the percentage is zero here then the task has
// consumed less than 1% of the total run time.
                     sprintf( pcWriteBuffer, "%s\t\t%lu\t\t<1%%\r\n", pxTaskStatusArray[ x ].pcTaskName, pxTaskStatusArray[ x ].ulRunTimeCounter );
                 }

                 pcWriteBuffer += strlen( ( char * ) pcWriteBuffer );
             }
         }

// The array is no longer needed, free the memory it consumes.
         vPortFree( pxTaskStatusArray );
     }
 }*/