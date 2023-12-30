#ifndef __WS2812_H
#define __WS2812_H

#include "hk32f030m.h"
#include "spi.h"
#include "io_ws2812.h"
#include <math.h>

typedef enum {
	Red,
	Green,
	Blue,
    Orange,
    Yellow,
    Cyan,
    Violet,
    Magenta,
    Pink,
    White,
    RedOrange,
    YellowGreen,
    GreenCyan,
    CyanBlue,
    Off
} COLORS;

#define     WS2812_N    10      	//��������
#define		WS2812_0	0xE0  	//0��
#define		WS2812_1	0xFF 	//1��
#define		WS2812_R	0x00  	//��λ��

extern uint16_t WS2812_Counter;
extern uint8_t WS2812_Buff[WS2812_N][24];

void WS2812_Init(void);
uint8_t rgb_by_colour(uint8_t colour);
void Lightbar_Set_RGB(uint8_t num, uint8_t red, uint8_t green, uint8_t blue);
void Lightbar_Set_RGB_Range(uint8_t start, uint8_t end, uint8_t red, uint8_t green, uint8_t blue);
void Lightbar_Set_Colour(uint8_t num, uint8_t colour, uint8_t brightness);
void Lightbar_Set_Colour_Range(uint8_t start, uint8_t end, uint8_t colour, uint8_t brightness);
void Lightbar_All_Off(void);
void Lightbar_Task(void);
void Lightbar_Refresh(void);

#endif


