int Create_Dir_PathFile(char *dirfile_name);
int Create_Dir_Path(char *d_name);
int __create_dir_path(char *d_name, char new_call);
int Open_ReadDir(char *d_name);
void Close_ReadDir(void);
int Get_ReadDirFile(char *f_name);
int Get_DirFile(char *d_name, char *f_name);
int SaveErr(char *err_data);
