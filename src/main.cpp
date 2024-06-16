#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_now.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_log.h"

static const char *TAG = "Master";

// Structure for received data
typedef struct struct_message {
    int temperature;
    int humidity;
    char message[32];
} struct_message_t;


static void esp_now_recv_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int data_len) {
    struct_message_t *recv_data = (struct_message_t *) data;

    ESP_LOGI(TAG, "Received data from MAC: %02x:%02x:%02x:%02x:%02x:%02x",
             recv_info->src_addr[0], recv_info->src_addr[1], recv_info->src_addr[2],
             recv_info->src_addr[3], recv_info->src_addr[4], recv_info->src_addr[5]);

    ESP_LOGI(TAG, "Temperature: %d, Humidity: %d, Message: %s",
             recv_data->temperature, recv_data->humidity, recv_data->message);
}

void init_esp_now() {
    // Initialize WiFi in Station mode
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Initialize ESP-NOW
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_set_pmk((uint8_t *) "pmk1234567890123"));

    // Register receive callback
    ESP_ERROR_CHECK(esp_now_register_recv_cb(esp_now_recv_cb));
}

extern "C" void app_main() {
    // Initialize NVS
    ESP_ERROR_CHECK(nvs_flash_init());

    // Initialize ESP-NOW
    init_esp_now();

    // The receiver will keep running, receiving data via ESP-NOW
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10000)); // Delay to keep the task alive
    }
}
