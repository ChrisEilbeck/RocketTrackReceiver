
function RotatePoint(x,y,angle)
{
	inx=x;
	iny=y;
	
	x=inx*cos(angle)-iny*sin(angle);
	y=inx*sin(angle)+iny*cos(angle);
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

function DrawTrackingCrosshairs(range,rx_heading)
{
	var canvas=document.getElementById("trackingplot");
	var ctx=canvas.getContext("2d");
	
	ctx.translate(canvas.width/2,canvas.height/2);
	
	var radius=canvas.width*0.9/2;
	var numcircles;
	var percircle;
	
	if(range>25000)			{	numcircles=5;	percircle=10000;	maxrange=50000;	}
	else if(range>10000)	{	numcircles=4;	percircle=5000;		maxrange=20000;	}
	else if(range>5000)		{	numcircles=5;	percircle=2000;		maxrange=10000;	}
	else if(range>2000)		{	numcircles=5;	percircle=1000;		maxrange=5000;	}
	else if(range>1000)		{	numcircles=4;	percircle=500;		maxrange=2000;	}
	else if(range>500)		{	numcircles=4;	percircle=250;		maxrange=1000;	}
	else if(range>200)		{	numcircles=5;	percircle=100;		maxrange=500;	}
	else if(range>100)		{	numcircles=4;	percircle=50;		maxrange=200;	}
	else if(range>50)		{	numcircles=4;	percircle=25;		maxrange=100;	}
	else if(range>20)		{	numcircles=5;	percircle=10;		maxrange=50;	}
	else					{	numcircles=4;	percircle=5;		maxrange=20;	}
	
// 	console.log("Range = "+range);
// 	console.log("Drawing "+numcircles+" of "+percircle+" m each");
	
	var cnt;
	
	ctx.rotate(rx_heading*Math.PI/180);
	
	ctx.font="40px Comic Sans MS";
	ctx.fillStyle="black";
	ctx.textAlign="center";
	ctx.fillText("N",0,-1.02*radius);
	
	for(cnt=1;cnt<=numcircles;cnt++)
	{
		ctx.beginPath();
		ctx.lineWidth=5;
		ctx.arc(0,0,radius*cnt/numcircles,0,2*Math.PI);
		ctx.stroke();
		ctx.font="20px Comic Sans MS";
		ctx.fillStyle="red";
		ctx.textAlign="center";
		ctx.fillText(cnt*percircle,radius*cnt/numcircles,-0.01*canvas.height);
		ctx.fillText(cnt*percircle,-radius*cnt/numcircles,-0.01*canvas.height);
		ctx.fillText(cnt*percircle,0,-radius*cnt/numcircles+0.01*canvas.height);
		ctx.fillText(cnt*percircle,0,radius*cnt/numcircles+0.01*canvas.height);
	}
	
	ctx.moveTo(0,-canvas.height/2);
	ctx.lineTo(0,canvas.height/2);
	ctx.stroke();
	
	ctx.moveTo(-canvas.width/2,0);
	ctx.lineTo(canvas.width/2,0);
	ctx.stroke();
	
	ctx.rotate(-rx_heading*Math.PI/180);
	
	return(maxrange);
}

function DrawTextData()
{
	var canvas=document.getElementById("trackingplot");
	var ctx=canvas.getContext("2d");
	
	var bat_volt="4.150V";
	var lora_freq="434.150MHz";
	var lora_mode="Long Range";
	var lora_rssi="-123";
	var lora_snr="10";
	var rx_latitude="52.0";
	var rx_longitude="-2.41";
	var rx_altitude="100";
	var tx_latitude="52.3";
	var tx_longitude="-2.11";
	var tx_altitude="350";
	var numsats=5;
	var id=1;
	var hdop=1.5;
	var hacc=5;
	
	// these will be filled in by templating in the asyncwebserver
	if(1)
	{
		bat_volt="%BEACON_VOLTAGE%";
		lora_freq="%LORA_FREQUENCY%";
		lora_mode="%LORA_MODE%";
		lora_rssi="%LORA_RSSI%";
		lora_snr="%LORA_SNR%";
		rx_latitude="%RX_LATITUDE%";
		rx_longitude="%RX_LONGITUDE%";
		rx_altitude="%RX_ALTITUDE%";
		tx_latitude="%TX_LATITUDE%";
		tx_longitude="%TX_LONGITUDE%";
		tx_altitude="%TX_ALTITUDE%";
		numsats="%NUMSATS%";
		id="%BEACON_ID%";
		hacc="%BEACON_HACC%";
	}
	
	console.log(rx_latitude+" "+rx_longitude+" "+rx_altitude);
	console.log(tx_latitude+" "+tx_longitude+" "+tx_altitude);
	
	var range=GreatCircleDistance(tx_latitude,tx_longitude,rx_latitude,rx_longitude);
	range=range.toFixed(1);
	var bearing=GreatCircleBearing(tx_latitude,tx_longitude,rx_latitude,rx_longitude);
	bearing=bearing.toFixed(1);
	
	console.log(range);
	console.log(bearing);
	
	ctx.font="30px Comic Sans MS";
	ctx.fillStyle="red";
	
	ctx.textAlign="left";
	ctx.fillText("Receiver:",-0.45*canvas.width,-0.45*canvas.height);
	
	if(rx_latitude>0)	ctx.fillText(rx_latitude+"\u00B0 N",-0.45*canvas.width,-0.40*canvas.height);
	else				ctx.fillText(-rx_latitude+"\u00B0 S",-0.45*canvas.width,-0.40*canvas.height);
	if(rx_longitude>0)	ctx.fillText(rx_longitude+"\u00B0 E",-0.45*canvas.width,-0.35*canvas.height);
	else				ctx.fillText(-rx_longitude+"\u00B0 W",-0.45*canvas.width,-0.35*canvas.height);
	ctx.fillText(rx_altitude+"m",-0.45*canvas.width,-0.30*canvas.height);
	
	ctx.fillText("ID: "+id,-0.45*canvas.width,0.25*canvas.height);
	ctx.fillText(numsats+" / "+hacc,-0.45*canvas.width,0.30*canvas.height);
	ctx.fillText("B: "+bearing,-0.45*canvas.width,0.35*canvas.height);
	ctx.fillText("R: "+range+" m",-0.45*canvas.width,0.40*canvas.height);
	ctx.fillText("V: "+bat_volt,-0.45*canvas.width,0.45*canvas.height);
	
	ctx.textAlign="right";
	ctx.fillText("Beacon:",0.45*canvas.width,-0.45*canvas.height);
	if(tx_latitude>0)	ctx.fillText(tx_latitude+"\u00B0 N",0.45*canvas.width,-0.40*canvas.height);
	else				ctx.fillText(-tx_latitude+"\u00B0 S",0.45*canvas.width,-0.40*canvas.height);
	if(tx_longitude>0)	ctx.fillText(tx_longitude+"\u00B0 E",0.45*canvas.width,-0.35*canvas.height);
	else				ctx.fillText(-tx_longitude+"\u00B0 W",0.45*canvas.width,-0.35*canvas.height);
	ctx.fillText(tx_altitude+"m",0.45*canvas.width,-0.30*canvas.height);
	
	ctx.fillText(lora_freq+" MHz",0.45*canvas.width,0.45*canvas.height);
	ctx.fillText(lora_mode,0.45*canvas.width,0.40*canvas.height);
	ctx.fillText("RSSI: "+lora_rssi,0.45*canvas.width,0.35*canvas.height);
	ctx.fillText("SNR: "+lora_snr,0.45*canvas.width,0.30*canvas.height);
}

function DrawSpotAt(range,bearing,maxrange,rx_heading)
{
	var canvas=document.getElementById("trackingplot");
	var ctx=canvas.getContext("2d");
	
//	bearing=0
	
	console.log("rx_heading = "+rx_heading);
	console.log("bearing before = "+bearing);
	
	bearing-=90;
	
//	bearing+=180;
	
	// adjust for the bearing of the receiver
	bearing-=rx_heading;
	
	while(bearing>360.0)	{	bearing-=360.0;		}
	while(bearing<0.0)		{	bearing+=360.0;		}
	
	console.log("bearing after = "+bearing);
	
	bearing*=Math.PI/180;
	
	var radius=0.9*canvas.width/2*range/maxrange;
	
	var colour="blue";
	ctx.beginPath();
	ctx.fillStyle=colour;
	ctx.arc(Math.cos(bearing)*radius,Math.sin(bearing)*radius,15,0,2*Math.PI);
	ctx.fill();
	
	var colour="white";
	ctx.beginPath();
	ctx.fillStyle=colour;
	ctx.arc(Math.cos(bearing)*radius,Math.sin(bearing)*radius,10,0,2*Math.PI);
	ctx.fill();
	
	var colour="red";
	ctx.beginPath();
	ctx.fillStyle=colour;
	ctx.arc(Math.cos(bearing)*radius,Math.sin(bearing)*radius,5,0,2*Math.PI);
	ctx.fill();
}

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
