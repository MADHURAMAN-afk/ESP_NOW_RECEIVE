#include "esp_now.h"
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_wifi.h"
#include <string.h>
#include "driver/gpio.h"

extern "C" {
    // Define the structure for shared variables
    uint8_t macAddress[6] = {0x3C, 0xE9, 0x0E, 0x87, 0x2F, 0xE0}; // sender mac address
    struct variables {
        int data;
    } variable;

    // void OnDataReceive(const uint8_t *mac_addr, const uint8_t *data, int data_len);

    void espNowReceiveWrapper(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len) {
        // Call your existing OnDataReceive function
        // OnDataReceive(esp_now_info->src_addr, data, data_len);
        void OnDataRecive(const uint8_t * mac, const uint8_t *incomingData, int len);
    }

    void espnowReceive(void *Parameter) {
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        esp_wifi_init(&cfg);
        esp_wifi_set_mode(WIFI_MODE_AP);
        esp_wifi_set_channel(0, WIFI_SECOND_CHAN_NONE);
        esp_wifi_start();

        esp_err_t esp_err;

        // Initialize ESP-NOW
        esp_err = esp_now_init();
        if (esp_err != ESP_OK) {
            printf("Failed to initialize ESP-NOW: %s\n", esp_err_to_name(esp_err));
            vTaskDelete(NULL); // Terminate the task
        }

        // esp_now_peer_info_t peer_info;
        // memcpy(peer_info.peer_addr, macAddress, ESP_NOW_ETH_ALEN);
        // peer_info.channel = 0;
        // peer_info.ifidx = WIFI_IF_AP;
        // peer_info.encrypt = false;
        // esp_err = esp_now_add_peer(&peer_info);
        // if (esp_err != ESP_OK) {
        //     printf("Failed to add peer: %s\n", esp_err_to_name(esp_err));
        //     vTaskDelete(NULL); // Terminate the task
        // }

        // Register the receive callback
        esp_err = esp_now_register_recv_cb(espNowReceiveWrapper);
        if (esp_err != ESP_OK) {
            printf("Failed to register receive callback: %s\n", esp_err_to_name(esp_err));
            vTaskDelete(NULL); // Terminate the task
        }

        while (1) {
            vTaskDelay(pdTICKS_TO_MS(1000));
            printf("from core 1\n");
        }
    }

        // void OnDataReceive(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
        //     if (data_len >= sizeof(struct variables)) {
        //         memcpy(&variable, data, sizeof(variable));
        //         printf("Bytes received: %d\n", data_len);
        //         printf("Data: %d\n", variable.data);
        //     } else {
        //         printf("Received data length is insufficient\n");
        //     }
        // }

        void OnDataRecive(const uint8_t * mac, const uint8_t *incomingData, int len) {
        memcpy(&variable, incomingData, sizeof(variable));
        printf(" Data received: %d ", variable.data);
        }


    void NullTask(void* parameter) {
        variables *myvar = (variables*)parameter;
        while (1) {
            // printf("Hello from NullTask\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
            printf("Received Data from other core is: %d\n", myvar->data);
        }
    }

    void app_main() {
        // Create the ESP-NOW receive task
        xTaskCreatePinnedToCore(&espnowReceive, "espnowReceive", 4096, NULL, 1, NULL, 0);

        // Create the NullTask
        xTaskCreatePinnedToCore(&NullTask, "NullTask", 2048, (void*)&variable, 1, NULL, 1);
    }
}

// #include "esp_now.h"
// #include <stdlib.h>
// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>
// #include "esp_wifi.h"
// #include <string.h>
// #include "driver/gpio.h"


// extern "C"{

// // Define the structure for shared variables
// struct variables {
//     int data;
// } variable;

// uint8_t macAddress[6] = {0x3C, 0xE9, 0x0E, 0x87, 0x2F, 0xE0}; // sender mac address

// void OnDataReceive(const uint8_t *mac_addr, const uint8_t *data, int data_len);

// void espNowReceiveWrapper(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len) {
//     // Call your existing OnDataReceive function
//     OnDataReceive(esp_now_info->src_addr, data, data_len);
// }

// void espnowReceive(void *Parameter) {
//         wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//         esp_wifi_init(&cfg);
//         esp_wifi_set_mode(WIFI_MODE_STA);
        
//         esp_wifi_set_channel(0, WIFI_SECOND_CHAN_NONE);
//         esp_wifi_start();

//     esp_err_t esp_err;

//     // Initialize ESP-NOW
//     esp_err = esp_now_init();
//     if (esp_err != ESP_OK) {
//         printf("Failed to initialize ESP-NOW: %s\n", esp_err_to_name(esp_err));
//         vTaskDelete(NULL); // Terminate the task
//     }

//     esp_now_peer_info_t peer_info;
//     memcpy(peer_info.peer_addr, macAddress, ESP_NOW_ETH_ALEN);
//     peer_info.channel = 0;
//     peer_info.ifidx = WIFI_IF_STA;
//     peer_info.encrypt = false;
//     esp_err = esp_now_add_peer(&peer_info);
//     if (esp_err != ESP_OK) {
//         printf("Failed to add peer: %s\n", esp_err_to_name(esp_err));
//         vTaskDelete(NULL); // Terminate the task
//     }

//     // Register the receive callback
//     esp_err = esp_now_register_recv_cb(espNowReceiveWrapper);
//     if (esp_err != ESP_OK) {
//         printf("Failed to register receive callback: %s\n", esp_err_to_name(esp_err));
//         vTaskDelete(NULL); // Terminate the task
//     }

//     while (1) {
//         vTaskDelay(pdMS_TO_TICKS(1000));
//         printf("from core 1\n");
//     }
// }

// void OnDataReceive(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
//     if (data_len >= sizeof(struct variables)) {
//         memcpy(&variable, data, sizeof(variable));
//         printf("Bytes received: %d\n", data_len);
//         printf("Data: %d\n", variable.data);
//     } else {
//         printf("Received data length is insufficient\n");
//     }
// }

// void NullTask(void* parameter) {
//     variables *myvar = (variables*)parameter;
//     while (1) {
//         // printf("Hello from NullTask\n");
//         vTaskDelay(pdMS_TO_TICKS(1000));
//         printf("Received Data from other core is: %d\n", myvar->data);
//     }
// }

// void app_main() {
//     // Create the ESP-NOW receive task
//     xTaskCreatePinnedToCore(&espnowReceive, "espnowReceive", 4096, NULL, 1, NULL, 0);

//     // Create the NullTask
//     xTaskCreatePinnedToCore(&NullTask, "NullTask", 2048, (void*)&variable, 1, NULL, 1);
// }
// }

// #include "esp_now.h"
// #include <stdlib.h>
// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>
// #include "esp_wifi.h"
// #include <string.h>
// #include "driver/gpio.h"
// extern "C" {

// static void receive_data(const uint8_t *mac_addr, const uint8_t *data, int data_len)
// {
//     char buffer[data_len + 1];
//     memcpy(buffer, data, data_len);
//     buffer[data_len] = 0;

//     printf("Received data: %s\n", buffer);
// }

// static void receive_callback(const esp_now_recv_info_t *recv_info, const uint8_t *data, int data_len)
// {
//     printf("Received data from %02X:%02X:%02X:%02X:%02X:%02X\n",
//            recv_info->src_addr[0], recv_info->src_addr[1], recv_info->src_addr[2],
//            recv_info->src_addr[3], recv_info->src_addr[4], recv_info->src_addr[5]);

//     receive_data(recv_info->src_addr, data, data_len);
// }

// void app_main(void)
// {
//     printf("Receiver started\n");

//     // Initialize WiFi in STA mode
//     ESP_ERROR_CHECK(esp_wifi_init(NULL));
//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

//     // Initialize ESP-NOW
//     ESP_ERROR_CHECK(esp_now_init());
//     esp_now_register_recv_cb(receive_callback);

//     while (true) {
//         vTaskDelay(pdMS_TO_TICKS(1000));
//     }
// }}