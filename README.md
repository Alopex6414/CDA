# CDA
This Program is a Simple Communication Debug Assistant (CDA) Project.   
CDA项目是Windows平台下简单的通信调试助手工程。

## 图标
![](https://github.com/Alopex6414/CDA/raw/master/Photo/Cat.png)

## 概述
CDA项目是Windows平台下简单的通信调试助手工程。CDA可以调试串口、网络通信数据。串口调试支持多种串口协议如RS-232、RS-422、RS-485，支持有线或无限的数据发送如串口、蓝牙、2.4GHz射频；实时曲线暂时只支持串口曲线，可以最多支持8条串口曲线的同时接收，可以导出数据到Excel或者截图保存；网络调试支持TCP和UDP两种方式，TCP调试中支持TCP服务端和客户端，UDP支持接收发送。CDA可以在32bit和64bit环境下部署，支持Windos7/8/10。

## 模块
  * ### 串口调试
    * #### *Serial*支持串口自动刷新获取
    * #### *Serial*支持多种串口协议:RS-232、RS-422、RS-485
    * #### *Serial*支持多种物理连接:串口(SerialPort)、蓝牙(BlueTooth)、射频(RF)
    * #### *Serial*支持串口异步收发  
    
  * ### 实时曲线
    * #### *Curve*支持串口曲线展示
    * #### *Curve*支持最多8条曲线
    * #### *Curve*支持导出数据到Excel(.csv)
    * #### *Curve*支持保存截图  
    
  * ### 网络调试
    * #### *Socket*支持TCP和UDP
    * #### *Socket*支持TCP服务端(Server)和TCP客户端(Client)
    * #### *Socket*支持UDP数据的同时接收发送
    * #### *Socket*支持网络异步通信

  * ### 数据分析
  
 ## 功能
   * ### 串口调试
     * #### *串口调试窗口*
   ![](https://github.com/Alopex6414/CDA/raw/master/Photo/Serial_0.png)
   
   * ### 实时曲线
     * #### *实时曲线窗口*
   ![](https://github.com/Alopex6414/CDA/raw/master/Photo/Curve_0.png)
   
   * ### 网络调试
     * #### *网络调试窗口*  
   ![](https://github.com/Alopex6414/CDA/raw/master/Photo/Socket_tcpserver_0.png)
   ![](https://github.com/Alopex6414/CDA/raw/master/Photo/Socket_tcpclient_0.png)
   ![](https://github.com/Alopex6414/CDA/raw/master/Photo/Socket_udp_0.png)
 
