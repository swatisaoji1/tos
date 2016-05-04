#include <kernel.h>

static WINDOW shell_window = {0, 0,  61, 24, 0, 0, 0xDC};
static WINDOW pacman_window= {61, 8, 19, 16, 0, 0, ' '};
#define CMD_BUFFER 80
#define CMD_SIZE 80

char cmd[CMD_BUFFER]; // stores the full command with arguments
int counter;
int pacman_running = 0;
int train_running = 0;


// helper functions 
/** Prints the welcome header of TOS shell */
void welcome(){
	wprintf(&shell_window,"                        WELCOME TO TOS\n");
 	wprintf(&shell_window,"============================================================\n");
}

/** 
* Compares the two words and returns true if  words match or false otherwise
* @param word1 - word to be compared
* @param word2 - word to compare with
*/
BOOL match_words(char* word1, char* word2){
	while (*word1 == *word2 && *word2 != '\0') {
        word1++;
        word2++;
    }
    return *word2 == '\0' && (*word1 == ' ' || *word1 == '\0') ;
}

/**
* copies one word from cmd (global) starting at index 'start' and stores in given parameter 'word'.
* @param word : stores the word.
* @param start : starting index to start looking for the word.
*/
int fetch_word(char* word, int start){
	int i = start;
	int j = 0;
	int first_letter = 0;
	while(cmd[i] == ' '){
		i++;
	}
	first_letter = i;
	while(cmd[i] != ' ' && cmd[i] != '\0'){
		word[j] = cmd[i];
		i++;
		j++;
	}
	word[j] = '\0';
	return first_letter;
}

/**
* gets the command word from global cmd into given parameter cmd_key
* @param cmd_key : stores the command
*/
int fetch_cmd(char* cmd_key){
	return fetch_word(cmd_key, 0);
}

/**
* gets one argument from global cmd into given parameter arg
* @param arg  - to store the arguments
* @param cmd_size - size of the command  
*/
void fetch_argument(char* arg, int cmd_size, int cmd_start){

	fetch_word(arg, cmd_size + cmd_start);
}


/** calculates the length of the given word */
int str_len(char* word){
	int i = 0;
	int length = 0;
	while(word[i] != '\0'){
		length++;
		i++;
	}
	return length;
}

/** prints help for cmd help */
void help_help(){
	wprintf(&shell_window, "help [cmd]      - displays help information. if cmd specified help about cmd is displayed.\n\n");
}

/** prints help for cmd cls */
void help_cls(){
	wprintf(&shell_window, "cls             - clears the screen.\n\n");
}

/** prints help for cmd pacman */
void help_pacman(){
	wprintf(&shell_window, "pacman [ghosts] - starts pacman with given ghost count.Uses default 3 ghosts if argument not provided.\n\n");
}

/** prints help for cmd echo */
void help_echo(){
	wprintf(&shell_window, "echo [string]   - prints the string given as argument or a blank line if no argument is given.\n\n");
}

/** prints help for cmd ps */
void help_ps(){
	wprintf(&shell_window, "ps              - prints all the running processes.\n\n");
}

/** prints help for cmd about */
void help_about(){
	wprintf(&shell_window, "about           - prints information about the shell author.\n\n");
}

/** prints help for cmd start_train */
void help_train_start(){
	wprintf(&shell_window, "start_train     - starts the train with default speed 5.\n\n");
}

/** prints help for cmd stop_train */
void help_train_stop(){
	wprintf(&shell_window, "stop_train      - stops the train .\n\n");
}


/** prints help */
void help(cmd_start){
	char arg[CMD_SIZE - 4];
	fetch_argument(arg, 4 ,cmd_start);
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
		}else if(match_words(arg, "start_train")){
			help_train_start();
		}else if(match_words(arg, "stop_train")){
			help_train_stop();
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
		help_train_start();
		help_train_stop();
		wprintf(&shell_window, "============================================================\n");
	}

	
}



/** prints about information of author */
void about(){
	wprintf(&shell_window, "\n");
	wprintf(&shell_window, "=========================================\n");
	wprintf(&shell_window, "TOS Shell \n");
	wprintf(&shell_window, "author     :  Swati Pradeep Patel\n");
	wprintf(&shell_window, "student id :  915583349\n");
	wprintf(&shell_window, "version    :  v1\n");
	wprintf(&shell_window, "=========================================\n");
}

/** prints all the processes running on TOS*/
void ps(){
	wprintf(&shell_window, "\n");
	print_all_processes(&shell_window);
}

/** prints the string passed as argument */
void echo(cmd_start){
	char* message = cmd + 4 + cmd_start;
	wprintf(&shell_window, "\n");
	wprintf(&shell_window, message);
	wprintf(&shell_window, "\n");
}

/** clears the shell window (except welcome header) */
void clrShellWin(){
	clear_window(&shell_window);
	welcome();
}

/** starts pacman */
void pacman(cmd_start){
	char arg[CMD_SIZE - 6];
	if(pacman_running){
		wprintf(&shell_window, "\nPacman already running !! \n");
		return;
	}
	int ghosts = 3;

	// get arguments if passed
	fetch_argument(arg, 6, cmd_start);
	if(str_len(arg))
		ghosts =  arg[0]- '0';
	if(ghosts < 0 || ghosts > 6){
		wprintf(&shell_window, "\n Enter ghost count between 1 - 6 \n");
		return;
	}
	
	init_pacman(&pacman_window, ghosts);
	pacman_running = 1;
	wprintf(&shell_window, "\n");
	
}

/** starts the train */
void train_start(){
	char* input_buffer;
	if (train_running){
		wprintf(&shell_window, "\n Train is  already Running !! \n");
		return;
	}
	//create the message 
	COM_Message msg;
	msg.input_buffer = input_buffer;
	msg.len_input_buffer = 0;
	msg.output_buffer ="L20S5\015";

	// send message to com port
	send(com_port,&msg);
	train_running = 1;
	wprintf(&shell_window, "\n Train is Running !! \n");
	
}

/** stops the train */
void train_stop(){
	char* input_buffer;
	if (!train_running){
		wprintf(&shell_window, "\n No train is running!! \n");
		return;
	}
	//create the message 
	COM_Message msg;
	msg.input_buffer = input_buffer;
	msg.len_input_buffer = 0;
	msg.output_buffer ="L20S0\015";

	// send message to com port
	send(com_port,&msg);
	train_running = 0;
	wprintf(&shell_window, "\n Train Stopped !!\n");
}

/** clears the command buffer  */
void clearCommandBuffer(){
	int i;
	for(i=0; i < CMD_BUFFER; i++){
		cmd[i] = '\0';
	}
}


/** starts the train */
void executeShellCommand(){
	counter = 0; // reset counter
	char cmd_key[CMD_SIZE];
	int cmd_start;

	cmd_start = fetch_cmd(cmd_key);

	if(match_words(cmd_key, "help")){
		help(cmd_start);
	}else if(match_words(cmd_key, "cls")){
		clrShellWin();
	}else if(match_words(cmd_key, "pacman")){		
		pacman(cmd_start);
	}else if(match_words(cmd_key, "ps")){		
		ps();
	}else if(match_words(cmd_key, "echo")){		
		echo(cmd_start);
	}else if(match_words(cmd_key, "about")){		
		about();
	}else if(match_words(cmd_key, "start_train")){		
		train_start();
	}else if(match_words(cmd_key, "stop_train")){		
		train_stop();
	}else if(!str_len(cmd_key)){
		wprintf(&shell_window, "\n");
	}
	else{
		wprintf(&shell_window, "\nunsupported command: try \"help\"\n\n");
	}
	clearCommandBuffer();
}


/**
* The Shell process - runs in while loop , fetches and executes  the commands.
*/
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

/** initialization */
void init_shell()
{
	create_process(tosShell,3,0,"Shell");
 	resign();
}
