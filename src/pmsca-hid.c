#include <stdio.h>
#include <string.h>

#include <hidapi/hidapi.h>

hid_device* open_hid(int flags, int argc, const char **argv){
	return hid_open(0x16c0, 0x27db, NULL);
}

int main(int argc, const char **argv){
	static char data[15];
	if(hid_init()){
		fprintf(stderr, "Unable to init HID system\n");
		return -1;
	}
	hid_device* dev=open_hid(0, argc, argv);
	if(!dev){
		fprintf(stderr, "Unable to open HID device\n");
		return -1;
	}
	int n=0;
	while(n<15){
		char buf[8];
		int r;
		if(r=hid_read_timeout(dev, buf, 16, 5000)<1){
			fprintf(stderr, "Unable to read from HID device\n");
			return -1;
		}
		printf("flags %x\n", buf[0]);
		for(char i=1;i<r;i++)
			printf("%02hhx ", buf[i]);
		printf("\n");
	}
	hid_close(dev);
	if(hid_exit()){
		fprintf(stderr, "Unable to close HID system\n");
		return -1;
	}
	
	printf("[pmsca] got %s\n", data);
	if(strncmp("cardid", data, 10))
		return 1;
	return 0;
}
