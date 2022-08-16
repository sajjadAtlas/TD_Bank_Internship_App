Small application developed for TD Risk Management Dept. during the summer 2022 co-op!

This Application was developed in order to make it a lot easier to find which users are in which access groups. Normally you'd have to manually parse several hundreds
of rows and remove many extra characters and details. This program does the dirty work for you! I used file handling, windows.h api to create an easier to use interface, and many datastructures and STL functions to make this work.

 For observers not part of TD, we essentially have AD groups which refers to different resources and who has access to said resources. The strings in the txt 
  files for those groups look something like this:
      
      CN=Atlas, Sajjad,OU=AAA,OU=BBB,DC=XXX,DC=YYY
      
  Now it would be a real pain to have to remove many characters for dozens of files across hundreds and even thousands of lines and then have to manually move it to another
  file, or update it yourself. That is basically why I made this application originally for personal use, but then added some extra features upon
  request from management.

Some details:

1. Has 2 modes: Aggregate and Group mode

  a) Aggregate mode essentially compiles a list of all names from N selection of files and parses the strings to remove extra details. 
     It will remove all duplicates on its own for your convenience!
     
  b) Group mode will group the names by the name of the resource group, and when the group has new members you can submit the file again and it will add only the
      new members (update basically). For example, 3 txt files with group names 1, 2 3. The program will make sure the names are are included before the members of each group,
      like so:
      
      GROUP1
        Sally
        Peter
        Bill
      Group2
        Melissa
        Albert
        Mohammad
      Group3
        Ali
        Henry
        Bob
        
  For Group mode you enter one file at a time, for Aggregate mode enter any amount at once (perferably <25 to prevent overflow :))
  
 




Known Issues:

1. Group mode duplicates some contents on work laptop, works fine on personal machine
