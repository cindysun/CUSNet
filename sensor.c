#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>

#define PORT 2014

time_t t;
int sockfd,sockfd0;
int nid[20],time[20];
double data[20];
int connfd[10]={0,0,0,0,0,0,0,0,0,0};
int connfl[10]={0,0,0,0,0,0,0,0,0,0};
int inde = 0;
pthread_mutex_t inde_mutex;
pthread_cond_t  inde_cond;

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


void listenconn()
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


int main(int argc, char* argv[])
{
    int result;
    int nodeid;
    int sockfd;
    struct sockaddr_in cliaddr;
    struct hostent *host;

    if(argc !=3)
    {
        fprintf(stderr,"err:client hostname nodeid\n");
        exit(1);
    }
    nodeid = atoi(argv[2]);
    host = gethostbyname(argv[1]);

    if (host == NULL)
    {
        herror("gethostbyname");
        exit(1);
    }

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd == -1)
    {
        perror("socket");
        exit(1);
    }
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(PORT);
    cliaddr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(cliaddr.sin_zero),8);
    result = connect(sockfd,(struct sockaddr *)&cliaddr,sizeof(struct sockaddr));
    if (result == -1)
    {
        perror("connect");
        exit(1);
    }
    printf("connect success!\n");

	result = pthread_create(&accthread,NULL,(void*)lisenconn,NULL);
    if (result !=0)
    {
        printf("Creat thread error!\r\n");
        exit(1);
    }


    fd_set read_fds,test_fds;
    //int fd;
    int max_fds;
    char buffer[1024];

    FD_ZERO(&read_fds);
    FD_SET(0,&read_fds);
    FD_SET(sockfd,&read_fds);
    max_fds = sockfd + 1;

    int i;
    int j;
    int flag = 0;
    char command[8] = "sensors";
    char buff[8];
    while(1)
    {
        flag = 0;
        FILE *pp = popen(command,"r");
        if(!pp)
        {
            printf("popen failed\n");
            sleep(3);
            continue;
        }
        char temp[1024];
        while(fgets(temp,sizeof(temp),pp)!=NULL)        //lines
        {
            flag = 0;
            for (i=0;i<=strlen(temp);i++)       //each char
            {
                if(temp[i]=='+')
                {
                    i++;
                    j=0;
                    while(temp[i]!= '.')//while not end
                    {
                        buff[j] = temp[i];
                        i++;
                        j++;
                    }
                    buff[j] = temp[i];
                    buff[j+1] = temp[i+1];
                    buff[j+2] = '\0';
                    printf("result = %s\n",buff);
                    flag = 1;
                    break;
                }
                if(flag == 1) break;
            }
            if (flag == 1) continue;
            if (temp[strlen(temp) -1 ]=='\n')
            {
                printf("%s",temp);
            }
        }
        memset(buffer, '\0',sizeof(buffer));
		t = time(0);
        sprintf(buffer,"%d[%s]<%d>",nodeid,buff,(int)t);
		sscanf(buffer,"%d[%d]<%d>",tempn,tempd,tempt);
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
        result = write(sockfd,buffer,sizeof(buffer));
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
        pclose(pp);
        sleep(3);
    }
}
