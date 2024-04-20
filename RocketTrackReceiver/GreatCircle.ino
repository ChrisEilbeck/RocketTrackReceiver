
#define DEBUG 0

float GreatCircleDistance(float lat1,float lon1,float lat2,float lon2)
{
	const float R= 6371e3;				// metres
	
	const float phi1=lat1*PI/180;	// φ, λ in radians
	const float phi2=lat2*PI/180;
	const float delta_p=(lat2-lat1)*PI/180;
	const float delta_l=(lon2-lon1)*PI/180;

	const float a=sin(delta_p/2)*sin(delta_p/2)
				+cos(phi1)*cos(phi2)*sin(delta_l/2)*sin(delta_l/2);
	
	const float c=2*atan2(sqrt(a),sqrt(1-a));
	
	const float d=R*c;					// in metres	
	
#if DEBUG
	Serial.printf("From lat=%.6f, lon=%.6f to lat=%.6f, lon=%.6f, distance=%.1f m\r\n",lat1,lon1,lat2,lon2,d);
#endif
	
	return(d);
}

float GreatCircleBearing(float lat2,float lon2,float lat1,float lon1)
{
#if DEBUG
	Serial.printf("From lat=%.6f, lon=%.6f to lat=%.6f, lon=%.6f",lat2,lon2,lat1,lon1);
#endif
	
	lat1*=PI/180;
	lon1*=PI/180;
	lat2*=PI/180;
	lon2*=PI/180;
	
	float lonDelta=lon2-lon1;
	float y=sin(lonDelta)*cos(lat2);
	float x=cos(lat1)*sin(lat2)-sin(lat1)*cos(lat2)*cos(lonDelta);
	
	float brng=atan2(y,x);
	brng=brng*(180/PI);
	
	if(brng<0)	{	brng+=360;	}
	
#if DEBUG
	Serial.printf(", bearing=%.1f degs\r\n",brng);
#endif
	
	return(brng);
}

