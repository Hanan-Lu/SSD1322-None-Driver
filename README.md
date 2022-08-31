# 简介

这是一个基于SSD1322控制器，4-bit灰度，256*64点数、采用SPI接口的单片机平台OLED驱动程序。

本程序仅提供基本的操作函数，例如OLED填充函数、OLED显示字符串函数

*PS: 后续考虑增加部分GFX功能，例如画线、画圆等*

程序中还附带了几种8\*11、8\*16、16\*16、12\*21点阵字体，用户也可根据自己需求增加字体，字体取模方式为：“阴码、逐行式、高位在前”。

本驱动程序需在单片机内部申请一段内存作为显存，大小为256\*64*4/8=8192 Bytes。用户可自行修改缓存大小，但是相对应的驱动程序也需修改。

需要注意的是，本程序未做参数合法性检测，因此请务必保证参数的合法性。当然有需要的可以自行添加合法性检测。

# 使用

1. 实现平台相关函数

   ***示例(基于STM32 HAL库，可根据需求自行修改)，函数名可自定义***

   + OLED片选函数

     ```c
     void _oledSelect(OLEDSelect_enum select){
           HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, (GPIO_PinState)select);
     }
     ```

     

   + OLED数据/命令选择函数

     ```c
     void _oledDataorCmd(OLEDDataType_enum type){
           HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, (GPIO_PinState)type);
     }
     ```

     

   + OLED复位函数

     ```c
     void _oledReset(OLEDRst_enum rst){
           HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, (GPIO_PinState)rst);
     }
     ```

     

   + OLED发送数据函数(DMA方式)

     ```c
     unsigned char _oledSendDataDMA(unsigned char *pData, unsigned short len){
         return HAL_SPI_Transmit_DMA(&hspi1, pData, len);
     }
     ```

     <font color="Red">***注意：使用DMA方式传输数据时应在DMA传输完成中断中调用  OLED_Select(OLED_NOTSELECT); 否则会造成OLED工作异常！！！***</font>

   + OLED发送数据函数(阻塞方式)

     ```c
     unsigned char _oledSendDataDMA(unsigned char *pData, unsigned short len, unsigned int timeout){
         return HAL_SPI_Transmit(&hspi1, data, len, timeout)
     }
     ```

     

1. 定义并初始化OLED

   + 定义<i><u>OLEDInitStruct</u></i>变量并初始化

     ```c
     OLEDInitStruct oledinit;				//声明一个OLEDInitStruct变量
     
     oledinit.pDataType = _oledDataorCmd;	//结构体赋值
     oledinit.pDelayms = HAL_Delay;
     oledinit.pReset = _oledReset;
     oledinit.pSelect = _oledSelect;
     oledinit.pWriteDataDMA = _oledSendDataDMA;
     
     //硬件相关初始化
     
     OLED_Init(&oledinit);					//OLED初始化
     ```

     <font color="Red">***注意：初始化函数不对指针合法性做检查，编码时务必确认无非法指针！！！***</font>

1. 使用OLED

   + OLED 测试

     程序提供了一个简单的测试程序。测试程序会在OLED屏幕上显示16个16*64大小，灰度值从0~16的灰度条

     + 调用

       ```c, cpp
       OLED_Test();
       ```

       

     + 显示效果

       ![灰度测试](https://raw.githubusercontent.com/Hanan-Lu/SSD1322-None-Driver/master/Documents/Picture/GrayTest.jpg)

       显示一串字符串(支持自动换行)

     + 函数原型

       ```c
       unsigned char OLED_DisplayASCII(unsigned short   startX,     /*起始横坐标 0-255*/
                                       unsigned short   startY,     /*起始纵坐标 0-63*/
                                       ASCIIFont        font,       /*字体， 可选值参见ASCIIFont*/
                                       char             * ptr,      /*待显示的字符串指针*/
                                       unsigned char    chrSpace,   /*字符间距*/
                                       unsigned char    lineSpace,  /*行间距*/
                                       unsigned char    gray);      /*灰度值，0-16*/ 
       ```

       

     + 调用

       ```c, cpp
       OLED_DisplayASCII(0,0,SOURCECODEPRO_16X24,"ABCDEFGHIJKLMN012345678",0,0,0xf);
       ```

     + 显示效果

       ![显示效果](https://raw.githubusercontent.com/Hanan-Lu/SSD1322-None-Driver/master/Documents/Picture/Display%20ASCII.jpg)

