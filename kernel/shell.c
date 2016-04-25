#include <kernel.h>

static WINDOW shell_window = {0, 11,  80, 14, 0, 0, ' '};
static WINDOW pacman_window= {0, 0, 80, 10, 0, 0, ' '};
#define CMD_BUFFER 80
#define CMD_SIZE 20

char cmd[CMD_BUFFER]; // stores the full command with arguments
int counter;

void welcome(){
	wprintf(&shell_window,"               WELCOME TO TOS\n");
 	wprintf(&shell_window,"============================================\n");
}


BOOL match_command(char* cmd1, char* cmd2){
	while (*cmd1 == *cmd2 && *cmd2 != '\0') {
        cmd1++;
        cmd2++;
    }
    return *cmd2 == '\0';
}

//commands 
void help(){
	wprintf(&shell_window, "\n-------------------TOS HELP----------------\n");
	wprintf(&shell_window, "help             - displays help information.\n");
	wprintf(&shell_window, "cls              - clears the screen.\n");
	wprintf(&shell_window, "pacman {ghosts}  - starts pacman with given ghost count.\n");
	wprintf(&shell_window, "===========================================\n");
}

void echo(char* message){
	wprintf(&shell_window, message);
}

void clrShellWin(){
	clear_window(&shell_window);
	welcome();
}

void pacman(){
	init_pacman(&pacman_window, 3);
}

void fetch_cmd(char* cmd_key){
	int i = 0;
	int j = 0;
	while(cmd[i] == ' '){
		i++;
	}
	while(cmd[i] != '\0' && i < CMD_SIZE){
		cmd_key[j] = cmd[i];
		i++;
		j++;
	}
	cmd_key[j] = '\0';
}

void fetch_argument(int cmd_size){

}

void executeShellCommand(){
	counter = 0; // reset counter
	char cmd_key[CMD_SIZE];
	fetch_cmd(cmd_key);

	if(match_command(cmd_key, "help")){
		help();
	}else if(match_command(cmd_key, "cls")){
		clrShellWin();
	}else if(match_command(cmd_key, "pacman")){		
		pacman();
	}else if(match_command(cmd_key, "")){		
		wprintf(&shell_window, "\n");
	}
	else{
		wprintf(&shell_window, "unsupported command: try \"help\":\n");
	}
	
}




void tosShell(PROCESS self, PARAM param){
 	char ch;
 	Keyb_Message command;

	clrShellWin();
	
 	

 	//print_all_processes(&shell_window);
 	//show_cursor(&shell_window);
 	while(1){
 		wprintf(&shell_window," tos-shell >> ");
 		//print_all_processes(&shell_window);
 		while(1){

 			// get the character from keboard
 			command.key_buffer = &ch;
			send(keyb_port, &command);
			switch(ch){
				case '\b':
					if(counter > 0) 
						counter-- ;
					break;
				case '\n':
    			case 13:
					cmd[counter] = '\0';
					executeShellCommand();
					wprintf(&shell_window," tos-shell >> ");
					break;
				default:
					// check buffer and print back the charater
					if(counter < CMD_BUFFER){
						cmd[counter] = ch;
						counter++;
					}
					wprintf(&shell_window, "%c", ch);
			}
 		}
 	}	
}


void init_shell()
{
	create_process(tosShell,3,0,"Shell");
 	resign();
}
