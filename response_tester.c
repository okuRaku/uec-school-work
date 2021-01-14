/*
  This program was designed to measure button pressing response
  time using two different methods.  Curses window used
  to keep the visual layout clean. Timer code copied from example.
  This program probably only works in *nix. Compile with -lncurses
  Written by Jesse Jennings for Human Interface 2007-5-18
  ID 0695023
 */
/* Copied here results for 100 repetitions of test one:

 --------------------------------------------- ---------------------------
 |         -------- RESULTS --------         | |       DATA              |
 |                                           | |332  538  366  594  339  |
 |             Quickest:   297 ms            | |316  380  397  336  830  |
 |                                           | |365  526  385  430  328  |
 |             Average:    392 ms            | |400  422  413  328  316  |
 |                                           | |348  451  376  337  373  |
 |             Slowest:    594 ms            | |327  317  408  319  395  |
 |                                           | |342  336  390  391  383  |
 |                                           | |360  350  374  307  419  |
 |          Press any key to exit            | |350  537  356  344  406  |
 |                                           | |361  603  370  365  440  |
 --------------------------------------------- |297  408  468  365  509  |
                                               |394  392  368  530  379  |
                                               |365  371  392  390  317  |
                                               |334  332  318  418  337  |
                                               |369  407  420  443  375  |
                                               |316  334  387  351  363  |
                                               |331  473  353  339  339  |
                                               |436  318  375  333  359  |
                                               |475  402  351  377  346  |
                                               |499  428  369  344  297  |
                                               ---------------------------

*/

/* Copied here results for 100 repetitions of test two:
 --------------------------------------------- ---------------------------
 |         -------- RESULTS --------         | |       DATA              |
 |                                           | |389  450  587  617  456  |
 |             Quickest:   342 ms            | |573  464  604  503  509  |
 |                                           | |652  469  537  519  626  |
 |             Average:    591 ms            | |748  515  1359 451  600  |
 |                                           | |482  497  681  439  444  |
 |             Slowest:    2155 ms           | |555  467  844  665  1018 |
 |                                           | |759  516  537  412  660  |
 |                                           | |571  412  459  549  1033 |
 |          Press any key to exit            | |396  681  420  375  542  |
 |                                           | |522  766  453  633  645  |
 --------------------------------------------- |467  655  470  342  520  |
                                               |527  397  545  374  589  |
                                               |683  425  2155 617  592  |
                                               |797  481  425  666  815  |
                                               |583  460  361  610  414  |
                                               |535  762  626  503  473  |
                                               |804  555  662  603  375  |
                                               |675  847  599  542  814  |
                                               |1048 425  420  527  465  |
                                               |842  377  486  488  647  |
                                               ---------------------------
*/

/* 
  Looking at the data, it appears that as might be expected
  reaction time is much slower when a decision is required.
  After seeing the second character we must stop to think which key
  to press, and sometimes we get stuck in the process.
  Also, we can guess that the time required for the decision to
  press Enter or Space takes less time for execution than to actually
  push the button.  This could be because the signal takes extra time to
  travel from the brain to the final action of our hand moving. 
  We should also consider the case where we make the wrong decision,
  or press without completing the decision. 
*/

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <curses.h>

char c, charOnScreen=' ';
char testMsg[24] = " Press any key to start ";
static WINDOW *mainwnd;
static WINDOW *screen;
static WINDOW *data;
WINDOW *my_win;

int milli, charNum, fastest, slowest, average, sum, reps = 100;

struct timeval tv, start;

/* curses window preparation */
void screen_init(void) {
   mainwnd = initscr();
   noecho();
   cbreak();
   nodelay(mainwnd, TRUE);
   refresh(); // 1)
   wrefresh(mainwnd);
   screen = newwin(13, 45, 1, 1);  //main left window
   data = newwin(23, 27, 1, 47);   //data display right window
   box(data, ACS_VLINE, ACS_HLINE);
   box(screen, ACS_VLINE, ACS_HLINE);
   curs_set(0);
   mvwprintw(screen,1,1,"RESPONSE TIME, PROBLEM A-3  Jesse Jennings");
   mvwprintw(screen,3,1,"1: Test one. When char shown, press Space");
   mvwprintw(screen,5,1,"2: Test two. Does second character match?");
   mvwprintw(screen,7,6,"Please choose which test to run.");
   mvwprintw(screen,10,7,"--- Any other key to exit ---");
   wrefresh(screen);

   mvwprintw(data,1,8,"DATA");
   wrefresh(data);
   refresh();
}

/* window refresh for test method one */
static void test_one_disp(void) {
   wclear(screen);
   box(screen, ACS_VLINE, ACS_HLINE);
   curs_set(0);
   mvwprintw(screen,1,5,"Press Space when char A appears.");
   mvwprintw(screen,3,10,testMsg);
   mvwprintw(screen,5,1,"%20c",charOnScreen);
   mvwprintw(screen,10,1,"%18d ms", milli);
   wrefresh(screen);
   wrefresh(data);
   refresh();
}

/* window refresh for test method two */
static void test_two_disp(void) {
   wclear(screen);
   box(screen, ACS_VLINE, ACS_HLINE);
   curs_set(0);
   mvwprintw(screen,1,1,"If # 2 is A, press Return, else Space");
   mvwprintw(screen,3,2,testMsg);
   mvwprintw(screen,5,1,"%20c",charOnScreen);
   mvwprintw(screen,7,1,"         Char # %d", charNum);
   mvwprintw(screen,10,1,"%18d ms", milli);
   wrefresh(screen);
   wrefresh(data);
   refresh();
}

/* final results calculation screen */
static void result_disp(void) {
   wclear(screen);
   box(screen, ACS_VLINE, ACS_HLINE);
   curs_set(0);
   mvwprintw(screen,1,10,"-------- RESULTS --------");
   mvwprintw(screen,3,14,"Quickest:   %d ms",fastest);
   mvwprintw(screen,5,14,"Average:    %d ms",average);
   mvwprintw(screen,7,14,"Slowest:    %d ms",slowest);
   mvwprintw(screen,10,11,"Press any key to exit");
   wrefresh(screen);
   refresh();
   c = getchar();  //wait for key to exit
}

void screen_end(void) {
   endwin();
}

main(){
        int delay;  //used to create varied timing
        screen_init();

        c=getchar(); //get test selection

        if(c == '1'){   // test method one selected
                test_one_disp();
                c=getchar();  // wait for start
                char newTestMsg[45] = "       GET READY!       ";
                int i=0, x=0, y=1;
                for(i; i<reps; i++, x++){
                        char *p = testMsg;     //
                        char *q = newTestMsg;  //
                        while (*q != '\0') {   // primitive strcpy method
                                *p++ = *q++;   //
                        }                      //
                        *p = '\0';             //
                        charOnScreen = ' ';  // remove 'A' if it was there from previous rep
                        test_one_disp();

                        if(i ==20) {y += 5; x = 0;}  // make columns in data window
                        if(i ==40) {y += 5; x = 0;}
                        if(i ==60) {y += 5; x = 0;}
                        if(i ==80) {y += 5; x = 0;}

                        srand(time(NULL));
                        delay = 1 + rand()%3;

                        sleep(delay);  // wait somewhat random time to display character A

                        gettimeofday(&start,0);

                        charOnScreen = 'A';
                        int k=0;
                        for(k; testMsg[k] != '\0'; k++) { testMsg[k] = ' '; }
                        test_one_disp();               

                        do{ c = getchar(); }
                        while(c != ' ');  // demand Space key to stop timer

                        gettimeofday(&tv,0);
                        milli= (tv.tv_sec-start.tv_sec)*1000 + (tv.tv_usec-start.tv_usec)/1000;
                        charOnScreen = ' ';
                        sum += milli;
                        mvwprintw(data,(x+2),y,"%d",milli);
                        if(milli < fastest || fastest == 0) fastest = milli;
                        if(milli > slowest || slowest == 0) slowest = milli;
                        test_one_disp();
                }
        average = sum/reps;
        result_disp();
        } else if(c == '2') {  // test method two selected
                test_two_disp();
                c=getchar();
                int i=0, x=0, y=1;
                char newTestMsg[45] = "       GET READY!       ";
                for(i; i<reps; i++, x++){
                        char *p = testMsg;
                        char *q = newTestMsg;
                        while (*q != '\0') {  //primited strcpy
                                *p++ = *q++;
                        }
                        *p = '\0';
                        charOnScreen = 'A';
                        charNum = 1;
                        test_two_disp();

                        if(i ==20) {y += 5; x = 0;}  // make columns in data window
                        if(i ==40) {y += 5; x = 0;}
                        if(i ==60) {y += 5; x = 0;}
                        if(i ==80) {y += 5; x = 0;}

                        srand(time(NULL));
                        delay = 1 + rand()%3;

                        sleep(delay);

                        srand(time(NULL));
                        int a = rand();  // used to flip a coin for A or B on next char
                        gettimeofday(&start,0);         


                        if(a % 2 == 0){   // heads, A is repeated
                                charOnScreen = 'A';
                                charNum = 2;
                                int i = 0;
                                for(i; testMsg[i] != '\0'; i++) { testMsg[i] = ' '; }
                                test_two_disp();
                                do{ c = getchar(); }
                                while(c != '\r');  //wait for return key
                                gettimeofday(&tv,0);
                                milli= (tv.tv_sec-start.tv_sec)*1000 + (tv.tv_usec-start.tv_usec)/1000;
                                sum += milli;
                                if(milli < fastest || fastest == 0) fastest = milli;
                                if(milli > slowest || slowest == 0) slowest = milli;
                                mvwprintw(data,(x+2),y,"%d",milli);
                                test_two_disp();
                        }       
                        else {            // tails, B is shown
                                charOnScreen = 'B';
                                charNum = 2;
                                int i = 0;
                                for(i; testMsg[i] != '\0'; i++) { testMsg[i] = ' '; }
                                test_two_disp();
                                do{ c = getchar(); }
                                while(c != ' ');  //wait for space key
                                gettimeofday(&tv,0);
                                milli= (tv.tv_sec-start.tv_sec)*1000 + (tv.tv_usec-start.tv_usec)/1000;
                                sum += milli;
                                if(milli < fastest || fastest == 0) fastest = milli;
                                mvwprintw(data,(x+2),y,"%d",milli);
                                test_two_disp();
                        }
                }
        average = sum/reps;
        result_disp();
        }
screen_end();
printf("\n\nProgram Ended\n\n");
}