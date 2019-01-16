#include <stdio.h>
#include <stdlib.h> // rand
#include <time.h>

void do mode7()
{
    char x, y, z;
    int a, b, c;
    int n, m;
    int i;

    /* 3桁の乱数値を生成 */
    srand((unsigned int)time(NULL));
    x = (char)(rand()%10);
    do {
        y = (char)(rand()%10);
    } while(y == x);
    do {
        z = (char)(rand()%10);
    } while(z == x||z == y);

    for (;;) {

        /* ユーザ入力を受け、1の位、10の位、100の位に分ける */
      while(!KEY_LONG_C){
	switch(i){
	case 1:
	  switch(ud->sw){  /*モード内でのキー入力別操作*/

	  case KEY_SHORT_U: /* 上短押し */
	    a++;
	    break;
	  case KEY_SHORT_D: /* 下短押し */
	    a--;
	    break;

	  case KEY_SHORT_L: /* 左短押し */
	    break;
	  
	  case KEY_SHORT_R: /* 右短押し */
	    i++;
	    break;
	  }
	  break;
	case 2:
	  switch(ud->sw){  /*モード内でのキー入力別操作*/

	  case KEY_SHORT_U: /* 上短押し */
	    a++;
	    break;
	  case KEY_SHORT_D: /* 下短押し */
	    a--;
	    break;

	  case KEY_SHORT_L: /* 左短押し */
	    i--;
	    break;
	  
	  case KEY_SHORT_R: /* 右短押し */
	    i++;
	    break;
	  }
	    break;
	  case 3:
	  switch(ud->sw){  /*モード内でのキー入力別操作*/

	  case KEY_SHORT_U: /* 上短押し */
	    a++;
	    break;
	  case KEY_SHORT_D: /* 下短押し */
	    a--;
	    break;

	  case KEY_SHORT_L: /* 左短押し */
	    i--;
	    break;
	  
	  case KEY_SHORT_R: /* 右短押し */
	    break;
	  }
	  break;
	}
      }
        /* 不正な入力値をはじく */
        if (n < 12||n > 987||a == b||a == c||b == c)
            continue;

        /* 「イート」の数 */
        n = 0;
        if (a == x) n++;
        if (b == y) n++;
        if (c == z) n++;

        /* 「バイト」の数 */
        m = 0;
        if (a == y || a == z) m++;
        if (b == x || b == y) m++;
        if (c == x || c == y) m++;

        if (n != 3)
            printf("%d-%d\n", n, m);
        else {
            puts("Hit!\n");
            break;
        }
    }
    return 0;
}
