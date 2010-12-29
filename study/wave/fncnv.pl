#!/usr/bin/env perl
#---------------------------------------------
# fncnv.pl - ????
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
opendir(my $DIR,$dname);
while (my $f = readdir($DIR)){
    if ( $f =~ /(\d+)\s\S+\s\d+\.wav/ ) {
	my $cmd = "mv \"$dname/$f\" $dname/0$1.wav";
	print $cmd."\n";
	system($cmd);
    }
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
