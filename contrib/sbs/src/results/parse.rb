#!/usr/bin/ruby

$filename = ARGV[0]
$events = []
$linesarray = {}
$r = 0.0
$w = 0.0

File.open($filename) do |file|
	lines = file.readlines()
	lines.each do |line|
		if line.include?("[time]")
			t = line.split(" ")[2].to_f
			$linesarray[t] = line
		end	
	end
	$linesarray = $linesarray.sort
	File.open($filename+".dat",'w+') do |output|
		$linesarray.each do |a|
			k = a[0]
			l = a[1]
			if l.include?("requesting")
				$w += 78.0
			elsif l.include?("read until")
				$r = l.split(" ")[11].to_f
			elsif l.include?("reached")
				$r = 0
				$w = 0
			end
			output << "#{k}\t#{$w}\t#{$r}\n"
		end
	end
end
