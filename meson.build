project('advent2020', 'c',
		default_options : ['c_std=gnu11'])

xxhash = subproject('xxhash')
xxhash_dep = xxhash.get_variable('xxhash_dep')
htab = library('htab', 'htab.c',
          dependencies:[xxhash_dep])	

numbers = library('numbers', 'numbers.c')
passwords = library('passwords', 'passwords.c',
	    link_with:htab)

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required: false)
	  
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

executable('day11', ['day11.c', 'lines.c'],
		   		   link_with: [htab])	
executable('day12', ['day12.c', 'lines.c'],
		   		   link_with: [htab])	

executable('day13', ['day13.c', 'lines.c'],
		   		   link_with: [htab])	

executable('day14', ['day14.c', 'lines.c'],
		   		   link_with: [htab])	
executable('day15', ['day15.c', 'lines.c'],
		   		   link_with: [htab])	

executable('day16', ['day16.c', 'lines.c'],
		   		   link_with: [htab])	

executable('day17', ['day17.c', 'lines.c'],
		    link_with: [htab],
		    dependencies: [m_dep, xxhash_dep])	

 