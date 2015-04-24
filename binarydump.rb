#!/usr/bin/env ruby

puts ARGF.each_byte
         .map { |b| "%08b" % b }
         .each_slice(4)
         .map { |l| l.join("   ") }