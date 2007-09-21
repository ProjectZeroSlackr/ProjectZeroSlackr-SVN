#!/usr/bin/perl -w

# convert binary file to C char array

open BINFILE, $ARGV[0] or die "could not read {$ARGV[0]}\n";
binmode BINFILE;

$array_name = $ARGV[1];
$len = -s BINFILE;

print "static unsigned char ${array_name}[$len] = {\n";

do {
        read BINFILE, $chunk, 16;
        @chars = split //, $chunk;
        @bytes = map {sprintf("0x%02x", ord($_))} @chars;
        print "\t ", (join ',', @bytes), (eof(BINFILE) ? '' : ','), "\n";
} until eof BINFILE;

print "};\n";
