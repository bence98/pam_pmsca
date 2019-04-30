# pmsca_pam
Poor Man's SmartCard Authenticator PAM module

## Functionality
With this tool, you can add additional passwords to your account, with which you can unlock your account (use `sudo`, log in to X etc.), on top of your UNIX password.

## Usage
Add the `.so` to the PAM module directory (`/lib/security`, `/lib64/security`, `/lib/x86_64-linux-gnu/security` etc.), then add this line to the top of the PAM configuration:

```
auth	sufficient	pam_pmsca.so
```

Then, enroll the card with the `sudo ./pmsca-enroll` command.

## Backstory
I have a cheap Chinese RFID card reader. The device emulates a keyboard, that types the card ID when you swipe your card.

I wanted to use it in PAM authentication, in addition with my normal user password. Now, the easiest route is to add a new password (the card ID). But I was unable to find any PAM modules that allows to do this (except for LDAP, which is, let's just say, _not particularily_ easy to set up). So, here you go folks.
