
function GreatCircleDistance(lat1,long1,lat2,long2)
{
	const R= 6371e3;					// metres
	
	const phi1=lat1*Math.PI/180;		// φ, λ in radians
	const phi2=lat2*Math.PI/180;
	const delta_p=(lat2-lat1)*Math.PI/180;
	const delta_l=(long2-long1)*Math.PI/180;

	const a=	Math.sin(delta_p/2)*Math.sin(delta_p/2)
			+	Math.cos(phi1)*Math.cos(phi2)*Math.sin(delta_l/2)*Math.sin(delta_l/2);
	
	const c=2*Math.atan2(Math.sqrt(a),Math.sqrt(1-a));
	
	const d=R*c;						// in metres	
	
	return(d);
}

function GreatCircleBearing(lat2,lon2,lat1,lon1)
{
	lat1*=Math.PI/180;
	lon1*=Math.PI/180;
	lat2*=Math.PI/180;
	lon2*=Math.PI/180;
	
	var lonDelta=lon2-lon1;
	var y=Math.sin(lonDelta)*Math.cos(lat2);
	var x=Math.cos(lat1)*Math.sin(lat2)-Math.sin(lat1)*Math.cos(lat2)*Math.cos(lonDelta);
	
	var brng=Math.atan2(y,x);
	brng=brng*(180/Math.PI);
	
	if(brng<0)	{	brng+=360;	}
	
	return(brng);
}

