#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"

// Task handles for task management
TaskHandle_t Task1_Handle;
TaskHandle_t Task2_Handle;
TaskHandle_t Task3_Handle;
TaskHandle_t Task4_Handle;
TaskHandle_t Task5_Handle;

// Function that represents the first task
void Task1(void *pvParameters) {
    for (;;) {
        printf("Task 1 is running...\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for 1 second
    }
}

// Function that represents the second task
void Task2(void *pvParameters) {
    for (;;) {
        printf("Task 2 is running...\n");
        vTaskDelay(2000 / portTICK_PERIOD_MS); // Delay for 2 seconds
    }
}

// Function that represents the third task
void Task3(void *pvParameters) {
    for (;;) {
        printf("Task 3 is monitoring...\n");
        vTaskDelay(1500 / portTICK_PERIOD_MS); // Delay for 1.5 seconds
    }
}

// Function that represents the fourth task
void Task4(void *pvParameters) {
    for (;;) {
        printf("Task 4 is executing a periodic check...\n");
        vTaskDelay(2500 / portTICK_PERIOD_MS); // Delay for 2.5 seconds
    }
}

// Function that represents the fifth task
void Task5(void *pvParameters) {
    for (;;) {
        printf("Task 5 is performing cleanup...\n");
        vTaskDelay(3000 / portTICK_PERIOD_MS); // Delay for 3 seconds
    }
}

int main(void) {
    // Initialize the FreeRTOS kernel
    xTaskCreate(Task1, "Task1", configMINIMAL_STACK_SIZE, NULL, 1, &Task1_Handle);
    xTaskCreate(Task2, "Task2", configMINIMAL_STACK_SIZE, NULL, 1, &Task2_Handle);
    xTaskCreate(Task3, "Task3", configMINIMAL_STACK_SIZE, NULL, 1, &Task3_Handle);
    xTaskCreate(Task4, "Task4", configMINIMAL_STACK_SIZE, NULL, 1, &Task4_Handle);
    xTaskCreate(Task5, "Task5", configMINIMAL_STACK_SIZE, NULL, 1, &Task5_Handle);

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    // If all is well, this line will never be reached
    for (;;);
    return 0;
}
