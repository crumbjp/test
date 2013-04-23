#!/usr/bin/env perl
use strict;
use warnings;

my $NUM = 10000000;
for ( my $f = 0 ; $f <= 9 ; $f++ ) {
    open ( my $FP , '>small'.$f.'.js');
    for ( my $i = $f*$NUM +1 ; $i <= $f*$NUM + $NUM; $i++ ) {
	my $LINE = '{' .
	    '"_id":'.$i.','.
	    '"value1":"aaaaaaaaaaaaaaaaaaaa",'.
	    '"value2":"bbbbbbbbbbbbbbbbbbbb",'.
	    '"value3":"cccccccccccccccccccc",'.
	    '"value4":"dddddddddddddddddddd",'.
	    '"value5":'.$i % 4000 . ','.
	    '"value6":"eeeeeeeeeeeeeeeeeeee",'.
	    '"value7":"ffffffffffffffffffff",'.
	    '"value8":"gggggggggggggggggggg",'.
	    '"value9":"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh",'.
	    '"value0":'.$i.'}'."\n";
	print $FP $LINE;
	
    }
    close($FP);
}

