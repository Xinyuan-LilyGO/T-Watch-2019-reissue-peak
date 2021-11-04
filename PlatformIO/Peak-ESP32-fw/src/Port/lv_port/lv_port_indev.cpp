/*
 * @Author: your name
 * @Date: 2021-10-11 09:13:16
 * @LastEditTime: 2021-11-04 11:24:57
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Peak-ESP32-fw\src\Port\lv_port\lv_port_indev.cpp
 */
/**
 * @file lv_port_indev_templ.c
 *
 */

/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
#include "HAL/HAL.h"
#include "focaltech.h"
#include "Wire.h"
#include "i2c_bus.h"
#include "FT5206.h"

/*********************
 *      DEFINES
 *********************/
FocalTech_Class *touch;
I2CBus *i2c = nullptr;
/**********************
 *      TYPEDEFS
 **********************/

#define TOUCH_IRQ_BIT (_BV(1))

#define NO_HARDWARE (_BV(0))
#define NO_POWER (_BV(1))
#define NO_TFT (_BV(2))
#define NO_TOUCH (_BV(3))
#define NO_SENSOR (_BV(4))
#define NO_BACKLIGHT (_BV(5))
/**********************
 *  STATIC PROTOTYPES
 **********************/

static void encoder_init(void);
static void encoder_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
static void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
/**********************
 *  STATIC VARIABLES
 **********************/

static lv_indev_t *encoder_indev;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
static lv_indev_drv_t indev_drv;
static lv_indev_drv_t indev_drv_touch;

EventGroupHandle_t _tpEvent = nullptr;

static void TOUCH_IRQ_HANDLE(void)
{
    portBASE_TYPE task_woken;
    if (_tpEvent)
    {
        xEventGroupSetBitsFromISR(_tpEvent, TOUCH_IRQ_BIT, &task_woken);
        if (task_woken == pdTRUE)
        {
            portYIELD_FROM_ISR();
        }
    }
    Serial.println("1");
}

static uint8_t writeBytes(uint8_t devAddress, uint8_t regAddress, uint8_t *data, uint8_t len)
{
    if (!i2c)
        return false;
    i2c->writeBytes(devAddress, regAddress, data, len);
    return true;
}
static uint8_t readBytes(uint8_t devAddress, uint8_t regAddress, uint8_t *data, uint8_t len)
{
    if (!i2c)
        return false;
    i2c->readBytes(devAddress, regAddress, data, len);
    return true;
}

void lv_port_indev_init(void)
{
    // i2c = new I2CBus(Wire1, 23, 32);
    _tpEvent = xEventGroupCreate();

    i2c = new I2CBus(Wire1, 23, 32);
    touch = new FocalTech_Class();
    // Wire.begin(32, 23);
    touch->begin(readBytes, writeBytes);
    /*     touch->setTheshold(100);
        Serial.printf("setTheshold : %d\n", touch->getThreshold());
        Serial.printf("getVendorID,0x%x\n", touch->getVendorID());
        Serial.printf("getVendor1ID,0x%x\n", touch->getVendor1ID());
        Serial.printf("getErrorCode,0x%x\n", touch->getErrorCode());
        Serial.printf("getTouched,0x%x\n", touch->getTouched());
        Serial.printf("getControl,0x%x\n", touch->getControl());
     */
    // FX5206 = new FocalTech_Class();

    /*------------------
     * Encoder
     * -----------------*/

    /*     if (!FX5206->begin(Wire))
        {
            Serial.println("Begin touch FAIL");
        } */

 /*    pinMode(38, INPUT);
    attachInterrupt(38, TOUCH_IRQ_HANDLE, FALLING); */

    /*Initialize your encoder if you have*/
    // encoder_init();

    /*Register a encoder input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_drv.read_cb = encoder_read;
    encoder_indev = lv_indev_drv_register(&indev_drv);

    lv_indev_drv_init(&indev_drv_touch);
    indev_drv_touch.type = LV_INDEV_TYPE_POINTER;
    indev_drv_touch.read_cb = touchpad_read;
    lv_indev_drv_register(&indev_drv_touch);

    /* Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     * add objects to the group with `lv_group_add_obj(group, obj)`
     * and assign this input device to group to navigate in it:
     * `lv_indev_set_group(indev_encoder, group);` */
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * Encoder
 * -----------------*/

/* Initialize your keypad */
static void encoder_init(void)
{
    /*Do it in Init*/
}

static void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{

    // bool isTouch = FX5206->getTouched();
    // data->state = isTouch ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    /*     data->point.x = XYWatch::Touch_GetX();
        data->point.y = XYWatch::Touch_GetY(); */
        uint16_t __x = 0, __y = 0;
        data->state = touch->getPoint(__x, __y) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
        if (data->state == LV_INDEV_STATE_PR)
        {

            data->point.x = __x;
            data->point.y = __y;
        }
        //Serial.printf("x:0x%x\ny:0x%x\n", __x, __y);

/*     static uint16_t x, y;
    if (xEventGroupGetBits(_tpEvent) & TOUCH_IRQ_BIT)
    {
        xEventGroupClearBits(_tpEvent, TOUCH_IRQ_BIT);
        // data->state = FX5206->getPoint(x, y) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
        data->point.x = x;
        data->point.y = y;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
        data->point.x = x;
        data->point.y = y;
    } */
}

/* Will be called by the library to read the encoder */
static void encoder_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    data->enc_diff = HAL::Encoder_GetDiff();
    data->state = HAL::Encoder_GetIsPush() ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
