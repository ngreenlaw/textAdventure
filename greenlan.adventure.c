/*Nathan Greenlaw
 * ONID: greenlan
 * cs 344
 * Program 2 game program*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include <time.h>
#include <assert.h>

//Create the room struct
struct Room
{
	char name[20];
	char connections[7][20];
	char roomType[1][20];
	int numConnections;
};

// Get the rooms data from the directory
void getRooms(struct Room **r)
{
	// Read a directory, taken from reading 2.4
	int newestDirTime = -1;
	char targetDirPrefix[32] = "greenlan.rooms.";
	char newestDirName[256];
	
	memset(newestDirName, '\0', sizeof(newestDirName));

	//open the directory
	DIR *dirToCheck;
	struct dirent *fileInDir;
	struct stat dirAttributes;
	
	dirToCheck = opendir(".");

	// Check through every directory in the current one and take the one with the newest time stamp
	if(dirToCheck > 0)
	{
		while ((fileInDir = readdir(dirToCheck)) != NULL)
		{
			if(strstr(fileInDir->d_name, targetDirPrefix) != NULL)
			{
				stat(fileInDir->d_name, &dirAttributes);
				if((int)dirAttributes.st_mtime > newestDirTime)
				{
					newestDirTime = (int)dirAttributes.st_mtime;
					memset(newestDirName, '\0', sizeof(newestDirName));
					strcpy(newestDirName, fileInDir->d_name);
				}
			}
		}	
	}

	DIR *roomDir;

	//Get the current working directory
	char cwd[1024];
	getcwd(cwd,sizeof(cwd));	

	//Open the directory and loop through the files
	char dirName[1024]; 
	sprintf(dirName,"%s/%s",cwd,newestDirName);	

	//Open the correct directory
	int a = chdir(dirName);
	roomDir = opendir(".");
	int roomNum = 0;
	
	//Loop through the files i the directory
	while ((fileInDir = readdir(roomDir)) != NULL)
	{
	if(!strcmp(fileInDir->d_name,".") || !strcmp(fileInDir->d_name,".."))
	{
		//Dont try ot read it
	}
	else
	{
		char buffer[60];
		//Open the file	
		FILE *file = fopen(fileInDir->d_name,"r");
		if( file == NULL)
		{
			perror("Error");
			fclose(file);
		}
		else
		{
			//line count is the lines in the file starting with -1 for easier connection writing
			int lineCount = -1;
			r[roomNum]->numConnections = 0;
			//Get the next line unthil ther are no more
			while(fgets(buffer, 60, file))
			{
				char *split;
				//Use strtok to split the line at the Colon
				split = strtok(buffer,":");

				//The first line is the room name
				if(lineCount == -1)
				{
					split = strtok(NULL, " \n");
					strcpy(r[roomNum]->name, split);
				}
				else
				{
					//Every other line other thatn the last is a connection
					if(lineCount > -1 && (strcmp(split, "ROOM TYPE")))
					{
						split = strtok(NULL, " \n");
						strcpy(r[roomNum]->connections[lineCount], split);
						r[roomNum]->numConnections++;
					}
					//This is the room type
					else
					{
						split = strtok(NULL, " \n");
						strcpy(r[roomNum]->roomType[0], split);
					}
				}
				lineCount++;
			}
		fclose(file);
		roomNum++;
		}
	}
	}
	//close all directories and move back to the main directory
	chdir(cwd);
	closedir(roomDir);
	closedir(dirToCheck);
}

// time function
void getTime()
{
	//Needs to use a second thread
	//write time to file
	//print time
	//I did not end up getting the second thread and mutexes to work correctly

	//From tutorialspoint.com on strftime
	FILE* file = fopen("currentTime.txt","a");
	char t[80];
	
	//get the time and then write it to the file
	time_t rawTime;
	struct tm *info;
	time( &rawTime );
	info = localtime( &rawTime );
	strftime(t,1024,"%I:%M, %A, %B %e, %Y",info);
	printf("%s\n",t);
	fprintf(file,"%s\n",t);	
	fclose(file);

}

// Run the prompt and act based on the input
struct Room* getChoice(char* c, struct Room** r, struct Room* cr)//, pthread_mutex_t myMutex,pthread_t* threads)
{
	//If time was entered than run time function
	if(strcmp(c,"time") == 0)
	{
		//pthread_mutex_unlock(&myMutex);
		//pthread_join(threads[1],NULL);
		getTime();
		printf("\n\n");
		return cr;	
	}
	else
	{
		//check to see if the input was one of the options
		int i=0;
		for(i=0; i<7; i++)
		{
			if(strcmp(c,r[i]->name) == 0)
			{
			//	printf("match\n");
			//	return the room if it matches
				struct Room *rRoom = r[i];
				return rRoom;
			}
			else
			{
			//	printf("Did not mtch\n");
			}	 
		}
		//otherwise retrun the curretn room and say there isnt a room by that name
		printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n\n");
		return cr;
	}	
}

// end function
void endGame(char* p, int s)
{
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\nYOU TOOK %d STEPS.", s);
	printf(" YOUR PATH TO VICTORY WAS:\n%s",p);
}

// Get the room type
char* getRoomType(struct Room* r)
{
	return r->roomType[0];
}

// main function
int main()
{
	//Non working thread and mutex code
	/*pthread_t threads[1];
	int thread_args[2];
	int result_code;
	int index;

	pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIAIZER;
	pthread_mutex_lock(&myMutex);
	
	thread_args[1] = 1;
	result_code = pthread_create(&threads[1],NULL,getTime(), (void*)&thread_args[1]);
	pthread_mutex_lock(&myMutex);
	assert( 0 == result_code );*/
	
	//init the path and step records
	char path[100000];
	int steps=0;

	//Create the array of rooms
	struct Room **rooms = malloc(7*sizeof(struct Room *));
	int u=0;
	for(u=0; u < 7; u++)
	{
		rooms[u] = malloc(sizeof(struct Room));
	}
	getRooms(rooms);

	//Set up the user input (c) and the crrent room object
	char c[256];

	char currentRoom[20];
	struct Room* currentRoomObj;

	// Find the starting room
	int i=0;
	for(i=0; i<7; i++)
	{
		if(strcmp(rooms[i]->roomType[0], "START_ROOM") == 0)
		{
			currentRoomObj = rooms[i];
			strcpy(currentRoom, rooms[i]->name);	
		} 
	}

	//The core loop that runs the game, ends when player hits the end room
	while((strcmp(getRoomType(currentRoomObj), "END_ROOM")) != 0)
	{
		printf("CURRENT LOCATION: %s\n", currentRoom);
		printf("POSSIBLE CONNECTIONS: ");
		//print possible connections
		int j=0;
		for(j=0; j < ((currentRoomObj->numConnections)-1); j++)
		{
			printf("%s, ",currentRoomObj->connections[j]);	
		} 
		printf("%s.\n",currentRoomObj->connections[(currentRoomObj->numConnections)-1]);
		
		printf("WHERE TO? >");	

		//get the user input and store it into c
		scanf("%s",c);
		printf("\n\n");

		//store the previous room as the current room to later check if they had moved
		char previousRoom[20];
		strcpy(previousRoom, currentRoom);

		//run the getChoice function and return the room that they moved to or did not move from
		currentRoomObj = getChoice(c, rooms, currentRoomObj);//, myMutex,threads);
		strcpy(currentRoom, currentRoomObj->name); 

		//increment path and steps only if they moved
		if((strcmp(currentRoom, previousRoom)) != 0)
		{
			steps++;
			strcat(path,currentRoom);
			strcat(path,"\n");
		}
	}
	//run the end game function
	endGame(path,steps);
	
	// remove allocated memory
	//pthread_mutex_destroy(&myMutex);	
	int z=0;
	for(z=0; z<7;z++)
	{
		free(rooms[z]);
	}
	free(rooms);
	
return 0;
}
