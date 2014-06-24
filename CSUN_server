#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#define MY_PORT 1314

double thres = 80;

int sockfd;
int connfd[10]={0,0,0,0,0,0,0,0,0,0};
int connfl[10]={0,0,0,0,0,0,0,0,0,0};
int inde = 0;
pthread_mutex_t inde_mutex;
pthread_cond_t  inde_cond;
int g_msg_queue_id = 0;

typedef struct _trap_message
{
    long msg_type;
    unsigned char msg_text[128];
}trap_message;

int findmax(int *a, int n)
{
    int max = *a, i;
    for (i=0;i<n;i=i+1)
        if (max<a[i])
            max = a[i];
    return max;
}

void acceptconn();

int main()
{
    int result;
    struct sockaddr_in my_addr;
    socklen_t clienlen;
    struct sockaddr_in cliaddr;
    clienlen = sizeof(cliaddr);
    pthread_t accthread;

	g_msg_queue_id = atoi(argv[1]);
    nodeid = atoi(argv[2]);

    trap_message msg;
    int type = 0;

    memset(&msg,0,sizeof(msg));

    result = pthread_mutex_init(&inde_mutex,NULL);
    if(result != 0)
    {
        perror("pthread_mutex_init");
        exit(1);
    }
    result = pthread_cond_init(&inde_cond,NULL);
    if(result != 0)
    {
        perror("pthread_cond_init");
        exit(1);
    }
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(1);
    }
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MY_PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero),8);

    result = bind(sockfd,(struct sockaddr*)&my_addr,sizeof(struct sockaddr));
    if (result == -1)
    {
        perror("bind");
        exit(1);
    }

    result = listen(sockfd,10);
    if (result == -1)
    {
        perror("listen");
        exit(1);
    }
    result = pthread_create(&accthread,NULL,(void*)acceptconn,NULL);
    if (result !=0)
    {
        printf("Creat thread error!\r\n");
        exit(1);
    }
    char buffer[1024];
    while(1)
    {
        connfd[inde] = accept(sockfd,(struct sockaddr*)&cliaddr, &clienlen);
        if(connfd[inde] == -1)
        {
            perror("accpet");
            exit(1);
        }
       
        int j;
		int result;
		fd_set read_fds,test_fds;
	    int max_fds;
		int inde1=inde;
		char buffer[1024];
		struct timeval tv;
		inde1 = inde;
        FD_ZERO(&read_fds);
		FD_SET(connfd[inde],&read_fds); 
		memset(buffer, '\0',sizeof(buffer));
		sprintf(buffer,"password");
        result = write(sockfd,buffer,sizeof(buffer));
		if (result == -1)
            {
                perror("write");
                exit(1);
            }
		sleep(2);
		if(FD_ISSET(sockfd,&test_fds))
        {
            memset(buffer,'\0',sizeof(buffer));
            result = read(sockfd,buffer,sizeof(buffer));
            if(result == -1)
            {
                perror("read");
                exit(1);
            }
            else if(result == 0)
            {
                printf("connect break");
                continue;
            }
            else if((strncmp("CUSNet",buffer,6))==0)
            {
                printf("%s\n",buffer);
				printf("A new gateway connect OK.\n");
				connfl[inde] = 1;
				inde = inde + 1;
            }
        }		
	}
    printf("hello world2!\n");
    close(sockfd);
    return 0;
}

void acceptconn()
{
	FILE* fp;
	int nodeid[20],timev[20],tempn,tempt;
	double data[20],tempd;
    int count=0,acount=0,type = 0;
    int result;
    fd_set read_fds,test_fds;
    //int fd;
    int max_fds;
    int inde1=inde;
    char buffer[1024];
    struct timeval tv;
	trap_message msg;
    memset(&msg,0,sizeof(msg));
    while(1)
    {
        inde1 = inde;
        FD_ZERO(&read_fds);
        for (count=0;count<inde1;count=count+1)
        {
            if (connfl[count]==1)
            {
                FD_SET(connfd[count],&read_fds);
            }
        }
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        max_fds = findmax(connfd,inde1) + 1;
        test_fds = read_fds;
        result = select(max_fds,&test_fds,(fd_set *)NULL,(fd_set *)NULL,&tv);
        int i = 0;
        while (i<inde1)
        {
            if(connfl[i]==1)
            {
                if(FD_ISSET(connfd[i],&test_fds))
                {
                    memset(buffer,'\0',sizeof(buffer));
                    result = read(connfd[i],buffer,sizeof(buffer));
                    //printf("read=%i\n",result);
                    if(result == -1)
                    {
                        perror("read");
                        exit(1);
                    }
                    else if(result >= 0)
                    {

                        if((strncmp(buffer,"exit",4)==0) || (buffer == NULL) || (result == 0))
                        {
                            close(connfd[i]);
                            connfl[i]=0;
                            int j;
                            printf("gateway%i is lost.\n",i);
                        }
                        else
                        {
                            int j;
							int flag = 0;
                            char stemp[]="client";
                            char st[]="0";
                            sprintf(st,"%i",i);
                            strcat(stemp,st);
                            sprintf(st,":%s",buffer);
                            strcat(stemp,st);
                            printf("node%s\n",buffer);
							sscanf(buffer,"%d[%d]<%d>",tempn,tempd,tempt);
							for (j=0;j<20;j++)
							{
								if(tempn == nodeid[j] && tempt == timev[j])
								{
									flag = 1;
									break;
								}
							}
							if(flag == 0)
							{
								nodeid[acount] = tempn;
								data[acount] = tempd;
								timev[acount] = tempt;
								acount++;
								if(acount ==20)
									acount = 0;
								if((fp=fopen(".\data.json","a+")==NULL)
								{
									printf("\nerror on open file");
									continue;
								}
								sprintf(buffer,"{\"nodeid\":[%d],\"data\":[%d],\"time\":[%d];}",tempn,tempd,tempt);
								fputs(buffer,fp);
								fclose(fp);

								if((fp=fopen(".\node.json","a+")==NULL)
								{
									printf("\nerror on open file");
									continue;
								}
								sprintf(buffer,"{\"nodeid\":[%d],\"status\":[\"OK\"];}",tempn);
								fputs(buffer,fp);
								fclose(fp);

								if((fp=fopen(".\gateway.json","a+")==NULL)
								{
									printf("\nerror on open file");
									continue;
								}
								sprintf(buffer,"{\"net\":[\"OK\"],\"num\":[%d];}",acount);
								fputs(buffer,fp);
								fclose(fp);

								if(tempd>thres)
								{
									sprintf(msg.msg_text,"node%d[%d]<%d>",tempn,tempd,tempt);
									msg.msg_type = type;
									if ((msgsnd(g_msg_queue_id,&msg.msg_text,sizeof(msg.msg_text),IPC_NOWAIT))!=0)
									{
										printf("send msg error!\n");
										printf("%m",errno);
									}
								}
							}
                        }
                    }
                    sleep(0);
                }
            }
            sleep(0);
            i = i+1;
        }
    }

}
