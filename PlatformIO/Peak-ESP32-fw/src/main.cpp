/*
 * @Author: your name
 * @Date: 2021-10-11 09:13:16
 * @LastEditTime: 2021-11-04 09:25:27
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \Peak-ESP32-fw\src\main.cpp
 */
#include <Arduino.h>
#include "HAL/HAL.h"
#include "Port/Display.h"
#include "App/App.h"

void setup()
{
    HAL::Init();
    Port_Init();
    App_Init();
}

void loop()
{
    HAL::Update();

    delay(20);
}