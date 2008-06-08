filename = ARGV[0]

file = File.open(filename){|file| file.readlines}

re1='.*?'	# Non-greedy match on filler
re2='(?:[a-z][a-z]*[0-9]+[a-z]*)'	# Uninteresting: alphanum
re3='.*?'	# Non-greedy match on filler
re4='((?:[a-z][a-z]*[0-9]+[a-z]*))'	# Alphanum 1

re=(re1+re2+re3+re4)
m=Regexp.new(re,Regexp::IGNORECASE);

file.each{|line|
  if m.match(line)
      alphanum1=m.match(line)[1];
      puts "("<<alphanum1<<")"<< "\n"
  end
}