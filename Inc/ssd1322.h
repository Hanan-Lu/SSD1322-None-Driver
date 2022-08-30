/**************************************************************************
 * File name: ssd1322.h
 * Function:  OLED 25664 4-bit gray display Driver(IC:ssd1322)
 * Note:      高内聚，低耦合
 * Author:    luhan
 * Date:      2022/08/29
 * Version:   V0.0
 * History:
 * Todo:
 * License:
 * Copyright 
 **************************************************************************/
#ifndef __SSD1322_H_
#define __SSD1322_H_

#define EN_GRAY_SCALE_TABLE_CMD         (0X00)
#define SET_COLUMN_ADDRESS_CMD          (0X15)
#define WRITE_RAM_CMD                   (0x5C)
#define READ_RAM_CMD                    (0x5D)
#define SET_ROW_ADDRESS_CMD             (0x75)
#define SET_REMAP_DUAL_CMD              (0xA0)
#define SET_DISPLAY_START_LINE_CMD      (0xA1)
#define SET_DISPLAY_OFFSET_CMD          (0xA2)
#define SET_DISPLAY_OFF_CMD             (0xA4)      //all pixel OFF
#define SET_DISPLAY_ON_CMD              (0xA5)      //all Pixel on
#define SET_DIPLAY_NORMAL_CMD           (0xA6)      //Normal Display GRAM
#define SET_DISPLAY_INVERSE_CMD         (0xA7)      //Inverse Display
#define EN_PARTIAL_DISPLAY_CMD          (0xA8)
#define EXIT_PARTIAL_DISPLAY_CMD        (0xA9)
#define VDD_SELECT_CMD                  (0xAB)
#define SLEEP_MODE_ON_CMD               (0xAE)      //Display off
#define SLEEP_MODE_OFF_CMD              (0xAF)
#define SET_PHASE_LENGTH_CMD            (0xB1)
#define SET_FONT_CLK_DIV_OSC_FREQ_CMD   (0xB3)
#define SET_ENABLE_EXT_VSL_CMD          (0xB4)              
#define SET_GPIO_CMD                    (0xB5)
#define SET_SEC_PRECHARGE_PERIOD_CMD    (0xB6)
#define SET_GRAY_SCALE_TABLE_CMD        (0xB8)
#define SEL_DEF_GRAY_TABLE_CMD          (0xB9)
#define SET_PRECHARGE_VLOTAGE_CMD       (0xBB)
#define SET_VCOMH_CMD                   (0xBE)
#define SET_CONTRAST_CURRENT_CMD        (0xC1)
#define SET_MASTER_CST_CURRENT_CMD      (0xC7)
#define SET_MUX_RATIO_CMD               (0xCA)
#define SET_COMMAND_LOCK_CMD            (0xFD)

#include "ASCII_Font.h"
//#include "GT30L32S4W.h"

#define HORIZONTAL_DOTS                 (256)           //水平像素点数
#define VERTICAL_DOTS                   (64)            //竖直像素点数
#define GRAY_BITS                       (4)             //灰度位数

#define GRAM_SIZE_BYTES                 (HORIZONTAL_DOTS * VERTICAL_DOTS * GRAY_BITS/ 8)   //GRAM Size

#define USE_DMA 1                           //Config use DMA
// data type
typedef enum{
    OLED_CMD=0,
    OLED_DATA,
}OLEDDataType_enum;
//enable / chip select
typedef enum
{
    OLED_SELECT = 0,
    OLED_NOTSELECT,
} OLEDSelect_enum;
// reset or normal
typedef enum
{
    OLED_RESET = 0,
    OLED_NORMAL,

} OLEDRst_enum;
/**
 * @brief OLED 初始化结构体，平台相关代码，需用户自己实现
 * @example 以下为STM32 HAL库初始化例程
 * 片选函数
 *  void _oledSelect(OLEDSelect_enum select){
 *      HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, (GPIO_PinState)select);
 *  }
 * 数据类型函数
 *  void _oledDataorCmd(OLEDDataType_enum type){
 *      HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, (GPIO_PinState)type);
 *  }
 * 复位函数
 *  void _oledReset(OLEDRst_enum rst){
 *      HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, (GPIO_PinState)rst);
 * }
 * DMA模式数据传输函数
 *  unsigned char _oledSendDataDMA(unsigned char *pData, unsigned short len){
 *     return HAL_SPI_Transmit_DMA(&hspi1, pData, len);
 * }
 *     OLEDInitStruct oledinit;             //定义初始化结构体
 *     oledinit.pDataType = _oledDataorCmd; //结构体成员赋值
 *     oledinit.pDelayms = HAL_Delay;
 *     oledinit.pReset = _oledReset;
 *     oledinit.pSelect = _oledSelect;
 *     oledinit.pWriteDataDMA = _oledSendDataDMA;
 *     OLED_Init(&oledinit);                //初始化
 */
typedef struct{
    void            (*pSelect)(OLEDSelect_enum);                         //oled select function
    void            (*pDataType)(OLEDDataType_enum);                     //oled data type set funtion
    void            (*pReset)(OLEDRst_enum);                             //oled reset function
    void            (*pDelayms)(unsigned int);                           //delay ms function
#if USE_DMA
                                    //Point to data,   data length
    unsigned char   (*pWriteDataDMA)(unsigned char *, unsigned short);  //Transmit data in non-blocking mode with DMA     
#else
                                    //Point to data,   data length, timeout
    unsigned char   (*pWriteDate)(unsigned char *, unsigned short, unsigned int);  //Transmit data inblocking mode
#endif 

} OLEDInitStruct;

void OLED_Reset(OLEDRst_enum _rst);
void OLED_Select(OLEDSelect_enum _select);
void OLED_DataorCmd(OLEDDataType_enum _type);

unsigned char OLED_Init(OLEDInitStruct *oled);
unsigned char OLED_Test(void);

unsigned char OLED_WritePointToFrameBuffer(unsigned short x, unsigned short y, unsigned char color);
unsigned char OLED_FillFrameBuffer(unsigned short startX, unsigned short startY,
                                   unsigned short endX, unsigned short endY,
                                   unsigned char color);

unsigned char OLED_RefreshFormRAM(void *pStartAddr);
unsigned char OLED_ManualRefresh(void);

unsigned char * OLED_GetFrameBufferStartAddr(void);
unsigned char OLED_GetPointFromFrameBuffer(unsigned short x, unsigned short y);

/***************************************/
unsigned char OLED_FastDrawPoint(unsigned short x, unsigned short y, unsigned char color);
unsigned char OLED_Fill(unsigned short startX, unsigned short startY,
                        unsigned short endX, unsigned short endY,
                        unsigned char color);

unsigned char OLED_DisplayASCII(unsigned short   startX,     /*起始横坐标 0-255*/
                                unsigned short   startY,     /*起始纵坐标 0-63*/
                                ASCIIFont       font,       /*字体，字符集*/
                                char             * ptr,      /*待显示的字符串指针*/
                                unsigned char    chrSpace,   /*字符间距*/
                                unsigned char    lineSpace,  /*行间距*/
                                unsigned char    gray);       /*灰度值，0-16*/  

#endif
