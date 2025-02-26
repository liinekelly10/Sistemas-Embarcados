#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Configurações globais
#define HISTORY_SIZE 10

// Handles para tarefas e recursos
TaskHandle_t producerTaskHandle, consumerTaskHandle, monitorTaskHandle, controlTaskHandle;
QueueHandle_t numberQueue;
SemaphoreHandle_t printSemaphore;
int history[HISTORY_SIZE];
int historyIndex = 0;

// Função da tarefa produtora
void producerTask(void *pvParameters) {
    srand(time(NULL));
    while (1) {
        int randomNum = rand() % 100;
        xQueueSend(numberQueue, &randomNum, portMAX_DELAY);
        xSemaphoreTake(printSemaphore, portMAX_DELAY);
        printf("Produzido: %d\n", randomNum);
        xSemaphoreGive(printSemaphore);
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 segundo
    }
}

// Função da tarefa consumidora
void consumerTask(void *pvParameters) {
    int receivedNum;
    while (1) {
        if (xQueueReceive(numberQueue, &receivedNum, portMAX_DELAY)) {
            xSemaphoreTake(printSemaphore, portMAX_DELAY);
            printf("Consumido: %d, quadrado: %d\n", receivedNum, receivedNum * receivedNum);
            xSemaphoreGive(printSemaphore);
        }
    }
}

// Função da tarefa de monitoramento
void monitorTask(void *pvParameters) {
    while (1) {
        UBaseType_t freeHeap = xPortGetFreeHeapSize();
        xSemaphoreTake(printSemaphore, portMAX_DELAY);
        printf("Monitoramento: Heap livre: %lu bytes\n", freeHeap);
        xSemaphoreGive(printSemaphore);
        vTaskDelay(pdMS_TO_TICKS(2000)); // 2 segundos
    }
}

// Função da tarefa de controle
void controlTask(void *pvParameters) {
    while (1) {
        UBaseType_t producerPriority = uxTaskPriorityGet(producerTaskHandle);
        if (producerPriority < tskIDLE_PRIORITY + 2) {
            vTaskPrioritySet(producerTaskHandle, producerPriority + 1);
        }
        xSemaphoreTake(printSemaphore, portMAX_DELAY);
        printf("Controle: Aumentando prioridade do produtor para %lu\n", producerPriority + 1);
        xSemaphoreGive(printSemaphore);
        vTaskDelay(pdMS_TO_TICKS(5000)); // 5 segundos
    }
}

// Função da tarefa de simulação de sensor
void sensorTask(void *pvParameters) {
    while (1) {
        int temperature = (rand() % 30) + 20; // Simula temperaturas entre 20 e 50 graus
        xQueueSend(numberQueue, &temperature, portMAX_DELAY);
        xSemaphoreTake(printSemaphore, portMAX_DELAY);
        printf("Sensor: Temperatura enviada %d°C\n", temperature);
        xSemaphoreGive(printSemaphore);
        vTaskDelay(pdMS_TO_TICKS(1500)); // 1,5 segundos
    }
}

// Função da tarefa de registro histórico
void historyTask(void *pvParameters) {
    int processedValue;
    while (1) {
        if (xQueueReceive(numberQueue, &processedValue, portMAX_DELAY)) {
            history[historyIndex] = processedValue;
            historyIndex = (historyIndex + 1) % HISTORY_SIZE; // Ciclo circular
            xSemaphoreTake(printSemaphore, portMAX_DELAY);
            printf("Histórico: Valor %d adicionado ao histórico\n", processedValue);
            xSemaphoreGive(printSemaphore);
        }
        vTaskDelay(pdMS_TO_TICKS(500)); // Processa rapidamente
    }
}

// Função da tarefa de alarme
void alarmTask(void *pvParameters) {
    int receivedValue;
    const int threshold = 40;
    while (1) {
        if (xQueueReceive(numberQueue, &receivedValue, portMAX_DELAY)) {
            if (receivedValue > threshold) {
                xSemaphoreTake(printSemaphore, portMAX_DELAY);
                printf("Alarme: Valor crítico detectado %d! (limite: %d)\n", receivedValue, threshold);
                xSemaphoreGive(printSemaphore);
            }
        }
    }
}

// Função da tarefa de relatório
void reportTask(void *pvParameters) {
    while (1) {
        xSemaphoreTake(printSemaphore, portMAX_DELAY);
        printf("Relatório: Histórico de valores armazenados:\n");
        for (int i = 0; i < HISTORY_SIZE; i++) {
            printf("[%d]: %d\n", i, history[i]);
        }
        xSemaphoreGive(printSemaphore);
        vTaskDelay(pdMS_TO_TICKS(10000)); // A cada 10 segundos
    }
}

int main(void) {
    // Inicialização de recursos
    numberQueue = xQueueCreate(10, sizeof(int));
    printSemaphore = xSemaphoreCreateMutex();

    // Criação das tarefas originais
    xTaskCreate(producerTask, "Producer", 1000, NULL, 1, &producerTaskHandle);
    xTaskCreate(consumerTask, "Consumer", 1000, NULL, 1, &consumerTaskHandle);
    xTaskCreate(monitorTask, "Monitor", 1000, NULL, 1, &monitorTaskHandle);
    xTaskCreate(controlTask, "Control", 1000, NULL, 2, &controlTaskHandle);

    // Criação das novas tarefas
    xTaskCreate(sensorTask, "Sensor", 1000, NULL, 1, NULL);
    xTaskCreate(historyTask, "History", 1000, NULL, 1, NULL);
    xTaskCreate(alarmTask, "Alarm", 1000, NULL, 1, NULL);
    xTaskCreate(reportTask, "Report", 1000, NULL, 1, NULL);

    // Inicialização do scheduler
    vTaskStartScheduler();

    // Código não deve chegar aqui
    while (1);
    return 0;
}

