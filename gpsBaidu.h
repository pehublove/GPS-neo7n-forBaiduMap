#ifndef GPSBAIDU_H_H_H
#define GPSBAIDU_H_H_H

class GpsBaidu
{
	public:
		GpsBaidu(){};
		virtual ~GpsBaidu(){};

		static double transformLat(double x, double y);
		static double transformLon(double x, double y);
		static void gps_transform( double wgLat, double wgLon, double& mgLat, double& mgLon);
		static void bd_encrypt(double gg_lat, double gg_lon, double &bd_lat, double &bd_lon);
		static void gps2bd(double wgLat, double wgLon, double &bd_lat, double &bd_lon);

	private:
		GpsBaidu(const GpsBaidu &);
		GpsBaidu &operator=(const GpsBaidu &);

		static const double pi;
		static const double a;
		static const double ee;
		static const double x_pi;
};

#endif
