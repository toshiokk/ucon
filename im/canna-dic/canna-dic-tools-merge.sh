dicfilei=canna-concat.dic
dicfileo=canna-sorted.dic
cat canna-dic/gcanna.ctd canna-dic/gcannaf.ctd canna-dic/gtankan.ctd | nkf -w >$dicfilei
./canna-dic-tools.rb merge <canna-concat.dic >$dicfileo
wc $dicfilei $dicfileo
wc -L $dicfilei $dicfileo
