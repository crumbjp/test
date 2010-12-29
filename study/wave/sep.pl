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

sub fproc {
    my ($fname,$number)=@_;
    open(my $FILE,"<$fname");
    binmode $FILE;
    # header
    my $riff="";
    my $riff_size=0;
    my $wave="";
    my $fmt="";
    my $fmt_size=0;
    my $fmt_data="";
    my $data="";
    my $data_size=0;
    read($FILE,$riff,4);
    read($FILE,$riff_size,4);
    $riff_size = unpack("L",$riff_size);
    read($FILE,$wave,4);
    read($FILE,$fmt,4);
    read($FILE,$fmt_size,4);
    $fmt_size = unpack("L",$fmt_size);
    read($FILE,$fmt_data,$fmt_size);
    read($FILE,$data,4);
    read($FILE,$data_size,4);
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
