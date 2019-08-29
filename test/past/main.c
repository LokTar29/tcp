#include <stdio.h>
#include <stdlib.h>
#include <getch.h>
#include <string.h>
#include "list.h"
#include "student.h"
#include "tools.h"
#include "file.h"
#include "teacher.h"
#include "principal.h"

extern List* list1,*list2,*list3,*list4;
extern int num1,num2,num3,num4;

extern Principal boss;

void student()
{
	clean_stdin();
	int n=0;
	printf("请输入您的账号:");
	scanf("%d",&n);
	clean_stdin();
	if (!signin_stu(n)) return ;
	first_signin_stu(n);
	while(1)
	{
		menu2();
    	switch(get_cmd('1','4')-'0')
		{       
    	    case 1:query_stu(n);break;
    	    case 2:modify_stu(n);break;
    	    case 3:see_stu(n);break;
    		case 4:return ;
    	}	
    	getch();
  		printf("请按任意键继续");
  		getch();
    }
}

void teacher()
{
	int n=0;
	printf("请输入您的账号:");
	scanf("%d",&n);
	clean_stdin();
	if (!signin_tea(n)) return ;
	first_signin_tea(n);
	while(1)
	{
		menu3();
		switch(get_cmd('0','9')-'0')
		{       
    	    case 1:add_stu();break;
    	    case 2:del_stu();break;
     		case 3:search_stu_tea();break;
    		case 4:modify_stu_tea();break;
    		case 5:input_stu_tea();break;
    		case 6:reset_stu_tea();break;
    		case 7:show_instu();break;
    		case 8:show_outstu();break;
    		case 9:sort_stu();break;
    		case 0:return;
  		}
  		getch();
  		printf("请按任意键继续");
  		getch();
    }
}

void principal()
{
	int n=0;
	printf("请输入您的账号:");
	scanf("%d",&n);
	clean_stdin();
	unlock_boss();
	clean_stdin();
	if (!signin_boss()) return ;
	//first_signin_boss();
	while(1)
	{
		menu4();
		switch(get_cmd('1','8')-'0')
		{       
    	    case 1:reset_boss_boss();break;
    	    case 2:reset_tea_boss();break;
    	    case 3:add_tea();break;
    		case 4:del_tea();break;
    		case 5:modify_tea_boss();break;
    		case 6:show_intea();break;
    		case 7:show_outtea();break;
    		case 8:return ;
    	}	
    	getch();
  		printf("请按任意键继续");
  		getch();
	}
}
int main()
{
	open_and_creat();
	menu1();
	switch(get_cmd('1','4')-'0')
    	{
           case 1:student();break;
           case 2:teacher();break;
           case 3:principal();break;
           case 4:return 0;
        }
	save();
}
