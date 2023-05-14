
function DrawSatPlot()
{
//	ctx.font="100px Comic Sans MS";
//	ctx.fillStyle = "red";
//	ctx.textAlign = "center";
//	ctx.fillText("Hello World", canvas.width/2, canvas.height/2);	

	const sat_nums=[%SAT_NUMS%];
	const sat_elevs=[%SAT_ELEVS%];
	const sat_azs=[%SAT_AZS%];
	const sat_snrs=[%SAT_SNRS%];
	
	var satcnt=0;
	
	for(let cnt=0;cnt<sat_nums.length;cnt++)
	{
		if(sat_snrs[cnt]>5)	{	satcnt++;	}
	}
	
	DrawSatCrosshairs();
	DrawSatSpots(sat_nums,sat_elevs,sat_azs,sat_snrs);
	DrawTextData(satcnt);
	
	DrawSatBars(sat_nums,sat_elevs,sat_azs,sat_snrs);
}

function DrawSatCrosshairs()
{
	var canvas=document.getElementById("satelliteplot");
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

function DrawSatSpots(sat_nums,sat_elevs,sat_azs,sat_snrs)
{
	var canvas=document.getElementById("satelliteplot");
	var ctx=canvas.getContext("2d");
	
	for(let cnt=0;cnt<sat_nums.length;cnt++)
	{
		SatSpotAt(canvas,ctx,sat_azs[cnt],sat_elevs[cnt],sat_nums[cnt],sat_snrs[cnt]);
	}
}

function DrawTextData(satcnt)
{
	var canvas=document.getElementById("satelliteplot");
	var ctx=canvas.getContext("2d");
	
	// these all need to be set by the ayncwebserver templating stuff
	
	var numCh=%NUM_CHANNELS%;
	var gpsFix=%GPS_FIX%;
	var hAcc=%HORIZONTAL_ACCURACY%;
	
	var batCurrent=%BAT_CURRENT%;
	var batVolt=%BAT_VOLTAGE%;
	var batStatus=%BAT_STATUS%;
	
	var frequency=434.150;
	var loramode="%LORA_MODE%";
	
	var latitude="%LATITUDE%";
	var longitude="%LONGITUDE%";
	var altitude="%ALTITUDE%";
	
	ctx.fillStyle = "red";
	ctx.textAlign = "left";
	
	ctx.font="30px Comic Sans MS";
	ctx.fillText(satcnt+" / "+numCh+" sats",-0.45*canvas.width,0.35*canvas.height);
	
	if(gpsFix==0)		ctx.fillText("No Fix",-0.45*canvas.width,0.40*canvas.height);
	else if(gpsFix==2)	ctx.fillText("2D Fix",-0.45*canvas.width,0.40*canvas.height);
	else				ctx.fillText("3D Fix",-0.45*canvas.width,0.40*canvas.height);
	
	ctx.fillText(hAcc+" m",-0.45*canvas.width,0.45*canvas.height);
	
	ctx.font="30px Comic Sans MS";
	ctx.fillText("V:"+batVolt+" V",-0.45*canvas.width,-0.45*canvas.height);
	ctx.fillText("I:"+(batCurrent*1000)+" mA",-0.45*canvas.width,-0.4*canvas.height);
	ctx.fillText(batStatus,-0.45*canvas.width,-0.35*canvas.height);
	
	ctx.textAlign = "right";
	ctx.font="30px Comic Sans MS";
	ctx.fillText(latitude,0.45*canvas.width,-0.45*canvas.height);
	ctx.fillText(longitude,0.45*canvas.width,-0.40*canvas.height);
	ctx.fillText(altitude+" m",0.45*canvas.width,-0.35*canvas.height);
	
	ctx.font="30px Comic Sans MS";
	ctx.fillText(frequency+" MHz",0.45*canvas.width,0.45*canvas.height);
	ctx.fillText(loramode,0.45*canvas.width,0.40*canvas.height);
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

function DrawSatBars(sat_nums,sat_elevs,sat_azs,sat_snrs)
{
	var canvas=document.getElementById("satellitebars");
	var ctx=canvas.getContext("2d");
	
	
	
	
	
	
	
	
	
	
	
	
	
	
}
