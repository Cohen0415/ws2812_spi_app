# 在ARM Linux应用层使用SPI驱动WS2812
本实例程序，主要验证了如何在Arm Linux应用层，使用SPI点亮WS2812

## 接线
SoC的SPI接口的DO脚 -----> WS2812的IN脚

## 获取程序
```shell
git clone git@github.com:Cohen0415/ws2812_spi_app.git
```

## 编译程序
如果你使用的是buildroot系统，需要交叉编译。这里使用跑Ubuntu的板子来验证，执行如下命令编译程序：
```shell
cd ws2812
sudo gcc -o main main.c spi.c
```

## 运行程序
```shell
# 亮红灯
sudo ./main /dev/spidev3.0 ff0000

# 亮绿灯
sudo ./main /dev/spidev3.0 00ff00

# 亮蓝灯
sudo ./main /dev/spidev3.0 0000ff
```
![image](https://github.com/user-attachments/assets/72ed1adc-03c1-426c-92c1-da6da1e4d993)
