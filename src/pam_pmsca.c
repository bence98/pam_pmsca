#define PAM_SM_AUTH
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>

#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <security/pam_ext.h>

int pmsca_auth(const char* user, const char* pass, const char* ln){
	size_t lenUser=strlen(user);
	if(ln[lenUser]!=':'||memcmp(user, ln, lenUser))
		return -1;
	char salt[11];
	memcpy(salt, &ln[lenUser+1], 11);
	if(strncmp(&ln[lenUser+1], crypt(pass, salt), 55))
		return 1;
	return 0;
}

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv){
	const char* user;
	if(pam_get_user(pamh, &user, NULL)!=PAM_SUCCESS)
		return PAM_AUTH_ERR;
	FILE* db=fopen("/etc/security/pmsca.db", "r");
	if(!db){
		pam_info(pamh, "Card database empty!");
		return PAM_AUTH_ERR;
	}
	
	char data[16];
	const struct pam_conv *conv;
	if(pam_get_item(pamh, PAM_CONV, (const void**)&conv)!=PAM_SUCCESS)
		return PAM_AUTH_ERR;
	struct pam_message* msg=malloc(sizeof(struct pam_message));
	msg->msg_style=PAM_PROMPT_ECHO_OFF;
	msg->msg="Please swipe your card or press ENTER to login with password...";
	struct pam_response* resp;
	if(conv->conv(1, (const struct pam_message**)&msg, &resp, conv->appdata_ptr)!=PAM_SUCCESS)
		return PAM_AUTH_ERR;
	strcpy(data, resp->resp);
	free(msg);
	/*
	pam_info(pamh, "Please swipe your card...");
	const char* data;
	pam_get_authtok(pamh, PAM_AUTHTOK, &data, NULL);
	*/
	pam_syslog(pamh, LOG_DEBUG, "[pmsca] got %s -> %s", user, data);
	
	char* ln=NULL;
	size_t lenLn=0;
	while(getline(&ln, &lenLn, db)>0){
		if(!pmsca_auth(user, data, ln)){
			pam_syslog(pamh, LOG_DEBUG, "[pmsca] allowed for %s", user);
			free(ln);
			fclose(db);
			return PAM_SUCCESS;
		}
	}
	pam_syslog(pamh, LOG_DEBUG, "[pmsca] denied for %s", user);
	free(ln);
	fclose(db);
	return PAM_USER_UNKNOWN;
}

PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv){
	return PAM_SUCCESS;
}
