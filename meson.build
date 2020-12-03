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
		   


