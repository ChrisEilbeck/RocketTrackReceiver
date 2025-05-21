#!/usr/bin/env perl

use strict;
use warnings;

use Data::Dumper;
use JSON;

sub Main()
{
	my $json;
	
	{
		$/=undef;
		$json=<DATA>;
	}
	
##	print($json."\n");
##	print(Dumper(decode_json($json)));
			
	my @packets=@{decode_json($json)};

##	print(Dumper(\@packets));
	
	print("#!/bin/bash\n\nPORT=/dev/ttyACM0\n\nstty cs8 -ixon -crtscts 115200 -echo < \${PORT}\n\n");
	
	foreach my $str (@packets)
	{
##		print(Dumper($str));
		
		my $packedlong=$str->{'long'}*131072.0;
		my $packedlat=$str->{'lat'}*131072.0;

##		print($packedlong."\t".$packedlat."\t");

		my $pkt=pack(
						"CCllsCCs",
						$str->{'id'},
						$str->{'gpsfix'}<<6|$str->{'numsats'},
						$packedlong,
						$packedlat,
						$str->{'height'},
						$str->{'accuracy'}/2,
						$str->{'voltage'}/20,
						$str->{'counter'}
					);
		
		my $hex=unpack("H*",$pkt);
		
##		print((length($hex)/2)." bytes: ".$hex."\n");

		print("echo \"rx ".$hex."\" > \${PORT}\n");
		print("sleep 1\n\n");
	
##		last;
	}







}

Main() unless caller;




__DATA__
[
	{"id": 0,"gpsfix": 3,"numsats": 1,"snr": 10,"rssi": -90,"long":-2.33387000,"lat":52.08047500,"height":50.0,"accuracy":1.0,"voltage": 4150,"counter": 100,"millis": 1000},
	{"id": 1,"gpsfix": 3,"numsats": 2,"snr": 10,"rssi": -91,"long":-2.33387000,"lat":52.08147500,"height":51.0,"accuracy":1.1,"voltage": 4150,"counter": 100,"millis": 2000},
	{"id": 2,"gpsfix": 3,"numsats": 3,"snr": 10,"rssi": -92,"long":-2.33387000,"lat":52.08247500,"height":52.0,"accuracy":1.2,"voltage": 4150,"counter": 100,"millis": 3000},
	{"id": 3,"gpsfix": 3,"numsats": 4,"snr": 10,"rssi": -93,"long":-2.33287000,"lat":52.08047500,"height":53.0,"accuracy":1.3,"voltage": 4150,"counter": 100,"millis": 4000},
	{"id": 4,"gpsfix": 3,"numsats": 5,"snr": 10,"rssi": -94,"long":-2.33287000,"lat":52.08247500,"height":54.0,"accuracy":1.4,"voltage": 4150,"counter": 100,"millis": 5000},
	{"id": 5,"gpsfix": 3,"numsats": 6,"snr": 10,"rssi": -95,"long":-2.33187000,"lat":52.08047500,"height":55.0,"accuracy":1.5,"voltage": 4150,"counter": 100,"millis": 6000},
	{"id": 6,"gpsfix": 3,"numsats": 7,"snr": 10,"rssi": -96,"long":-2.33187000,"lat":52.08147500,"height":56.0,"accuracy":1.6,"voltage": 4150,"counter": 100,"millis": 7000},
	{"id": 7,"gpsfix": 3,"numsats": 8,"snr": 10,"rssi": -97,"long":-2.33187000,"lat":52.08247500,"height":57.0,"accuracy":1.7,"voltage": 4150,"counter": 100,"millis": 8000},
	{"id": 8,"gpsfix": 3,"numsats": 9,"snr": 10,"rssi": -98,"long":-2.33287000,"lat":52.08197500,"height":58.0,"accuracy":1.8,"voltage": 4150,"counter": 100,"millis": 7000},
	{"id": 9,"gpsfix": 3,"numsats": 10,"snr": 10,"rssi": -99,"long":-2.33287000,"lat":52.08097500,"height":59.0,"accuracy":1.9,"voltage": 4150,"counter": 100,"millis": 8000}
]
