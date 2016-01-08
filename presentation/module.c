struct module {
    enum module_state state;

	 /* Member of list of modules */
	 struct list_head list;

	 /* Unique handle for this module */
	 char name[MODULE_NAME_LEN];
	 ..
	 // skipped for brevity
}