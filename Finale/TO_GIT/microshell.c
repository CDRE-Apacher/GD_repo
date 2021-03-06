#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <dirent.h>
		
#define C_NRM  "\x1B[0m"
#define C_RED  "\x1B[31m"
#define C_GRN  "\x1B[32m"
#define C_YLW  "\x1B[33m"
#define C_BLU  "\x1B[34m"
#define C_MAG  "\x1B[35m"
#define C_CYA  "\x1B[36m"
#define C_WHT  "\x1B[37m"

	char cmd_line_input[256];
	char *p, *a;
	/* *p USEED AS STRTOK STORAGE*/
	/*  *a USED AS 'ACTION' COMMAND NAME*/
 	char current_path[1024];
	static char host[256];	
	static char username[64];
	int p_id;

/* ///////////// COPY THE USERNAME FROM ENVIRONMENTALS /////////////// */
void get_username(char * dest, int bufSize){
	strncpy(dest, getenv("USER"), bufSize);
}

void print_prompt(char * host, char * username, char * current_path){
	printf("\n%s%s%s@%s%s:%s$>%s", C_CYA, username, C_GRN, host, C_RED, current_path, C_NRM);
}

void print_help(){
	printf("%s*****************************************************\n",C_CYA);
	printf("MICROSHELL BY: MAREK GULAWSKI\n");
	printf("AVAILABLE COMMANDS:\n");
	printf("~help - prints this window.\n");
	printf("~cd <relative path> - changes the current directory using relative path. No argument takes the user to /home.\n");
	printf("~cp <source file> <destination file> - copies the contesnts of the source file to the destination file.\n");
	printf("~whoami - prints the currently logged in user.\n");
	printf("~touch <file name> - creates a file of the specified name.\n");
	printf("~remove <file name> - removes the specified file.");
    printf("~exit - exits the microshell");
	printf("OTHER COMMANDS ARE SUPPORTED BUT EXECUTED OUTSIDE OF THIS MICROSHELL\n");
	printf("*******************************************************\n%s", C_NRM);
}

void whoami(){
	printf("Logged in as:%s%s%s",C_CYA, getenv("USER"), C_NRM);
}

int main()
{
	while(1)	
	{
		getcwd(current_path, sizeof(current_path));
		gethostname(host, sizeof(host));
		get_username(username, 64);
        
        print_help();
		print_prompt(host, username, current_path);

		fgets(cmd_line_input, sizeof(cmd_line_input),stdin);	

		/* //////////////// EXIT /////////////////////////////////// */
		if(strcmp(cmd_line_input,"exit\n")==0){
			exit(EXIT_SUCCESS);	
		}
		/* ///////////////// END EXIT ///////////////////////// */

		/* /////////////// HELP //////////////////////////////// */
		else if(strcmp(cmd_line_input,"help\n")==0){
			print_help();
		}
		/* ////////////// END HELP ///////////////////////// */

		/* /////////////// TOKENIZE INPUT /////////////////////// */
		else{	
			cmd_line_input[strlen(cmd_line_input)-1]='\0';/*NULL BIT TERMINATION*/
			p=strtok(cmd_line_input, " ");/*TOKENIZE WITH SPACES*/
			int i=0;
			char *args[50];
			while(p!=NULL){
				args[i]=p;
				i++;
				p=strtok(NULL, " ");
			}
			a=args[0];
			args[i]=NULL;
		/* //////////////// END TOKENIZE INPUT ///////////////////////// */

		/* ////////////////////// CP ////////////////////////// */
			if(strcmp(a,"cp")==0){
				FILE *f_input, *f_output;
				if(args[1]==NULL){
					printf("%sError:%s No source file specified!", C_RED, C_NRM);
				}
				else{
					if(access(args[1], F_OK) != -1){/*CHECK IF FILE EXISTS*/
					/* ////////////////////// FILE EXISTS ////////////////////////// */
 						f_input = fopen(args[1], "r");/*R READ MODE*/
						if(args[2]==NULL)
						{
							printf("%sError:%s No destination file specified!", C_RED, C_NRM);
						}	
						else{
 							f_output = fopen(args[2], "w");/*W WRITE MODE*/
 							char temp;
					 		while ((temp = fgetc(f_input)) != EOF){
      							fputc(temp, f_output);/*COPY BY CHARACTER*/
      						}
      						fclose(f_input);
   							fclose(f_output);
 							}
 					}
 					/* ////////////////////// END FILE EXISTS ////////////////////////////// */

 					/* ////////////////////// SOURCE FILE DOES NOT EXIST /////////////////////// */
 					else{
 						printf("%sError:%s Source file does not exist!", C_RED, C_NRM);
						}
					/* ////////////////////// END SOURCE FILE DOES NOT EXIST /////////////////////// */
 				}
			}
		/* ///////////////////// END CP /////////////////////////// */

		/* ////////////////////// CD //////////////////////////////// */
			else if(strcmp(a,"cd")==0){
				if(args[1]==NULL) chdir("/home");
				else{
					DIR * directory = opendir(args[1]);
					if(directory){
						chdir(args[1]);
						closedir(directory);
					}
					else{
						printf("%sError:%s Specified directory does nor exist!", C_RED, C_NRM);
					}
				}
			}
		/* ////////////////////// END CD ////////////////////////// */

		/* ///////////////////// WHO AM I //////////////////////// */
			else if(strcmp(a,"whoami")==0){
				whoami();
			}
		/* /////////////////////// END WHO AM I ////////////////////////////// */

		/* /////////////////////// TOUCH //////////////////////////////////// */
			else if(strcmp(a,"touch")==0){
				if(args[1]==NULL) printf("%sError:%s File name not specified!", C_RED, C_NRM);
				else{
					if(access(args[1],F_OK) != -1){/*FILE EXISTS*/
						printf("%sError:%s File already exists!", C_RED, C_NRM);
					}else{/*FILE DOES NOT EXIST*/
						FILE * to_create;
						to_create = fopen(args[1],"w");
						fclose(to_create);
						printf("File created successfully!");
					}
				}
			}
		/* ////////////////////// END TOUCH //////////////////////////// */

		/* ////////////////////// REMOVE /////////////////////////// */
			else if(strcmp(a,"remove")==0){
				if(args[1]==NULL) printf("%sError:%s File name not specified!", C_RED, C_NRM);
				else{
					if(access(args[1],F_OK) != -1){/*FILE EXISTS*/
						int rem = remove(args[1]);
						if(rem == 0) printf("File deleted successfully!");
						else printf("%sError:%s Unable to delete file!", C_RED, C_NRM);
					}else{/*FILE DOES NOT EXIST*/
						printf("%sErros:%s Specified file already does not exist!", C_RED, C_NRM);
					}
				}
			}
		/* ///////////////////// END REMOVE //////////////////// */

			else{/* INCOMPASES ALL NON CUSTOM FUNCTIONS USING BUILT IN SYSTEM SHELL */
				p_id = fork();
				if(p_id>0) wait(NULL);
				else if(p_id==0){
					execvp(a, args);
				}else if(p_id<0){
					printf("%sErros:%s Fork() operation failed!", C_RED, C_NRM);
				}
			}
		}
	}				
return EXIT_SUCCESS;
}
/* /////////////// END SHELL_LOOP HERE /////////////////////////// */
