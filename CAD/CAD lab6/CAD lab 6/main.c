//Technical University Of Crete
//Date:30/05/2022
//HRY 419:CAD tools for vlsi design
//Professor: Apostolos Dollas
//Author: Ioannis Peridis
//****************************************************************************************
//This is a program that takes as input a netlist with a random  placement for a 8 bit adder.
//At first it calculates the number of rows in the solution and the length of the wires of the random placement
//After that, it re-arranges the gates to create a better placement , using graph theory techniques
//Then it calculates the number of rows in the solution and the length of the wires of the new-better placement
//AT last it uses the Simulated Annealing Method to find the best placement
//It also produces an output netlist with the final rows in solution and the routing
//****************************************************************************************

//libraries used
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
//function declaration
void swapRows(int array[][2],int rowA,int rowB);
void swapRowsGraph(int array[][41],int rowA,int rowB);
int RouteAndCompact(int numOfShells,int routing[][3],int newPlacement[]);

int main()
{
    FILE *fp;
    char str[3],row[2],checkIN[2],checkAB[2];
    int numOfShells;

    fp=fopen("C:\\test\\Askhsh_2_FA_8bit_v2.txt","r");
    //open netlist input file (you have to enter the netlist path)
    //check if the netlist exists
    if(fp==NULL){
        printf("Netlist file not found!\n");
        exit(0);
    }

    //this is used to move the cursor to the number of gates in the netlist
    fseek(fp,18,SEEK_CUR);
    //this is used to read the number of gates=number of shells
    fscanf(fp,"%d",&numOfShells);

    //arrays of inputs, placement and routing from the netlist
    int placement[numOfShells];
    int routing[numOfShells][3];

    //this is used to move the cursor to the placement section in the netlist
    fseek(fp,14,SEEK_CUR);
    fgets(row,2,fp);
    fseek(fp,1,SEEK_CUR);
    //this is used to read all the gate placements and store them to the placement array
    for(int i=0;i<numOfShells;i++){
    //reads the gate number as characters
    fgets(str,3,fp);
    //turns the gate number characters to integers
    placement[i]=atoi(str);
    fseek(fp,7,SEEK_CUR);
    fgets(row,2,fp);
    fseek(fp,1,SEEK_CUR);
    }
    //this is used to move the cursor to the routing section in the netlist
    fseek(fp,9,SEEK_CUR);
    fgets(row,2,fp);
    //this is used to read all the gate routings and store them to the routing array
    for(int i=0;i<numOfShells;i++){
    fseek(fp,1,SEEK_CUR);
    //reads the gate routing as characters
    fgets(str,3,fp);
    //turns the gate routing characters to integers
    routing[i][0]=atoi(str);
    fseek(fp,1,SEEK_CUR);
    //CHECK if it is A or B
    fgets(checkAB,2,fp);
    //if the output is from outside the circuit move 1 position before
    fgets(checkIN,2,fp);
    if(strcmp(checkIN,"N")!=0){
       fseek(fp,-1,SEEK_CUR);
    }
    //reads the gate routing as characters
    fgets(str,3,fp);
    //turns the gate routing characters to integers
    routing[i][1]=atoi(str);
    //if the output is from outside the circuit add 100 to it so it stands out from the others
    if(strcmp(checkIN,"N")==0){
       routing[i][1]+=100;
    }
    //if the output is from outside A the circuit add 200 to it so it stands out from the others
    if(strcmp(checkAB,"A")==0){
       routing[i][1]+=200;
    }
    //if the output is from outside B the circuit add 300 to it so it stands out from the others
    if(strcmp(checkAB,"B")==0){
       routing[i][1]+=300;
    }
    fseek(fp,1,SEEK_CUR);
    //CHECK if it is A or B
    fgets(checkAB,2,fp);
    //if the output is from outside the circuit move 1 position before
    fgets(checkIN,2,fp);
    if(strcmp(checkIN,"N")!=0){
       fseek(fp,-1,SEEK_CUR);
    }
    //reads the gate routing as characters
    fgets(str,3,fp);
    //turns the gate routing characters to integers
    routing[i][2]=atoi(str);
    //if the output is from outside the circuit add 100 to it so it stands out from the others
    if(strcmp(checkIN,"N")==0){
       routing[i][2]+=100;
    }
    //if the output is from outside A the circuit add 200 to it so it stands out from the others
    if(strcmp(checkAB,"A")==0){
       routing[i][2]+=200;
    }
    //if the output is from outside B the circuit add 300 to it so it stands out from the others
    if(strcmp(checkAB,"B")==0){
       routing[i][2]+=300;
    }
    fgets(row,2,fp);
    }

    //close the file
    fclose(fp);

    //This is the first random placement
    printf("\n***This is the Random Starting Placement Given from the Netlist:***\n");
    printf("-------------------------------------------------------------------------------------------------------------------\n");
    for(int i=0;i<numOfShells;i++){
        printf("%d|",placement[i]);
    }printf("\n-------------------------------------------------------------------------------------------------------------------\n");
    //This is the Route and Compact of the starting random placement
    int finalRows1=RouteAndCompact(numOfShells,routing,placement);
    printf("\n\n*****ROWS IN FINAL SOLUTION*****\n===========================================\nNumber of rows after the Place & Route algorithm:%d\n",finalRows1);

    //This is used for creating the graph nodes connections
    int graphConnections[numOfShells][numOfShells];
    int totalGraph[numOfShells+1][numOfShells+1];
    //These are the 4 sub-graphs
    int subGraph1[10],subGraph2[10],subGraph3[10],subGraph4[10];
    //Initialization of the graph connections matrix
    for(int i=0;i<numOfShells;i++){
        for(int j=0;j<numOfShells;j++){
            graphConnections[i][j]=0;
        }
    }

    //Here we create all the connections between the nodes
    //This graph is non-directed, in the matrix we represent a connection between 2 gates i,j by putting '1' in the pair(i,j) or '0' if the nodes i,j are not connected
    //We have an 8 bit adder, so the connection are going to be the same for each bit
    int k=0;
    for(int i=0;i<numOfShells;i+=5){
        graphConnections[i][k+4]=1;
        graphConnections[i+1][k+2]=1;graphConnections[i+1][k+3]=1;
        graphConnections[i+2][k+1]=1;graphConnections[i+2][k+4]=1;graphConnections[i+2][k+9]=1;
        graphConnections[i+3][k+1]=1;graphConnections[i+3][k+9]=1;
         graphConnections[i+4][k]=1;graphConnections[i+4][k+2]=1;graphConnections[i+4][k+7]=1;graphConnections[i+4][k+8]=1;
        k+=5;
    }
    routing[1][0]=2;

    //Here we create the first column and the first row of the total graph
    //First Row= the placement of the gates by order
    //First Column= the random placement of the gates given
    totalGraph[0][0]=0;//This is an empty element
    //At first we put the placement ordered and then we are going to create the random placement given
    for(int i=1;i<numOfShells+1;i++){
           totalGraph[i][0]=routing[i-1][0];
           totalGraph[0][i]=routing[i-1][0];
    }
    //Here we combine the first row and first column with the graph connecitons
    for(int i=1;i<numOfShells+1;i++){
        for(int j=1;j<numOfShells+1;j++){
            totalGraph[i][j]=graphConnections[i-1][j-1];
        }
    }

    //Here we swap rows of the total graph connection matrix, so we can create the random placement given by the netlist
    int found=0;
    for(int i=1;i<numOfShells+1;i++){
        if(totalGraph[i][0]!=placement[i-1]){
            for(int j=1;j<numOfShells+1;j++){
                if(placement[j-1]==totalGraph[i][0]){
                    found=j;
                }
            }
            swapRowsGraph(totalGraph,i,found);
            i=0;
        }
    }

    //This is the Total Circuit printed ,Represented by the Graph as we explain above
    printf("\n***This is the Circuit Represented as a Graph***\n");
    printf("==================================================\n");
    for(int i=0;i<numOfShells+1;i++){
        for(int j=0;j<numOfShells+1;j++){
            printf("%2d ",totalGraph[i][j]);
        }
        printf("\n");
    }

    //This is used to calculate the total connections of each gate, we initialize it to zero
    int sum[numOfShells];
    for(int i=0;i<numOfShells;i++){
        sum[i]=0;
    }
    //Here we calculate the number of connections that each gate has, we basically calculate the sum of '1' of every column, each one represents one connection
    for(int col=1;col<numOfShells+1;col++){
      for(int row=1;row<numOfShells+1;row++){
        sum[col-1]+=totalGraph[row][col];
      }
    }

    //These are used to keep track of each row we find the first , second, third and fourth connection ='1'
    int count=0,first=0,second=0,third=0,fourth=0;
    //This is the re-arrangement of the gates algorithm by using graph theory
    //We are taking the gates with the most connections and we try to put them near each other
    for(int i=0;i<numOfShells;i++){
        //4 connections are the maximum
        if(sum[i]==4){
            //We go to the columns with 4 connections and we keep track in which rows are the connections
            //that means with which gates is the gate of the column is connected with
            for(int j=1;j<numOfShells+1;j++){
                if(totalGraph[j][i+1]==1){
                    count++;
                    if(count==1){
                        first=j;
                    }
                    if(count==2){
                        second=j;
                    }
                    if(count==3){
                        third=j;
                    }
                    if(count==4){
                        fourth=j;
                        break;
                    }
                }
            }
            //THIS IS USED TO BRING THE GATES THAT ARE HIGHLY CONNECTED CLOSE TO EACH OTHER
            //if the first connection is not in the 2 first rows
            if(first-2>0){
                if((totalGraph[first-2]!=1)&&((first-2)!=second)){
                    //we swap rows with the gate that has the second connection and the gate that is 2 rows above from the gate that has the first connection
                    swapRowsGraph(totalGraph,first-2,second);
                }
            }else{//if the first connection is in the 2 first rows
                if((totalGraph[first+2]!=1)&&((first+2)!=second)){
                    //we swap rows with the gate that has the second connection and the gate that is 2 rows after from the gate that has the first connection
                    swapRowsGraph(totalGraph,first+2,second);
                }
            }

            if((totalGraph[first-1]!=1)&&((first-1)!=third)){
                //we swap rows with the gate that has the third connection and the gate that is 1 row above from the gate that has the second connection
                swapRowsGraph(totalGraph,first-1,third);
            }

            if((totalGraph[first+3]!=1)&&((first+3)!=fourth)){
                //we swap rows with the gate that has the fourth connection and the gate that is 3 rows after from the gate that has the first connection
                swapRowsGraph(totalGraph,first+3,fourth);
            }
            count=0;
        }
    }
    //Here we print the new graph with the re-arrangement of the gates
    printf("\n***This is the Graph After the Re-Arrangement of the Nodes-Gates***\n");
    printf("=====================================================================\n");
    for(int i=0;i<numOfShells+1;i++){
        for(int j=0;j<numOfShells+1;j++){
            printf("%2d ",totalGraph[i][j]);
        }
        printf("\n");
    }

    //Here we fill the 4 sub-graphs with gates by spliting the total graph in 4 pieces , each one with 10 gates (total 40 gates)
    for(int i=0;i<10;i++){
        subGraph1[i]=totalGraph[i+1][0];
        subGraph2[i]=totalGraph[i+11][0];
        subGraph3[i]=totalGraph[i+21][0];
        subGraph4[i]=totalGraph[i+31][0];
    }

    //Here we combine the 4 graphs with the optimal order to create the new improved placement
    for(int i=0;i<10;i++){
        placement[i]=subGraph4[i];
        placement[i+10]=subGraph2[i];
        placement[i+20]=subGraph1[i];
        placement[i+30]=subGraph3[i];
    }

    //Here we print the new re-arranged and improved placement
    printf("\n***This is the Placement After the Graph-Theory Based Re-Arrangement of the Gates:***\n");
    printf("-------------------------------------------------------------------------------------------------------------------\n");
    for(int i=0;i<numOfShells;i++){
        printf("%d|",placement[i]);
    }printf("\n-------------------------------------------------------------------------------------------------------------------\n");
    //Here we find the routing of the new placement
    int finalRows2=RouteAndCompact(numOfShells,routing,placement);
    printf("\n\n*****ROWS IN FINAL SOLUTION*****\n===========================================\nNumber of rows after the Place & Route algorithm:%d\n",finalRows2);

    //counter of what iteration we are now and counter of the total iterations
    int iterationNum=1,totalIterations=0;
    //this is the limit that if we pass it we accept a bad solution
    int acceptBadSolution=8;
    //this is the termination condition and flag that terminates the loop
    int termination=64,notDone=1;
    //these are the number of rows old and the new one ,after every iteration
    int numOfRows=1000,newNumOfRows=0;
    //these are temps used to help ,make the switch of two shells
    int shelNo1=0,shelNo2=0,temp=0;

    //this is an array of the new placements that we have after every iteration, at first it is same as the basic placement array
    int newPlacement[numOfShells];
    for(int i=0;i<numOfShells;i++){
        newPlacement[i]=placement[i];
    }

    //==================================================
    //This is the placement algorithm, it tries a lot of different placements as inputs for the route and compact function
    //and keeps a solution very close to the optimal. It uses the MonteCarlo/Simulated Annealing method to swap 2 shells at a time
    //Sometimes it keeps a solution that is not the best one, so it wont stuck at local maximums/minimums
    //==================================================
    //loop that is running until the iteration number is bigger than the termination condition

    //This is the seed of the random number generator, it is used to keep track of the good/bad number sequences
    srand(10);

    printf("\n***These are the steps of the Place & Route algorithm:***\n");
    printf("===========================================================\n");
    while(notDone){
        //The new number of rows for every iteration is given by the route and compact function
        //We put as input **THE NEW PLACEMENT ARRAY FOR EACH ITERATION**
        newNumOfRows=RouteAndCompact(numOfShells,routing,newPlacement);

        //If the solution is better( with less rows) we keep it ,so we make the placement array equal to the new placement array of this iteration
        if(newNumOfRows<numOfRows){
            //the new best number of rows that we have is changed
            numOfRows=newNumOfRows;
            for(int j=0;j<numOfShells;j++){
                placement[j]=newPlacement[j];
            }

            printf("We found a better solution: Iteration Number:%3d | Total Iterations:%3d | Rows in Solution:%2d\n",iterationNum,totalIterations,newNumOfRows);

            //we initialize the iteration counter back to 1
            iterationNum=1;
        //If the solution is worse (with more rows) we don't keep it, so we don't change the placement array
        }else{
            //iteration number increased by 1
            iterationNum++;
            //If the iteration number reaches the limit of bad solution acceptance, then we keep the solution , so we make the placement array equal to the new placement array of this iteration
            if(iterationNum>acceptBadSolution){
               for(int j=0;j<numOfShells;j++){
                   placement[j]=newPlacement[j];
                   }

                printf("We accept a worse solution: Iteration Number:%3d | Total Iterations:%3d | Rows in Solution:%2d\n",iterationNum,totalIterations,newNumOfRows);


               //the bad solution acceptance limit is doubled, because we wan to do less changes as the algorithm deepens in time
               acceptBadSolution=acceptBadSolution*2;
               //we initialize the iteration counter back to 1
               iterationNum=1;
            }
        }
        //if the iteration number reaches the termination condition, we end the placement and then we just keep the best solution that we founded
        if(iterationNum>termination){
            notDone=0;
        }
        //this is the counter of the total iterations
        totalIterations++;

        //Here is happening the swap of the 2 standard shells
        //We pick 2 random shells to swap, so we use the function rand with a seed so we can keep the track of the best/worst solutions
        //The rand function is producing a random number between 0 and Number Of Shells -1,these are the 2 random elements of the placement arrays
        shelNo1=rand()%(numOfShells-1);
        shelNo2=rand()%(numOfShells-1);

        //we swap the 2 random elements VALUES of the arrays
        temp=newPlacement[shelNo1];
        newPlacement[shelNo1]=newPlacement[shelNo2];
        newPlacement[shelNo2]=temp;

    }


    //The final number of rows in the solution is given by the route and compact funciton
    //We put as input **THE FINAL PLACEMENT ARRAY THAT CONTAINS THE PLACEMNET WITH THE BEST SOLUTION**
    int finalRows=RouteAndCompact(numOfShells,routing,placement);
    printf("\n*****ROWS IN FINAL SOLUTION*****\n===========================================\nNumber of rows after the Place & Route algorithm:%d\n",finalRows);
    printf("The total Iterations needed:%d\n",totalIterations);


}

//this function is used to swap 2 rows in a 2 dimensional array
 void swapRows(int array[][2],int rowA,int rowB){
     int temp=0;
     for(int j=0;j<2;j++){
        temp=array[rowA][j];
        array[rowA][j]=array[rowB][j];
        array[rowB][j]=temp;
     }
 }

 //this function is used to swap 2 rows in a 41 dimensional array
 void swapRowsGraph(int array[][41],int rowA,int rowB){
     int temp=0;
     for(int j=0;j<41;j++){
        temp=array[rowA][j];
        array[rowA][j]=array[rowB][j];
        array[rowB][j]=temp;
     }
 }

 //This function is used to route and compact the given circuit, with inputs the number of standard shells, the gates routing and the placement of the shells
 //It Returns the number of rows of the solution and it also makes an output file with the total routing
 int RouteAndCompact(int numOfShells,int routing[][3],int placement[]){
     //array of inputs
    int inputs[numOfShells][3];
    int pos=0;

    //this is used to fill the inputs array from the routing array
    //it also uses the placement array to fill the inputs in the right order of the standard shells
    for(int i=0;i<numOfShells;i++){
        inputs[i][0]=placement[i];
        for(int j=0;j<40;j++){
            if(routing[j][0]==placement[i]){
                pos=j;
                break;
            }
        }
        inputs[i][1]=routing[pos][1];
        inputs[i][2]=routing[pos][2];
    }
    //outputs array
    int outputs[numOfShells][numOfShells];
    //initialization of the outputs array to zero
    for(int i=0;i<numOfShells;i++){
        for(int j=0;j<numOfShells;j++){
            outputs[i][j]=0;
        }
    }

    //these are the maximum outputs of the netlist
    int count,maxOuts=0;
    //this is used to fill the outputs array from the routing array
    //it also uses the placement array to fill the outputs in the right order of the standard shells
    for(int i=0;i<numOfShells;i++){
        outputs[i][0]=placement[i];
        //the outputs columns are not the same every time
        //if an output is going to N gates then we have N columns
        count=1;
        for(int j=0;j<numOfShells;j++){
            if(inputs[i][0]==inputs[j][1]){
                outputs[i][count]=inputs[j][0];
                count++;
                if(count>maxOuts){
                   maxOuts=count;
                }
            }
        }
        for(int j=0;j<numOfShells;j++){
            if(inputs[i][0]==inputs[j][2]){
                outputs[i][count]=inputs[j][0];
                count++;
                if(count>maxOuts){
                   maxOuts=count;
                }
            }
        }
    }

    maxOuts--;
    //array of the right placement+inputs+outputs
    int total[numOfShells][maxOuts+3];

    //this is used to fill the total array by using the placement ,inputs and outputs arrays
    for(int i=0;i<numOfShells;i++){
        total[i][0]=placement[i];
        total[i][1]=inputs[i][1];
        total[i][2]=inputs[i][2];
        for(int j=3;j<maxOuts+3;j++){
            total[i][j]=outputs[i][j-2];
        }
    }

    //printing the total array
    /*printf("\n");
    printf("GATES| IN1 | IN2 | OUTS ...\n========================\n");
    for(int i=0;i<numOfShells;i++){
       for(int j=0;j<maxOuts+3;j++){
        printf("%4d |",total[i][j]);
       }printf("\n");
    }*/

    //array of every wires length
    int length[numOfShells*maxOuts];
    //initialization of lengths array
    for(int i=0;i<numOfShells*maxOuts;i++){
        length[i]=0;
    }

    int position=0;
    //this is used to fill the length array by calculating the length of every wire
    for(int i=0;i<numOfShells;i++){
        for(int j=0;j<numOfShells;j++){
            for(int w=3;w<maxOuts+3;w++){
                 if(total[i][w]!=0){
                    if(total[i][w]==total[j][0]){
                    length[position]=abs(j-i);
                    position++;
                    }
                 }
            }
        }
    }

    //this is the maximum length of the biggest wire
    int maxLength=0;
    for(int i=0;i<numOfShells*maxOuts;i++){
        if(maxLength<length[i]){
            maxLength=length[i];
        }
    }

    //array of the order of placement of every wire
    int order[numOfShells*maxOuts];
    //initialization of order array to zero
    for(int i=0;i<numOfShells*maxOuts;i++){
        order[i]=0;
    }
    //array of connections of 2 gates sorted by their order= first we place the biggest length wires and then the smaller ones
    int totalConnections[numOfShells*maxOuts][2];
    //initialization of totalOutputs array
    for(int i=0;i<numOfShells*maxOuts;i++){
        totalConnections[i][0]=0;
        totalConnections[i][1]=0;
    }

    //this is preprocessing for the routing
    //printf("\n*****PREPROCESSING*****\n");
   //printf("=======================\n");
    position=0;

    //this is used to fill and print the total connections, wire lengths and order of placement, before they are sorted in the right order
    for(int i=0;i<numOfShells;i++){
        for(int j=3;j<maxOuts+3;j++){
            if(total[i][j]!=0){
                order[position]=maxLength-length[position]+1;
                totalConnections[position][0]=total[i][j];
                totalConnections[position][1]=total[i][0];

               /* if(totalConnections[position][0]<10&&totalConnections[position][1]>=10){
                    printf("(U0%d_IN,U%d_OUT)| wire length:%2d| order of placement:%2d|\n",totalConnections[position][0],totalConnections[position][1],length[position],order[position]);
                }else if(totalConnections[position][0]>=10&&totalConnections[position][1]<10){
                    printf("(U%d_IN,U0%d_OUT)| wire length:%2d| order of placement:%2d|\n",totalConnections[position][0],totalConnections[position][1],length[position],order[position]);
                }else if(totalConnections[position][0]<10&&totalConnections[position][1]<10){
                    printf("(U0%d_IN,U0%d_OUT)| wire length:%2d| order of placement:%2d|\n",totalConnections[position][0],totalConnections[position][1],length[position],order[position]);
                }else{
                    printf("(U%d_IN,U%d_OUT)| wire length:%2d| order of placement:%2d|\n",totalConnections[position][0],totalConnections[position][1],length[position],order[position]);
                }*/
                position++;
            }
        }
    }
    //printf("\n");
    int tmp=0;int tmpL=0;
    //this is used to sort the total connections array by their order of placement
    for(int i=0;i<numOfShells*maxOuts;i++){
        if(order[i]>order[i+1]){
            //we are using the swap rows function to put the rows by their order
            swapRows(totalConnections,i,i+1);
            //also swaping the rows of the order
            tmp=order[i];
            order[i]=order[i+1];
            order[i+1]=tmp;
            //also swaping the rows of the length
            tmpL=length[i];
            length[i]=length[i+1];
            length[i+1]=tmpL;

            i=0;
        }
    }

    //here we start the routing of the wires
    //printf("*****ROUTING*****\n");
    //printf("=================\n");
    //the number of rows in the solution
    int numOfRows=0;
    //this is used to  print the total connections array after they are sorted
    //=connections of 2 gates sorted by their order= first we place the biggest length wires and then the smaller ones
    for(int i=0;i<numOfShells*maxOuts;i++){
        if(order[i]!=0){

               /* if(totalConnections[i][0]<10&&totalConnections[i][1]>=10){
                    printf("(U0%d_IN,U%d_OUT)| wire length:%2d |order of placement:%2d|\n",totalConnections[i][0],totalConnections[i][1],length[i],order[i]);
                }else if(totalConnections[i][0]>=10&&totalConnections[i][1]<10){
                    printf("(U%d_IN,U0%d_OUT)| wire length:%2d |order of placement:%2d|\n",totalConnections[i][0],totalConnections[i][1],length[i],order[i]);
                }else if(totalConnections[i][0]<10&&totalConnections[i][1]<10){
                    printf("(U0%d_IN,U0%d_OUT)| wire length:%2d |order of placement:%2d|\n",totalConnections[i][0],totalConnections[i][1],length[i],order[i]);
                }else{
                    printf("(U%d_IN,U%d_OUT)| wire length:%2d |order of placement:%2d|\n",totalConnections[i][0],totalConnections[i][1],length[i],order[i]);
                }*/
                numOfRows++;
        }
    }

    //this is the number of rows in the solution at first without compaction
    //printf("\n*****ROWS IN SOLUTION*****\n==========================\nNumber of rows in the solution before compaction:%d\n\n",numOfRows);

    //this is the new number of rows after the compaction
    int newNumOfRows=numOfRows;
    //array of outputs that are going to more than 1 gates
    int sameOutRows[numOfShells*maxOuts];
    //initialization to zero
    for(int i=0;i<numOfShells*maxOuts;i++){
        sameOutRows[i]=0;
    }

   // printf("*****COMPACTION FIRST STEP*****\n");
   // printf("===============================\n");
    //this is the compaction algorithm, it is used to tell us which outputs is going to more than 1 inputs so it is extended so we have 1 less row if this happens
    for(int i=0;i<numOfShells*maxOuts;i++){
        for(int j=i+1;j<numOfShells*maxOuts;j++){
            if(totalConnections[i][1]==totalConnections[j][1]&&order[i]!=0){
               newNumOfRows--;

               //printf("wire (U%d_IN,U%d_OUT) is extended. Because U%d_OUT goes to more than 1 inputs\n",totalConnections[i][0],totalConnections[i][1],totalConnections[i][1]);
               sameOutRows[i]=totalConnections[i][1];
            }
        }
    }
    //printf("\n");

    //this is the total length=number of std shells
    int totalShellLength=numOfShells;
    //this is the arrays of empty space from left and right of the wire
    int spaceLeft[numOfRows];
    int spaceRight[numOfRows];
    int start=0,end=0;
    int startBefore=0,endBefore=0;
    int tempo=0;

    //printf("*****COMPACTION SECOND STEP*****\n");
    //printf("================================\n");
    //this is the compaction algorithm.
    //first we find the start and then end of every wire
    //then we find the empty space that it has at its left and at its right
    //if the wire we are routing fits at the row before at the left or at the right, we fit it there and we have 1 less row, else if it dose not we put it to its own row
    for(int j=0;j<numOfShells*maxOuts;j++){
            endBefore=end;
            startBefore=start;
            if(order[j]!=0){
                for(int i=0;i<numOfShells;i++){
                    if(totalConnections[j][0]==placement[i]){
                        start=i+1;
                    }
                    if(totalConnections[j][1]==placement[i]){
                        end=i+1;
                    }
                }
                if(start<end){
                    spaceRight[j]=totalShellLength-end;
                    spaceLeft[j]=start-1;
                }else if(start>end){
                    tempo=start;
                    start=end;
                    end=tempo;
                    spaceRight[j]=totalShellLength-end;
                    spaceLeft[j]=start-1;
                }
                //printf("start of wire: %2d| end of wire: %2d| wire length: %2d| empty space left: %2d| empty space right: %2d|\n",start,end,length[j],spaceLeft[j],spaceRight[j]);
                if(sameOutRows[j]==0&&sameOutRows[j-1]==0&&j!=0){
                    //put the new wire at right
                    if(length[j]<spaceRight[j-1]&&start>=endBefore){
                        newNumOfRows--;
                        length[j]=0;
                        //printf("\n**wire (U%d_IN,U%d_OUT) is moved 1 row up, at the right of wire (U%d_IN,U%d_OUT)**\n\n",totalConnections[j][0],totalConnections[j][1],totalConnections[j-1][0],totalConnections[j-1][1]);
                    }
                    //put the new wire at left
                    if(length[j]<spaceLeft[j-1]&&end<=startBefore){
                        newNumOfRows--;
                        length[j]=0;
                        //printf("\n**wire (U%d_IN,U%d_OUT) is moved 1 row up, at the left of wire (U%d_IN,U%d_OUT)**\n\n",totalConnections[j][0],totalConnections[j][1],totalConnections[j-1][0],totalConnections[j-1][1]);
                    }
                }
            }
    }

    //this is the number of rows in the solution  after the compaction
    //printf("\n*****ROWS IN SOLUTION AFTER COMPACTION*****\n===========================================\nNumber of rows in the solution after the compaction:%d\n\n",newNumOfRows);

    //Here we calculate the mean length of all wires
    /*float meanLength=0;
    for(int i=0;i<numOfShells;i++){
        meanLength+=length[i];
    }meanLength=meanLength/numOfShells;
    //Here we print the length of the biggest wire
    printf("\n***The Length of the Biggest Wire (measured in std shells distance) is %2d***\n",maxLength);
    //Here we print the mean wire length
    printf("***The Mean Length of All Wires   (measured in std shells distance) is %2f***\n",meanLength);
    printf("=============================================================================");*/

    FILE *outputFile;
    //we create an output netlist file.txt
    outputFile=fopen("C:\\test\\outputNetlist.txt","w");

    //here we are writing the gates in the circuit with the right order of pld shells
    fprintf(outputFile,"**GATES IN THE CIRCUIT\n");
    for(int i=0;i<numOfShells;i++){
        if(placement[i]<10){
            fprintf(outputFile,"U0%d ",placement[i]);
        }else{
            fprintf(outputFile,"U%d ",placement[i]);
        }
    }
    //here we are writing the number of rows in the solution after the compaction
    fprintf(outputFile,"\n**ROWS IN THE SOLUTION\n%d\n**ROUTING\n",newNumOfRows);
    //here we are writing the routing after the sorting
    for(int i=0;i<numOfShells*maxOuts;i++){
        if(order[i]!=0){

                if(totalConnections[i][0]<10&&totalConnections[i][1]>=10){
                    fprintf(outputFile,"(U0%d_IN,U%d_OUT)\n",totalConnections[i][0],totalConnections[i][1]);
                }else if(totalConnections[i][0]>=10&&totalConnections[i][1]<10){
                    fprintf(outputFile,"(U%d_IN,U0%d_OUT)\n",totalConnections[i][0],totalConnections[i][1]);
                }else if(totalConnections[i][0]<10&&totalConnections[i][1]<10){
                    fprintf(outputFile,"(U0%d_IN,U0%d_OUT)\n",totalConnections[i][0],totalConnections[i][1]);
                }else{
                    fprintf(outputFile,"(U%d_IN,U%d_OUT)\n",totalConnections[i][0],totalConnections[i][1]);
                }
        }
    }
    //close the file
    fclose(outputFile);

    return newNumOfRows;
 }
