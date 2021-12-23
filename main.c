/******************************************************************************
* File Name:   main.c
*
* Description: This file contains the main function along with all BLE related
*              functionality.
*
* Related Document: See Readme.md
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

#include <app_utils.h>
#include "cybsp.h"
#include "cy_retarget_io.h"
#include <FreeRTOS.h>
#include <task.h>
#include "stdlib.h"
#include "wiced_bt_dev.h"
#include "wiced_bt_ble.h"
#include "wiced_bt_gatt.h"
#include "cycfg_bt_settings.h"
#include "cycfg_gap.h"
#include "cybsp_bt_config.h"
#include "wiced_bt_stack.h"
#include "cycfg_gatt_db.h"
#include "wifi_task.h"
#include "wiced_memory.h"
#include <inttypes.h>
/******************************************************************************
 *                                Constants
 ******************************************************************************/

/* Priority for the button interrupt */
#define GPIO_INTERRUPT_PRIORITY (7)

/* LE Key Size */
#define MAX_KEY_SIZE (16)

/* WICED Heap size for app */
#define APP_HEAP_SIZE 0x1000
/******************************************************************************
 *                                 TYPEDEFS
 ******************************************************************************/
typedef void (*pfn_free_buffer_t)(uint8_t *);

/******************************************************************************
 *                             Global Variables
 ******************************************************************************/
/* Task Handle for WiFi Task */
TaskHandle_t wifi_task_handle;

/* Maintains the connection id of the current connection */
uint16_t conn_id = 0;

/* This variable is set to true when button callback is received and
 * data is present in EMEEPROM. It is set to false after the WiFi Task
 * processes Disconnection notification. It is used to check button
 * interrupt while the device is trying to connect to WiFi
 */
volatile bool button_pressed = false;

/* This enables RTOS aware debugging. */
volatile int uxTopUsedPriority;

/* Pointer to the heap created */
wiced_bt_heap_t * app_heap_pointer;
/******************************************************************************
 *                              Function Prototypes
 ******************************************************************************/
static wiced_result_t app_management_callback(wiced_bt_management_evt_t event,
                                              wiced_bt_management_evt_data_t *p_event_data);
static wiced_bt_gatt_status_t app_gatts_callback(wiced_bt_gatt_evt_t event,
                                                 wiced_bt_gatt_event_data_t *p_data);
static void application_init(void);
static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event);

cyhal_gpio_callback_data_t cb_data =
{
.callback = gpio_interrupt_handler,
.callback_arg = NULL
};

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function for CM4 CPU
*    1. Initializes the BSP
*    2. Configures the button for interrupt
*    3. Initializes retarget IO for UART debug printing
*    4. Initializes platform configuration
*    5. Initializes BT stack and heap
*    6. Creates WiFi task
*    7. Starts the RTOS scheduler
*
* Return:
*  int
*
*******************************************************************************/
int main()
{
    /* This enables RTOS aware debugging in OpenOCD. */
    uxTopUsedPriority = configMAX_PRIORITIES - 1;

    /* Initialize the board support package */
    if (CY_RSLT_SUCCESS != cybsp_init())
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize retarget-io to use the debug UART port */
    cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
                        CY_RETARGET_IO_BAUDRATE);

    printf("***************************\n"
           "Wi-Fi Onboarding Using BLE\n"
           "***************************\n\n");

    /* Configure platform specific settings for Bluetooth */
    cybt_platform_config_init(&cybsp_bt_platform_cfg);

    /* Initialize the Bluetooth stack with a callback function and stack
     * configuration structure */
    if (WICED_SUCCESS != wiced_bt_stack_init(app_management_callback,
                                             &wiced_bt_cfg_settings))
    {
        printf("Error in initializing the BT stack\n");
        CY_ASSERT(0);
    }

    /* Initialize WiFi Tasks */
    xTaskCreate(wifi_task, "WiFi-Task", WIFI_TASK_STACK_SIZE, NULL,
                WIFI_TASK_PRIORITY, &wifi_task_handle);

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    /* Should never get here */
    CY_ASSERT(0);
}

/*******************************************************************************
* Function Name: application_init
********************************************************************************
* Summary:
* This function is called from the BTM enabled event
*    1. Initializes and registers the GATT DB
*    2. Sets pairable mode to true
*    3. Sets ADV data and starts advertising
*
*******************************************************************************/
void application_init(void)
{
    wiced_result_t result = WICED_ERROR;
    wiced_bt_gatt_status_t gatt_status = WICED_BT_GATT_INVALID_CONNECTION_ID;
    cy_rslt_t cy_result = 0;

    /* Return status for EEPROM */
    cy_en_em_eeprom_status_t eepromReturnValue;

    /* Create a buffer heap, make it the default heap */
    app_heap_pointer = wiced_bt_create_heap("app", NULL, APP_HEAP_SIZE, NULL,
                                            WICED_TRUE);

    if(NULL == app_heap_pointer)
    {
        printf("failed to create heap\n");
        CY_ASSERT(0);
    }

    /* Initialize the user button.*/
    cy_result = cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT,
                    CYHAL_GPIO_DRIVE_NONE, CYBSP_BTN_OFF);
    if(cy_result)
    {
        printf("GPIO Init failed\n");
    }

    /* Configure GPIO interrupt */
    cyhal_gpio_register_callback(CYBSP_USER_BTN,&cb_data);

    /* Enabling button interrupt */
    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL,
                            GPIO_INTERRUPT_PRIORITY, true);

    /* Register with stack to receive GATT callback */
    gatt_status = wiced_bt_gatt_register(app_gatts_callback);

    if(WICED_BT_GATT_SUCCESS !=gatt_status)
    {
        printf("\nGATT register failed. Status: %s\n", get_bt_gatt_status_name(
                                                       gatt_status));
    }

    /*  Inform the stack to use our GATT database */
    gatt_status = wiced_bt_gatt_db_init(gatt_database, gatt_database_len, NULL);

    if(WICED_BT_GATT_SUCCESS !=gatt_status)
    {
        printf("\nGATT db init failed. Status :%s\n",get_bt_gatt_status_name(
                                                     gatt_status));
    }

    /* Allow peer to pair */
    wiced_bt_set_pairable_mode(WICED_TRUE, false);

    /* Set BLE advertisement data */
    result = wiced_bt_ble_set_raw_advertisement_data(CY_BT_ADV_PACKET_DATA_SIZE,
                                                     cy_bt_adv_packet_data);
    if (WICED_SUCCESS != result)
    {
        printf("Set ADV data failed\n");
    }

    /* Initialize the EMEEPROM. */
    eepromReturnValue = Cy_Em_EEPROM_Init(&Em_EEPROM_config, &Em_EEPROM_context);

    if (CY_EM_EEPROM_SUCCESS != eepromReturnValue)
    {
        printf("Error initializing EMEEPROM\n");
        CY_ASSERT(0);
    }

    /* Determine if the EEPROM has a previously stored WiFi SSID value.
     * If it does, use the stored credentials to connect to WiFi.
     * Otherwise, start BLE advertisements so that the user can use BLE
     * to enter the WiFi credentials. */
    eepromReturnValue = Cy_Em_EEPROM_Read(LOGICAL_EEPROM_START,
                                      (void *)&wifi_details.wifi_ssid[0],
                                      sizeof(wifi_details), &Em_EEPROM_context);

    if ((CY_EM_EEPROM_SUCCESS == eepromReturnValue) &&
        (0 != wifi_details.ssid_len))
    {
        printf("Data present in EMEEPROM\n");

        /* Set the WiFi Connection parameters structure to 0 before copying
         * data */
        memset(&wifi_conn_param, 0, sizeof(cy_wcm_connect_params_t));

        /* Copy the WiFi credentials to the global variable */
        memcpy(wifi_conn_param.ap_credentials.SSID, &wifi_details.wifi_ssid[0],
               wifi_details.ssid_len);

        memcpy(wifi_conn_param.ap_credentials.password,
               &wifi_details.wifi_password[0], wifi_details.password_len);

        /* Unblock WiFi task with notification */
        xTaskNotify(wifi_task_handle, (NOTIF_SCAN | NOTIF_CONNECT),
                    eSetValueWithOverwrite);
    }
    else /* WiFi credentials not found in EMEEPROM */
    {
        printf("Data not present in EMEEPROM\n");
        result = wiced_bt_start_advertisements(BTM_BLE_ADVERT_UNDIRECTED_LOW,
                                               BLE_ADDR_PUBLIC, NULL);
        if(WICED_SUCCESS != result)
        {
            printf("Start ADV failed");
        }
    }
}

/*******************************************************************************
* Function Name: app_management_callback
********************************************************************************
* Summary:
* This function handles the BT stack events.
*
* Parameters:
*  wiced_bt_management_evt_t: event code
*  p_event_data: Pointer to the event data
*
* Return:
*  wiced_result_t: Result
*
*******************************************************************************/
wiced_result_t app_management_callback(wiced_bt_management_evt_t event,
                                       wiced_bt_management_evt_data_t *p_event_data)
{
    wiced_result_t result = WICED_BT_SUCCESS;
    wiced_bt_device_address_t bda = {0};
    wiced_bt_dev_ble_io_caps_req_t *pairing_io_caps = &(p_event_data->
                                           pairing_io_capabilities_ble_request);

    printf("Bluetooth Management Event: %s\n", get_bt_event_name(event));

    switch (event)
    {
    case BTM_ENABLED_EVT:

        /* Bluetooth is enabled */
        wiced_bt_set_local_bdaddr((uint8_t *)cy_bt_device_address,
                                   BLE_ADDR_PUBLIC);

        /* Read and print the BD address */
        wiced_bt_dev_read_local_addr(bda);
        printf("Local Bluetooth Address: ");
        print_bd_address(bda);
        printf("\n");

        application_init();
        break;

    case BTM_DISABLED_EVT:
        break;
        /* Print passkey to the screen so that the user can enter it. */
    case BTM_PASSKEY_NOTIFICATION_EVT:
        printf( "********************************************************\r\n");
        printf( "Passkey Notification\r\n");
        printf("PassKey: %" PRIu32 "\r\n",
        p_event_data->user_passkey_notification.passkey );
        printf( "***********************************************************\r\n");
        break;

    case BTM_PAIRING_IO_CAPABILITIES_BLE_REQUEST_EVT:
        pairing_io_caps->local_io_cap = BTM_IO_CAPABILITIES_DISPLAY_ONLY;

        pairing_io_caps->oob_data = BTM_OOB_NONE;

        pairing_io_caps->auth_req = BTM_LE_AUTH_REQ_SC;

        pairing_io_caps->max_key_size = MAX_KEY_SIZE;

        pairing_io_caps->init_keys = BTM_LE_KEY_PENC | BTM_LE_KEY_PID |
                                    BTM_LE_KEY_PCSRK | BTM_LE_KEY_LENC;

        pairing_io_caps->resp_keys = BTM_LE_KEY_PENC | BTM_LE_KEY_PID |
                                    BTM_LE_KEY_PCSRK | BTM_LE_KEY_LENC;
        break;

    case BTM_PAIRING_COMPLETE_EVT:
        if (WICED_SUCCESS == p_event_data->
                            pairing_complete.pairing_complete_info.ble.status)
        {
            printf("Pairing Complete: SUCCESS\n");
        }
        else /* Pairing Failed */
        {
            printf("Pairing Complete: FAILED\n");
        }
        break;

    case BTM_PAIRED_DEVICE_LINK_KEYS_UPDATE_EVT:
        /* Paired Device Link Keys update */
        result = WICED_SUCCESS;
        break;

    case BTM_PAIRED_DEVICE_LINK_KEYS_REQUEST_EVT:
        /* Paired Device Link Keys Request */
        result = WICED_BT_ERROR;
        break;

    case BTM_LOCAL_IDENTITY_KEYS_UPDATE_EVT:
        /* Local identity Keys Update */
        result = WICED_SUCCESS;
        break;

    case BTM_LOCAL_IDENTITY_KEYS_REQUEST_EVT:
        /* Local identity Keys Request */
        result = WICED_BT_ERROR;
        break;

    case BTM_ENCRYPTION_STATUS_EVT:
        if (WICED_SUCCESS == p_event_data->encryption_status.result)
        {
            printf("Encryption Status Event: SUCCESS\n");
        }
        else /* Encryption Failed */
        {
            printf("Encryption Status Event: FAILED\n");
        }
        break;

    case BTM_SECURITY_REQUEST_EVT:
        wiced_bt_ble_security_grant(p_event_data->security_request.bd_addr,
                                    WICED_BT_SUCCESS);
        break;

    case BTM_BLE_ADVERT_STATE_CHANGED_EVT:
        printf("\n");
        printf("Advertisement state changed to %s\n", get_bt_advert_mode_name(
                                       p_event_data->ble_advert_state_changed));
        break;

    default:
        break;
    }

    return result;
}

/*******************************************************************************
* Function Name: app_get_attribute
********************************************************************************
* Summary:
* This function searches through the GATT DB to point to the attribute
* corresponding to the given handle
*
* Parameters:
*  uint16_t handle: Handle to search for in the GATT DB
*
* Return:
*  gatt_db_lookup_table_t *: Pointer to the correct attribute in the GATT DB
*
*******************************************************************************/
gatt_db_lookup_table_t *app_get_attribute(uint16_t handle)
{
    /* Search for the given handle in the GATT DB and return the pointer to the
    correct attribute */
    uint8_t array_index = 0;

    for (array_index = 0; array_index < app_gatt_db_ext_attr_tbl_size; array_index++)
    {
        if (app_gatt_db_ext_attr_tbl[array_index].handle == handle)
        {
            return (&app_gatt_db_ext_attr_tbl[array_index]);
        }
    }
    return NULL;
}

/*******************************************************************************
* Function Name: app_gatts_req_read_handler
********************************************************************************
* Summary:
* This function handles the GATT read request events from the stack
*
* Parameters:
*  uint16_t conn_id: Connection ID
*  wiced_bt_gatt_read_t * p_read_data: Read data structure
*  wiced_bt_gatt_opcode_t opcode: GATT opcode
*  uint16_t len_requested: Length requested
* Return:
*  wiced_bt_gatt_status_t: GATT result
*
*******************************************************************************/
wiced_bt_gatt_status_t app_gatts_req_read_handler(uint16_t conn_id,
                                                  wiced_bt_gatt_opcode_t opcode,
                                                  wiced_bt_gatt_read_t *p_read_data,
                                                  uint16_t len_requested)
{

    gatt_db_lookup_table_t *puAttribute;
    int attr_len_to_copy;

    /* Get the right address for the handle in Gatt DB */
    if (NULL == (puAttribute = app_get_attribute(p_read_data->handle)))
    {
        printf("Read handle attribute not found. Handle:0x%X\n",
                p_read_data->handle);
        wiced_bt_gatt_server_send_error_rsp(conn_id, opcode, p_read_data->handle,
                                            WICED_BT_GATT_INVALID_HANDLE);
        return WICED_BT_GATT_INVALID_HANDLE;
    }

    attr_len_to_copy = puAttribute->cur_len;

    printf("GATT Read handler: handle:0x%X, len:%d\n",
           p_read_data->handle, attr_len_to_copy);

    /* If the incoming offset is greater than the current length in the GATT DB
    then the data cannot be read back*/
    if (p_read_data->offset >= puAttribute->cur_len)
    {
        wiced_bt_gatt_server_send_error_rsp(conn_id, opcode, p_read_data->handle,
                                            WICED_BT_GATT_INVALID_OFFSET);
        return (WICED_BT_GATT_INVALID_OFFSET);
    }

    int to_send = MIN(len_requested, attr_len_to_copy - p_read_data->offset);

    uint8_t *from = ((uint8_t *)puAttribute->p_data) + p_read_data->offset;

    wiced_bt_gatt_server_send_read_handle_rsp(conn_id, opcode, to_send, from, NULL);

    return WICED_BT_GATT_SUCCESS;
}

/*******************************************************************************
* Function Name: app_gatt_read_by_type_handler
********************************************************************************
* Summary:
* This function handles the GATT read by type request events from the stack
*
* Parameters:
*  uint16_t conn_id: Connection ID
*  wiced_bt_gatt_opcode_t opcode: GATT opcode
*  wiced_bt_gatt_read_by_type_t * p_read_data: Read data structure
*  uint16_t len_requested: Length requested
*
* Return:
*  wiced_bt_gatt_status_t: GATT result
*
*******************************************************************************/
wiced_bt_gatt_status_t app_gatt_read_by_type_handler(uint16_t conn_id,
                                         wiced_bt_gatt_opcode_t opcode,
                                         wiced_bt_gatt_read_by_type_t *p_read_data,
                                         uint16_t len_requested)
{
    gatt_db_lookup_table_t *puAttribute;
    uint16_t    attr_handle = p_read_data->s_handle;
    uint8_t     *p_rsp = wiced_bt_get_buffer(len_requested);
    uint8_t     pair_len = 0;
    int used = 0;

    if (p_rsp == NULL)
    {
        wiced_bt_gatt_server_send_error_rsp(conn_id, opcode, attr_handle,
                                            WICED_BT_GATT_INSUF_RESOURCE);
        return WICED_BT_GATT_INSUF_RESOURCE;
    }

    /* Read by type returns all attributes of the specified type,
     * between the start and end handles */
    while (WICED_TRUE)
    {
        attr_handle = wiced_bt_gatt_find_handle_by_type(attr_handle,
                p_read_data->e_handle, &p_read_data->uuid);

        if (attr_handle == 0)
            break;

        if ((puAttribute = app_get_attribute(attr_handle)) == NULL)
        {
            wiced_bt_gatt_server_send_error_rsp(conn_id, opcode,
                    p_read_data->s_handle, WICED_BT_GATT_ERR_UNLIKELY);
            wiced_bt_free_buffer(p_rsp);
            return WICED_BT_GATT_ERR_UNLIKELY;
        }

        int filled = wiced_bt_gatt_put_read_by_type_rsp_in_stream(
                p_rsp + used, len_requested - used, &pair_len, attr_handle,
                 puAttribute->cur_len, puAttribute->p_data);
        if (filled == 0)
        {
            break;
        }
        used += filled;

        /* Increment starting handle for next search to one past current */
        attr_handle++;
    }

    if (used == 0)
    {
        wiced_bt_gatt_server_send_error_rsp(conn_id, opcode, p_read_data->s_handle,
                                            WICED_BT_GATT_INVALID_HANDLE);
        wiced_bt_free_buffer(p_rsp);
        return WICED_BT_GATT_INVALID_HANDLE;
    }

    /* Send the response */
    wiced_bt_gatt_server_send_read_by_type_rsp(conn_id, opcode, pair_len,
            used, p_rsp, (wiced_bt_gatt_app_context_t)wiced_bt_free_buffer);

    return WICED_BT_GATT_SUCCESS;
}

/*******************************************************************************
* Function Name: app_gatts_req_write_handler
********************************************************************************
* Summary:
* This function handles the GATT write request events from the stack
*
* Parameters:
*  uint16_t conn_id: Connection ID
*  wiced_bt_gatt_opcode_t opcode: GATT opcode
*  wiced_bt_gatt_write_t * p_data: Write data structure
*
* Return:
*  wiced_bt_gatt_status_t: GATT result
*
*******************************************************************************/
wiced_bt_gatt_status_t app_gatts_req_write_handler(uint16_t conn_id,
                                                   wiced_bt_gatt_opcode_t opcode,
                                                   wiced_bt_gatt_write_req_t *p_data)
{
    wiced_bt_gatt_status_t result = WICED_BT_GATT_SUCCESS;
    uint8_t *p_attr = p_data->p_val;
    gatt_db_lookup_table_t *puAttribute;

    /* Return status for EEPROM. */
    cy_en_em_eeprom_status_t eepromReturnValue;

    /* Variables for TLV data extraction */
    uint8_t byte_no = 0;
    uint8_t data_length = 0;

    printf("GATT write handler: handle:0x%X len:%d, opcode:0x%X\n",
           p_data->handle, p_data->val_len, opcode);

    /* Get the right address for the handle in Gatt DB */
    if (NULL == (puAttribute = app_get_attribute(p_data->handle)))
    {
        printf("\nWrite Handle attr not found. Handle:0x%X\n", p_data->handle);
        return WICED_BT_GATT_INVALID_HANDLE;
    }

    switch (p_data->handle)
    {
    /* Write request for the WiFi SSID characteristic. Copy the incoming data
    to the WIFI SSID variable */
    case HDLC_CUSTOM_SERVICE_WIFI_SSID_VALUE:

        /* First copy the value to the GATT DB variable and then to the global
         * variable to be stored in NV memory
         */
        memset(app_custom_service_wifi_ssid, 0,
               strlen((char *)app_custom_service_wifi_ssid));
        memcpy(app_custom_service_wifi_ssid, p_attr, p_data->val_len);
        puAttribute->cur_len = p_data->val_len;

        memcpy(&wifi_details.wifi_ssid[0], p_attr, p_data->val_len);
        wifi_details.ssid_len = p_data->val_len;

        printf("Wi-Fi SSID: %s\n", app_custom_service_wifi_ssid);
        break;

    /* Write request for the WiFi password characteristic. Accept the password.
    Copy the incoming data to the WIFI Password variable */
    case HDLC_CUSTOM_SERVICE_WIFI_PASSWORD_VALUE:

        /* First copy the value to the GATT DB variable and then to the global
         * variable to be stored in NV memory
         */
        memset(app_custom_service_wifi_password, 0,
                strlen((char *)app_custom_service_wifi_password));
        memcpy(app_custom_service_wifi_password, p_attr, p_data->val_len);
        puAttribute->cur_len = p_data->val_len;

        memcpy(&wifi_details.wifi_password[0], p_attr, p_data->val_len);
        wifi_details.password_len = p_data->val_len;

        printf("Wi-Fi Password: %s\n", app_custom_service_wifi_password);
        break;

    /* Write request for the WiFi SSID and password characteristic. Accept the
    values and Copy the incoming data to the WIFI SSID Password variable */
    case HDLC_CUSTOM_SERVICE_WIFI_SSID_PASSWORD_VALUE:

        /* Extract the value from the received data */
        for(byte_no = 0; byte_no < p_data->val_len;)
        {
            /* Check if the Type is SSID packet type */
            if(DATA_PACKET_TYPE_SSID == p_attr[byte_no])
            {
                /* Extract the length of the data */
                data_length =  p_attr[++byte_no];

                /* Copy the SSID value based on the length */
                memcpy(&wifi_details.wifi_ssid[0], &p_attr[++byte_no],
                        data_length);

                wifi_details.ssid_len = data_length;

                /* Move the pointer to the address after the SSID value */
                byte_no = byte_no + data_length;

            }
            /* Check if the Type is password packet type */
            else if(DATA_PACKET_TYPE_PASSWORD == p_attr[byte_no])
            {
                /* Extract the length of the data */
                data_length = p_attr[++byte_no];

                /* Copy the password value based on the length */
                memcpy(&wifi_details.wifi_password[0], &p_attr[++byte_no],
                        data_length);

                wifi_details.password_len = data_length;

                /* Move the pointer to the end of the packet. No more data
                 * expected
                 */
                byte_no = byte_no + data_length;
            }
        }

        printf("Wi-Fi SSID: %s\n", wifi_details.wifi_ssid);
        printf("Wi-Fi Password: %s\n", wifi_details.wifi_password);
        break;


    /* Handle the CCCD values for WIFI NETWORKS characteristic */
    case HDLD_CUSTOM_SERVICE_WIFI_NETWORKS_CLIENT_CHAR_CONFIG:
        app_custom_service_wifi_networks_client_char_config[0] = p_attr[0];
        app_custom_service_wifi_networks_client_char_config[1] = p_attr[1];
        break;

    /* Write request for the control characteristic. Copy the incoming data
    to the WIFI control variable. Based on the value, start connect, disconnect
     or scan procedure */
    case HDLC_CUSTOM_SERVICE_WIFI_CONTROL_VALUE:
        app_custom_service_wifi_control[0] = p_attr[0];
        puAttribute->cur_len = p_data->val_len;

        /* Connect command */
        if (WIFI_CONTROL_CONNECT == app_custom_service_wifi_control[0])
        {
            if(0 != wifi_details.ssid_len)
            {
                /* Set the WiFi Connection parameters structure to 0 before copying
                 * data */
                memset(&wifi_conn_param, 0, sizeof(cy_wcm_connect_params_t));

                /* Copy the WiFi credentials to the global variable */
                memcpy(wifi_conn_param.ap_credentials.SSID,
                       &wifi_details.wifi_ssid[0], wifi_details.ssid_len);

                memcpy(wifi_conn_param.ap_credentials.password,
                       &wifi_details.wifi_password[0], wifi_details.password_len);

                /* Write data to EEPROM. */
                eepromReturnValue = Cy_Em_EEPROM_Write(LOGICAL_EEPROM_START,
                                            (void *)&wifi_details.wifi_ssid[0],
                                            sizeof(wifi_details),
                                            &Em_EEPROM_context);

                if(CY_EM_EEPROM_SUCCESS != eepromReturnValue)
                {
                    printf("Failed to write to EMEEPROM: %d\n", eepromReturnValue);
                }

                /* Send notification to the WiFi task to scan and connect with
                 * the given WiFi credentials
                 */
                xTaskNotify(wifi_task_handle, (NOTIF_SCAN | NOTIF_CONNECT),
                            eSetValueWithOverwrite);
            }
            else
            {
                printf("WiFi Credentials not present\n");
            }
        }
        /* Scan command */
        else if(WIFI_CONTROL_SCAN == app_custom_service_wifi_control[0])
        {
            /* Check if notifications are enabled or not. If not then there is
             * no point of starting the scan. If GATT notification is enabled
             * then ask the WiFi task to start WiFi scan
             */
            if ((conn_id != 0) &&
                (app_custom_service_wifi_networks_client_char_config[0] &
                GATT_CLIENT_CONFIG_NOTIFICATION))
            {
                xTaskNotify(wifi_task_handle, (NOTIF_SCAN), eSetValueWithOverwrite);
            }
            else
            {
                printf("Notifications for WiFi Networks characteristic are "
                        "disabled. Cannot scan for networks\n");
            }
        }
        /* Disconnect command */
        else if(WIFI_CONTROL_DISCONNECT == app_custom_service_wifi_control[0])
        {
            xTaskNotify(wifi_task_handle, NOTIF_DISCONNECT,
                        eSetValueWithOverwrite);
        }
        else
        {
            printf("Invalid command\n");
        }
        break;

    /* Notification for control characteristic.
     */
    case HDLD_CUSTOM_SERVICE_WIFI_CONTROL_CLIENT_CHAR_CONFIG:
        app_custom_service_wifi_control_client_char_config[0] = p_attr[0];
        app_custom_service_wifi_control_client_char_config[1] = p_attr[1];
        break;

    default:
        printf("Write GATT Handle not found\n");
        result = WICED_BT_GATT_INVALID_HANDLE;
        break;
    }

    return result;
}

/*******************************************************************************
* Function Name: app_gatt_connect_handler
********************************************************************************
* Summary:
* This function handles the GATT connect request events from the stack
*
* Parameters:
*  wiced_bt_gatt_connection_status_t *p_conn_status: Connection or disconnection
*
* Return:
*  wiced_bt_gatt_status_t: GATT result
*
*******************************************************************************/
wiced_bt_gatt_status_t app_gatt_connect_handler(
                       wiced_bt_gatt_connection_status_t *p_conn_status)
{
    wiced_bt_gatt_status_t status = WICED_BT_GATT_ERROR;
    wiced_result_t result;

    /* Check whether it is a connect event or disconnect event. If the device
    has been disconnected then restart advertisement */
    if (NULL != p_conn_status)
    {
        if (p_conn_status->connected)
        {
            /* Device got connected */
            printf("\nConnected: Peer BD Address: ");
            print_bd_address(p_conn_status->bd_addr);
            printf("\n");
            conn_id = p_conn_status->conn_id;
        }
        else /* Device got disconnected */
        {
            printf("\nDisconnected: Peer BD Address: ");
            print_bd_address(p_conn_status->bd_addr);
            printf("\n");

            printf("Reason for disconnection: %s\n",
                    get_bt_gatt_disconn_reason_name(p_conn_status->reason));

            conn_id = 0;

            result = wiced_bt_ble_set_raw_advertisement_data(CY_BT_ADV_PACKET_DATA_SIZE,
                                                             cy_bt_adv_packet_data);
            if (WICED_SUCCESS != result)
            {
                printf("Set ADV data failed\n");
            }

            result = wiced_bt_start_advertisements(BTM_BLE_ADVERT_UNDIRECTED_LOW,
                                                   0, NULL);
            if(WICED_SUCCESS != result)
            {
                printf("Start ADV failed");
            }
        }
        status = WICED_BT_GATT_SUCCESS;
    }

    return status;
}

/*******************************************************************************
* Function Name: app_gatts_attr_req_handler
********************************************************************************
* Summary:
* This function redirects the GATT attribute requests to the appropriate
* functions
*
* Parameters:
*  wiced_bt_gatt_attribute_request_t *p_data: GATT request data structure
*
* Return:
*  wiced_bt_gatt_status_t: GATT result
*
*******************************************************************************/
wiced_bt_gatt_status_t app_gatts_attr_req_handler(wiced_bt_gatt_attribute_request_t *p_data)
{
    wiced_bt_gatt_status_t result = WICED_BT_GATT_INVALID_PDU;
    wiced_bt_gatt_write_req_t *p_write_request = &p_data->data.write_req;

    switch (p_data->opcode)
    {
    case GATT_REQ_READ:
    case GATT_REQ_READ_BLOB:
        result = app_gatts_req_read_handler(p_data->conn_id, p_data->opcode,
                                &p_data->data.read_req, p_data->len_requested);
        break;

    case GATT_REQ_READ_BY_TYPE:
        result = app_gatt_read_by_type_handler(p_data->conn_id, p_data->opcode,
                            &p_data->data.read_by_type, p_data->len_requested);
        break;

    case GATT_REQ_WRITE:
    case GATT_CMD_WRITE:
    case GATT_CMD_SIGNED_WRITE:
        result = app_gatts_req_write_handler(p_data->conn_id, p_data->opcode,
                                             &(p_data->data.write_req));
        if ((p_data->opcode == GATT_REQ_WRITE) && (result == WICED_BT_GATT_SUCCESS))
        {
            wiced_bt_gatt_server_send_write_rsp(p_data->conn_id, p_data->opcode,
                                                p_write_request->handle);
        }
        else
        {
            wiced_bt_gatt_server_send_error_rsp(p_data->conn_id, p_data->opcode,
                                                p_write_request->handle, result);
        }
        break;

    case GATT_REQ_MTU:
        printf("Exchanged MTU from client: %d\n", p_data->data.remote_mtu);
        wiced_bt_gatt_server_send_mtu_rsp(p_data->conn_id, p_data->data.remote_mtu,
                          wiced_bt_cfg_settings.p_ble_cfg->ble_max_rx_pdu_size);
        result = WICED_BT_GATT_SUCCESS;
        break;

    default:
        break;
    }

    return result;
}

/*******************************************************************************
* Function Name: app_gatts_callback
********************************************************************************
* Summary:
* This function redirects the GATT requests to the appropriate functions
*
* Parameters:
*  wiced_bt_gatt_attribute_request_t *p_data: GATT request data structure
*  wiced_bt_gatt_event_data_t *p_data       : Pointer to BLE GATT event structures
* Return:
*  wiced_bt_gatt_status_t: GATT result
*
*******************************************************************************/
wiced_bt_gatt_status_t app_gatts_callback(wiced_bt_gatt_evt_t event,
                                          wiced_bt_gatt_event_data_t *p_data)
{
    wiced_bt_gatt_status_t result = WICED_BT_GATT_INVALID_PDU;

    printf("GATTS event: %s\n", get_bt_gatt_evt_name(event));

    switch (event)
    {
    case GATT_CONNECTION_STATUS_EVT:
        result = app_gatt_connect_handler(&p_data->connection_status);
        break;

    case GATT_ATTRIBUTE_REQUEST_EVT:
        result = app_gatts_attr_req_handler(&p_data->attribute_request);
        break;

    case GATT_GET_RESPONSE_BUFFER_EVT:
        p_data->buffer_request.buffer.p_app_rsp_buffer = wiced_bt_get_buffer(
                                          p_data->buffer_request.len_requested);

        p_data->buffer_request.buffer.p_app_ctxt = (void *)wiced_bt_free_buffer;

        if(NULL == p_data->buffer_request.buffer.p_app_rsp_buffer)
        {
            printf("Insufficient resources\n");
            result = WICED_BT_GATT_INSUF_RESOURCE;
        }
        else
        {
            result = WICED_BT_GATT_SUCCESS;
        }
        break;

    case GATT_APP_BUFFER_TRANSMITTED_EVT:
    {
        pfn_free_buffer_t pfn_free = (pfn_free_buffer_t)p_data->
                                      buffer_xmitted.p_app_ctxt;

        /* If the buffer is dynamic, the context will point to a function to
         * free it.
         */
        if (pfn_free)
            pfn_free(p_data->buffer_xmitted.p_app_data);

        result = WICED_BT_GATT_SUCCESS;
    }
    break;

    default:
        printf("GATT event not handled\n");
    }
    return result;
}

/*******************************************************************************
 * Function Name: gpio_interrupt_handler
 *******************************************************************************
 * Summary:
 *  GPIO interrupt service routine. This function detects button presses
 *  and passes a notification to the WiFi task to disconnect from WiFi and
 *  delete EMEEPROM data.
 *
 *
 * Parameters:
 *  void *callback_arg : pointer to variable passed to the ISR (unused)
 *  cyhal_gpio_event_t event : GPIO event type
 *
 *******************************************************************************/
void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event)
{
    /* Notify the WiFi task to disconnect */
    button_pressed = true;
    xTaskNotifyFromISR(wifi_task_handle, (NOTIF_DISCONNECT | NOTIF_ERASE_DATA),
                        eSetValueWithOverwrite, NULL);
}

/* [] END OF FILE */

