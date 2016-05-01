#include <kernel.h>

static WINDOW shell_window = {0, 0,  61, 24, 0, 0, 0xDC};
static WINDOW pacman_window= {61, 8, 19, 16, 0, 0, ' '};
#define CMD_BUFFER 80
#define CMD_SIZE 20

char cmd[CMD_BUFFER]; // stores the full command with arguments
int counter;

void welcome(){
	wprintf(&shell_window,"                        WELCOME TO TOS\n");
 	wprintf(&shell_window,"============================================================\n");
}


BOOL match_words(char* cmd1, char* cmd2){
	while (*cmd1 == *cmd2 && *cmd1 != '\0') {
        cmd1++;
        cmd2++;
    }
    return *cmd1 == '\0';
}

//commands 
void help(){
	char arg[CMD_SIZE - 4];
	fetch_argument(arg, 4);
	wprintf(&shell_window, "\n");
	if(str_len(arg)){
		if(match_words(arg, "help")){

			help_help();
		}else if(match_words(arg, "cls")){
			help_cls();
		}else if(match_words(arg, "pacman")){
			help_pacman();
		}else if(match_words(arg, "echo")){
			help_pacman();
		}else if(match_words(arg, "ps")){
			help_ps();
		}else if(match_words(arg, "about")){
			help_about();
		}else{
			wprintf(&shell_window, "%s : command not found.\n\n", arg);
		}
	}else{
		wprintf(&shell_window, "\n--------------------------TOS HELP------------------------\n");
		help_help();
		help_cls();
		help_pacman();
		help_echo();
		help_ps();
		help_about();
		wprintf(&shell_window, "============================================================\n");
	}

	
}

void help_help(){
	wprintf(&shell_window, "help [cmd]      - displays help information. if cmd specified help about cmd is displayed.\n\n");
}

void help_cls(){
	wprintf(&shell_window, "cls             - clears the screen.\n\n");
}

void help_pacman(){
	wprintf(&shell_window, "pacman [ghosts] - starts pacman with given ghost count.Uses default 3 ghosts if argument not provided.\n\n");
}

void help_echo(){
	wprintf(&shell_window, "echo [string]   - prints the string given as argument or a blank line if no argument is given.\n\n");
}

void help_ps(){
	wprintf(&shell_window, "ps              - prints all the running processes.\n\n");
}

void help_about(){
	wprintf(&shell_window, "about           - prints information about the shell author.\n\n");
}


void about(){
	wprintf(&shell_window, "\n");
	wprintf(&shell_window, "=========================================\n");
	wprintf(&shell_window, "TOS Shell \n");
	wprintf(&shell_window, "author     :  Swati Pradeep Patel\n");
	wprintf(&shell_window, "student id :  915583349\n");
	wprintf(&shell_window, "version    :  v1\n");
	wprintf(&shell_window, "=========================================\n");
}

void ps(){
	wprintf(&shell_window, "\n");
	print_all_processes(&shell_window);
}

void echo(){
	char* message = cmd + 4;
	wprintf(&shell_window, "\n");
	wprintf(&shell_window, message);
	wprintf(&shell_window, "\n");
}

void clrShellWin(){
	clear_window(&shell_window);
	welcome();
}

void pacman(){
	char arg[CMD_SIZE - 6];
	int ghosts = 3;

	// get arguments if passed
	fetch_argument(arg, 6);
	if(str_len(arg))
		ghosts =  arg[0]- '0';

	init_pacman(&pacman_window, ghosts);
	wprintf(&shell_window, "\n");
}



void fetch_word(char* word, int start){
	int i = start;
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


void fetch_argument(char* arg, int cmd_size){
	fetch_word(arg, cmd_size);
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
	if(match_words(cmd_key, "help")){
		help();
	}else if(match_words(cmd_key, "cls")){
		clrShellWin();
	}else if(match_words(cmd_key, "pacman")){		
		pacman();
	}else if(match_words(cmd_key, "ps")){		
		ps();
	}else if(match_words(cmd_key, "echo")){		
		echo();
	}else if(match_words(cmd_key, "about")){		
		about();
	}else if(!str_len(cmd_key)){
		wprintf(&shell_window, "\n");
	}
	else{
		wprintf(&shell_window, "\nunsupported command: try \"help\"\n\n");
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
 	wprintf(&shell_window,"tos-shell >>", ch);
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
				wprintf(&shell_window,"tos-shell >>");
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
}


void init_shell()
{
	create_process(tosShell,3,0,"Shell");
 	resign();
}
