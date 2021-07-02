To run the project use:
```./runJob.sh Distributed_Project```

if you are having problems with running the program, try changing number of clusters in 'runJob.sh', for example change '-np 10' to '-np 9' or lower. 

To compile a new executable run ```make``` in the 'SourceFiles' directory. 

There is a backup program in 'SourceFiles` (run with no issues on 2nd of July on hamarabi/stamp34) in case a newly compiled executable won't run. Please specify correct path when using the backup executable, i.e. ```./runJob.sh ./SourceFiles/Distributed_Project ```