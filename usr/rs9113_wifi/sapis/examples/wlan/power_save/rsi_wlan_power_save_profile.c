/**
 * @file    rsi_wlan_power_save.c
 * @version 0.4
 * @date    19 May 2016
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contains example application for power save.
 *  @section Description  This file contains example application for power save.
 *
 *
 */
/**
 * Include files
 * */

//! include file to refer data types
#include "rsi_data_types.h"

//! COMMON include file to refer wlan APIs
#include "rsi_common_apis.h"

//! WLAN include file to refer wlan APIs
#include "rsi_wlan_apis.h"

//! socket include file to refer socket APIs
#include "rsi_socket.h"

//! Error include files
#include "rsi_error.h"

//! OS include file to refer OS specific functionality
#include "rsi_os.h"
//! Access point SSID to connect
#define SSID                  "REDPINE_AP"

//! Security type
#define SECURITY_TYPE         RSI_OPEN

//! Password
#define PSK                  ""

//! DHCP mode 1- Enable 0- Disable
#define DHCP_MODE             1

//! If DHCP mode is disabled given IP statically
#if !(DHCP_MODE)

//! IP address of the module 
//! E.g: 0x650AA8C0 == 192.168.10.101
#define DEVICE_IP             0x650AA8C0

//! IP address of Gateway
//! E.g: 0x010AA8C0 == 192.168.10.1
#define GATEWAY               0x010AA8C0

//! IP address of netmask
//! E.g: 0x00FFFFFF == 255.255.255.0
#define NETMASK               0x00FFFFFF 

#endif

//! Server port number
#define SERVER_PORT       5001

//! Server IP address. Should be in reverse long format
//! E.g: 0x640AA8C0 == 192.168.10.100
#define SERVER_IP_ADDRESS 0x6700A8C0

//! Number of packet to send or receive
#define NUMBER_OF_PACKETS 1000

//! Power Save Profile Mode
#define PSP_TYPE              RSI_MAX_PSP

//! Runs scheduler for some delay and do not send any command
#define RSI_DELAY             6000000

//! Memory length for driver
#define GLOBAL_BUFF_LEN 8000

//! Wlan task priority
#define RSI_WLAN_TASK_PRIORITY   1

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY   2

//! Wlan task stack size
#define RSI_WLAN_TASK_STACK_SIZE  500

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE  500


//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];

void rsi_init_gpio_pin();

int32_t rsi_powersave_profile_app()
{
  int32_t     client_socket;
  struct      sockaddr_in server_addr;
  int32_t     status       = RSI_SUCCESS;
  int32_t     packet_count = 0;
  uint32_t    delay;
#if !(DHCP_MODE)
  uint32_t    ip_addr      = DEVICE_IP;
  uint32_t    network_mask = NETMASK;
  uint32_t    gateway      = GATEWAY;
#endif

  	//! WC initialization
  status = rsi_wireless_init(0, 0);
  if(status != RSI_SUCCESS)
  {
    return status;
  }

  while (1)
  {
    //! Wlan radio init
    status = rsi_wlan_radio_init();
    if(status != RSI_SUCCESS)
    {

      //! Return the status if error in sending command occurs
      return status;
    }

    //! Apply power save profile with deep sleep
    status = rsi_wlan_power_save_profile(RSI_SLEEP_MODE_8, PSP_TYPE);
    if(status != RSI_SUCCESS)
    {
      return status;
    }


    //! wait in scheduler for some time
    for(delay = 0; delay < RSI_DELAY; delay++)
    {
#ifndef RSI_WITH_OS
    	rsi_wireless_driver_task();
#endif
    }

    //! Disable power save profile
    status = rsi_wlan_power_save_profile(RSI_ACTIVE, PSP_TYPE);
    if(status != RSI_SUCCESS)
    {
      return status;
    }

    //! Connect to an Acces point
    status = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, PSK);
    if(status != RSI_SUCCESS)
    {
      return status;
    }

    //! Configure IP
#if DHCP_MODE
    status = rsi_config_ipaddress(RSI_IP_VERSION_4, RSI_DHCP, 0, 0, 0, NULL, 0, 0);
#else
    status = rsi_config_ipaddress(RSI_IP_VERSION_4, RSI_STATIC, (uint8_t *)&ip_addr, (uint8_t *)&network_mask, (uint8_t *)&gateway, NULL, 0, 0);
#endif 

    //! Create socket
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(client_socket < 0)
    {
      status = rsi_wlan_get_status();
      return status;
    }

    //! Set server structure
    memset(&server_addr, 0, sizeof(server_addr));

    //! Set server address family
    server_addr.sin_family = AF_INET;

    //! Set server port number, using htons function to use proper byte order
    server_addr.sin_port = htons(SERVER_PORT);

    //! Set IP address to localhost
    server_addr.sin_addr.s_addr = SERVER_IP_ADDRESS;

    while(packet_count < NUMBER_OF_PACKETS)
    {
      //! Send data on socket
      status = sendto(client_socket, (int8_t *)"Hello from UDP client!!!", (sizeof("Hello from UDP client!!!") - 1), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
      if(status < 0)
      {
        status = rsi_wlan_get_status();
        return status;
      }

      //! Increase packet count
      packet_count++;
    }

    //! Disconnect from AP
    rsi_wlan_disconnect();

    //! setting packet count to 0 to send data again
    packet_count = 0;

  }

  return status;
}

void app_task()
{
	////////////////////////
	//! Application code ///
	////////////////////////
	return;
}

void main_loop(void)
{
  while(1)
  {
    //! Application task
    app_task();

    //! event loop 
    rsi_wireless_driver_task();

  }
}

int main()
{
  int32_t status;

#ifdef RSI_WITH_OS

  rsi_task_handle_t wlan_task_handle = NULL;

  rsi_task_handle_t driver_task_handle = NULL;
#endif

#ifndef RSI_SAMPLE_HAL
  //! Board Initialization
  Board_init();
#endif

  //! Driver initialization
    status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
    if((status < 0) || (status > GLOBAL_BUFF_LEN))
    {
      return status;
    }

    //! RS9113 intialisation
    status = rsi_device_init(RSI_LOAD_IMAGE_I_FW);
    if(status != RSI_SUCCESS)
    {
      return status;
    }

#ifdef RSI_WITH_OS
  //! OS case
  //! Task created for WLAN task
  rsi_task_create(rsi_powersave_profile_app, "wlan_task", RSI_WLAN_TASK_STACK_SIZE, NULL, RSI_WLAN_TASK_PRIORITY, &wlan_task_handle);

  //! Task created for Driver task
  rsi_task_create(rsi_wireless_driver_task, "driver_task",RSI_DRIVER_TASK_STACK_SIZE, NULL, RSI_DRIVER_TASK_PRIORITY, &driver_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

#else
  //! NON - OS case
  //! CallPower save application
  status = rsi_powersave_profile_app();

  //! Application main loop
  main_loop();
#endif
  return status;

}

