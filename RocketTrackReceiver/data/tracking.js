
function DrawTrackingPlot()
{
	DrawTrackingCrosshairs();
	DrawTextData();
}

function DrawTrackingCrosshairs()
{
	var canvas=document.getElementById("trackingplot");
	var ctx=canvas.getContext("2d");
	
	ctx.translate(canvas.width/2,canvas.height/2);
	
	var radius=canvas.width*0.9/2;
	
	ctx.beginPath();
	ctx.arc(0,0,radius/3,0,2*Math.PI);
	ctx.stroke();
	
	ctx.beginPath();
	ctx.arc(0,0,2*radius/3,0,2*Math.PI);
	ctx.stroke();
	
	ctx.beginPath();
	ctx.arc(0,0,radius,0,2*Math.PI);
	ctx.stroke();
	
	ctx.moveTo(0,-canvas.height/2);
	ctx.lineTo(0,canvas.height/2);
	ctx.stroke();
	
	ctx.moveTo(-canvas.width/2,0);
	ctx.lineTo(canvas.width/2,0);
	ctx.stroke();
}

function DrawTextData()
{
	var canvas=document.getElementById("trackingplot");
	var ctx=canvas.getContext("2d");
	
	var bat_volt="4.150V";
	var lora_freq="434.650MHz";
	var lora_mode="Long Range";
	var lora_rssi="-123";
	var lora_snr="10";
	var rx_latitude="52.1";
	var rx_longitude="-2.31";
	var rx_altitude="200";
	var tx_latitude="52.2";
	var tx_longitude="-2.21";
	var tx_altitude="250";
	var range=5200;
	var bearing=45;
	var numsats=5;
	var id=1;
	var hdop=1.5;
	
	// these will be filled in by templating in the asyncwebserver
	
// 	bat_volt="%BAT_VOLTAGE%";
// 	lora_freq="%LORA_FREQUENCY%";
// 	lora_mode="%LORA_MODE%";
// 	lora_rssi="%LORA_RSSI%";
// 	lora_snr="%LORA_SNR%";
// 	rx_latitude="%RX_LATITUDE%";
// 	rx_longitude="%RX_LONGITUDE%";
// 	rx_altitude="%RX_ALTITUDE%";
// 	tx_latitude="%TX_LATITUDE%";
// 	tx_longitude="%TX_LONGITUDE%";
// 	tx_altitude="%TX_ALTITUDE%";
	
	ctx.font="30px Comic Sans MS";
	
	ctx.textAlign="left";
	ctx.fillText("Receiver:",-0.45*canvas.width,-0.45*canvas.height);
	
	if(rx_latitude>0)	ctx.fillText(rx_latitude+"\u00B0 N",-0.45*canvas.width,-0.40*canvas.height);
	else				ctx.fillText(-rx_latitude+"\u00B0 S",-0.45*canvas.width,-0.40*canvas.height);
	if(rx_longitude>0)	ctx.fillText(rx_longitude+"\u00B0 E",-0.45*canvas.width,-0.35*canvas.height);
	else				ctx.fillText(-rx_longitude+"\u00B0 W",-0.45*canvas.width,-0.35*canvas.height);
	ctx.fillText(rx_altitude+"m",-0.45*canvas.width,-0.30*canvas.height);
	
	ctx.fillText("ID: "+id,-0.45*canvas.width,0.30*canvas.height);
	ctx.fillText(numsats+"/"+hdop,-0.45*canvas.width,0.35*canvas.height);
	ctx.fillText("B: "+bearing,-0.45*canvas.width,0.40*canvas.height);
	ctx.fillText("R: "+range+" m",-0.45*canvas.width,0.45*canvas.height);
	
//	ctx.fillText("V:"+bat_volt,-0.45*canvas.width,-0.45*canvas.height);
	
	ctx.textAlign="right";
	ctx.fillText("Beacon:",0.45*canvas.width,-0.45*canvas.height);
	if(tx_latitude>0)	ctx.fillText(tx_latitude+"\u00B0 N",0.45*canvas.width,-0.40*canvas.height);
	else				ctx.fillText(-tx_latitude+"\u00B0 S",0.45*canvas.width,-0.40*canvas.height);
	if(tx_longitude>0)	ctx.fillText(tx_longitude+"\u00B0 E",0.45*canvas.width,-0.35*canvas.height);
	else				ctx.fillText(-tx_longitude+"\u00B0 W",0.45*canvas.width,-0.35*canvas.height);
	ctx.fillText(tx_altitude+"m",0.45*canvas.width,-0.30*canvas.height);
	
	ctx.fillText(lora_freq,0.45*canvas.width,0.45*canvas.height);
	ctx.fillText(lora_mode,0.45*canvas.width,0.40*canvas.height);
	ctx.fillText("RSSI: "+lora_rssi,0.45*canvas.width,0.35*canvas.height);
	ctx.fillText("SNR: "+lora_snr,0.45*canvas.width,0.30*canvas.height);
}

function SatSpotAt(canvas,ctx,az,el,num,snr)
{
	az-=90;
	
	console.log(Math.cos(az*Math.PI/180));
	
	var radius=0.9*canvas.width/2*(90-el)/90;
	
	var colour="green";
	
	if(snr<5)	{	colour="red";	}
	
	ctx.beginPath();
	ctx.fillStyle=colour;
	ctx.arc(Math.cos(az*Math.PI/180)*radius,Math.sin(az*Math.PI/180)*radius,15,0,2*Math.PI);
	ctx.fill();
	
	var sathoriz=25;
	var satvert=10;
	
	ctx.strokeStyle=colour;
	ctx.beginPath();
	ctx.moveTo(Math.cos(az*Math.PI/180)*radius-sathoriz,Math.sin(az*Math.PI/180)*radius);
	ctx.lineTo(Math.cos(az*Math.PI/180)*radius+sathoriz,Math.sin(az*Math.PI/180)*radius);
	ctx.lineWidth=3;
	ctx.stroke();
	
	ctx.beginPath();
	ctx.moveTo(Math.cos(az*Math.PI/180)*radius-sathoriz,Math.sin(az*Math.PI/180)*radius-satvert);
	ctx.lineTo(Math.cos(az*Math.PI/180)*radius-sathoriz,Math.sin(az*Math.PI/180)*radius+satvert);
	ctx.lineWidth=3;
	ctx.stroke();
	
	ctx.beginPath();
	ctx.moveTo(Math.cos(az*Math.PI/180)*radius+sathoriz,Math.sin(az*Math.PI/180)*radius-satvert);
	ctx.lineTo(Math.cos(az*Math.PI/180)*radius+sathoriz,Math.sin(az*Math.PI/180)*radius+satvert);
	ctx.lineWidth=3;
	ctx.stroke();
	
	ctx.font="20px Comic Sans MS";
	ctx.fillStyle="white";
	ctx.textAlign="center";
	ctx.fillText(num,Math.cos(az*Math.PI/180)*radius,Math.sin(az*Math.PI/180)*radius+7);
}

