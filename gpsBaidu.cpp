#include "gpsBaidu.h"
#include <math.h>
const double GpsBaidu::pi = 3.14159265358979324;
const double GpsBaidu::a = 6378245.0;
const double GpsBaidu::ee = 0.00669342162296594323;
const double GpsBaidu::x_pi = 3.14159265358979324 * 3000.0 / 180.0;

double GpsBaidu::transformLat(double x, double y)
{
	double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * sqrt(abs(x));
	ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;
	ret += (20.0 * sin(y * pi) + 40.0 * sin(y / 3.0 * pi)) * 2.0 / 3.0;
	ret += (160.0 * sin(y / 12.0 * pi) + 320 * sin(y * pi / 30.0)) * 2.0 / 3.0;
	return ret;
}

double GpsBaidu::transformLon(double x, double y)
{
	double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(abs(x));
	ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;
	ret += (20.0 * sin(x * pi) + 40.0 * sin(x / 3.0 * pi)) * 2.0 / 3.0;
	ret += (150.0 * sin(x / 12.0 * pi) + 300.0 * sin(x / 30.0 * pi)) * 2.0 / 3.0;
	return ret;
}

void GpsBaidu::gps_transform( double wgLat, double wgLon, double& mgLat, double& mgLon)
{
	double dLat = transformLat(wgLon - 105.0, wgLat - 35.0);
	double dLon = transformLon(wgLon - 105.0, wgLat - 35.0);
	double radLat = wgLat / 180.0 * pi;
       	double magic = sin(radLat);
	magic = 1 - ee * magic * magic;
       	double sqrtMagic = sqrt(magic);
	dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);
	dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * pi);
	mgLat = wgLat + dLat;
       	mgLon = wgLon + dLon;
}

void GpsBaidu::bd_encrypt(double gg_lat, double gg_lon, double &bd_lat, double &bd_lon)
{
	double x = gg_lon, y = gg_lat;
	double z = sqrt(x * x + y * y) + 0.00002 * sin(y * x_pi);
	double theta = atan2(y, x) + 0.000003 * cos(x * x_pi);
	bd_lon = z * cos(theta) + 0.0065;
	bd_lat = z * sin(theta) + 0.006;
}

void GpsBaidu::gps2bd(double wgLat, double wgLon, double &bd_lat, double &bd_lon)
{
	double gg_lat,gg_lon;
	gps_transform(wgLat, wgLon, gg_lat, gg_lon);
	bd_encrypt(gg_lat, gg_lon, bd_lat, bd_lon);
}
