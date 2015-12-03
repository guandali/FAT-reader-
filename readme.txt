 CPSC 313 Assignment #5 
Guanda Li 
V7y9a 
The output of part1  and part2 is ok. part 3 could print all files recursively but miss 
complete directories. I tried pass a buffer and it didt work. 
The time spent: 25 hours, most of my time spent on part 3 and it is definitely very challenging as described. In my part 3 directory_read() read root first and call print_directory(); After that return back to  directory_read(); then call FAT_read() to get a array of clusters. When FAT_read() finished, in directory_read() to read new set of directories from data recursively. However my input seems will not handle subdirectory properly.(it won’t take previous directory) I spent 15 hours and still unable to resolve this issue.

 
One thing should note: when I run it on department machine(dead.ugrad.cs.ubc.ca) it could run properly, however when I run it on my macbook it terminated with “Abort trap: 6” . 

Thanks. 

