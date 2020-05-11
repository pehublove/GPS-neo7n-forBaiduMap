# GPS-neo7n-forBaiduMap
linux下通过串口通讯获取GPS信号，并转化为百度地图的坐标形式。（linux get the gps signal throught serial, translate for baidu's map）
GPS模块使用的是neo-7n-0-002型号，通过USB传入串口方式。

GPS数据可以分类为GPGGA、GPGSA、GPGSV、GPRMC等,使用的为GPRMC数据，从中提取GPS坐标信息。
从串口得到的数据格式如：3241.8675，N，10509.5573，E。需要先将其转化为WGS-84的坐标，再通过gps_transform（）函数将坐标转化为GCJ-02标准，此时可以在google地图、高德地图、以及腾讯地图上定位。
百度地图有些特殊，它在GCJ-02基础上进行BD-09加密后，才能用于百度地图的定位。
