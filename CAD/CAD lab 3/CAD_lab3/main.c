//Technical University Of Crete
//Date:12/04/2022
//HRY 419:CAD tools for vlsi design
//Professor: Apostolos Dollas
//Author: Ioannis Peridis
//****************************************************************************************
//This program takes as an input a netlist with the number of gates,placement and routing
//and gives as output a very good routing and compaciton solution for the gates by using an algorithm
//and also the number of rows in this solution
//****************************************************************************************

//************* г еяцасиа е╨маи йамомийа емтоР тгс пяохеслиас апостокгс, деите тгм амажояа*************************

//libraries used
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//function declaration
void swapRows(int array[][2],int rowA,int rowB);

int main()
{
    FILE *fp;
    char str[3],row[2],checkIN[2],checkAB[2];
    int numOfShells;

    //open netlist input file (you have to enter the netlist path)
    fp=fopen("C:\\test\\optimal_8bit.txt","r");
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

    //array of inputs
    int inputs[numOfShells][3];
    int pos=0;

    //this is used to fill the inputs array from the routing array
    //it also uses the placement array to fill the inputs in the right order of the standard shells
    for(int i=0;i<numOfShells;i++){
        inputs[i][0]=placement[i];
        for(int j=0;j<numOfShells;j++){
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
    printf("\n");
    printf("GATES| IN1 | IN2 | OUTS ...\n========================\n");
    for(int i=0;i<numOfShells;i++){
       for(int j=0;j<maxOuts+3;j++){
        printf("%4d |",total[i][j]);
       }printf("\n");
    }

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
    printf("\n*****PREPROCESSING*****\n");
    printf("=======================\n");
    position=0;

    //this is used to fill and print the total connections, wire lengths and order of placement, before they are sorted in the right order
    for(int i=0;i<numOfShells;i++){
        for(int j=3;j<maxOuts+3;j++){
            if(total[i][j]!=0){
                order[position]=maxLength-length[position]+1;
                totalConnections[position][0]=total[i][j];
                totalConnections[position][1]=total[i][0];

                if(totalConnections[position][0]<10&&totalConnections[position][1]>=10){
                    printf("(U0%d_IN,U%d_OUT)| wire length:%2d| order of placement:%2d|\n",totalConnections[position][0],totalConnections[position][1],length[position],order[position]);
                }else if(totalConnections[position][0]>=10&&totalConnections[position][1]<10){
                    printf("(U%d_IN,U0%d_OUT)| wire length:%2d| order of placement:%2d|\n",totalConnections[position][0],totalConnections[position][1],length[position],order[position]);
                }else if(totalConnections[position][0]<10&&totalConnections[position][1]<10){
                    printf("(U0%d_IN,U0%d_OUT)| wire length:%2d| order of placement:%2d|\n",totalConnections[position][0],totalConnections[position][1],length[position],order[position]);
                }else{
                    printf("(U%d_IN,U%d_OUT)| wire length:%2d| order of placement:%2d|\n",totalConnections[position][0],totalConnections[position][1],length[position],order[position]);
                }
                position++;
            }
        }
    }
    printf("\n");
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
    printf("*****ROUTING*****\n");
    printf("=================\n");
    //the number of rows in the solution
    int numOfRows=0;
    //this is used to  print the total connections array after they are sorted
    //=connections of 2 gates sorted by their order= first we place the biggest length wires and then the smaller ones
    for(int i=0;i<numOfShells*maxOuts;i++){
        if(order[i]!=0){

                if(totalConnections[i][0]<10&&totalConnections[i][1]>=10){
                    printf("(U0%d_IN,U%d_OUT)| wire length:%2d |order of placement:%2d|\n",totalConnections[i][0],totalConnections[i][1],length[i],order[i]);
                }else if(totalConnections[i][0]>=10&&totalConnections[i][1]<10){
                    printf("(U%d_IN,U0%d_OUT)| wire length:%2d |order of placement:%2d|\n",totalConnections[i][0],totalConnections[i][1],length[i],order[i]);
                }else if(totalConnections[i][0]<10&&totalConnections[i][1]<10){
                    printf("(U0%d_IN,U0%d_OUT)| wire length:%2d |order of placement:%2d|\n",totalConnections[i][0],totalConnections[i][1],length[i],order[i]);
                }else{
                    printf("(U%d_IN,U%d_OUT)| wire length:%2d |order of placement:%2d|\n",totalConnections[i][0],totalConnections[i][1],length[i],order[i]);
                }
                numOfRows++;
        }
    }

    //this is the number of rows in the solution at first without compaction
    printf("\n*****ROWS IN SOLUTION*****\n==========================\nNumber of rows in the solution before compaction:%d\n\n",numOfRows);

    //this is the new number of rows after the ocmpaction
    int newNumOfRows=numOfRows;
    //array of outputs that are going to more than 1 gates
    int sameOutRows[numOfShells*maxOuts];
    //initialization to zero
    for(int i=0;i<numOfShells*maxOuts;i++){
        sameOutRows[i]=0;
    }

    printf("*****COMPACTION FIRST STEP*****\n");
    printf("===============================\n");
    //this is the compaction algorithm, it is used to tell us which outputs is going to more than 1 inputs so it is extended so we have 1 less row if this happens
    for(int i=0;i<numOfShells*maxOuts;i++){
        for(int j=i+1;j<numOfShells*maxOuts;j++){
            if(totalConnections[i][1]==totalConnections[j][1]&&order[i]!=0){
               newNumOfRows--;
               printf("wire (U%d_IN,U%d_OUT) is extended. Because U%d_OUT goes to more than 1 inputs\n",totalConnections[i][0],totalConnections[i][1],totalConnections[i][1]);
               sameOutRows[i]=totalConnections[i][1];
            }
        }
    }
    printf("\n");

    //this is the total length=number of std shells
    int totalShellLength=numOfShells;
    //this is the arrays of empty space from left and right of the wire
    int spaceLeft[numOfRows];
    int spaceRight[numOfRows];
    int start=0,end=0;
    int startBefore=0,endBefore=0;
    int tempo=0;

    printf("*****COMPACTION SECOND STEP*****\n");
    printf("================================\n");
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
                printf("start of wire: %2d| end of wire: %2d| wire length: %2d| empty space left: %2d| empty space right: %2d|\n",start,end,length[j],spaceLeft[j],spaceRight[j]);
                if(sameOutRows[j]==0&&sameOutRows[j-1]==0&&j!=0){
                    //put the new wire at right
                    if(length[j]<spaceRight[j-1]&&start>=endBefore){
                        newNumOfRows--;
                        printf("\n**wire (U%d_IN,U%d_OUT) is moved 1 row up, at the right of wire (U%d_IN,U%d_OUT)**\n\n",totalConnections[j][0],totalConnections[j][1],totalConnections[j-1][0],totalConnections[j-1][1]);
                    }
                    //put the new wire at left
                    if(length[j]<spaceLeft[j-1]&&end<=startBefore){
                        newNumOfRows--;
                        printf("\n**wire (U%d_IN,U%d_OUT) is moved 1 row up, at the left of wire (U%d_IN,U%d_OUT)**\n\n",totalConnections[j][0],totalConnections[j][1],totalConnections[j-1][0],totalConnections[j-1][1]);
                    }
                }
            }

    }

    //this is the number of rows in the solution  after the compaction
    printf("\n*****ROWS IN SOLUTION AFTER COMPACTION*****\n===========================================\nNumber of rows in the solution after the compaction:%d\n\n",newNumOfRows);

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
