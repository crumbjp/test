#!/usr/bin/env perl
use strict;
use warnings;

my $NUM = 10000000;
for ( my $f = 0 ; $f <= 9 ; $f++ ) {
    open ( my $FP , '>small'.$f.'.js');
    for ( my $i = $f*$NUM +1 ; $i <= $f*$NUM + $NUM; $i++ ) {
	my $LINE = '{' .
	    '"_id":'.$i.','.
	    '"value1":"aaaaaaaaaa",'.
	    '"value2":"bbbbbbbbbbbbbbbbbbbb",'.
	    '"value3":"cccccccccccccccccccccccccccccc",'.
	    '"value4":"dddddddddddddddddddddddddddddddddddddddddddddddddd",'.
	    '"value5":'.$i % 40000 . ','.
	    '"value6":"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",'.
	    '"value7":"",'.
	    '"value8":"",'.
	    '"value9":"hhhhhhhhhhhhhhhhhhhhhhhhhhhhhh",'.
	    '"value0":'.$i.'}'."\n";
	print $FP $LINE;
	
    }
    close($FP);
}

