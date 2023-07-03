#!/usr/bin/ruby

#require "jcode"
#$KCODE = "UTF8"

#------------------------------------------------------------------------------
def max_line_len
 line_num = 1
 line_num_max_len = 0
 max_words = 0
 max_len = 0
 while line = $stdin.gets()
  if line.bytesize > max_len then
   printf "%d: %s\n", line_num, line
   max_len = line.bytesize
   line_num_max_len = line_num
  end
  words = line.split(" ")
  if words.count() > max_words
   max_words = words.count()
  end
  line_num += 1
 end
 printf "line: %d, line length: %d, words: %d\n", line_num_max_len, max_len, max_words
end

#p ARGV
max_line_len
