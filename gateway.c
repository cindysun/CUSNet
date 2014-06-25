#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#define MY_PORT 2014
#define PORT 1314

int sockfd,sockfd0;
int nid[20],time[20];
double data[20];
int connfd[10]={0,0,0,0,0,0,0,0,0,0};
int connfl[10]={0,0,0,0,0,0,0,0,0,0};
int inde = 0;
pthread_mutex_t inde_mutex;
pthread_cond_t  inde_cond;

int findmax(int *a, int n)
{
    int max = *a, i;
    for (i=0;i<n;i=i+1)
        if (max<a[i])
            max = a[i];
    return max;
}

void acceptconn();

int main(int argc, char* argv[])
{
    int result,i;
	int max_fds;
	char buffer[1024];
    int inde1=inde;
	fd_set read_fds,test_fds;
    struct sockaddr_in my_addr;
    socklen_t clienlen;
    struct sockaddr_in cliaddr;
    clienlen = sizeof(cliaddr);
    pthread_t accthread;
	struct hostent *host;

	if(argc <2)
    {
        fprintf(stderr,"err:server hostname\n");
        exit(1);
    }

	host = gethostbyname(argv[1]);
	if (host == NULL)
    {
        herror("gethostbyname");
        exit(1);
    }

	sockfd0 = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(1);
    }

	while(1)
	{
		cliaddr.sin_family = AF_INET;
		cliaddr.sin_port = htons(PORT);
		cliaddr.sin_addr = *((struct in_addr *)host->h_addr);
		bzero(&(cliaddr.sin_zero),8);
		result = connect(sockfd0,(struct sockaddr *)&cliaddr,sizeof(struct sockaddr));
		if (result == -1)
		{
			perror("connect");
			exit(1);
		}

		FD_ZERO(&read_fds);
		FD_SET(sockfd,&read_fds);
		sleep(2);
		if(FD_ISSET(sockfd0,&test_fds))
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
						if(strncmp(buffer,"password",8)==0) 
						{
							int j;
                            char stemp[]="client";
                            char st[]="0";
                            sprintf(st,"%i",i);
                            strcat(stemp,st);
                            sprintf(st,":%s",buffer);
                            strcat(stemp,st);
                            printf("node%s\n",buffer);
							memset(buffer, '\0',sizeof(buffer));
							//password
        sprintf(buffer,"CUSNet");
        result = write(sockfd0,buffer,sizeof(buffer));
        if (result == -1)
        {
            perror("write");
            //exit(1);
        }
						}
						else
						{
							continue;
						}
					}
		}
		else
		{
			sleep(10);
		}
	}
    printf("connect success!\n");

	for(i=0;i<20;i++)
	{
		nid[i]=0;
		data[i]=0;
		timev[i]=0;
	}


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
        printf("A new node set up OK.\n");
        connfl[inde] = 1;
        inde = inde + 1;
        int j;
    }
    printf("hello world2!\n");
    close(sockfd);
    return 0;
}

void acceptconn()
{
    int count=0,acc=0;
    int result;
	int tempn,tempd,tempt;
    fd_set read_fds,test_fds;
    //int fd;
    int max_fds;
    int inde1=inde;
    char buffer[1024];
    struct timeval tv;
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
                            printf("node%i is lost.\n",i);
                        }
                        else
                        {
                            int j;
                            char stemp[]="client";
                            char st[]="0";
                            sprintf(st,"%i",i);
                            strcat(stemp,st);
                            sprintf(st,":%s",buffer);
                            strcat(stemp,st);
                            printf("node%s\n",buffer);
							sscanf(buffer,"%d[%d]<%d>",tempn,tempd,tempt);
							//filter wrong points
							if(tempn<0||tempd<0||tempt<0)
							{
								i++;
								continue;
							}
							sscanf(buffer,"%d[%d]<%d>",nid[acc],data[acc],timev[acc]);
							acc++;
							if(acc==20)
							{
								acc = 0;
							}
							result = write(sockfd0,stemp,sizeof(buffer));
                            if (result == -1)
                            {
                                perror("write");
								//reconnect
								while(1)
	{
		cliaddr.sin_family = AF_INET;
		cliaddr.sin_port = htons(PORT);
		cliaddr.sin_addr = *((struct in_addr *)host->h_addr);
		bzero(&(cliaddr.sin_zero),8);
		result = connect(sockfd0,(struct sockaddr *)&cliaddr,sizeof(struct sockaddr));
		if (result == -1)
		{
			perror("connect");
			exit(1);
		}

		FD_ZERO(&read_fds);
		FD_SET(sockfd,&read_fds);
		sleep(2);
		if(FD_ISSET(sockfd0,&test_fds))
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
						if(strncmp(buffer,"password",8)==0) 
						{
							int j;
                            char stemp[]="client";
                            char st[]="0";
                            sprintf(st,"%i",i);
                            strcat(stemp,st);
                            sprintf(st,":%s",buffer);
                            strcat(stemp,st);
                            printf("node%s\n",buffer);
							
							memset(buffer, '\0',sizeof(buffer));
        sprintf(buffer,"CUSNet");
        result = write(sockfd0,buffer,sizeof(buffer));
        if (result == -1)
        {
            perror("write");
            //exit(1);
        }
						}
						else
						{
							continue;
						}
					}
		}
		else
		{
			sleep(10);
		}
	}
    printf("connect success!\n");
	//reupdate
	for(ii = 0;ii<20;ii++)
	{
		if(data[ii]>0)
		{
		sprintf(buffer,"%d[%d]<%d>",nid[ii],data[ii],timev[ii]);
		write(sockfd0,buffer,sizeof(buffer));
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
