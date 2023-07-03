#!/usr/bin/ruby
#
# list_up_all_hinshis ---------------------------------------------------------
# List up all hinshi code of canna-dic.
#
# replece_word_type -----------------------------------------------------------
# Replace "ヴ" with "う゛" in YOMI.
# Replace canna-word-type "#KJ" with "#zz".
#  そうしないと単漢字変換の候補が前に来てしまう。
# [before]
#  えき #KJ*13 易 液 益 駅 役 #KJ*11 疫 #KJ*7 亦 #KJ*5 奕 懌 掖 繹 腋 蜴 驛 鯣
#  ヴぁいおりん #T35*250 ヴァイオリン
# [after]
#  えき #zz*13 易 液 益 駅 役 #zz*11 疫 #zz*7 亦 #zz*5 奕 懌 掖 繹 腋 蜴 驛 鯣
#  う゛ぁいおりん ヴァイオリン
#
# merge_lines_of_the_same_yomi ------------------------------------------------
# [before]
#  えき #CNSUC1*20 駅 #JS*20 駅
#  えき #KJ*13 易 液 益 駅 役 #KJ*11 疫 #KJ*7 亦 #KJ*5 奕 懌 掖 繹 腋 蜴 驛 鯣
# [after]
#  えき #CNSUC1*20 駅 #JS*20 駅 #zz*13 易 液 益 駅 役 #zz*11 疫 #zz*7 亦 ...
# -----------------------------------------------------------------------------
#
require "jcode"
$KCODE = "UTF8"

##dicfile = "canna-sorted.dic"
##p "ヴぁいおりん".sub('ヴ', 'う゛')
##p "#G5"["#G5\|#K5\|#S5\|#T5\|#N5\|#B5\|#M5\|#R5\|#W5"]
##p [ "abc", "def" ].include?("def")
##p [ "abc", "def" ].include?("dee")
##exit

#------------------------------------------------------------------------------
def list_up_all_hinshis
 hinshis = Array.new
 words_per_hinshi = Array.new
 doushis = 0
 tangos = 0
 max_candidates = 0
 while line = $stdin.gets()
  line = line.sub(/ヴ/, 'う゛')
  fields = line.split(" ")
  if fields.size > max_candidates then
   max_candidates = fields.size
   max_candidates_line = line
  end
#      if fields[0].jlength == 1
#       print "One-char-YOMI: " + line.to_s
   hinshi = ""
   fields.each do |field|
    if field =~ /(^#[^\*]+)/
     hinshi = $1
#print "hinshi: " + hinshi + "\n"
     unless hinshis.include?(hinshi)
#print "New hinshi: " + hinshi + "\n"
      hinshis << hinshi
      words_per_hinshi << 0
     end
    else
     if hinshi != ""
#print "word: " + field + "(" + hinshi + ")\n"
      words_per_hinshi[hinshis.index(hinshi)] += 1
      tangos += 1
     end
    end
   end
#      end
 end
 hinshis.each do |hinshi|
   if hinshi =~ /#.5/ || hinshi =~ /(KS|SX|ZX)/
    words = words_per_hinshi[hinshis.index(hinshi)]
    doushis += words
    print "Hinshi list: " + hinshi.to_s + " : " + words.to_s + "\n"
   end
 end
 hinshis.each do |hinshi|
   unless hinshi =~ /#.5/ || hinshi =~ /(KS|SX|ZX)/
    words = words_per_hinshi[hinshis.index(hinshi)]
    print "Hinshi list: " + hinshi.to_s + " : " + words.to_s + "\n"
   end
 end
 print hinshis.size.to_s + " HINSHI types\n"
 print doushis.to_s + " DOUSHI\n"
 print tangos.to_s + " TANGO\n"
 print max_candidates.to_s + " TANGO per line\n"
 print "[" + max_candidates_line.chomp.to_s + "]\n"
end

#------------------------------------------------------------------------------
def merge_lines_of_the_same_yomi
 $stderr.print "Reading DIC file and Converting s/ヴ/う゛/ s/#KJ/#zz/ ...\n"
 lines = Array.new
 while line = $stdin.gets()
  line.gsub!(/#KJ/, "#zz")
  fields = line.split(" ")
  fields[0].gsub!(/ヴ/, "う゛")
  lines << fields.join(" ")
 end
 $stderr.print "Sorting DIC ...\n"
 lines.sort!
 $stderr.print "Merging lines of the same YOMI ...\n"
 prev_line = ""
 lines_merged = Array.new
 lines.each { |line|
  if prev_line.split(" ")[0] == line.split(" ")[0] # compare YOMI
   # Merge two lines of the same YOMI into one.
   prev_line = prev_line.chomp + " " + line.split(" ")[1..-1].join(" ")
  else
   if prev_line != ""
    lines_merged << prev_line
   end
   prev_line = line
  end
 }
 lines_merged << prev_line
 $stderr.print "Converting s/#zz/#KJ/ and Writing DIC file ...\n"
 lines_merged.each { |line|
  line.gsub!(/#zz/, "#KJ")
  $stdout.puts(line)
 }
 $stderr.print "Complete.\n"
end

#p ARGV
case ARGV[0]
when "list" then list_up_all_hinshis	# list up all HINSHI and num of words
when "merge" then merge_lines_of_the_same_yomi
else print "???? Undefined option [#{ARGV[0]}] ????"
end
