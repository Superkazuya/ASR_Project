#include "config.h"

#include "ff.h"
#include "usbh_core.h"
#include <stdio.h>
#include "usbh_msc_core.h"
#include "usb_hcd_int.h"
#include "usb.h"

 USB_OTG_CORE_HANDLE          USB_OTG_Core;
 USBH_HOST                    USB_Host;

FATFS fatfs;
FIL file;
/*
FIL fileR;
DIR dir;
FILINFO fno;
*/

void not_a_single_xxxx_is_given();
void foo(void*);
void bar(uint8_t);
void foobar(USBH_CfgDesc_TypeDef *, USBH_InterfaceDesc_TypeDef *, USBH_EpDesc_TypeDef *);
USBH_USR_Status UserInput();
void EnumerationDone();
uint32_t write_file(char* _filename, void* _buffer, uint32_t _buf_size);
int USBH_USR_MSC_Application();


USBH_Usr_cb_TypeDef usr_cbs = 
{
  not_a_single_xxxx_is_given,       /* HostLibInitialized */
  not_a_single_xxxx_is_given,       /* HostLibDeinitialized */  
  not_a_single_xxxx_is_given,           /* DeviceAttached */ //todo
  not_a_single_xxxx_is_given,	      /* ResetDevice */
  not_a_single_xxxx_is_given,  /*DeviceDisconnected*/
  not_a_single_xxxx_is_given,  /*OverCurrentDetected*/
  bar,          /* DeviceSpeed */
  foo,    /* DeviceDescriptor is available */
  not_a_single_xxxx_is_given,  /* Address is assigned to USB Device */
  foobar, /* Configuration Descriptor available */
  not_a_single_xxxx_is_given,     /* ManufacturerString*/
  not_a_single_xxxx_is_given,          /* ProductString*/
  not_a_single_xxxx_is_given,        /* SerialNubString*/
  EnumerationDone,           /* Enumeration finished */
  UserInput,			  /* User Input */
  USBH_USR_MSC_Application,
  not_a_single_xxxx_is_given, /* Device is not supported*/
  not_a_single_xxxx_is_given  /* Unrecovered Error */
};

USBH_USR_Status UserInput()
{
  return USBH_USR_RESP_OK;
}

void EnumerationDone()
{
  USB_OTG_BSP_mDelay(500);
  USBH_USR_MSC_Application();
}

int USBH_USR_MSC_Application()
{
  static uint8_t is_rdy = 0;
  if(is_rdy)
  {
    write_file("mfcc.dat", (void*)(&feature_vec[0][0]), sizeof(float32_t)*NUM_FRAME*DCT_DIGIT);
    write_file("audio.wav", (void*)data, sizeof(uint16_t)*MAX_BUF_SIZE);
    f_mount(0, 0);
    STM_EVAL_LEDOn(LED6);
    while(1);
  }
  else
  {
    if(f_mount( 0, &fatfs ) != FR_OK )
      while(1);
    if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
      while(1);
    is_rdy = 1;
  }
  return 0;
}

void OTG_FS_IRQHandler(void)
{
  USBH_OTG_ISR_Handler(&USB_OTG_Core);
}

void not_a_single_xxxx_is_given()
{}
void foo(void* bar)
{}
void bar(uint8_t foo)
{}
void foobar(USBH_CfgDesc_TypeDef * n, USBH_InterfaceDesc_TypeDef * v, USBH_EpDesc_TypeDef *m)
{}

void usb_init()
{
  USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &USBH_MSC_cb, &usr_cbs);
}

void usb_process()
{
  USBH_Process(&USB_OTG_Core, &USB_Host);
}

uint32_t write_file(char* _filename, void* _buffer, uint32_t _buf_size)
{
  UINT ret = 0;
  f_unlink(_filename);
  if((HCD_IsDeviceConnected(&USB_OTG_Core) != 1) || (f_open(&file, _filename, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK))
    while(1);
  f_write(&file, _buffer, (UINT)_buf_size, &ret);
  f_close(&file);
  return((uint32_t)ret);
}


