filename = ARGV[0]

file = File.open(filename){|file| file.readlines}

re='([01]{8})'
binRegex = Regexp.new(re,Regexp::IGNORECASE);
fullRegex = Regexp.new('(0b[01]{8})')

file.each{|line|
  if binRegex.match(line)
      number = binRegex.match(line)[1].to_i(2).to_s(16)
      if number.length == 1
        number = "0" + number
      end
      line.sub!(/0b[01]{8}/, "0x#{number}")
      puts line
  end
}

File.open(ARGV[0] + ".out", "wb"){|newfile|
  newfile.puts file
}