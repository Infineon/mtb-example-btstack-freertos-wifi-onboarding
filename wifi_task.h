/******************************************************************************
* File Name: wifi_task.h
*
* Description: This is the header file for wifi_task.c. It contains macros,
* enums and structures used by the functions in wifi_task.c. It also contains
* function prototypes and externs of global variables that can be used by
* other files
*
* Related Document: See README.md
*
*******************************************************************************
* Copyright 2021, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

#ifndef __WIFI_TASK_H__
#define __WIFI_TASK_H__

#include "cy_em_eeprom.h"

/******************************************************************************
 *                                Constants
 ******************************************************************************/

/* Maximum number of connection retries to the Wi-Fi network. */
#define MAX_CONNECTION_RETRIES            (3)

/* Logical Start of Emulated EEPROM in bytes. */
#define LOGICAL_EEPROM_START    (0u)

/* EEPROM Configuration details. */
#define SIMPLE_MODE             (0u) 
#define EEPROM_SIZE             (512u)
#define BLOCKING_WRITE          (1u)
#define REDUNDANT_COPY          (1u)
#define WEAR_LEVELLING_FACTOR   (2u)

/* Set the macro FLASH_REGION_TO_USE to either USER_FLASH or
 * EMULATED_EEPROM_FLASH to specify the region of the flash used for
 * emulated EEPROM.
 */
#define USER_FLASH              (0u)
#define EMULATED_EEPROM_FLASH   (1u)
#define FLASH_REGION_TO_USE     EMULATED_EEPROM_FLASH

/* Task parameters for WiFi tasks */
#define WIFI_TASK_STACK_SIZE                    (4096u)
#define WIFI_TASK_PRIORITY                      (5u)

/* Macros defining the packet type */
#define DATA_PACKET_TYPE_SSID (1u)
#define DATA_PACKET_TYPE_PASSWORD (2u)

/* Macros defining packet type for scan data */
#define SCAN_PACKET_TYPE_SSID (1u)
#define SCAN_PACKET_TYPE_SECURITY (2u)
#define SCAN_PACKET_TYPE_RSSI (3u)

/* Macros defining the commands for WiFi control point characteristic */
#define WIFI_CONTROL_DISCONNECT (0u)
#define WIFI_CONTROL_CONNECT (1u)
#define WIFI_CONTROL_SCAN (2u)

/* Task notification value to indicate whether to use
 * WiFi credentials from EMEEPROM or from GATT DB
 */
enum wifi_task_notifications
{
    NOTIF_SCAN               = 0x0001,
    NOTIF_CONNECT            = 0x0002,
    NOTIF_DISCONNECT         = 0x0004,
    NOTIF_ERASE_DATA         = 0x0008,
    NOTIF_GATT_NOTIFICATION  = 0x0010
};

/******************************************************************************
 *                                Structures
 ******************************************************************************/
/* Structure to store WiFi details that goes into EEPROM */
typedef __PACKED_STRUCT
{
    uint8_t wifi_ssid[CY_WCM_MAX_SSID_LEN];
    uint8_t ssid_len;
    uint8_t wifi_password[CY_WCM_MAX_PASSPHRASE_LEN];
    uint8_t password_len;
}wifi_details_t;

/******************************************************************************
 *                              Extern Variables
 ******************************************************************************/
extern cy_stc_eeprom_context_t Em_EEPROM_context;
extern cy_stc_eeprom_config_t Em_EEPROM_config;
extern wifi_details_t wifi_details;
extern cy_wcm_connect_params_t wifi_conn_param;

/******************************************************************************
 *                              Function Prototypes
 ******************************************************************************/
void wifi_task(void * arg);
void scan_callback(cy_wcm_scan_result_t *result_ptr, void *user_data,
                   cy_wcm_scan_status_t status);

#endif      /* __WIFI_TASK_H__ */


/* [] END OF FILE */
