#!/usr/bin/env perl

use strict;
use warnings;

my $ipf;
##open($ipf,"<"."mrc-adr2.txt") or die("Can't open file for input\n");
##open($ipf,"<"."cartesting.txt") or die("Can't open file for input\n");
open($ipf,"<"."FlightData/LOG00356.TXT") or die("Can't open file for input\n");

while(<$ipf>)
{
	chomp;
	my $line1=$_;
##	my $line2=<$ipf>;
##	$line2=~s/\\r//g;
##	$line2=~s/\\n//g;
	
##	if(!defined($line2))	{	last;	}
	
##	$line1.=$line2;
	
	if(0)	{	print($line1);		}
	
	my $packetnumber=0;
	my $beaconlat=0;
	my $beaconlong=0;
	my $beaconalt=0;
	my $rssi=0;
	my $snr=0;
	my $offset=0;
	
	## Received packet '00cd257b17ff32ae641f6e0003ce6b00' with RSSI -77, with SNR 9 and offset 1803 HzRx packet: BeaconLat = 52.669189, BeaconLong = -1.523836, BeaconHeight = 110.0, BeaconAcc = 0.00	High Rate Mode
	
	if(!($line1=~/Setting/))
	{	
		if($line1=~/^Received packet '(.{2})..(.{8})(.{8})(.{4}).*(.{4})'/)
		{
			if(0)	{	print($line1."\n");				}
			
			$packetnumber=hex(substr($5,0,2))+256*hex(substr($5,2,2));
			if(0)	{	print($packetnumber."\t");		}
			
			$beaconlong=unpack("l",pack("H*",$2))/131072.0;
			if(0)	{	print($2."\n");					}
			
			$beaconlat=unpack("l",pack("H*",$3))/131072.0;
			if(0)	{	print($3."\n");					}
			
			$beaconalt=unpack("s",pack("H*",$4));
			if(0)	{	print($4."\n");					}
						
			if($line1=~/RSSI (.*),/)
			{
				$rssi=$1;
				if(0)	{	print($rssi."\t");			}
			}
			
			if($line1=~/SNR (.*) and/)
			{
				$snr=$1;
				if(0)	{	print($snr."\t");			}
			}
			
			if(		($rssi<0)
				&&	($beaconalt>=-1000)
				&&	($beaconalt<=20000)		)
			{
				print($packetnumber."\t".$rssi."\t".$snr."\t".sprintf("%0.6f",$beaconlat)."\t".sprintf("%0.6f",$beaconlong)."\t".$beaconalt."\n");
			}
			
##			last;
		}
		
	}
	
##	last;
}

close($ipf);

