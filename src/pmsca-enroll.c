#define _XOPEN_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/random.h>

int main(int argc, const char **argv){
	char data[16], valid=0;
	size_t dataLen=0;
	
	FILE* db=fopen("/etc/security/pmsca.db", "a");
	if(!db){
		printf("Failed to open database!\n");
		return -1;
	}
	
	int fl = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, fl | O_NONBLOCK);
	
	struct termios oldt, newt;
	tcgetattr(STDIN_FILENO, &oldt);
	newt=oldt;
	newt.c_lflag&=~ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	
	printf("Please swipe your card...  ");
	for(int i=5;i>0;i--){
		printf("\x1b[D%d", i);
		fflush(stdout);
		int r=read(0, &data[dataLen], 15-dataLen);
		if(r>0){
			dataLen+=r;
			if(data[dataLen-1]=='\n'){
				valid=1;
				break;
			}
		}
		sleep(1);
	}
	printf("\n");
	
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, fl);
	
	if(!valid){
		printf("Timeout\n");
		return 1;
	}
	
	if(data[dataLen-1]=='\n')
		data[dataLen-1]=0;
	else
		data[dataLen]=0;
	
	char* user=getlogin();
	char bSalt[4];
	getrandom(bSalt, 4, 0);
	char sSalt[]="$5$--------";
	sprintf(&sSalt[3], "%02hhx%02hhx%02hhx%02hhx", bSalt[0], bSalt[1], bSalt[2], bSalt[3]);
	char* hash=crypt(data, sSalt);
	if(!hash){
		perror("Hash is NULL");
		return 10;
	}
	fprintf(db, "%s:%s\n", user, hash);
	fclose(db);
	
	printf("Added card to %s\n", user);
	return 0;
}
