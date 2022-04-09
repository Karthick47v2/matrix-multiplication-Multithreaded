#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>

float mat1[10000][10000], mat2[10000][10000];
float sTMat[10000][10000], mTMat[10000][10000];

int row1, row2, col1, col2;

// read from matrix fro file
void readMatrix(){

  FILE *inMat1 = fopen("inputMat1.txt", "r");
  FILE *inMat2 = fopen("inputMat2.txt", "r");

  if(inMat1 == NULL || inMat2 == NULL){
      printf("ERROR: Can't open file\n");
      exit(1);
  }

  for(int i = 0; i < row1; i++){
    for(int j = 0;j < col1; j++) fscanf(inMat1,"%f,",&mat1[i][j]);
  }
  fclose(inMat1);

  for(int i = 0;i < row2; i++){
    for(int j = 0; j < col2; j++) fscanf(inMat2,"%f,",&mat2[i][j]);
  }
  fclose(inMat2);
}

// create random mat
void randomMatrix(){

  for(int i = 0; i < row1; i++)
    for(int j = 0; j < col1; j++){
      if(rand() % 2 == 1) mat1[i][j] = (float)(rand() % 100) / RAND_MAX * 10000;
      else mat1[i][j] = (rand() % 100);
    }
      
  for (int i = 0; i < row2; i++)
    for (int j = 0; j < col2; j++){
      if(rand() % 2 == 1) mat2[i][j] = (float)(rand() % 100) / RAND_MAX * 10000;
      else
        mat2[i][j] = (rand() % 100);
    }
}

// print input mat
void printInMatrix(){

  printf("\n%s\n","Matrix 1");
  for(int i = 0; i < row1; i++){
    for(int j = 0; j < col1; j++){
      if((mat1[i][j] - (int)mat1[i][j]) > 0) printf("\t%f ",mat1[i][j]);
      else printf("\t%0.f\t ",mat1[i][j]);
    }
    printf("\n");
  }

  printf("\n%s\n","Matrix 2");
  for(int i = 0; i < row2; i++){
    for(int j = 0; j < col2; j++){
      if((mat2[i][j] - (int)mat2[i][j]) > 0) printf("\t%f ",mat2[i][j]);
      else printf("\t%0.f\t ",mat2[i][j]);
    }
    printf("\n");
  }
}

// print output mat
void printOutMatrix(){

  printf("\n%s\n","Output Matrix");
  for(int i = 0; i < row1; i++){
    for(int j = 0; j < col2; j++){
      if((sTMat[i][j] - (int)sTMat[i][j]) > 0) printf("\t%f ",sTMat[i][j]);
      else printf("\t%0.f\t ",sTMat[i][j]);
    }
    printf("\n");
  }
}

// mat multiplication
void multiplyMat(){

  for(int i = 0; i < row1; i++){
    for(int j = 0; j < col2; j++){
      for(int k = 0; k < col1; k++) sTMat[i][j] += mat1[i][k] * mat2[k][j];
    }
  }
}

// row multiplier
void *rowMultiplier(void *arg){

  int threadID, rStart, rEnd;
  double res;

  threadID = *(int *)(arg);                  
  rStart = threadID * (row2 / col1);
  rEnd = (threadID + 1) * (row2 / col1);

  for(int i = rStart; i < rEnd; i++){ 
    for(int j = 0; j < col2; j++){          
      res = 0;                              
      for(int k = 0; k < col1; k++) res += mat1[i][k] * mat2[k][j];
      mTMat[i][j] = res;
    }
  }
}

// row multip in multithread 
void mTMultiply() {

  pthread_t * threads;
  int n = row1;
  
  threads = (pthread_t *) malloc( n * sizeof(pthread_t));
  
  for(int i = 0; i < n; i++){
    int *threadID;
    threadID = (int *) malloc( sizeof(int));
    *threadID = i;
    pthread_create(&threads[i], NULL, rowMultiplier, (void *)threadID);
  }

  for (int i = 0; i < n; i++) pthread_join(threads[i], NULL);
}

// cal time
void calcTime(){

  FILE *output;
  output = fopen("output.txt","a+");
  
  clock_t time;
  double timeDiff;
  time = clock();       
  
  multiplyMat();
  
  time = clock() - time; 
  timeDiff = ((double)time) / CLOCKS_PER_SEC; 
  
  printOutMatrix();

  printf("\nSingle Thread Time :: %.3lf s", timeDiff);

  fprintf(output,"\t%d\t\t ", row1 * col2);
  fprintf(output,"\t%f\t\t ", timeDiff);

  time = clock();      
  mTMultiply();
  time = clock() - time; 
  timeDiff = ((double)time)/CLOCKS_PER_SEC; 

  printf("\nMultiThread Time :: %.3lf s", timeDiff);
  
  fprintf(output,"\t%f\t\t\n", timeDiff);
}

int main() {

  int userInput;
  srand((unsigned)time(NULL));

  printf(" 1st Matrix\n");
  printf("No. of rows : ");
  scanf("%d", &row1);
  printf("No. of cols : ");
  scanf("%d", &col1);

  printf("\n\n 2nd Matrix\n");
  printf("No. olf rows : ");
  scanf("%d", &row2);
  printf("No. of cols : ");
  scanf("%d", &col2);

  if(col1 != row2){
    printf("Matrix not multiplicable \n");
    return 0;
  }

  printf("1. Fill matrix with vals in file");
  printf("\n2. Fill matrix with random vals");

  FILE *output;
    output = fopen("output.txt","w");
    fprintf(output,"\tElements\t\t");
    fprintf(output,"Single-Thread\t\t\t");
    fprintf(output,"Multi-Thread\t\t\n");

  do{  
    printf("\nPress appropiate no : ");
    scanf("%d",&userInput);

    if(userInput == 1){
        readMatrix();             
        printInMatrix();       
        calcTime();             
    }
    
    else if(userInput == 2){
        // for generate the size increesed matrix and get data to output file

        for(int i = 0; i <= 1000; i += 10) {
          row1=row2=col1=col2=i;
        //assign Random values to 2 randomMat
          randomMatrix();                 
          printInMatrix();        
          calcTime();            
        }
    }
  }while(userInput != 1 && userInput != 2);

  return 0;
}
