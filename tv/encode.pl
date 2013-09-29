#!/usr/bin/env perl
use strict;
use Getopt::Long;
use File::Basename;

my $ENC_DIR = 'mp4';
my $DONE_DIR = 'done';
my $DIR = dirname($0);

my %options = (
               file    => undef,
               dir     => undef,
               dry     => undef,
               force   => undef,
               p       => undef,
               s       => undef,
               preset  => 1
               );
my %PRESET = (
 1 => 'lib264.ffpreset.1',
 2 => 'lib264.ffpreset.2'
);

# Option proc.
GetOptions(
    'f:s'         => \$options{file},
    'd:s'         => \$options{dir},
    'p:s'         => \$options{p},
    's:s'         => \$options{s},
    'q:s'         => \$options{preset},
    '-dry-run'    => \$options{dry},
    '-force'      => \$options{force},
    );
my $file = $options{file};
my $dir= $options{dir};
my $dry = $options{dry};
my $force = $options{force};
my $P = $options{p};
my $S = $options{s};
my $preset = $PRESET{$options{preset}};

my $cmd_fmt = "ffmpeg -i '%s' 
-y
-f mp4 
-vcodec libx264 -fpre $DIR/$preset
-r 30000/1001 
-aspect 16:9 
-s %s
-bufsize 20000k 
-vsync 1 
-acodec libfaac -ac 2 -ar 48000 -ab 128k 
%s
'%s'";

#my $encoder = "mencoder";
# my $option = "-lavcopts vcodec=libx264 -oac faac -ovc x264  -x264encopts   qp=25:crf=25:direct=auto:nombtree:nomixed_refs:ref=1:subq=2:trellis=0:weightp=0 -lavdopts fast -aspect 16:9 -noskip -vf scale=1280:768";
# my $option = "-lavcopts vcodec=libx264 -oac faac -ovc x264  -x264encopts  preset=ultrafast:bitrate=8000 -lavdopts fast -aspect 16:9 -noskip -vf scale=960:512";


sub encode_ts {
    my ($outdir,$input) = @_;
    my $base= basename($input,('.ts'));
    # Check done
    my $enc_dir = "$outdir/$ENC_DIR";
    my $target = "$enc_dir/$base.mp4";
    if ( -r $target && ! $force ) {
	my $done_dir = "$outdir/$DONE_DIR";
	my $done = "$done_dir/". basename($input);

	print "mv '$input' '$done'" . "\n";
	if ( ! $dry ) {
	    system "mkdir -p '$done_dir'";
	    system "mv '$input' '$done'";
	}
	return;
    }
    # Check size (now recordeing)
    my ($dev, $ino, $mode, $nlink, $uid, $gid, $rdev, $size, $atime, $mtime, $ctime, $blksize, $blocks) = stat($input);
    my $before = $size;
    sleep(2);
    ($dev, $ino, $mode, $nlink, $uid, $gid, $rdev, $size, $atime, $mtime, $ctime, $blksize, $blocks) = stat($input);
    my $after = $size;
    if ( $before != $after ) {
	return ;
    }
    my $str = `ffmpeg -i '$input' 2>&1`;

    my @lines = split(/\n/,$str);
    my %programs;
    my $curP = '-';
    foreach(@lines){
	if($_ =~ /^\s*  Program ([\d\.:]+)/){
	    $curP = $1;
	    $programs{$curP};
	} elsif($_ =~ /^\s*Stream \#([\d\.:]+)[^:]*:\s+Video:\s+(\S+)\s*\([^\)]+\)\s*\([^\)]+\)/ ){
	    $programs{$curP}{'v'}{$1} = "$2 $'";
	} elsif($_ =~ /^\s*Stream \#([\d\.:]+)[^:]*:\s+Audio:\s+(\S+)\s*\([^\)]+\)\s*/){
	    my $ch = $1;
	    my $type = $2;
	    my $tail = "$'";
	    if ( $tail =~ /\d+\s*Hz/ ) {
		$programs{$curP}{'a'}{$ch} = "$type $tail";
	    }
	}
    }
    print "== $input ==\n";
    if ( keys(%programs) == 1 && ! $P ){
	$P = 'auto';
    }
    foreach my $program (sort keys %programs) {
	if ( ! defined(%{$programs{$program}{'v'}}) || ! defined(%{$programs{$program}{'a'}}) ) {
	    delete($programs{$program});
	    next;
	}
	if ( $P eq 'auto' ) {
	    $P = $program;
	}
	if ( $program == $P ) {
	    print '**';
	}else{
	    print '  ';
	}
	print "[Program $program]\n";
	while( my ($ch,$remark) = each(%{$programs{$program}{'v'}})) {
	    print "   - $ch : $remark\n";
	}
	while( my ($ch,$remark) = each(%{$programs{$program}{'a'}})) {
	    print "   - $ch : $remark\n";
	}
    }
    if ( ! $P || $P eq 'auto') {
	print "Please select program : -p ??? or -p auto \n";
	return;
    }
    my $map ='';
    foreach my $ch (keys(%{$programs{$P}{'v'}})) {
	$map .= " -map $ch";
	if ( ! $S && $programs{$P}{'v'}{$ch} =~ / (\d\d\d+x\d\d\d+)/ ) {
	    $S = $1;
	}
	last;
    }
    foreach my $ch (keys(%{$programs{$P}{'a'}})) {
	$map .= " -map $ch";
    }
    $cmd_fmt =~ s/\r?\n/ /g;
    my $cmd = sprintf($cmd_fmt,$input,$S,$map,$target);

    print "nice "  . $cmd . "\n";
    if ( ! $dry ) {
	system "mkdir -p '$enc_dir'";
	if ( system "nice "  . $cmd ) {
	    return;
	}
    }

    my $done_dir = "$outdir/$DONE_DIR";
    my $done = "$done_dir/". basename($input);

    print "mv '$input' '$done'" . "\n";
    if ( ! $dry ) {
	system "mkdir -p '$done_dir'";
	system "mv '$input' '$done'";
    }
}

if ( $file ) {
    my $dir = dirname($file);
    encode_ts($dir,$file);
}elsif( $dir ) {
    opendir(my $dh, $dir) or die "$!:$dir";
    while (my $child = readdir($dh)) {
	my $file = "$dir/$child";
	if ( -f $file ){
	    encode_ts($dir,$file);
	}
    }
    closedir $dh;
}

__END__
