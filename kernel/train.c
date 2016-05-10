
#include <kernel.h>
#define TRAIN_NO "20\0"
#define LEN 3
#define SLEEP_TICKS 15
WINDOW* train_window;

COM_Message msg ;

/**
* concatenates str2 to str1 and returns pointer to str1
*/

void str_concat(char* str1, char* str2){
	int i = 0;
	int j = 0;
	while(str1[i] != '\0') i++ ;
	while(str2[j] != '\0'){
		 str1[i] = str2[j];
		 i++;
		 j++;
	}
	str1[i] = '\0';
}


int send_cmd(char* cmd){
	char* i_buffer;

	//create the message 
	COM_Message msg;
	msg.input_buffer = i_buffer;
	msg.len_input_buffer = 0;
	msg.output_buffer =cmd;

	//send the msg
	sleep(SLEEP_TICKS);
	send(com_port,&msg);
	msg.output_buffer = '\0' ;
}



int send_cmd_for_answer(char* cmd){
	char* i_buffer;
	//create the message 
	COM_Message msg;
	msg.input_buffer = i_buffer;
	msg.len_input_buffer = 3;
	msg.output_buffer =cmd;

	//send the msg
	sleep(SLEEP_TICKS);
	send(com_port,&msg);

	if(msg.input_buffer[1]=='1') return 1;
   		else return 0;
}

/** Change the speed of the train : L#S#{CR} */
void change_speed(int speed){
	char sp[2];
	char cmd[10];

	// get the speed
	sp[0] = '0' + speed;
	sp[1] = '\0';

	// make the command
	cmd[0] = 'L';
	cmd[1] = '\0';

	str_concat(cmd, TRAIN_NO);
	str_concat(cmd, "S\0");
	str_concat(cmd, &sp);
	str_concat(cmd, "\015");

	//send the command
	send_cmd(cmd) ;
}

void change_direction(){
	char cmd[10];
	cmd[0] = 'L';
	cmd[1] = '\0';
	str_concat(cmd, TRAIN_NO);
	str_concat(cmd, "D\015\0");
	send_cmd(cmd) ;

}

void change_switch(int switch_no, char color){
	char cmd[5];
	cmd[0] = 'M';
	cmd[1] = '0' + switch_no;
	cmd[2] = color;
	cmd[3] = '\015';
	send_cmd(cmd) ;
}



/** Get the status of a contact.  "*1\015" is returned if there is a vehicle on the contact. */
int check_train_on_track(char* track_contact){
	char cmd[5];
	send_cmd("R\015");
	cmd[0] = 'C';
	cmd[1] = '\0';
	str_concat(cmd, track_contact );
	str_concat(cmd, "\015\0" );
	//cmd[2] = '\015';	
	//wprintf(train_window,"c command %s\n", &cmd);
	return send_cmd_for_answer(cmd);

}

int check_configutation(){
	int config;
	if(check_train_on_track("8") && check_train_on_track("2")){
		config= 1;
	}else if(check_train_on_track("11") && check_train_on_track("5")){
		config= 3;
	}else if(check_train_on_track("16")&& check_train_on_track("5")){
		config= 4;
	}else {
		config=0;
	}
	return config;

}

void init_safe_switches(){
	change_switch(5, 'G');
	change_switch(9, 'R');
	change_switch(8, 'G');
	change_switch(1, 'G');
	change_switch(4, 'G');
}
int check_zamboni(){
	int i;
	wprintf(train_window,"Scanning for zambini \n");
	for(i = 0 ;i < 30 ; i++){
		wprintf(train_window,"zamboni check %d\n", i);
		if(check_train_on_track("10"))
			return 1;
	}
	return 0;

}
void solve_config1(){
	if(check_zamboni()){
		wprintf(train_window,"Zamboni detected\n");
		wprintf(train_window,"Trapping zamboni in inner loop\n");
		change_switch(8, 'R');
		change_switch(1, 'R');
		change_switch(2, 'R');
		change_switch(7, 'R');

		wprintf(train_window,"Waiting...\n");
		// wait for zamboni to reach 12
		while(!check_train_on_track("12"));
		wprintf(train_window,"Zamboni Trapped...\n");

	}else{
		wprintf(train_window,"No zamboni detected\n");
	}
	wprintf(train_window,"Retrieving Wagon...\n");
	// retriving wagon
	change_switch(4, 'R');
	change_switch(3, 'G');
	change_speed(5);
	while(!check_train_on_track("1"));

	change_speed(0);
	wprintf(train_window,"Got Wagon...\n");
	change_direction();
	change_switch(5, 'R');
	change_switch(6, 'R');
	change_speed(5);
	while(!check_train_on_track("8"));
	change_speed(0);
	wprintf(train_window,"Wagon and Train Safe home :) \n");

	

}
void solve_config3(){
	int zamboni = check_zamboni();
	if(zamboni){
		wprintf(train_window,"Zamboni Detected..\n");
	}else{
		wprintf(train_window,"NO Zamboni Detected..\n");
	}


	if(zamboni){
		// wait for zamboni to cross swich 5
		wprintf(train_window,"Waiting for zamboni to cross..\n");
		while(!check_train_on_track("7"));
	}

	// start the train
	change_speed(5);
	wprintf(train_window,"Retreiving wagon..\n");

	if(zamboni){
		// check for zamboni before changing swicth for train
		while(!check_train_on_track("10"));
	}
	

	change_switch(5, 'R');
	change_switch(6, 'G');

	//change_speed(5);

	// wait for train to reach 9
	while(!check_train_on_track("9"));
	change_switch(5, 'G'); // for zamboni

	while(!check_train_on_track("12"));

	change_speed(0);
	change_direction();
	change_switch(7, 'R');
	change_switch(8, 'R');
	change_speed(5);
	while(!check_train_on_track("13"));

	change_speed(0);
	change_direction();
	change_speed(5);

	while(!check_train_on_track("12"));
	change_speed(0);
	change_direction();

	// wait for zamboni to cross
	if(zamboni){
		wprintf(train_window,"Waiting for zamboni to cross..\n");
		while(!check_train_on_track("10"));
	}

	change_switch(7, 'G');
	change_speed(5);
	change_switch(4, 'R');
	change_switch(3, 'R');

	while(!check_train_on_track("5"));
	change_speed(0);
	wprintf(train_window,"Wagon and Train Safe home :) \n");

	init_safe_switches();
	// start train


}

void solve_config4(){
	int check = 0;
	int zamboni = check_zamboni();
	// wait for zamboni
	if(zamboni){
		wprintf(train_window,"Waiting for zamboni to pass \n");
		while(!check_train_on_track("4"));
		wprintf(train_window,"Zamboni passed.. retrieving wagon \n");
	}

	change_speed(5);


	while(!check_train_on_track("6"));
	change_speed(0);
	change_direction();
	change_switch(4, 'R');
	change_switch(3, 'G');
	change_speed(5);

	if(zamboni){
		while(!check_train_on_track("2"));
		change_switch(4, 'G');
		change_speed(0);
		wprintf(train_window,"Playing too safe .. \n");
		wprintf(train_window,"Waiting for zamboni to pass \n");
		while(!check_train_on_track("14"));
		wprintf(train_window,"Zamboni passed..Getting Wagon\n");
		change_speed(5);
		// zamboni passing 13 so we dont land up confusing zambini for train
		while(!check_train_on_track("13"));
	}
		
	while(!check_train_on_track("14"));

	change_speed(0);
	change_direction();
	change_switch(9, 'G');
	change_speed(5);

	// poll 3 times, so that there is enough time for train to attach to wagon
	// this is possible run out of track --> no solution
	while(check < 3){
		check += check_train_on_track("16");
	}
	change_speed(0);
	change_direction();

	// create a gap between zamboni and train
	if(zamboni){
		wprintf(train_window,"Wait for zamboni to go ahead\n");
		while(!check_train_on_track("10"));
	}
	
	wprintf(train_window,"going home ..\n");
	change_speed(5);

	// let zambini reach 4 then change switch for train
	if(zamboni){
		while(!check_train_on_track("4"));
	}
	change_switch(4, 'R');
	change_switch(3, 'R');

	while(!check_train_on_track("5"));
	wprintf(train_window,"Wagon and Train Safe home :) \n");
	change_speed(0);
	init_safe_switches();


}

void train_process(PROCESS self, PARAM param)
{

	int config;
	clear_window(train_window);
	send_cmd("R\015");

	wprintf(train_window,"Starting Train App\n");
	wprintf(train_window,"Initializing safe switches\n");
	init_safe_switches();

	//wprintf(train_window,"traing on %d \n", check_train_on_track(8));
	
	//wprintf(train_window,"result %s\n", str_concat("test1\0", "test2\0"));
	//wprintf(train_window,"result %s\n", get_change_speed_cmd(4));
	


	wprintf(train_window,"Checking Configuration\n");
	config = check_configutation();

	switch(config){
		case 1: 
			wprintf(train_window,"Found- Config 1 or Config 2\n");
			solve_config1();
			break;
		case 3: 
			wprintf(train_window,"Found- Config 3\n");
			solve_config3();
			break;
		case 4: 
			wprintf(train_window,"Found- Config 4\n");
			solve_config4();
			break;
		default:
			wprintf(train_window,"Something wrong\n");
			break;
	}
	//change_speed(4);
	
	while(1){
		sleep(SLEEP_TICKS);
	};
}



void init_train(WINDOW* wnd)
{	
	train_window = wnd;
	create_process(train_process, 4, 0, "Train Process");
	return;
}
