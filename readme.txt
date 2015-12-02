 CPSC 313 Assignment #5 
Guanda Li 
V7y9a 
This assignment finished part 1 and part 2;
And I also attempted to do part 3, but seems like recursive call in read_directory() is wrong. 
The time spent: 25 hours, most of my time spent on part 3 and it is definitely very challenging as described. In my part 3 directory_read() read root first and call print_directory(); After that return back to  directory_read(); then call FAT_read() to get a array of clusters. When FAT_read() finished, in directory_read() to read new set of directories from data recursively. However my input seems will not terminate properly. I spent 15 hours and still unable to resolve this issue. 
