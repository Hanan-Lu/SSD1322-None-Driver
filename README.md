# SSD1322-None-Driver
基于SSD1322控制器的单片机驱动程序

移植:
  需要用户针对自己的平台实现SSD1322的Write、Reset、ChipSelect及DataorComman函数，赋值给OLEDInitStruct对应成员并调用OLED_Init函数初始化
  
例(基于STM32 HAL库):

//...


void _oledSelect(OLEDSelect_enum select){
      HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, (GPIO_PinState)select);
}

void _oledDataorCmd(OLEDDataType_enum type){
      HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, (GPIO_PinState)type);
}

void _oledReset(OLEDRst_enum rst){
      HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, (GPIO_PinState)rst);
}

unsigned char _oledSendDataDMA(unsigned char *pData, unsigned short len){
    return HAL_SPI_Transmit_DMA(&hspi1, pData, len);
}

//...


int main(void)
{
  /* USER CODE BEGIN 1 */
    OLEDInitStruct oledinit;
    //.....
    oledinit.pDataType = _oledDataorCmd;
    oledinit.pDelayms = HAL_Delay;
    oledinit.pReset = _oledReset;
    oledinit.pSelect = _oledSelect;
    oledinit.pWriteDataDMA = _oledSendDataDMA;
    /* USER CODE END 1 */
    
    //GPIO、Peripheral Config & Initial
    
    OLED_Init(&oledinit);
    //...
 }
