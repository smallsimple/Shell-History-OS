/* Bobo Shi */
/*
type 'gcc shell.c -o mysh' to compile
type './mysh' to run program.
Test cases are in readme.txt file.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */
#define HIS_SIZE 5

/**
 * setup() reads in the next command line, separating it into distinct tokens
 * using whitespace as delimiters. setup() sets the args parameter as a 
 * null-terminated string.
 */

void setup(char inputBuffer[], char *args[],int *background)
{
    int length, /* # of characters in the command line */
        i,      /* loop index for accessing inputBuffer array */
        start,  /* index where beginning of next command parameter is */
        ct;     /* index of where to place the next parameter into args[] */
    
    ct = 0;
    
    /* read what the user enters on the command line */
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE);  

    start = -1;
    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */
    if (length < 0){
        perror("error reading the command");
	exit(-1);           /* terminate with error code of -1 */
    }
    
    /* examine every character in the inputBuffer */
    for (i = 0; i < length; i++) { 
        switch (inputBuffer[i]){
        case ' ':
        case '\t' :               /* argument separators */
            if(start != -1){
                args[ct] = &inputBuffer[start];    /* set up pointer */
                ct++;
            }
            inputBuffer[i] = '\0'; /* add a null char; make a C string */
            start = -1;
            break;
            
        case '\n':                 /* should be the final char examined */
            if (start != -1){
                args[ct] = &inputBuffer[start];     
                ct++;
            }
            inputBuffer[i] = '\0';
            args[ct] = NULL; /* no more arguments to this command */
            break;

        case '&':
            *background = 1;
            inputBuffer[i] = '\0';
            break;
            
        default :             /* some other character */
            if (start == -1)
                start = i;
	} 
    }    
    args[ct] = NULL; /* just in case the input line was > 80 */
} 

/*void add_command_to_history( const char *command )
{

}*/

struct history{
  char buffer[MAX_LINE];
  char *args[MAX_LINE/2+1];
  int count;
  int background;
};

int main(void)
{
    char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
    int background;             /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE/2+1];/* command line (of 80) has max of 40 arguments */
    int i,j,rr_flag; //bobo add
    pid_t pid;
    int status;
    char *runargs[MAX_LINE/2+1];
    struct history his[HIS_SIZE];
    int command_count=0;
    char temp[MAX_LINE];
    char *t_args[MAX_LINE/2+1];
    int rr_num, rr_i;

    //printf("sizeof= %d", sizeof(his[0].args));
    for(i=0; i<HIS_SIZE; i++){
      for(j=0; j<MAX_LINE/2+1; j++){
	//	printf("%d %d\n", i, j);
	his[i].args[j]=NULL;
      }
    }

    while (1){            /* Program terminates normally inside setup */
	background = 0;
	printf("SystemsIIShell->");
        fflush(0);
        setup(inputBuffer, args, &background);       /* get next command */
	
	// detect if the command is 'history' or 'h'
	if (strcmp(args[0],"history")==0 || strcmp(args[0], "h")==0){
	  if (command_count<5)
	    for (i=HIS_SIZE-command_count; i<HIS_SIZE; i++){
	      
	      if ( his[i].count > 0 && his[i].buffer != NULL && his[i].args[0] != NULL){
		printf("%7d  ", his[i].count);
		j=0;
		while(his[i].args[j]){
		  if(his[i].args[j])
		    printf(" %s", his[i].args[j]);
		  j++;
		}
		printf("\n");
	      }
	    }
	  else{
	    for (i=0; i<HIS_SIZE; i++)
	      {
		printf("%7d  ", his[i].count);
		j=0;
		while(his[i].args[j]){
		  if(his[i].args[j]){
		    printf(" %s", his[i].args[j]);
		  }
		  j++;
		}
		printf("\n");
	      }
	  }
	}

	// if it is 'rr' or 'r num'
	else if (strcmp(args[0], "rr") == 0){
	  if (his[HIS_SIZE-1].count<1) {
	    printf("No recent command\n");
	    break;
	  }
	  else{
	    j=0;
	    while(his[HIS_SIZE-1].args[j]){
	      if(his[HIS_SIZE-1].args[j])
		runargs[j]=his[HIS_SIZE-1].args[j];
	      j++;
	    }
	    
	  }	  
	}

	else if (strcmp(args[0], "r")==0){
	  if(rr_num = atoi(args[1])){
	    rr_flag=0;
	    for (i=0; i<HIS_SIZE; i++){
	      if(his[i].count==rr_num){
		j=0;
		while(his[i].args[j]){
		  if(his[i].args[j])
		    runargs[j]=his[i].args[j];
		  j++;
		}
		rr_flag=1;
		break;
	      }
	    }
	    if (rr_flag==0)
	      printf("the num you indicate is not in history\n");
	  }
	  else {
	    printf("%s is not a num!\n", args[1]);
	  }
	  
	}
	else{
	  j=0;
	  while(args[j]){
	    if(args[j])
	      runargs[j]=args[j];
	    j++;
	  }
	}
	
	if(strcmp(runargs[0],"h") == 0 || strcmp(runargs[0],"history")==0){
	  if (command_count<5)
	    for (i=HIS_SIZE-command_count; i<HIS_SIZE; i++){	      
	      if ( his[i].count > 0 && his[i].buffer != NULL && his[i].args[0] != NULL){
		printf("%7d  ", his[i].count);
		j=0;
		while(his[i].args[j]){
		  if(his[i].args[j])
		    printf(" %s", his[i].args[j]);
		  j++;
		}
		printf("\n");
	      }
	    }
	  else{
	    for (i=0; i<HIS_SIZE; i++)
	      {
		printf("%7d  ", his[i].count);
		j=0;
		while(his[i].args[j]){
		  if(his[i].args[j]){
		    printf(" %s", his[i].args[j]);
		  }
		  j++;
		}
		printf("\n");
	      }
	  }
	  j=0;
	  while(args[j]){
	    if(args[j])
	      runargs[j]=args[j];
	    j++;
	  }

	} 


	else{
	  pid = fork(); /* fork a child */
	  
	  if (pid < 0){ /* error occurred */
	    fprintf(stderr, "Fork Failed\n");
	    return 1;
	  }
	  else if (pid == 0){ /* child process*/
	    
	    if (execvp (*runargs, runargs) < 0){ /* if wrong command */
	      printf("*** ERROR: exec failed\n");
	      exit(1);
	    }
	  }
	  else { /* parent process */
	    if (background==0) /* wait if '&' */
	      while (wait(&status) != pid);	    
	  }
	
	}
       // shift value in his one by one
	command_count++;
	for (i=0; i<HIS_SIZE-1; i++){
	  if(command_count+i > HIS_SIZE){
	    strcpy(his[i].buffer,"");	    
	    j=0;
	    while(his[i].args[j]){
	      if(his[i].args[j]){
		free(his[i].args[j]);
		his[i].args[j]=NULL;
	      }
	      j++;
	    }
	  }
	  if(command_count+i > HIS_SIZE-1){
	    j=0;
	    while(his[i+1].args[j]){
	      if(his[i+1].args[j]){
		strcpy(temp,his[i+1].args[j]);
		his[i].args[j] = malloc(sizeof(temp));
		strcpy(his[i].args[j],temp);
	      }
	      j++;
	    }
	    strcpy(his[i].buffer,his[i+1].buffer);
	    his[i].background=his[i+1].background;
	    his[i].count=his[i+1].count;
	  }
	}
    
	//store command to his[HIS_SIZE-1]	
	if(command_count>1){
	  i=0;
	  while(his[HIS_SIZE-1].args[i]){
	    //printf("free last: his[HIS_SIZE-1].args[%d]=%s\n",i,his[HIS_SIZE-1].args[i]);
	    if(his[HIS_SIZE-1].args[i]){
	      free(his[HIS_SIZE-1].args[i]);
	      his[HIS_SIZE-1].args[i]=NULL;
	    }
	    i++;
	  }
	  strcpy(his[HIS_SIZE-1].buffer,"");
	}
	
	i=0;
	while(args[i]){
	  if(args[i]){
	    strcpy(temp,args[i]);
	    his[HIS_SIZE-1].args[i]=malloc(sizeof(temp));
	    strcpy(his[HIS_SIZE-1].args[i],temp);
	  }
	  i++;
	}
	his[HIS_SIZE-1].count=command_count;
	his[HIS_SIZE-1].background=background;
    
	i=0;
	while(runargs[i]){
	    runargs[i]=NULL;
	  i++;
	}
    }
}
