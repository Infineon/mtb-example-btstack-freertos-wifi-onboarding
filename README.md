# Wi-Fi Onboarding Using Bluetooth&reg; LE

This example uses the Arm&reg; Cortex&reg;-M4 (CM4) CPU of PSoC&trade; 6 MCU to communicate with the CYW43xxx combo devices and control the Wi-Fi and Bluetooth LE functionality. It uses Bluetooth LE on the combo device to help connect the Wi-Fi to the AP. It also enables low-power mode on Wi-Fi and Bluetooth LE.

[View this README on GitHub.](https://github.com/Infineon/mtb-example-btstack-freertos-wifi-onboarding)

[Provide feedback on this code example.](https://cypress.co1.qualtrics.com/jfe/form/SV_1NTns53sK2yiljn?Q_EED=eyJVbmlxdWUgRG9jIElkIjoiQ0UyMzAyMjMiLCJTcGVjIE51bWJlciI6IjAwMi0zMDIyMyIsIkRvYyBUaXRsZSI6IldpLUZpIE9uYm9hcmRpbmcgVXNpbmcgQmx1ZXRvb3RoJnJlZzsgTEUiLCJyaWQiOiJhbWthIiwiRG9jIHZlcnNpb24iOiI0LjIuMCIsIkRvYyBMYW5ndWFnZSI6IkVuZ2xpc2giLCJEb2MgRGl2aXNpb24iOiJNQ0QiLCJEb2MgQlUiOiJJQ1ciLCJEb2MgRmFtaWx5IjoiV0lGSSJ9)

## Requirements

- [ModusToolbox&trade;](https://www.infineon.com/modustoolbox) v3.1 or later (tested with v3.1)
- Board support package (BSP) minimum required version: 4.0.0
- Programming language: C
- Associated parts: All [PSoC&trade; 6 MCU](https://www.infineon.com/cms/en/product/microcontroller/32-bit-psoc-arm-cortex-microcontroller/psoc-6-32-bit-arm-cortex-m4-mcu) parts, [AIROC&trade; CYW20819 Bluetooth&reg; & Bluetooth&reg; LE SoC](https://www.infineon.com/cms/en/product/wireless-connectivity/airoc-bluetooth-le-bluetooth-multiprotocol/airoc-bluetooth-le-bluetooth/cyw20819), [AIROC&trade; CYW43012 Wi-Fi & Bluetooth&reg; combo chip](https://www.infineon.com/cms/en/product/wireless-connectivity/airoc-wi-fi-plus-bluetooth-combos/wi-fi-4-802.11n/cyw43012), [AIROC&trade; CYW4343W Wi-Fi & Bluetooth&reg; combo chip](https://www.infineon.com/cms/en/product/wireless-connectivity/airoc-wi-fi-plus-bluetooth-combos/wi-fi-4-802.11n/cyw4343w), [AIROC&trade; CYW4373 Wi-Fi & Bluetooth&reg; combo chip](https://www.infineon.com/cms/en/product/wireless-connectivity/airoc-wi-fi-plus-bluetooth-combos/wi-fi-5-802.11ac/cyw4373), [AIROC&trade; CYW43439 Wi-Fi & Bluetooth&reg; combo chip](https://www.infineon.com/cms/en/product/wireless-connectivity/airoc-wi-fi-plus-bluetooth-combos/wi-fi-4-802.11n/cyw43439), ,[AIROC&trade; CYW43022 Wi-Fi & Bluetooth&reg; combo chip](https://www.infineon.com/cms/en/product/wireless-connectivity/airoc-wi-fi-plus-bluetooth-combos/wi-fi-5-802.11ac/cyw43022)


## Supported toolchains (make variable 'TOOLCHAIN')

- GNU Arm&reg; Embedded Compiler v11.3.1 (`GCC_ARM`) – Default value of `TOOLCHAIN`
- Arm&reg; Compiler v6.16 (`ARM`)
- IAR C/C++ Compiler v9.30.1 (`IAR`)


## Supported kits

- [PSoC&trade; 62S2 Wi-Fi Bluetooth&reg; Pioneer Kit](https://www.infineon.com/CY8CKIT-062S2-43012) (`CY8CKIT-062S2-43012`) – Default value of `TARGET`
- Rapid IoT Connect Developer kit (`CYSBSYSKIT-DEV-01`)
- [PSoC&trade; 6 Wi-Fi Bluetooth&reg; Pioneer Kit](https://www.infineon.com/CY8CKIT-062-WIFI-BT) (`CY8CKIT-062-WIFI-BT`)
- [PSoC&trade; 62S1 Wi-Fi Bluetooth&reg; Pioneer Kit](https://www.infineon.com/CYW9P62S1-43438EVB-01) (`CYW9P62S1-43438EVB-01`)
- [PSoC&trade; 62S1 Wi-Fi Bluetooth&reg; Pioneer Kit](https://www.infineon.com/CYW9P62S1-43012EVB-01) (`CYW9P62S1-43012EVB-01`)
- [PSoC&trade; 6 Evaluation Kit](https://www.infineon.com/CY8CEVAL-062S2) (`CY8CEVAL-062S2-LAI-4373M2`,`CY8CEVAL-062S2-CYW43022CUB`)

## Hardware setup

This example uses the board's default configuration. See the kit user guide to ensure that the board is configured correctly.

> **Note:** The PSoC&trade; 6 Bluetooth&reg; LE Pioneer Kit (CY8CKIT-062-BLE) and the PSoC&trade; 6 Wi-Fi Bluetooth&reg; Pioneer Kit (CY8CKIT-062-WIFI-BT) ship with KitProg2 installed. ModusToolbox&trade; requires KitProg3. Before using this code example, make sure that the board is upgraded to KitProg3. The tool and instructions are available in the [Firmware Loader](https://github.com/Infineon/Firmware-loader) GitHub repository. If you do not upgrade, you will see an error like "unable to find CMSIS-DAP device" or "KitProg firmware is out of date".

## Software setup

**Note:** This code example consists of two parts: a Bluetooth LE GAP Peripheral and a Bluetooth LE GAP Central.

For the Bluetooth LE GAP Central, download and install the AIROC&trade; Bluetooth&reg; Connect App for [Android](https://play.google.com/store/apps/details?id=com.infineon.airocbluetoothconnect).

Scan the following QR code from your mobile phone to download the AIROC&trade; Bluetooth&reg; Connect mobile app.

![AppQR](images/qr.png)

**Note:** AIROC&trade; Bluetooth&reg; Connect App for iOS is coming soon on App Store. If you are using an iPhone, optionally you can download and install Lightblue app.

Install a terminal emulator if you don't have one. instructions in this document use [Tera Term](https://teratermproject.github.io/index-en.html). all other required software come bundled with the Eclipse IDE for ModusToolbox&trade;.

## Using the code example


### Create the project

The ModusToolbox&trade; tools package provides the Project Creator as both a GUI tool and a command line tool.

<details><summary><b>Use Project Creator GUI</b></summary>

1. Open the Project Creator GUI tool.

   There are several ways to do this, including launching it from the dashboard or from inside the Eclipse IDE. For more details, see the [Project Creator user guide](https://www.infineon.com/ModusToolboxProjectCreator) (locally available at *{ModusToolbox&trade; install directory}/tools_{version}/project-creator/docs/project-creator.pdf*).

2. On the **Choose Board Support Package (BSP)** page, select a kit supported by this code example. See [Supported kits](#supported-kits-make-variable-target).

   > **Note:** To use this code example for a kit not listed here, you may need to update the source files. If the kit does not have the required resources, the application may not work.

3. On the **Select Application** page:

   a. Select the **Applications(s) Root Path** and the **Target IDE**.

   > **Note:** Depending on how you open the Project Creator tool, these fields may be pre-selected for you.

   b.	Select this code example from the list by enabling its check box.

   > **Note:** You can narrow the list of displayed examples by typing in the filter box.

   c. (Optional) Change the suggested **New Application Name** and **New BSP Name**.

   d. Click **Create** to complete the application creation process.

</details>

<details><summary><b>Use Project Creator CLI</b></summary>

The 'project-creator-cli' tool can be used to create applications from a CLI terminal or from within batch files or shell scripts. This tool is available in the *{ModusToolbox&trade; install directory}/tools_{version}/project-creator/* directory.

Use a CLI terminal to invoke the 'project-creator-cli' tool. On Windows, use the command-line 'modus-shell' program provided in the ModusToolbox&trade; installation instead of a standard Windows command-line application. This shell provides access to all ModusToolbox&trade; tools. You can access it by typing "modus-shell" in the search box in the Windows menu. In Linux and macOS, you can use any terminal application.

The following example clones the "[LE Find me](https://github.com/Infineon/mtb-example-btstack-freertos-wifi-onboarding)" application with the desired name "WiFiOnboarding" configured for the *CY8CKIT-062-WIFI-BT* BSP into the specified working directory, *C:/mtb_projects*:

   ```
   project-creator-cli --board-id CY8CKIT-062-WIFI-BT --app-id mtb-example-btstack-freertos-wifi-onboarding --user-app-name WiFiOnboarding Using Bluetooth&reg; LE--target-dir "C:/mtb_projects"
   ```

The 'project-creator-cli' tool has the following arguments:

Argument | Description | Required/optional
---------|-------------|-----------
`--board-id` | Defined in the <id> field of the [BSP](https://github.com/Infineon?q=bsp-manifest&type=&language=&sort=) manifest | Required
`--app-id`   | Defined in the <id> field of the [CE](https://github.com/Infineon?q=ce-manifest&type=&language=&sort=) manifest | Required
`--target-dir`| Specify the directory in which the application is to be created if you prefer not to use the default current working directory | Optional
`--user-app-name`| Specify the name of the application if you prefer to have a name other than the example's default name | Optional

> **Note:** The project-creator-cli tool uses the `git clone` and `make getlibs` commands to fetch the repository and import the required libraries. For details, see the "Project creator tools" section of the [ModusToolbox&trade; tools package user guide](https://www.infineon.com/ModusToolboxUserGuide) (locally available at {ModusToolbox&trade; install directory}/docs_{version}/mtb_user_guide.pdf).

</details>


### Open the project

After the project has been created, you can open it in your preferred development environment.


<details><summary><b>Eclipse IDE</b></summary>

If you opened the Project Creator tool from the included Eclipse IDE, the project will open in Eclipse automatically.

For more details, see the [Eclipse IDE for ModusToolbox&trade; user guide](https://www.infineon.com/MTBEclipseIDEUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_ide_user_guide.pdf*).

</details>


<details><summary><b>Visual Studio (VS) Code</b></summary>

Launch VS Code manually, and then open the generated *{project-name}.code-workspace* file located in the project directory.

For more details, see the [Visual Studio Code for ModusToolbox&trade; user guide](https://www.infineon.com/MTBVSCodeUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_vscode_user_guide.pdf*).

</details>


<details><summary><b>Keil µVision</b></summary>

Double-click the generated *{project-name}.cprj* file to launch the Keil µVision IDE.

For more details, see the [Keil µVision for ModusToolbox&trade; user guide](https://www.infineon.com/MTBuVisionUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_uvision_user_guide.pdf*).

</details>


<details><summary><b>IAR Embedded Workbench</b></summary>

Open IAR Embedded Workbench manually, and create a new project. Then select the generated *{project-name}.ipcf* file located in the project directory.

For more details, see the [IAR Embedded Workbench for ModusToolbox&trade; user guide](https://www.infineon.com/MTBIARUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_iar_user_guide.pdf*).

</details>


<details><summary><b>Command line</b></summary>

If you prefer to use the CLI, open the appropriate terminal, and navigate to the project directory. On Windows, use the command line 'modus-shell' program; on Linux and macOS, you can use any terminal application. From there, you can run various `make` commands.

For more details, see the [ModusToolbox&trade; tools package user guide](https://www.infineon.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mtb_user_guide.pdf*).

</details>


## Operation

If using a PSoC&trade; 64 "Secure" MCU kit (like CY8CKIT-064B0S2-4343W), the PSoC&trade; 64 device must be provisioned with keys and policies before being programmed. Follow the instructions in the ["Secure Boot" SDK user guide](https://www.infineon.com/dgdlac/Infineon-PSoC_64_Secure_MCU_Secure_Boot_SDK_User_Guide-Software-v07_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f8c361a7666) to provision the device. If the kit is already provisioned, copy-paste the keys and policy folder to the application folder.

1. Connect the board to your PC using the provided USB cable through the KitProg3 USB connector.

2. Open a terminal program and select the KitProg3 COM port. Set the serial port parameters to 8N1 and 115200 baud.

3. Program the board using one of the following:

   <details><summary><b>Using Eclipse IDE for ModusToolbox&trade;</b></summary>

      1. Select the application project in the Project Explorer.

      2. In the **Quick Panel**, scroll down, and click **\<Application Name> Program (KitProg3_MiniProg4)**.
   </details>


   <details><summary><b>In other IDEs</b></summary>

   Follow the instructions in your preferred IDE.
   </details>


   <details><summary><b>Using CLI</b></summary>

     From the terminal, execute the `make program` command to build and program the application using the default toolchain to the default target. The default toolchain is specified in the application's Makefile but you can override this value manually:
      ```
      make program TOOLCHAIN=<toolchain>
      ```

      Example:
      ```
      make program TOOLCHAIN=GCC_ARM
      ```
   </details>

      After programming, the application starts automatically. Observe the messages on the UART terminal, and wait for the device to initialize BT stack and Wi-Fi.

      The device initializes the BT stack and starts advertisement.

     **Figure 1. Boot-up Log**
     ![Figure 1](./images/figure-1.png)

5. Do the following to test using the AIROC&trade; Bluetooth&reg; Connect mobile app:

    1. Turn ON Bluetooth on your Android or iOS device.

    2. Launch the AIROC&trade; Bluetooth&reg; Connect app.

    3. Press the reset switch on the kit to start sending advertisements.

    4. Swipe down on the AIROC&trade; Bluetooth&reg; Connect app home screen to start scanning for Bluetooth LE Peripherals. Your device (“bleProv”) appears in the AIROC&trade; Bluetooth&reg; Connect app home screen. Select your device to establish a Bluetooth LE connection.

    5. Select the **GATT DB** Profile from the carousel view then select **Unknown Service**.

    6. To scan for the availabe Wi-Fi networks, enable the notifications in the characteristic with UUID ending in **66**, then select the characteristic with UUID ending in **67** and Select **Notify**. Write hex value 2 to this characterisitc. The device will start scanning and send the network details as notifications in the characteristic with UUID ending in 66. The data is in the Type, Length, Value (TLV) format. The first byte gives the type which is **1** for SSID and **2** for security details. The next byte gives the length of the data. After this data there will be another type byte followed by length and data. The security value consists of 4 bytes (little-endian) which needs to be combined into a 32-bit value.

       **Note:** If the notifications are not enabled in the characteristic with UUID ending in 66 the scan will not be started as there is no way to report available networks to the user. The user can still connect to a network by entering WIFI SSID (in UUID ending with 63), Password (in UUID ending with 64) and then initiating the connect request by writing '1' in WIFI control chanracteristic (in UUID ending with 67).

    7. To connect to the Wi-Fi network you need to send SSID and password data to the client device. You can select one of the networks which was discovered during scan or you can give another set of details. If the given network is not available then the device will store the values and try to connect on next restart. Note that the data is stored in non-volatile storage only when the connect command is sent. There are two ways to send the WiFi credentials. Either send the WiFi SSID and password separately or together.

       1. Send SSID and password separately
           1. Select the UUID ending in 63. Write your Wi-Fi SSID in hex or ASCII format.
           2. Select the UUID ending in 64. Write your Wi-Fi password in hex or ASCII format.
       2. Send the SSID and password together
           1. Format the SSID and password data in TLV format. For SSID the type value is 1 and for password the type value is 2. The first byte of the data should be type and for this example it will be 1 for SSID followed by length of the SSID and then the SSID data which is in hex format. This is followed by TLV value for password. For e.g if the SSID is WIFISSID and password is PASSWORD then the formatted value will be:

           ```
           01 08 57 49 46 49 53 53 49 44 02 08 50 41 53 53 57 4f 52 44
           ```
           2. Select the UUID ending in 65. Write your formatted data

    8. If you are sending SSID and password separately then it is easier to input the data directly in the ASCII format. If you are sending them together then you have to use the hex format as the type and length values are in hex format.

       **Note:** You can use an online tool for converting the SSID and password from string to hex but be careful about where you type in your password.

       **Figure 2. AIROC&trade; Bluetooth&reg; Connect App Flow**
       
      ![Figure 2](./images/figure-2.png)

    9. Select the attribute with the UUID ending in **67**. Select **Notify** (if not already done earlier). Write hex value 1 to this characteristic to connect to the WiFi network. If the connection is successful then the server will send a notification with value 1 otherwise with value 0.

       **Figure 3. Connection Log**

      ![Figure 3](./images/figure-3.png)

6. Once the Wi-Fi SSID and password is provided by the client it is stored in the EEPROM. To delete this data the user needs to press User Button.

## Debugging

You can debug the example to step through the code.


<details><summary><b>In Eclipse IDE</b></summary>

Use the **\<Application Name> Debug (KitProg3_MiniProg4)** configuration in the **Quick Panel**. For details, see the "Program and debug" section in the [Eclipse IDE for ModusToolbox&trade; user guide](https://www.infineon.com/MTBEclipseIDEUserGuide).

<mark> Add the below Note for relevant CEs only, like PSoC 6 MCU based. Remove this note for others.

> **Note:** **(Only while debugging)** On the CM4 CPU, some code in `main()` may execute before the debugger halts at the beginning of `main()`. This means that some code executes twice – once before the debugger stops execution, and again after the debugger resets the program counter to the beginning of `main()`. See [KBA231071](https://community.infineon.com/docs/DOC-21143) to learn about this and for the workaround.

</details>


<details><summary><b>In other IDEs</b></summary>

Follow the instructions in your preferred IDE.
</details>
## Design and implementation

### Introduction

In this example, Bluetooth LE provides a mechanism for the device to connect to a Wi-Fi AP by providing the Wi-Fi SSID and password in a secure manner. The Wi-Fi credentials are stored in EEPROM so that the device can use this data upon reset to connect to an AP without requiring Bluetooth LE intervention. Note that the data stored in the EEPROM is unencrypted

The Wi-Fi SSID and password are exchanged using custom GATT service and characteristics. There are three characteristics to send SSID and password. The first two are used to send the SSID and password separately and the third is used to send the data together as described previously. The fourth characteristic is to receive WIFi netowrk details when the device is asked to scan for networks. There is a fifth custom characteristic, which gives the command to connect, disconnect and scan. Only the Wi-Fi SSID characteristic is readable; All others are either writable or notifiable or both. The device needs to be paired before any characteristic can be read from or written to.

**Table 1. Application Source Files**

|**File Name**            |**Comments**      |
| ------------------------|---------------------------------------|
| *main.c* | Has the application entry function. It initializes the UART for debugging and then initializes the controller stack. It handles Bluetooth LE initialization, configuration, advertisement, and responses to Bluetooth LE events.|
| *wifi_task.c* | It contains the wifi_task which helps to connect/disconnect to/from the Wi-Fi AP based on the Wi-Fi SSID and password provided.|
| *app_utils.c* | Has some utility functions which help to convert return and status values to meaningful text|

**Table 2. Functions in *main.c***

| **Function Name** | **Functionality** |
|-------------------|-------------------|
| `main` | This is the main function for the CM4 CPU. It does the following:
| |1. Initializes the BSP  
| |2. Enables global interrupt
| |3. Initializes retarget IO
| |4. Initializes platform configuration 
| |5. Creates Wi-Fi connect and disconnect tasks 
| |6. Starts the scheduler |
| `application_init` |This function is called from the BTM enabled event, and does the following:  
| |1. Creates a WICED heap
| |2. Initializes button GPIO, enable interrupt and register callback
| |3. Checks if Wi-Fi credentials data is available in the EEPROM
| |4. Initializes and registers the GATT DB 
| |5. Sets pairable mode to `TRUE` 
| |6. Sets ADV data and starts advertising |
| `app_management_cback` | Handles BT stack events |
| `app_get_attribute` | Searches through the GATT DB to point to the attribute corresponding to the given handle |
| `app_gatts_req_read_handler` | Handles GATT read request events from the stack |
| `app_gatt_read_by_type_handler` | Handles GATT read by type requests |
| `app_gatts_req_write_handler` | Handles GATT write request events from the stack |
| `app_gatt_connect_callback` | Handles GATT connect request events from the stack |
| `app_gatts_req_cb` | Redirects GATT attribute requests to the appropriate functions |
| `app_gatts_callback` | This is the callback function for GATT events that was registered when the GATT database was initialized in the BTM enabled event.|
| `gpio_interrupt_handler` | GPIO interrupt service routine. This function detects button presses, deletes the Wi-Fi data from the EEPROM, and starts Bluetooth LE ADV. |

**Table 3. Functions in *wifi_task.c***

| **Function Name** | **Functionality** |
|-------------------|-------------------|
| `wifi_task` | Initializes the WCM module and connects disconnects to/from the AP as well as starts Wi-Fi scan |
| `scan_callback` | The callback function which accumulates the scan results |


### Bluetooth LE GATT Custom Service
This example uses custom GATT service and characteristics to communicate with the Bluetooth LE GATT client. The GATT database was created using the Bluetooth Configurator. The configurator generates files called cycfg_gatt_db.c and cycfg_gatt_db.h which contain the GATT DB. See the {ModusToolbox install directory}/tools_{version}/bt-configurator/docs/bt-configurator.pdf to learn to create a GATT database.

The custom characteristics used in this example is detailed in the specification called "Infineon Wi-Fi onboarding service (IWOS)" (file name: 002-33722_00_V.pdf included with this code example)

### Low Power
To enable Low Power in your design you can refer to the Cypress **Low Power Assistant Middleware Library Guide** located in *<application_folder>/libs/lpa/docs/lpa_api_reference_manual/html/index.html*. Refer to the **Part 2** and **Part 3** of this guide for Wi-Fi and Bluetooth low power respectively.

### Resources and settings
This section explains the ModusToolbox resources and their configuration as used in this code example. Note that all the configuration explained in this section has already been done in the code example. The ModusToolbox IDE stores the configuration settings of the application in the *design.modus* file. This file is used by the graphical configurators, which generate the configuration firmware. This firmware is stored in the application’s *GeneratedSource* folder.

- **Device Configurator:** The Device Configurator is used to enable/configure the peripherals and the pins used in the application. See the
[Device Configurator Guide](https://www.infineon.com/dgdl/Infineon-ModusToolbox_Device_Configurator_Guide_4-UserManual-v01_00-EN.pdf?fileId=8ac78c8c7d718a49017d99ab297631cb&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-files).

- **Bluetooth Configurator:** The Bluetooth Configurator is used for generating/modifying the Bluetooth LE GATT database. See the
[Bluetooth Configurator Guide](https://www.infineon.com/dgdl/Infineon-ModusToolbox_Bluetooth_Configurator_Guide_3-UserManual-v01_00-EN.pdf?fileId=8ac78c8c7d718a49017d99aaf5b231be&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-files).

## Related resources


Resources  | Links
-----------|----------------------------------
Application notes  | [AN228571](https://www.infineon.com/AN228571) – Getting started with PSoC&trade; 6 MCU on ModusToolbox&trade; <br>  [AN215656](https://www.infineon.com/AN215656) – PSoC&trade; 6 MCU: Dual-CPU system design <br> [AN79953](https://www.infineon.com/AN79953) – Getting started with PSoC&trade; 4 <br>  [AN85951](https://www.infineon.com/AN85951) – PSoC&trade; 4 and PSoC&trade; 6 MCU CAPSENSE&trade; design guide
Code examples  | [Using ModusToolbox&trade;](https://github.com/Infineon/Code-Examples-for-ModusToolbox-Software) on GitHub
Device documentation | [PSoC&trade; 6 MCU datasheets](https://documentation.infineon.com/html/psoc6/bnm1651211483724.html) <br> [PSoC&trade; 6 technical reference manuals](https://documentation.infineon.com/html/psoc6/zrs1651212645947.html)<br> [PSoC&trade; 4 datasheets](https://www.infineon.com/cms/en/search.html?intc=searchkwr-return#!view=downloads&term=psoc%204&doc_group=Data%20Sheet) <br>[PSoC&trade; 4 technical reference manuals](https://www.infineon.com/cms/en/search.html#!term=psoc%204%20technical%20reference%20manual&view=all)
Development kits | Select your kits from the [Evaluation board finder](https://www.infineon.com/cms/en/design-support/finder-selection-tools/product-finder/evaluation-board).
Libraries on GitHub  | [mtb-pdl-cat1](https://github.com/Infineon/mtb-pdl-cat1) – PSoC&trade; 6 Peripheral Driver Library (PDL)  <br> [mtb-hal-cat1](https://github.com/Infineon/mtb-hal-cat1) – Hardware Abstraction Layer (HAL) library <br> [retarget-io](https://github.com/Infineon/retarget-io) – Utility library to retarget STDIO messages to a UART port <br>  [mtb-pdl-cat2](https://github.com/Infineon/mtb-pdl-cat2) – PSoC&trade; 4 Peripheral Driver Library (PDL) <br>  [mtb-hal-cat2](https://github.com/Infineon/mtb-hal-cat2) – Hardware Abstraction Layer (HAL) library
Middleware on GitHub  | [capsense](https://github.com/Infineon/capsense) – CAPSENSE&trade; library and documents <br> [psoc6-middleware](https://github.com/Infineon/modustoolbox-software#psoc-6-middleware-libraries) – Links to all PSoC&trade; 6 MCU middleware
Tools  | [ModusToolbox&trade;](https://www.infineon.com/modustoolbox) – ModusToolbox&trade; is a collection of easy-to-use libraries and tools enabling rapid development with Infineon MCUs for applications ranging from wireless and cloud-connected systems, edge AI/ML, embedded sense and control, to wired USB connectivity using PSoC&trade; Industrial/IoT MCUs, AIROC&trade; Wi-Fi and Bluetooth&reg; connectivity devices, XMC&trade; Industrial MCUs, and EZ-USB&trade;/EZ-PD&trade; wired connectivity controllers. ModusToolbox&trade; incorporates a comprehensive set of BSPs, HAL, libraries, configuration tools, and provides support for industry-standard IDEs to fast-track your embedded application development.

<br>

## Other resources


Infineon provides a wealth of data at [www.infineon.com](https://www.infineon.com) to help you select the right device, and quickly and effectively integrate it into your design.

For PSoC&trade; 6 MCU devices, see [How to design with PSoC&trade; 6 MCU - KBA223067](https://community.infineon.com/docs/DOC-14644) in the Infineon Developer community.


## Document history

Document title: *CE230223* - *Wi-Fi Onboarding Using Bluetooth&reg; LE*

| Version | Description of Change |
| ------- | --------------------- |
| 1.0.0   | New code example      |
| 2.0.0   | Major update to support ModusToolbox software v2.2<br> This version is not backward compatible with ModusToolbox software v2.1  |
| 2.1.0   | Added support for CYSBSYSKIT-DEV-01|
| 3.0.0   | BTSTACK version updated to 3.0<br>This update required that the example be used with ModusToolbox sofware 2.3 (with patch 2.3.1)|
| 3.1.0   | Added new characteristics to the custom service |
| 3.2.0   | Added support for 43439 kit |
| 4.0.0   | Updated to support ModusToolbox&trade; v3.0 and BSPs v4.X
| 4.1.0   | Added support for CY8CEVAL-062S2-CYW43022CUB
| 4.2.0   | Added support for CY8CKIT-062S2-AI
<br>



All referenced product or service names and trademarks are the property of their respective owners.

The Bluetooth&reg; word mark and logos are registered trademarks owned by Bluetooth SIG, Inc., and any use of such marks by Infineon is under license.


---------------------------------------------------------

© Cypress Semiconductor Corporation, 2020-2024. This document is the property of Cypress Semiconductor Corporation, an Infineon Technologies company, and its affiliates ("Cypress").  This document, including any software or firmware included or referenced in this document ("Software"), is owned by Cypress under the intellectual property laws and treaties of the United States and other countries worldwide.  Cypress reserves all rights under such laws and treaties and does not, except as specifically stated in this paragraph, grant any license under its patents, copyrights, trademarks, or other intellectual property rights.  If the Software is not accompanied by a license agreement and you do not otherwise have a written agreement with Cypress governing the use of the Software, then Cypress hereby grants you a personal, non-exclusive, nontransferable license (without the right to sublicense) (1) under its copyright rights in the Software (a) for Software provided in source code form, to modify and reproduce the Software solely for use with Cypress hardware products, only internally within your organization, and (b) to distribute the Software in binary code form externally to end users (either directly or indirectly through resellers and distributors), solely for use on Cypress hardware product units, and (2) under those claims of Cypress's patents that are infringed by the Software (as provided by Cypress, unmodified) to make, use, distribute, and import the Software solely for use with Cypress hardware products.  Any other use, reproduction, modification, translation, or compilation of the Software is prohibited.
<br>
TO THE EXTENT PERMITTED BY APPLICABLE LAW, CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS DOCUMENT OR ANY SOFTWARE OR ACCOMPANYING HARDWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  No computing device can be absolutely secure.  Therefore, despite security measures implemented in Cypress hardware or software products, Cypress shall have no liability arising out of any security breach, such as unauthorized access to or use of a Cypress product. CYPRESS DOES NOT REPRESENT, WARRANT, OR GUARANTEE THAT CYPRESS PRODUCTS, OR SYSTEMS CREATED USING CYPRESS PRODUCTS, WILL BE FREE FROM CORRUPTION, ATTACK, VIRUSES, INTERFERENCE, HACKING, DATA LOSS OR THEFT, OR OTHER SECURITY INTRUSION (collectively, "Security Breach").  Cypress disclaims any liability relating to any Security Breach, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any Security Breach.  In addition, the products described in these materials may contain design defects or errors known as errata which may cause the product to deviate from published specifications. To the extent permitted by applicable law, Cypress reserves the right to make changes to this document without further notice. Cypress does not assume any liability arising out of the application or use of any product or circuit described in this document. Any information provided in this document, including any sample design information or programming code, is provided only for reference purposes.  It is the responsibility of the user of this document to properly design, program, and test the functionality and safety of any application made of this information and any resulting product.  "High-Risk Device" means any device or system whose failure could cause personal injury, death, or property damage.  Examples of High-Risk Devices are weapons, nuclear installations, surgical implants, and other medical devices.  "Critical Component" means any component of a High-Risk Device whose failure to perform can be reasonably expected to cause, directly or indirectly, the failure of the High-Risk Device, or to affect its safety or effectiveness.  Cypress is not liable, in whole or in part, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any use of a Cypress product as a Critical Component in a High-Risk Device. You shall indemnify and hold Cypress, including its affiliates, and its directors, officers, employees, agents, distributors, and assigns harmless from and against all claims, costs, damages, and expenses, arising out of any claim, including claims for product liability, personal injury or death, or property damage arising from any use of a Cypress product as a Critical Component in a High-Risk Device. Cypress products are not intended or authorized for use as a Critical Component in any High-Risk Device except to the limited extent that (i) Cypress's published data sheet for the product explicitly states Cypress has qualified the product for use in a specific High-Risk Device, or (ii) Cypress has given you advance written authorization to use the product as a Critical Component in the specific High-Risk Device and you have signed a separate indemnification agreement.
<br>
Cypress, the Cypress logo, and combinations thereof, ModusToolbox, PSoC, CAPSENSE, EZ-USB, F-RAM, and TRAVEO are trademarks or registered trademarks of Cypress or a subsidiary of Cypress in the United States or in other countries. For a more complete list of Cypress trademarks, visit www.infineon.com. Other names and brands may be claimed as property of their respective owners.
