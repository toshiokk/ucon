# convert SKK-JISYO.zipcode(S-JIS) to zipcode.dic(UTF-8)
outfile=zipcode.dic
if [ -f $outfile ]; then
  echo "File $outfile already exists."
  exit
fi
nkf -w SKK-JISYO.zipcode | grep -v ";;" | sed 's/\// /g' | sed 's/  / /g' | sed 's/ $//g' >$outfile
