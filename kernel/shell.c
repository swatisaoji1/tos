#include <kernel.h>

static WINDOW shell_window = {0, 0,  50, 24, 0, 0, ' '};
static WINDOW pacman_window= {50, 8, 30, 16, 0, 0, ' '};
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
	fetch_argument(6);
	init_pacman(&pacman_window, 3);
	wprintf(&shell_window, "\n");
}

void fetch_word(char* word, int start){
	int i = 0;
	int j = 0;
	while(cmd[i] == ' '){
		i++;
	}
	while(cmd[i] != ' ' && i < CMD_SIZE-1 && cmd[i] != '\0'){
		word[j] = cmd[i];
		i++;
		j++;
	}
	word[j] = '\0';
}
void fetch_cmd(char* cmd_key){
	fetch_word(cmd_key, 0);
}


void fetch_argument(int cmd_size){
	int i = cmd_size - 1;
}

int str_len(char* word){
	int i = 0;
	int length = 0;
	while(word[i] != '\0'){
		length++;
		i++;
	}
	return length;
}

void executeShellCommand(){
	counter = 0; // reset counter
	char cmd_key[CMD_SIZE];
	
	
	fetch_cmd(cmd_key);

	//wprintf(&shell_window, "length: %d\n", str_len(cmd_key));
	if(match_command(cmd_key, "help")){
		help();
	}else if(match_command(cmd_key, "cls")){
		clrShellWin();
	}else if(match_command(cmd_key, "pacman")){		
		pacman();
	}else if(!str_len(cmd_key)){
		wprintf(&shell_window, "\n");
	}
	else{
		wprintf(&shell_window, "unsupported command: try \"help\"\n");
	}
	
	clearCommandBuffer();
}

void clearCommandBuffer(){
	int i;
	for(i=0; i < CMD_BUFFER; i++){
		cmd[i] = '\0';
	}
}


void tosShell(PROCESS self, PARAM param){
 	char ch;
 	Keyb_Message command;

	clrShellWin();
 	//print_all_processes(&shell_window);
 	//show_cursor(&shell_window);
 	//while(1){
 		//wprintf(&shell_window," tos-shell >>", ch);
 		//print_all_processes(&shell_window);
 		wprintf(&shell_window," tos-shell >>", ch);
 		while(1){

 			// get the character from keboard
 			command.key_buffer = &ch;
			send(keyb_port, &command);

			switch(ch){
				case '\b':
					if(counter > 0){
						counter-- ;
						wprintf(&shell_window, "%c", ch);
					}
					break;
				case '\n':
    			case 13:
					cmd[counter] = '\0';
					executeShellCommand();
					wprintf(&shell_window," tos-shell >>");
					break;
				default:
					// check buffer and print back the charater
					cmd[counter] = ch;
					if(counter < CMD_BUFFER-4){
						counter++;
						wprintf(&shell_window, "%c", ch);
					}
					break;
					
			}
 		}
 	//}	
}


void init_shell()
{
	create_process(tosShell,3,0,"Shell");
 	resign();
}
