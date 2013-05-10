#!/usr/bin/env perl
use strict;
use warnings;
use Getopt::Long;

my $NUM = 0;
my $SIZE = 0;
GetOptions(
    'n:n'    		    => \$NUM,
    's:n'   		    => \$SIZE,
	   ) or exit(1);
if ( ! $NUM or ! $SIZE ) {
    exit(1);
}
my $DIV1= 2500;
my $DIV2= $NUM/2500;
my $H = '';
for ( my $i=0; $i < ($SIZE - 390) ; $i++){
    $H .= 'H';
}
$NUM /= 10;
for ( my $f = 0 ; $f <= 9 ; $f++ ) {
    open ( my $FP , '>data'.$f.'.js');
    for ( my $i = $f*$NUM +1 ; $i <= $f*$NUM + $NUM; $i++ ) {
	my $LINE = '{' .
	    '"_id":'.$i.','.
	    '"value1":'.$i % $DIV1 . ','.
	    '"value2":'.$i % $DIV2 . ','.
	    '"value3":"aaaaaaaaaa",'.
	    '"value4":"bbbbbbbbbbbbbbbbbbbb",'.
	    '"value5":"cccccccccccccccccccccccccccccc",'.
	    '"value6":"dddddddddddddddddddddddddddddddddddddddddddddddddd",'.
	    '"value7":"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",'.
	    '"value8":0,'.
	    '"value9":"'.$H.'",'.
	    '"value0":'.$i.'}'."\n";
	print $FP $LINE;
	
    }
    close($FP);
}

