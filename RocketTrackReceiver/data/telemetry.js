
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

function UpdateRangeAndBearing()
{
	var rx_latitude="%RX_LATITUDE%";
	var rx_longitude="%RX_LONGITUDE%";
	var tx_latitude="%TX_LATITUDE%";
	var tx_longitude="%TX_LONGITUDE%";
	
	var range=GreatCircleDistance(rx_latitude,rx_longitude,tx_latitude,tx_longitude);
	var bearing=GreatCircleBearing(tx_latitude,tx_longitude,rx_latitude,rx_longitude);
	
	document.getElementById("Range").textContent=range.toFixed();
	document.getElementById("Bearing").textContent=bearing.toFixed();
}

function DrawTrackingPlot()
{
	var rx_latitude="52.1";
	var rx_longitude="-2.31";
	var tx_latitude="50.213461";
	var tx_longitude="-5.476708";
	var rx_heading=-30;
	
	if(1)
	{
		rx_latitude="%RX_LATITUDE%";
		rx_longitude="%RX_LONGITUDE%";
		tx_latitude="%TX_LATITUDE%";
		tx_longitude="%TX_LONGITUDE%";
		rx_heading="%RX_HEADING%";
	}
	
	console.log("input RX_HEADING = "+rx_heading);
	
	var range=GreatCircleDistance(rx_latitude,rx_longitude,tx_latitude,tx_longitude);
	var bearing=GreatCircleBearing(tx_latitude,tx_longitude,rx_latitude,rx_longitude);
	var maxrange;
	
	maxrange=DrawTrackingCrosshairs(range,-rx_heading);
	DrawTextData();
	
	DrawSpotAt(range,bearing,maxrange,rx_heading);
}

