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
		
		print((length($hex)/2)." bytes: ".$hex."\n");
	
##		last;
	}







}

Main() unless caller;




__DATA__
[
	{"id": 0,"gpsfix": 3,"numsats": 5,"snr": 10,"rssi": -100,"long": -2.064870,"lat": 52.059950,"height": 100.0,"accuracy": 5.0,"voltage": 4150,"counter": 100,"millis": 100},
	{"id": 0,"gpsfix": 3,"numsats": 5,"snr": 10,"rssi": -100,"long": -2.064880,"lat": 52.059960,"height": 101.0,"accuracy": 5.0,"voltage": 4150,"counter": 101,"millis": 100},
	{"id": 0,"gpsfix": 3,"numsats": 5,"snr": 10,"rssi": -100,"long": -2.064890,"lat": 52.059970,"height": 102.0,"accuracy": 5.0,"voltage": 4150,"counter": 102,"millis": 100},
	{"id": 0,"gpsfix": 3,"numsats": 5,"snr": 10,"rssi": -100,"long": -2.064900,"lat": 52.059980,"height": 103.0,"accuracy": 5.0,"voltage": 4150,"counter": 103,"millis": 100},
	{"id": 0,"gpsfix": 3,"numsats": 5,"snr": 10,"rssi": -100,"long": -2.064910,"lat": 52.059990,"height": 104.0,"accuracy": 5.0,"voltage": 4150,"counter": 104,"millis": 100},
	{"id": 0,"gpsfix": 3,"numsats": 5,"snr": 10,"rssi": -100,"long": -2.064920,"lat": 52.060000,"height": 105.0,"accuracy": 5.0,"voltage": 4150,"counter": 105,"millis": 100},
	{"id": 0,"gpsfix": 3,"numsats": 5,"snr": 10,"rssi": -100,"long": -2.064930,"lat": 52.060010,"height": 106.0,"accuracy": 5.0,"voltage": 4150,"counter": 106,"millis": 100},
	{"id": 0,"gpsfix": 3,"numsats": 5,"snr": 10,"rssi": -100,"long": -2.064940,"lat": 52.060020,"height": 107.0,"accuracy": 5.0,"voltage": 4150,"counter": 107,"millis": 100},
	{"id": 0,"gpsfix": 3,"numsats": 5,"snr": 10,"rssi": -100,"long": -2.064950,"lat": 52.060030,"height": 108.0,"accuracy": 5.0,"voltage": 4150,"counter": 108,"millis": 100},
	{"id": 0,"gpsfix": 3,"numsats": 5,"snr": 10,"rssi": -100,"long": -2.064960,"lat": 52.060040,"height": 109.0,"accuracy": 5.0,"voltage": 4100,"counter": 109,"millis": 100},
	{"id": 0,"gpsfix": 3,"numsats": 5,"snr": 10,"rssi": -100,"long": -2.064970,"lat": 52.060050,"height": 110.0,"accuracy": 5.0,"voltage": 4100,"counter": 110,"millis": 100},
	{"id": 0,"gpsfix": 3,"numsats": 5,"snr": 10,"rssi": -100,"long": -2.064980,"lat": 52.060060,"height": 111.0,"accuracy": 5.0,"voltage": 4100,"counter": 111,"millis": 100},
	{"id": 1,"gpsfix": 3,"numsats": 6,"snr": 10,"rssi": -100,"long": -2.064980,"lat": 52.060000,"height": 111.0,"accuracy": 5.0,"voltage": 4100,"counter": 111,"millis": 100},
	{"id": 2,"gpsfix": 3,"numsats": 7,"snr": 10,"rssi": -100,"long": -2.064980,"lat": 52.060100,"height": 111.0,"accuracy": 5.0,"voltage": 4100,"counter": 111,"millis": 100},
	{"id": 3,"gpsfix": 3,"numsats": 8,"snr": 10,"rssi": -100,"long": -2.064980,"lat": 52.060200,"height": 111.0,"accuracy": 5.0,"voltage": 4100,"counter": 111,"millis": 100},
	{"id": 4,"gpsfix": 3,"numsats": 9,"snr": 10,"rssi": -100,"long": -2.064980,"lat": 52.060300,"height": 111.0,"accuracy": 5.0,"voltage": 4100,"counter": 111,"millis": 100},
	{"id": 5,"gpsfix": 3,"numsats": 10,"snr": 10,"rssi": -100,"long": -2.064980,"lat": 52.060400,"height": 111.0,"accuracy": 5.0,"voltage": 4100,"counter": 111,"millis": 100},
	{"id": 6,"gpsfix": 3,"numsats": 9,"snr": 10,"rssi": -100,"long": -2.064980,"lat": 52.060500,"height": 111.0,"accuracy": 5.0,"voltage": 4100,"counter": 111,"millis": 100},
	{"id": 7,"gpsfix": 3,"numsats": 8,"snr": 10,"rssi": -100,"long": -2.064980,"lat": 52.060600,"height": 111.0,"accuracy": 5.0,"voltage": 4100,"counter": 111,"millis": 100},
	{"id": 8,"gpsfix": 3,"numsats": 7,"snr": 10,"rssi": -100,"long": -2.064980,"lat": 52.060700,"height": 111.0,"accuracy": 5.0,"voltage": 4100,"counter": 111,"millis": 100},
	{"id": 9,"gpsfix": 3,"numsats": 6,"snr": 10,"rssi": -100,"long": -2.064980,"lat": 52.060800,"height": 111.0,"accuracy": 5.0,"voltage": 4100,"counter": 111,"millis": 100},
	{"id": 10,"gpsfix": 3,"numsats": 5,"snr": 10,"rssi": -100,"long": -2.064980,"lat": 52.060900,"height": 111.0,"accuracy": 5.0,"voltage": 4100,"counter": 111,"millis": 100},
	{"id": 11,"gpsfix": 3,"numsats": 6,"snr": 10,"rssi": -100,"long": -2.064980,"lat": 52.061000,"height": 111.0,"accuracy": 5.0,"voltage": 4100,"counter": 111,"millis": 100},
	{"id": 12,"gpsfix": 3,"numsats": 7,"snr": 10,"rssi": -100,"long": -2.064980,"lat": 52.061100,"height": 111.0,"accuracy": 5.0,"voltage": 4100,"counter": 111,"millis": 100}
]
