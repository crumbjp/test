#!/usr/bin/env perl
#---------------------------------------------
# sep.pl - ????
# 
#   Copyright (C) 2010 rakuten 
#     by  <hiroaki.kubota@rakuten.co.jp> 
#     Date : 2010/12/29
#---------------------------------------------
use strict;
use warnings;
use Getopt::Long;
use Pod::Usage;

use locale;
use POSIX qw(setlocale strftime mktime LC_ALL);

# use Dumpvalue;
# my $dmp = new Dumpvalue();

my $dname = undef;
my %options = (
	       y   		  => 0,
	       debug              => undef,
	       );

# Option proc.
GetOptions(
	   'help' 		    => sub {pod2usage( -verbose => 3 , -exitval => 0 );},
	   'd:s'    		    => \$dname,
	   'y'    		    => \$options{y},
	   'debug:n'   		    => \$options{debug},
	   ) or pod2usage( -verbose => 0 , -exitval => 1 );
my $dname_dst=$dname.".dst";
system("mkdir -p $dname_dst");

opendir(my $DIR,$dname);
while (my $f = readdir($DIR)){
    if ( $f =~ /(\d+)\.wav/ ) {
	fproc("$dname/$f",$1);
	last;
    }
}
fproc("English1.dst/036-00.wav","036");

sub fproc {
    my ($fname,$number)=@_;
    print "=== $fname === \n";
    open(my $FILE,"<$fname");
    binmode $FILE;
    # header
    read($FILE,my $riff,4);
    read($FILE,my $riff_size,4);
    $riff_size = unpack("L",$riff_size);
    read($FILE,my $wave,4);
    read($FILE,my $fmt,4);
    read($FILE,my $fmt_size,4);
    $fmt_size = unpack("L",$fmt_size);
    read($FILE,my $fmt_data,$fmt_size);
    read($FILE,my $data,4);
    read($FILE,my $data_size,4);
    $data_size = unpack("L",$data_size);
    print $riff."\n";
    print $riff_size."\n";
    print $wave."\n";
    print $fmt."\n";
    print $fmt_size."\n";
    my ($fmt_tag,$fmt_ch,$fmt_sample,$fmt_byte,$fmt_block,$fmt_bit) = unpack("S S L L S S",$fmt_data);
    print $fmt_tag."\n";
    print $fmt_ch."\n";
    print $fmt_sample."\n";
    print $fmt_byte."\n";
    print $fmt_block."\n";
    print $fmt_bit."\n";
    print $data."\n";
    print $data_size."\n";
    
    if ( $fmt_ch != 2 ) {
	die "Not supported ! FMT_CH:$fmt_ch";
    }
    if ( $fmt_sample != 48000 ) {
	die "Not supported ! FMT_SAMPLE:$fmt_sample";
    }
    if ( $fmt_byte != 192000 ) {
	die "Not supported ! FMT_BYTE:$fmt_byte";
    }
    if ( $fmt_block != 4 ) {
	die "Not supported ! FMT_BLOCK:$fmt_block";
    }
    if ( $fmt_bit != 16 ) {
	die "Not supported ! FMT_BIT:$fmt_bit";
    }
    my $done = 0;
    my $avg=0;
    my $AVG_TERM=4800;
    my $avg_count=0;
    my $sec=0;
    my $zero_range=0;
    my @range_data=();
    my $fcount=0;
    while ( $done < $data_size ) {
	my $DATA;
	read($FILE,$DATA,4);
	$done += 4;
	my ($ch1,$ch2) = unpack("s s",$DATA);
	push(@range_data,$ch1);
	push(@range_data,$ch2);
	my $avg += ($ch1+$ch2)/2;
	$avg_count++;
	if ( $avg_count == $AVG_TERM ) {
	    if ( ! ( $avg > 50 or $avg < -50) ) {
		$avg = 0;
		$zero_range++;
		if ( $zero_range >= 6 ) {
		    if ( $zero_range == 6 ) {
			print "=== ".sprintf("- %s-%02d.wav",$number,$fcount)."=== \n";
			my $S = @range_data;
			$S *= 2;
			open(my $OUT,sprintf(">%s/%s-%02d.wav",$dname_dst,$number,$fcount));
			syswrite($OUT,$riff,4);
			syswrite($OUT,pack("L",$S+36),4); # riff size ( data size - 36 )
			syswrite($OUT,$wave,4);
			syswrite($OUT,$fmt,4);
			syswrite($OUT,pack("L",$fmt_size),4);
			syswrite($OUT,$fmt_data,$fmt_size);
			syswrite($OUT,$data,4);
			syswrite($OUT,pack("L",$S),4);  # data size
			foreach my $d (@range_data) {
			    syswrite($OUT,pack("s",$d),2);
			}
			close($OUT);
			$fcount++;
		    }
		    @range_data=();
		}
	    }else{
		$zero_range=0;
	    }
	    my $S = @range_data;
	    printf("%03.1f (%d) : %d (%d) \n",$sec,$zero_range,$avg,$S);
	    $sec += 0.1;
	    $avg_count=0;
	    $avg=0;
	}
    }

    close($FILE);
}

__END__
=head1 NAME

fncnv.pl - ???? 

=head1 SYNOPSIS

fncnv.pl  [options] [???]

=head1 OPTIONS

=over 18

=item -y

Auto 'y' answar.

=item -help

View help document.

=back

=head1 TODO

=over 18

=item 

=back

=cut
