#include "esp_now.h"
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_wifi.h"
#include <string.h>
#include "driver/gpio.h"
#include "nvs_flash.h"


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

    void OnDataRecive(const uint8_t * mac, const uint8_t *incomingData, int len) {
        memcpy(&variable, incomingData, sizeof(variable));
        printf(" Data received: %d ", variable.data);
        }

    void espnowReceive(void *Parameter) {
        

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
        


        
        while (1) {
            printf("just the loop \n");
            vTaskDelay(pdMS_TO_TICKS(1000));

        }
    }

        void OnDataReceive(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
            if (data_len >= sizeof(struct variables)) {
                memcpy(&variable, data, sizeof(variable));
                printf("Bytes received: %d\n", data_len);
                printf("Data: %d\n", variable.data);
            } else {
                printf("Received data length is insufficient\n");
            }
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

        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_init());
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_loop_create_default());
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_init(&cfg));
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_set_storage(WIFI_STORAGE_RAM));
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_start());

        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);

        esp_err_t esp_err;

        // Initialize ESP-NOW
        esp_err = esp_now_init();
        if (esp_err != ESP_OK) {
            printf("Failed to initialize ESP-NOW: %s\n", esp_err_to_name(esp_err));
            vTaskDelete(NULL); // Terminate the task
        }

         esp_err = esp_now_register_recv_cb(espNowReceiveWrapper);
        
        if (esp_err != ESP_OK) {
            printf("Failed to register receive callback: %s\n", esp_err_to_name(esp_err));
            vTaskDelete(NULL); // Terminate the task
        }

        // Create the ESP-NOW receive task
        xTaskCreatePinnedToCore(&espnowReceive, "espnowReceive", 4096, NULL, 1, NULL, 0);

        // Create the NullTask
        xTaskCreatePinnedToCore(&NullTask, "NullTask", 2048, (void*)&variable, 1, NULL, 1);
    }
}