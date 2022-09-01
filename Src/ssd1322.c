/**************************************************************************
 * File name: ssd1322.c
 * Function:  OLED 25664 4-bit gray display Driver(IC:ssd1322)
 * Author:    luhan
 * Date:      2022/08/29
 * Version:   V0.0
 * History:
 * Todo:
 * License:
 * Copyright
 **************************************************************************/

#include "ssd1322.h"

/**
 * @brief 片内缓存，8kB
 *
 */
static __align(4) unsigned char OLED_GRAM[GRAM_SIZE_BYTES] = {0};
/**
 * @brief
 *
 */
OLEDInitStruct _OLEDInitstructure;

/**
 * @brief 向OLED写入指定长度数据
 *
 * @param dataType          数据类型, 详见 COMMAND_Type
 *
 * @param pdat              指向数据起始指针
 * @param len               待发送数据长度
 * @return unsigned char    0:成功; 其他: 失败
 * @note   使用DMA传输时应在DMA传输完成中断中调用OLED_Select(OLED_NOTSELECT)
 */
unsigned char OLED_WriteData(OLEDDataType_enum dataType,
                             unsigned char *pdat,
                             unsigned short len)
{
    unsigned char retval;
    _OLEDInitstructure.pSelect(OLED_SELECT);
    _OLEDInitstructure.pDataType(dataType);

#if USE_DMA
    retval = _OLEDInitstructure.pWriteDataDMA(pdat, len);
#else
    retval = _OLEDInitstructure.pWriteDate(pdat, len, len * 1);
    _OLEDInitstructure.pSelect(OLED_NOTSELECT);
#endif
    return retval;
}

/**
 * @brief                   向OLED写入一字节数据
 *
 * @param dataType          数据类型, 详见 COMMAND_Type
 * @param pdat              指向数据起始指针
 * @return unsigned char    0:成功; 其他: 失败
 * @note   使用DMA传输时应在DMA传输完成中断中调用 OLED_Select(OLED_NOTSELECT)
 */
unsigned char OLED_WriteByte(OLEDDataType_enum dataType, unsigned char pdat)
{
    unsigned char retval;
    _OLEDInitstructure.pSelect(OLED_SELECT);
    _OLEDInitstructure.pDataType(dataType);
#if USE_DMA
    retval = _OLEDInitstructure.pWriteDataDMA(&pdat, 5);
#else
    retval = _OLEDInitstructure.pWriteDate(&pdat, 1, 5);
    _OLEDInitstructure.pSelect(OLED_NOTSELECT);
#endif
    return retval;
}

/**
 * @brief                   写入一个点到帧缓冲区
 *
 * @param x                 列地址, 0-255
 * @param y                 行地址, 0-63
 * @param color             颜色, 0x0-0xf
 * @return unsigned char    未使用
 */
unsigned char OLED_WritePointToFrameBuffer(unsigned short x, unsigned short y, unsigned char color)
{
    unsigned char retval = 0;
    if ((x % 2) == 1)
    {
        OLED_GRAM[x / 2 + y * (HORIZONTAL_DOTS >> 1)] &= 0xf0;           // Clear low half byte
        OLED_GRAM[x / 2 + y * (HORIZONTAL_DOTS >> 1)] |= (color & 0x0f); // Set low half byte
    }
    else
    {
        OLED_GRAM[x / 2 + y * (HORIZONTAL_DOTS >> 1)] &= 0x0f;                  // Clear high half byte
        OLED_GRAM[x / 2 + y * (HORIZONTAL_DOTS >> 1)] |= ((color & 0x0f) << 4); // Set high half byte
    }
    return retval;
}

/**
 * @brief                   填充帧缓冲区
 *
 * @param startX            起始列地址, 0-256
 * @param startY            起始列地址, 0-64
 * @param endX              终止列地址, 0-256
 * @param endY              终止列地址, 0-64
 * @param color             颜色, 0x0-0xf
 * @return unsigned char    未使用
 */
unsigned char OLED_FillFrameBuffer(unsigned short startX, unsigned short startY,
                                   unsigned short endX, unsigned short endY,
                                   unsigned char color)
{
    unsigned char retval = 0;
    unsigned short _x, _y;

    color &= 0x0f; //限制颜色值
    if (startX > endX)
    { //交换XY
        startX = startX + endX;
        endX = startX - endX;
        startX = startX - endX;
    }
    if (startY > endY)
    {
        startY = startY + endY;
        endY = startY - endY;
        startY = startY - endY;
    }
    /*画点*/
    for (_y = startY; _y < endY; _y++)
    {
        for (_x = startX; _x < endX; _x++)
        {
            retval = OLED_WritePointToFrameBuffer(_x, _y, color);
        }
    }
    return retval;
}
/**
 * @brief  刷新OLED显示
 *
 * @return unsigned char
 */
unsigned char OLED_ManualRefresh(void)
{
    return OLED_RefreshFormRAM(OLED_GRAM);
}

/**
 * @brief 将RAM数据发送至OLED
 *
 * @param pStartAddr RAM 起始地址
 * @return * unsigned char
 */
unsigned char OLED_RefreshFormRAM(void *pStartAddr)
{
    unsigned char retval;
    /*设置列地址*/
    retval = OLED_WriteByte(OLED_CMD, SET_COLUMN_ADDRESS_CMD);
    retval = OLED_WriteByte(OLED_DATA, 0x1C + 0);
    retval = OLED_WriteByte(OLED_DATA, 0x1C + (HORIZONTAL_DOTS >> 2) - 1);
    /*设置行地址*/
    retval = OLED_WriteByte(OLED_CMD, SET_ROW_ADDRESS_CMD);
    retval = OLED_WriteByte(OLED_DATA, 0);
    retval = OLED_WriteByte(OLED_DATA, VERTICAL_DOTS - 1);
    /*写RAM命令*/
    retval = OLED_WriteByte(OLED_CMD, WRITE_RAM_CMD);
    /*写RAM*/
    retval = OLED_WriteData(OLED_DATA, pStartAddr, GRAM_SIZE_BYTES);
    return retval;
}

/**
 * @brief  测试OLED，显示灰度条
 * @param  None
 *
 * @note   None
 *
 */
unsigned char OLED_Test(void)
{
    /*显示灰度条 测试*/
    OLED_FillFrameBuffer(0, 0, 16, 64, 0xff);
    OLED_FillFrameBuffer(16, 0, 32, 64, 0x22);
    OLED_FillFrameBuffer(32, 0, 48, 64, 0x33);
    OLED_FillFrameBuffer(48, 0, 64, 64, 0x44);
    OLED_FillFrameBuffer(64, 0, 80, 64, 0x55);
    OLED_FillFrameBuffer(80, 0, 96, 64, 0x66);
    OLED_FillFrameBuffer(96, 0, 112, 64, 0x77);
    OLED_FillFrameBuffer(112, 0, 128, 64, 0x88);
    OLED_FillFrameBuffer(128, 0, 144, 64, 0x99);
    OLED_FillFrameBuffer(144, 0, 160, 64, 0xaa);
    OLED_FillFrameBuffer(160, 0, 176, 64, 0xbb);
    OLED_FillFrameBuffer(176, 0, 192, 64, 0xcc);
    OLED_FillFrameBuffer(192, 0, 208, 64, 0xdd);
    OLED_FillFrameBuffer(208, 0, 224, 64, 0xee);
    OLED_FillFrameBuffer(224, 0, 240, 64, 0xff);
    OLED_FillFrameBuffer(240, 0, 256, 64, 0xff);
    return OLED_ManualRefresh();
}

/**
 * @brief 获取GRAM地址
 *
 * @return void*
 */
unsigned char *OLED_GetFrameBufferStartAddr(void)
{
    return OLED_GRAM;
}

/**
 * @brief 从内部缓冲区读取点
 *
 * @param x
 * @param y
 * @return unsigned char
 */
unsigned char OLED_GetPointFromFrameBuffer(unsigned short x, unsigned short y)
{
    return OLED_GRAM[x / 2 + y * 128];
}

/**
 * @brief OLED Reset
 *
 * @param _rst OLED_RESET(0):Reset Mode, OLED_NORMAL(1):Normal Operation
 */
void OLED_Reset(OLEDRst_enum _rst)
{
    _OLEDInitstructure.pReset(_rst);
}
/**
 * @brief Select or Not Select OLED
 *
 * @param _select OLED_SELECT(0):Select. OLED_NOTSELECT(1):Not Select
 */
void OLED_Select(OLEDSelect_enum _select)
{
    _OLEDInitstructure.pSelect(_select);
}
/**
 * @brief oled transmit data type select
 *
 * @param _type OLED_CMD(0) send command. OLED_DATA(1):send data
 */
void OLED_DataorCmd(OLEDDataType_enum _type)
{
    _OLEDInitstructure.pDataType(_type);
}

/**
 * @brief OLED初始化
 *
 * @param hSPI OLED使用的SPI
 * @return unsigned char
 */
unsigned char OLED_Init(OLEDInitStruct *oled)
{

    _OLEDInitstructure.pDataType = oled->pDataType;
    _OLEDInitstructure.pDelayms = oled->pDelayms;
    _OLEDInitstructure.pReset = oled->pReset;
    _OLEDInitstructure.pSelect = oled->pSelect;
#if USE_DMA
    _OLEDInitstructure.pWriteDataDMA = oled->pWriteDataDMA;
#else
    _OLEDInitstructure.pWriteDate = oled->pWriteDate;
#endif

    _OLEDInitstructure.pReset(OLED_RESET);
    _OLEDInitstructure.pDelayms(10);
    _OLEDInitstructure.pReset(OLED_NORMAL);

    OLED_WriteByte(OLED_CMD, SET_COMMAND_LOCK_CMD);
    OLED_WriteByte(OLED_DATA, 0x12);

    OLED_WriteByte(OLED_CMD, SLEEP_MODE_ON_CMD);

    OLED_WriteByte(OLED_CMD, SET_FONT_CLK_DIV_OSC_FREQ_CMD);
    OLED_WriteByte(OLED_DATA, 0x91);

    OLED_WriteByte(OLED_CMD, SET_MUX_RATIO_CMD);
    OLED_WriteByte(OLED_DATA, 0x3f);

    OLED_WriteByte(OLED_CMD, SET_DISPLAY_OFFSET_CMD);
    OLED_WriteByte(OLED_DATA, 0x00);

    OLED_WriteByte(OLED_CMD, SET_DISPLAY_START_LINE_CMD);
    OLED_WriteByte(OLED_DATA, 0x00);
    /*扫描显示方向在这修改*/
    OLED_WriteByte(OLED_CMD, SET_REMAP_DUAL_CMD);
    OLED_WriteByte(OLED_DATA, 0x14);

    OLED_WriteByte(OLED_CMD, SET_GPIO_CMD);
    OLED_WriteByte(OLED_DATA, 0x00);

    OLED_WriteByte(OLED_CMD, VDD_SELECT_CMD);
    OLED_WriteByte(OLED_DATA, 0x01);

    OLED_WriteByte(OLED_CMD, SET_ENABLE_EXT_VSL_CMD);
    OLED_WriteByte(OLED_DATA, 0xa0);
    OLED_WriteByte(OLED_DATA, 0xfd);

    OLED_WriteByte(OLED_CMD, SET_CONTRAST_CURRENT_CMD);
    OLED_WriteByte(OLED_DATA, 0xff);

    OLED_WriteByte(OLED_CMD, SET_MASTER_CST_CURRENT_CMD);
    OLED_WriteByte(OLED_DATA, 0x0f);

    OLED_WriteByte(OLED_CMD, SEL_DEF_GRAY_TABLE_CMD);

    OLED_WriteByte(OLED_CMD, SET_PHASE_LENGTH_CMD);
    OLED_WriteByte(OLED_DATA, 0XE2);

    OLED_WriteByte(OLED_CMD, 0XD1);
    OLED_WriteByte(OLED_DATA, 0X82);
    OLED_WriteByte(OLED_DATA, 0X20);

    OLED_WriteByte(OLED_CMD, SET_PRECHARGE_VLOTAGE_CMD);
    OLED_WriteByte(OLED_DATA, 0X1f);

    OLED_WriteByte(OLED_CMD, SET_SEC_PRECHARGE_PERIOD_CMD);
    OLED_WriteByte(OLED_DATA, 0x08);

    OLED_WriteByte(OLED_CMD, SET_VCOMH_CMD);
    OLED_WriteByte(OLED_DATA, 0x07);

    OLED_WriteByte(OLED_CMD, SET_DIPLAY_NORMAL_CMD);
    // OLED_WriteByte(OLED_CMD, SET_DISPLAY_ON_CMD);
    OLED_WriteByte(OLED_CMD, SLEEP_MODE_OFF_CMD);
    return 0;
}

/**
 * @brief 快速画点 立即更新到屏幕上, 此函数会同步更新片内缓冲区
 *        此函数为非覆盖模式，即仅重绘目标区域
 *
 * @param x         横坐标  0~255
 * @param y         纵坐标  0~63
 * @param color     颜色值  0-15
 * @return unsigned char
 */
unsigned char OLED_FastDrawPoint(unsigned short x, unsigned short y, unsigned char color)
{
    unsigned long int index = 0;
    if ((x % 2) == 1)
    {
        OLED_GRAM[x / 2 + y * (HORIZONTAL_DOTS >> 1)] &= 0xf0;           // Clear low half byte
        OLED_GRAM[x / 2 + y * (HORIZONTAL_DOTS >> 1)] |= (color & 0x0f); // Set low half byte
    }
    else
    {
        OLED_GRAM[x / 2 + y * (HORIZONTAL_DOTS >> 1)] &= 0x0f;                  // Clear high half byte
        OLED_GRAM[x / 2 + y * (HORIZONTAL_DOTS >> 1)] |= ((color & 0x0f) << 4); // Set high half byte
    }
    /*计算开始写入索引*/
    if (x % 4 > 1)
    {
        index = x / 2 + y * (HORIZONTAL_DOTS >> 1) - 1; //坐标位于奇数SEG, 索引前移1
    }
    else
    {
        index = x / 2 + y * (HORIZONTAL_DOTS >> 1); //坐标位于偶数SEG，索引不变
    }

    //设置列起始
    x = x / 4;
    OLED_WriteByte(OLED_CMD, SET_COLUMN_ADDRESS_CMD);
    OLED_WriteByte(OLED_DATA, 0x1C + x);
    OLED_WriteByte(OLED_DATA, 0x1C + 63);
    OLED_WriteByte(OLED_CMD, SET_ROW_ADDRESS_CMD);
    //设置行起始
    OLED_WriteByte(OLED_DATA, y);
    OLED_WriteByte(OLED_DATA, 63);
    //写入数据 一次写入2 byte
    OLED_WriteByte(OLED_CMD, WRITE_RAM_CMD);
    return OLED_WriteData(OLED_DATA, &OLED_GRAM[index], 2);
}

/**
 * @brief
 *
 * @param startX    列起点      0 - 255
 * @param startY    行起点      0 - 63
 * @param endX      列终点      startX - 255
 * @param endY      行终点      startY - 63
 * @param color     颜色        0-15
 * @return unsigned char
 */
unsigned char OLED_Fill(unsigned short startX, unsigned short startY,
                        unsigned short endX, unsigned short endY,
                        unsigned char color)
{
    unsigned char retval = 0;
    unsigned short _x, _y;

    if (startX > endX)
    {
        startX = startX + endX;
        endX = startX - endX;
        startX = startX - endX;
    }
    if (startY > endY)
    {
        startY = startY + endY;
        endY = startY - endY;
        startY = startY - endY;
    }

    for (_y = startY; _y < endY; _y++)
    {
        for (_x = startX; _x < endX; _x++)
        {
            retval = OLED_WritePointToFrameBuffer(_x, _y, color);
        }
    }
    OLED_WriteByte(OLED_CMD, SET_COLUMN_ADDRESS_CMD);
    OLED_WriteByte(OLED_DATA, 0x1C + 0);
    OLED_WriteByte(OLED_DATA, 0x1C + 63);

    OLED_WriteByte(OLED_CMD, SET_ROW_ADDRESS_CMD);
    OLED_WriteByte(OLED_DATA, 0);
    OLED_WriteByte(OLED_DATA, 64 - 1);
    OLED_WriteByte(OLED_CMD, WRITE_RAM_CMD);
    retval = OLED_WriteData(OLED_DATA, OLED_GRAM, GRAM_SIZE_BYTES);
    return retval;
}
/**
 * @brief Display ASCII String, auto swap line.
 *
 * @param startX    列起点      0 - 255
 * @param startY    行起点      0 - 63
 * @param font      字体        see ASCII_Font
 * @param chrSpace  字间距
 * @param lineSpace 行间距
 * @param gray      灰度        0-15
 * @return unsigned char
 */
unsigned char OLED_DisplayASCII(unsigned short startX,   /*起始横坐标 0-255*/
                                unsigned short startY,   /*起始纵坐标 0-63*/
                                ASCIIFont font,          /*字体，字符集*/
                                char *ptr,               /*待显示的字符串指针*/
                                unsigned char chrSpace,  /*字符间距*/
                                unsigned char lineSpace, /*行间距*/
                                unsigned char gray)      /*灰度值，0-16*/
{
    //#define _MAX_DOT_BYTES  128
    unsigned char const *dotArray = 0;                                   //点阵数据存储区
    unsigned char len;                                                   //获取字符串长度
    unsigned char dotArray_index = 0;                                    //点阵数据下标
    unsigned short itorx = startX, itory = startY * HORIZONTAL_DOTS / 2; // X Y 迭代变量
    unsigned char tempIndex = 0;                                         //存储临时索引，现存数组
    unsigned char isodd = (startX % 2 == 0) ? 0 : 1;                     //横坐标是否为奇数
    unsigned char swapLineNum = 0;
    unsigned char fontWidth = 0;
    unsigned char fontHigh = 0;

    for (unsigned short i = 0; ptr[i] != '\0'; i++)
    {
        // 获取字体数据信息
        dotArray = getFontptr(font, ptr[i]);
        fontWidth = getFontWidth(font);
        fontHigh = getFontHigh(font);
        len = getFontByteNum(font);

        /*点阵数据转换到4byte灰度, 适用于横置横排，高位在前字模（阴码,逐行式,高位在前）*/
        for (dotArray_index = 0; dotArray_index < len;)
        {
            isodd = (itorx % 2 == 0) ? 0 : 1;
            for (unsigned char _itorBit = 0; _itorBit < fontWidth; _itorBit++)
            {
                tempIndex = itorx / 2 + (_itorBit + isodd) / 2;
                if (itorx % 2 == 0)
                {
                    if (_itorBit % 2 == 0)
                    {
                        OLED_GRAM[tempIndex + itory] =
                            ((dotArray[dotArray_index + _itorBit / 8] >> (7 - _itorBit % 8) & 0x01) * (gray << 4));
                    }
                    else
                    {
                        OLED_GRAM[tempIndex + itory] |=
                            ((dotArray[dotArray_index + _itorBit / 8] >> (7 - _itorBit % 8) & 0x01) * gray);
                    }
                }
                else
                {
                    if (_itorBit % 2 == 0)
                    {
                        OLED_GRAM[tempIndex + itory] |=
                            ((dotArray[dotArray_index + _itorBit / 8] >> (7 - _itorBit % 8) & 0x01) * gray);
                    }
                    else
                    {
                        OLED_GRAM[tempIndex + itory] =
                            ((dotArray[dotArray_index + _itorBit / 8] >> (7 - _itorBit % 8) & 0x01) * (gray << 4));
                    }
                }
            }
            //点阵数据下标自增，根据宽度不同自动更改偏移
            dotArray_index = dotArray_index + (fontWidth + 7) / 8;
            //转换完一行数据 转换下一行
            itory = itory + HORIZONTAL_DOTS / 2;
            if (itory >= GRAM_SIZE_BYTES){
                goto nextchar;
            }
        }
nextchar:
        /*横坐标自增一个字符宽度+字间距*/
        itorx = itorx + fontWidth + chrSpace; // X增量一个字符宽度
        //预判下一个字符是否超限
        if (itorx + fontWidth > HORIZONTAL_DOTS)
        {
            itorx = startX; //重回起始行，或者改为0
            swapLineNum++;  //换行数自加
        }
        //换行
        itory = (startY + (fontHigh + lineSpace) * swapLineNum) * HORIZONTAL_DOTS / 2;
        if(itory/HORIZONTAL_DOTS / 2 > VERTICAL_DOTS - 1){
            break;
        }
        //超过显示区域，防止内存溢出
           if(itorx + itory > GRAM_SIZE_BYTES-1)
               break;
    }

    OLED_WriteByte(OLED_CMD, SET_COLUMN_ADDRESS_CMD);
    OLED_WriteByte(OLED_DATA, 0x1C + 0);
    OLED_WriteByte(OLED_DATA, 0x1C + 63);

    OLED_WriteByte(OLED_CMD, SET_ROW_ADDRESS_CMD);
    OLED_WriteByte(OLED_DATA, 0);
    OLED_WriteByte(OLED_DATA, 64 - 1);
    OLED_WriteByte(OLED_CMD, WRITE_RAM_CMD);
    return OLED_WriteData(OLED_DATA, OLED_GRAM, GRAM_SIZE_BYTES);
}
