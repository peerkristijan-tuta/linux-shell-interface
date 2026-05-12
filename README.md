This program has several flags. & carries out command concurrently. > imports output of command on the left to file on the right. < exports content of file of the right to command on the left. | takes output of left command to be input of right command. Command parameters are currently not supported for file IO commands or piped commands. In addition to that, Linux terminal command flags like -la are also unsupported. 

Example inputs:
- sleep 3
- sleep 3 &
- ls
- ls &
- ls > out.txt
- ls > out.txt
- cat < out.txt
- cat < out.txt &
- ls | grep .txt
- ls | grep .txt &

Behavior of commands:
- sleep causes program process to sleep
- ls causes the name of all files in program directory to be printed either on the terminal or pasted on a file
- cat prints out content of input. (in the case of example above, it prints the contents of out.txt)
- grep filters for specific file types (in the case of example above, it allows only those file names from ls that end in .txt to be printed)
