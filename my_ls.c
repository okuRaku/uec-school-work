/* 
Simple "ls" command program.  Demonstrates several useful features of "stat" command.
Written by Jesse Jennings for Operating Systems Bonus 4
University of Electro-Communications
*/

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

struct stat sb;
struct group *grp;
struct passwd *pwd;
void updateStat(char *newPath);
// This function displays the information for the file/directory currently in stat sb
static void dispFileInfo(const char *f_name)
{
	////////////////
	// This section turns the st_mode numerical permission into familiar rwx style
       char mode[11];

       if(S_ISDIR(sb.st_mode))
               mode[0] = 'd';
       else if(S_ISCHR(sb.st_mode))
               mode[0] = 'c';
       else if(S_ISBLK(sb.st_mode))
               mode[0] = 'b';
       else if(S_ISREG(sb.st_mode))
               mode[0] = '-';
       else if(S_ISFIFO(sb.st_mode))
               mode[0] = 'p';

       char *x[] = { "---", "--x", "-w-", "-wx",
                     "r--", "r-x", "rw-", "rwx" };
       strcpy(mode + 1, x[(sb.st_mode & S_IRWXU) >> 6]);
       strcpy(mode + 4, x[(sb.st_mode & S_IRWXG) >> 3]);
       strcpy(mode + 7, x[(sb.st_mode & S_IRWXO) >> 0]);
       if((sb.st_mode & S_ISUID)) {
           if((sb.st_mode & S_IXUSR))
               mode[3] = 's';
           else
               mode[3] = 'S';
       }
       if((sb.st_mode & S_ISGID)) {
           if((sb.st_mode & S_IXGRP))
               mode[6] = 's';
           else
               mode[6] = 'S';
       }
	//////////////////////

       printf(mode);
       pwd = getpwuid(sb.st_uid);
       grp = getgrgid(sb.st_gid);
	// the output line
       printf("|%10lld||%.8s氏 %.10s組 %-20s %s",
       (long long)sb.st_size,pwd->pw_name , grp->gr_name, f_name,ctime(&sb.st_mtime) );
}
//This function reevaluates the stat sb value.  Used in a loop for directories
void updateStat(char *newPath)
{
       if(stat(newPath, &sb) == -1){
               fprintf(stderr,"can't stat %s\n",newPath);
               exit(0);
       }
}
int main(int argc, char *argv[])
{

       int i=1;
        char *arg;

       printf("\n%s|%10s||%s|%10s|%8s          %10s\n",
       "permission","サイズ","ユーザ名","グループ名","ファイル名","最終更新時刻");
       printf("--------------------------------------------------------------------------------\n");
       if(argc == 1) arg = ".";
       else arg  = argv[i];

       updateStat(arg);

	// For some reason, without an initial output, directories can't be traversed
       dispFileInfo(arg);

       if(S_ISDIR(sb.st_mode)){  // only attempt to transverse if arg's a dir
               DIR *dir;
               struct dirent* dp;
               if((dir = opendir(arg)) == NULL){
                       perror("Failed to open directory");
                       return;
               }
               while( (dp = readdir(dir)) != NULL ){
			const char *ARG = arg;
			char *subarg;  //subarg is the name of file inside a directory
			subarg = strcpy(subarg, ARG);
			subarg = strcat(subarg, "/");
			subarg = strcat(subarg, dp->d_name);
			updateStat(subarg); // update sb for this file
			dispFileInfo(dp->d_name); //passes filename for output
               }
               closedir(dir);
       }
        else
       { // This is the case if the command argument is a file
               dispFileInfo(arg);
       }
       return 0;
}
