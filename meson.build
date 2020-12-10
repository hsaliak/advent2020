project('advent2020', 'c',
		default_options : ['c_std=gnu11'])

xxhash = subproject('xxhash')
xxhash_dep = xxhash.get_variable('xxhash_dep')
htab = library('htab', 'htab.c',
          dependencies:[xxhash_dep])	

numbers = library('numbers', 'numbers.c')
passwords = library('passwords', 'passwords.c',
	    link_with:htab)
	  
executable('day1', 'day1.c',
		   link_with: numbers)
executable('day2', 'day2.c',
		   link_with: passwords)	

executable('day3', 'day3.c',
		   link_with: htab)	

executable('day4', ['day4.c', 'lines.c'],
		   		   link_with: htab)				

executable('day5', ['day5.c', 'lines.c'],
		   		   link_with: [htab, numbers])				
executable('day6', ['day6.c', 'lines.c'],
		   		   link_with: [htab])				

executable('day7', ['day7.c', 'lines.c'],
		   dependencies : [xxhash_dep],
		    link_with: [htab])				

executable('day8', ['day8.c', 'lines.c'],
		   		   link_with: [htab])				
executable('day9', ['day9.c', 'lines.c'],
		   		   link_with: [htab, numbers])				
executable('day10', ['day10.c', 'lines.c'],
		   		   link_with: [htab, numbers])	


