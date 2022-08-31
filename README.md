# 简介
这是一个采用SSD1322控制器的4-bit灰度、256*64像素OLED、SPI接口的驱动程序，本程序适用于单片机。
本程序提供了基本的接口用于操作SSD1322，包括但不限于以下函数：

+ OLED_Test(void);
	OLED测试函数，本函数会在OLED上绘制16个灰度从0x02-0x0f, 宽度为16个像素的灰度条
+ OLED_RefreshFromRAM(void *pStartAddr);
	从内部RAM刷新OLED，pStartAddr指向内部RAM的起始地址。写入长度为256**64*4/8=8192 Bytes;
+ OLED_FastDrawPoint(x, y, color);
	快速画点函数，x、y为指定坐标，color为指定灰度(0x0-0xf);
+ OLED_Fill(startX, startY, endX, endY, color);
	快速填充顶点为(startX, startY) -（endX, endY)、灰度为color的矩形
+ OLED_DisplayASCII(...)
	详见源码，本函数可绘制一串字符串至OLED，支持自动换行。

本程序还提供了部分字体点阵数据，可直接应用
*本程序支持的取模方式为：阴码、逐行、高位在前模式*

# 移植&使用
1. 移植
本驱动已将平台相关代码接口封装在 [<u>*OLEDInitStruct*</u>](#oled-init-struct-def) 结构体内，用户根据自己应用平台实现响应接口并传递给  <u>*OLED_Init(OLEDInitStruct \*p)*</u> 函数初始化即可。	***注意：在使用DMA模式传输数据时，应在DMA传输完成中断中调用: OLED_Select(OLED_NOTSELECT);函数***

    + *示例*
	```cpp, c
		//OLED 片选函数
		void _oledSelect(OLEDSelect_enum select){
      		HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, (GPIO_PinState)select);
	  	}
	  	//OLED 数据/命令选择函数
		void _oledDataorCmd(OLEDDataType_enum type){
      		HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, (GPIO_PinState)type);
		}
	  	//OLED 复位函数
		void _oledReset(OLEDRst_enum rst){
			HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, (GPIO_PinState)rst);
		}
		//OLED 发送数据函数 本示例使用的为DMA传输，用户可修改SSD1322.h中的USE_DMA宏来使用阻塞发送
		unsigned char _oledSendDataDMA(unsigned char *pData, unsigned short len){
			return HAL_SPI_Transmit_DMA(&hspi1, pData, len);
		}
		
		//其他代码
		//声明一个 OLEDInitStruct 变量
		OLEDInitStruct oledinit;
		//oledinit 成员赋值
		oledinit.pDataType = _oledDataorCmd;
    	oledinit.pDelayms = HAL_Delay;
    	oledinit.pReset = _oledReset;
    	oledinit.pSelect = _oledSelect;
    	oledinit.pWriteDataDMA = _oledSendDataDMA;
		//初始化
		OLED_Init(&oledinit);
	```

2. 使用
   + 显示字符串
     ```cpp,c
	 OLED_DisplayASCII(0,0,SOURCECODEPRO_16X24,"ABCEDFGHIJKLMN",0,0,0xf);
	 ```
   + 显示效果
   
   *待上传图片*
   
   + 测试OLED
     ```cpp,c
	 OLED_Test();
	 ```
   + 显示效果
   
   *待上传图片*
   
<a id="oled-init-struct-def">OLEDInitStruct 定义</a>
```cpp,c
typedef struct{
    void (*pSelect)(OLEDSelect_enum);      //oled select function
    void (*pDataType)(OLEDDataType_enum);  //oled data type set funtion
    void (*pReset)(OLEDRst_enum);          //oled reset function
    void (*pDelayms)(unsigned int);        //delay ms function
#if USE_DMA
    unsigned char   (*pWriteDataDMA)(unsigned char *, unsigned short);  //Transmit data in non-blocking mode with DMA     
#else
    unsigned char   (*pWriteDate)(unsigned char *, unsigned short, unsigned int);  //Transmit data inblocking mode
    #endif 
}OLEDInitStruct;
```
