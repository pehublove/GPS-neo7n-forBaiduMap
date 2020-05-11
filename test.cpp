#include <stdio.h> 	//标准输入输出定义
#include <stdlib.h>	//标准函数库定义
#include <unistd.h>	//Unix标准函数定义
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 	//文件控制定义
#include <termios.h>	//POSIX中断控制定义
#include <errno.h>	//错误号定义
#include <string.h>

#include <iostream>
#include <string>
#include <vector>
#include "gpsBaidu.h"

#define serial_device "/dev/serial/by-id/usb-u-blox_AG_-_www.u-blox.com_u-blox_7_-_GPS_GNSS_Receiver-if00"
using namespace std;
//打开串口
int open_port(void)
{
	int fd;		//串口的标识符
	//O_NOCTTY用来告诉Linux这个程序不会成为“控制终端”
	//O_NDELAY用来告诉Linux这个程序不关心DCD信号
	fd=open(serial_device,O_RDWR | O_NOCTTY | O_NDELAY);
	if(fd == -1)
	{
		//不能打开串口
		perror("open_port: Unable to open /dev/ttyS0 -");
		return(fd);
	}
	else
	{
		fcntl(fd, F_SETFL, 0);
		printf("open ttys0 .....\n");
		return(fd);
	}
}

//设置波特率
void set_speed_and_parity(int fd, int speed)
{
	int i=0;		//设置循环标志——注意不要在for内设置，否则会出错
	struct termios Opt;	//定义termios结构
	if(tcgetattr(fd,&Opt)!=0)
	{
		perror("tcgetattr fd");
		return;
	}
	tcflush(fd, TCIOFLUSH);
	cfsetispeed(&Opt, speed);
	cfsetospeed(&Opt, speed);
	/*tcsetattr函数标志：
	TCSANOW：立即执行而不等待数据发送或者接受完成。
	TCSADRAIN：等待所有数据传递完成后执行。
	TCSAFLUSH：Flush input and output buffers and make the change
	*/
	if(tcsetattr(fd, TCSANOW, &Opt) != 0)
	{	
		perror("tcsetattr fd");
		return;
	}
	tcflush(fd, TCIOFLUSH);
	//设置奇偶校验——默认8个数据位、没有校验位
        
    	Opt.c_cflag &= ~PARENB;
   	 Opt.c_cflag &= ~CSTOPB;
    	Opt.c_cflag &= ~CSIZE;
    	Opt.c_cflag |= CS8;
	
	//其他的一些配置
	//原始输入，输入字符只是被原封不动的接收
	Opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	//软件流控制无效，因为硬件没有硬件流控制，所以就不需要管了
	Opt.c_iflag &= ~(IXON | IXOFF | IXANY);
	//原始输出方式可以通过在c_oflag中重置OPOST选项来选择：
	Opt.c_oflag |= ~OPOST;
	//VMIN可以指定读取的最小字符数。如果它被设置为0，那么VTIME值则会指定每个字符读取的等待时间。
    	Opt.c_cc[VTIME] = 0;
    	Opt.c_cc[VMIN] = 0;
//    	Opt.c_cflag &= ~INPCK;
//    	Opt.c_cflag |= (CLOCAL | CREAD);
//	
//    	Opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
// 
//    	Opt.c_oflag &= ~OPOST;
   	Opt.c_oflag &= ~(ONLCR | OCRNL);    
//
   	Opt.c_iflag &= ~(ICRNL | INLCR);
//    	Opt.c_iflag &= ~(IXON | IXOFF | IXANY);    
//    
//    	Opt.c_cc[VTIME] = 0;
//    	Opt.c_cc[VMIN] = 0;
    
    	tcflush(fd, TCIOFLUSH);

}


/**
  *串口发送数据函数
  *fd：串口描述符
  *data：待发送数据
  *datalen：数据长度
  */
int serial_write(int fd ,char *data, int datalen)
{
	int len=0;
	//获取实际传输数据的长度
	len=write(fd,data,datalen);
	printf("send data OK! datalen=%d\n",len);
	return len;	
}

/** 
  *串口接收数据 
  *要求启动后，在pc端发送ascii文件 
  */ 
int serial_read(int fd,char buff[],int datalen)
{
	int nread=0;
	//printf("Ready for receiving data...");
	nread=read(fd,buff,datalen);
	if(nread>0)
	{
	//	printf("readlength=%d\n",nread);
		buff[nread]='\0';
	//	printf("%s\n",buff);
	}
	return nread;
}

int serialport()
{
	int fd;	
	//打开串口
	if((fd=open_port())<0)
    	{
        	perror("open_port error");
        	return 0;
    	}
	//设置波特率和校验位
	set_speed_and_parity(fd,9600);
	return (fd);
}

void split(vector<string> &elems, const string& s, const string& delim)
{
	size_t pos = 0;
	size_t len = s.length();
	size_t delim_len = delim.length();
	if (delim_len == 0) return;
	while (pos < len)
	{
		int find_pos = s.find(delim, pos);
		if (find_pos < 0)
		{
			elems.push_back(s.substr(pos, len - pos));
			break;
		}
		elems.push_back(s.substr(pos, find_pos - pos));
		pos = find_pos + delim_len;
	}
}

int main(int argc, char **argv)
{
	int fd;
	int nread,i,n =0,datalen=255, len = 0;
	char testbuff[]="Hello\r\n";
	char readbuff[256];
	
	fd=serialport();
	printf("fd=%d\n",fd);
	//尝试先写内容
	nread=write(fd,testbuff,datalen);
	if(nread==-1)
	{
		printf("write bad\n");
	}
	else{
		printf("the test is ok!\n");
	}	
	//循环读取串口并输出
	while(1)
	{
		//printf("enter the while loop\n");
		bzero(readbuff, sizeof(readbuff));
		datalen=serial_read(fd,readbuff,255);
		if(datalen > 0)
		{
			cout << datalen << endl;
			string parseStr = readbuff;
			cout << parseStr << endl;
			vector<string> every;
			split(every, parseStr, "\n");
			vector<string>::iterator it = every.begin();
			for(;it != every.end();it++)
			{
			if(it->find("GPRMC") != string::npos)
			{
				vector<string> vecGps;
				vecGps.reserve(15);
				split(vecGps, *it, ",");
				if(vecGps.at(2) == "A")
				{
					cout << "valid data" << endl;
					cout << "longitude: " << vecGps.at(5) << endl;
					cout << "latitude: " << vecGps.at(3) << endl;
					double lon = stod(vecGps.at(5));
					double lat = stod(vecGps.at(3));
					int tempF = (int)lon / 100;
					double point = (lon - tempF * 100)/60;
					double longitude = tempF + point;

					int tempF1 = (int)lat / 100;
					double point1 = (lat - tempF1 * 100)/60;
					double latitude = tempF1 + point1;

					double latbd,lonbd;
					GpsBaidu::gps2bd(latitude, longitude, latbd, lonbd);
					cout << endl;
					cout << "Baidu longitude: " << lonbd << endl;
					cout << "Baidu latitude: " << latbd << endl;
				}
			}
			}

		}
	}
	return 0;
}
