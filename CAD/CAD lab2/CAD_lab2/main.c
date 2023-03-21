//Technical University Of Crete
//Date:19/03/2022
//HRY 419:CAD tools for vlsi design
//Professor: Apostolos Dollas
//Author: Ioannis Peridis
//****************************************************************************************
//This program calculates the IDS current for a number of NMOS transistors with two different ways.
//First way is by the NMOS analytical equations.
//Second way is by a macromodel (an approach) of a NMOS transistor.
//Also the program compares the two results and finds the error of the macromodel and the cost of each way.
//****************************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define V_T 1.0
//This is the number of NMOS transistor your testbench contains * 4(number of parameters for each transistor)=number of all data in the testbench
//To test another testbench you have to multiply the num of your NMOS*4 and re-right the testbenchNum
#define testbenchNum 4992
//for testbench: 4992

int main()
{
       //This is way to enter the number of NMOS and their parameters by your own
       //if you want to do that , you have to un-comment this section, comment the next section that takes input from file and replace testbenchNum/4 with just num to the remaining program

       /* float V_GS,V_DS,W,L;
        int num;
        printf("\n********NMOS Transistor Macromodel:********\n\n");
        printf("Please enter the Crowd Number of your Transistors:\n");
        printf("NUM = ");
        scanf("%d",&num);

        float inputArray[num][4];
        printf("\nPlease enter with the order below the VGS,VDS,W,L of each Transistor you have:\n");
        for(int i=0;i<num;i++){
            printf("U%d  ",i+1);
            scanf("%f %f %f %f",&V_GS,&V_DS,&W,&L);
            inputArray[i][0]=V_GS;
            inputArray[i][2]=W;
            inputArray[i][1]=V_DS;
            inputArray[i][3]=L;
        }*/

        printf("\n********************************************\n");
        printf("******* NMOS TRANSISTOR MACROMODEL: ********\n");
        printf("********************************************\n\n");
        printf("ALL THE NMOS TRANSISTOR's VALUES: Number of NMOS Transistors Tested: %d\n",testbenchNum/4);
        printf("===============================================\n");
        printf("     |   VGS   |    VDS    |    W    |    L   |\n");
        printf("===============================================\n");
        float testbenchArray[testbenchNum];
        //This is how we read the testbench from a txt file, you have to enter the address that you putted your file below
        FILE *fptr;
        fptr=fopen("C:\\testbench.txt","r");

        if(fptr==NULL){
            printf("ERROR testbench file not found!");
            exit(1);
        }
        //Reading th testbench input data
        for(int i=0;i<testbenchNum;i++){
            fscanf(fptr,"%f",&testbenchArray[i]);
        }

        //Putting each parameter of the testbench data to an matrix rows= num of NMOS, columns= the 4 NMOS parameters
        float inputArray[testbenchNum][4];
        int position=0;
        for(int i=0;i<testbenchNum;i+=4){
        inputArray[position][0]=testbenchArray[i];   //V_GS
        inputArray[position][1]=testbenchArray[i+1]; //V_DS
        inputArray[position][2]=testbenchArray[i+2]; //W
        inputArray[position][3]=testbenchArray[i+3]; //L
        position++;
        //Printing of all the NMOS transistors and their parameters VGS,VDS,W,L
        printf("U%4d|   %0.3f     %0.3f     %0.3f     %0.3f\n",position,testbenchArray[i],testbenchArray[i+1],testbenchArray[i+2],testbenchArray[i+3]);
        }
        fclose(fptr);

            //Initialization of outputs
            float kArray[testbenchNum];
            float IdsEquationsArray[testbenchNum];
            int equIntegers=0,equFpMuls=0,equFpAdds=0,equFpSubs=0,equFpDivs=0;

            //This is the first method
            //Calculating the IDS current from the NMOS analytical equations:
            //====================================================================

            for(int i=0;i<(testbenchNum/4);i++){
                //constant calculation k=0.2*(W/L)
                kArray[i]=0.2*(inputArray[i][2]/inputArray[i][3]);
                //integer,fp subtraction and fp multiplication cost
                equFpMuls++;
                equFpDivs++;

                //cost for the 3 ifs
                equIntegers+=4;
                //Linear or Triode or Non-Saturated region of NMOS
                //VDS>=0 & VDS<=VGS-VT
                if(inputArray[i][1]>=0 && inputArray[i][1]<=(inputArray[i][0]-V_T)){
                    //IDS=k*[2*(VDS-VT)^2*VDS-VDS^2]
                    IdsEquationsArray[i]=kArray[i]*(2*(inputArray[i][0]-V_T)*inputArray[i][1]-pow(inputArray[i][1],2));
                    //integer,fp subtraction and fp multiplication cost
                    equFpMuls+=5;
                    equFpSubs++;

                //Saturated region of NMOS
                //VDS>=0 & VDS>=VGS-V_T
                }else if(inputArray[i][0]-V_T>=0 && inputArray[i][1]>=(inputArray[i][0]-V_T)){
                    //IDS=k*(VDS-VT)^2
                    IdsEquationsArray[i]=kArray[i]*pow(inputArray[i][0]-V_T,2);
                    //integer,fp subtraction cost
                    equFpSubs++;
                    equFpMuls+=2;

                //Cutoff region of NMOS
                //VGS-VT<=0
                }else if(inputArray[i][0]-V_T<=0){
                    //IDS=0
                    IdsEquationsArray[i]=0;
                    //integer cost
                    equIntegers++;
                }
            }

            //Initialization of outputs
            float IdsMacromodelArray[testbenchNum];
            float kArrayMacromodel[testbenchNum];
            float errorPerCentArray[testbenchNum];
            int macroIntegers=0,macroFpMuls=0,macroFpAdds=0,macroFpSubs=0,macroFpDivs=0;
            float moError;
            float idsTmpArray[1000];
            float vgsArray[20];
            float vdsArray[50];
            vgsArray[0]=0.25;
            vdsArray[0]=0.0;
            int counter=0;
            float W[9]={2,3,4,5,6,7,8,9,10};
            float L[9]={2,3,4,5,6,7,8,9,10};
            float kArrayTmp[81];
            int kcounter=0;

            //This is the second method
            //Calculating the IDS current from the NMOS macromodel approach:
            //====================================================================

            //Here we create an array with 1000 different values of IDS that are all the possible combinations of VGS and VDS ( also we store to two arrays all the VDS and VGS values)
            //The calculations are based to the equations of the NMOS transistor, EXPECT that we are not using k factor, so the IDS is a combinations of VDS and VGS only
            for(int j=0;j<20;j++){
                for(int i=0;i<50;i++){
                    //Linear or Triode or Non-Saturated
                    if(vdsArray[i]>=0 && vdsArray[i]<=(vgsArray[j]-V_T)){
                        idsTmpArray[counter]=(2*(vgsArray[j]-V_T)*vdsArray[i]-pow(vdsArray[i],2));
                    //Saturated
                    }else if(vgsArray[j]-V_T>=0 && vdsArray[i]>=(vgsArray[j]-V_T)){
                        idsTmpArray[counter]=pow(vgsArray[j]-V_T,2);
                    //Cutoff
                    }else if(vgsArray[j]-V_T<=0){
                        idsTmpArray[counter]=0;
                    }
                    counter++;
                    //VDS takes all values from 0 to 5 with 0.1 step, that is 50 different values
                    vdsArray[i+1]=vdsArray[i]+0.1;
                }
                //VGS takes values from 0 to 5 with 0.25 step, that is 20 different values
                vgsArray[j+1]=vgsArray[j]+0.25;
            }


            //Here we create an array with 81 different values of factor k that are all the possible combinations of W and L
            for(int j=0;j<9;j++){
                for(int i=0;i<9;i++){
                    kArrayTmp[kcounter]=0.2*(W[j]/L[i]);
                    kcounter++;
                    }
                }

            //Here we make all calculations for IDS for every NMOS transistor
            for(int i=0;i<(testbenchNum/4);i++){

                //The 4 for loops below are used to calculate what values we are going to take from the 2 pre-fixed arrays , "k factor array" and "IDS array"
                //********************************************************************************************************************************************************
                //********************************************************************************************************************************************************
                int vgsCount=0,vdsCount=0;
                //Here we compare the VGS given input with the 20 prefixed VGS values from our array and then we give to the input the closest value possible from the array
                for(int j=0;j<20;j++){
                    //if the input value is different from the prefixed value by 0.249 or less then make it the new input and exit the loop
                    if(inputArray[i][0]-vgsArray[j]<=0.249){
                        inputArray[i][0]=vgsArray[j];
                        break;
                    }vgsCount++;
                }

                //Here we compare the VDS given input with the 50 prefixed VDS values from our array and then we give to the input the closest value possible from the array
                for(int j=0;j<50;j++){
                    //if the input value is different from the prefixed value by 0.09 or less then make it the new input and exit the loop
                    if(inputArray[i][1]-vdsArray[j]<=0.09){
                        inputArray[i][1]=vdsArray[j];
                        break;
                    }vdsCount++;
                }

                int Lcount=0,Wcount=0;
                //Here we compare the W given input with the 9 prefixed W values from our array and then we give to the input the exact value
                for(int j=0;j<9;j++){
                    //if the input value is same as the value in the array we exit the loop
                    if(inputArray[i][2]-W[j]==0){
                        inputArray[i][2]=W[j];
                        break;
                    }Wcount++;
                }

                //Here we compare the W given input with the 9 prefixed W values from our array and then we give to the input the exact value
                for(int j=0;j<9;j++){
                    //if the input value is same as the value in the array we exit the loop
                    if(inputArray[i][3]-L[j]==0){
                        inputArray[i][3]=L[j];
                        break;
                    }Lcount++;
                }

                //These are the two positions that we are taking from the pre-fixed "IDS array" and "k factor array"
                int kPosition=Wcount*9+Lcount;
                int idsPosition=vgsCount*50+vdsCount;
                //********************************************************************************************************************************************************
                //********************************************************************************************************************************************************

                //Constant k calculation
                //We take the value frm the prefixed array with all the possible values of k
                kArrayMacromodel[i]=kArrayTmp[kPosition];

                //Linear or Triode or Non-Saturated region of NMOS  OR  Saturated region of NMOS
                //VDS>=0 & VDS<=VGS-VT  OR  VDS>=0 & VDS>=VGS-VT
                //IDS=k*ids value from the prefixed array
                IdsMacromodelArray[i]=kArrayMacromodel[i]*idsTmpArray[idsPosition];
                //fp multiplication cost
                macroFpMuls++;

                //cost of the if
                macroIntegers++;
                //Cutoff region of NMOS
                //VGS-VT<=0
                if(inputArray[i][0]-V_T<=0){
                    //IDS=0
                    IdsMacromodelArray[i]=0;
                    //integer cost
                    macroIntegers++;
                }

                 //Fix for position if its the last position in the IDS array
                if(inputArray[i][1]==5.0){
                    IdsMacromodelArray[i]=kArrayMacromodel[i]*idsTmpArray[idsPosition-1];
                    //fp mul cost
                    macroFpMuls++;
                }
                //Corner case solution,for very small IDS currents if equations are in linear mode and macromodel is at cutoff mode
                if(IdsMacromodelArray[i]==0 && IdsEquationsArray[i]!=0){
                    IdsMacromodelArray[i]=kArrayMacromodel[i]*0.029;
                    //integer cost,fp mul cost
                    macroFpMuls++;
                }

                //Here we calculate the macromodel IDS error in reference to to equation IDS
                if(IdsMacromodelArray[i]==0 && IdsEquationsArray[i]==0){
                     errorPerCentArray[i]=0;
                }else{
                     //ERROR in percentage=(IDSmacromodel/IDSequation -1)*100
                     errorPerCentArray[i]=((IdsMacromodelArray[i]/IdsEquationsArray[i])-1)*100;}

                 //This is the summary of all errors
                 moError+=abs(errorPerCentArray[i]);
            }

            //Printing the outputs, IDS equations, IDS macromodel and ERROR percentage
            printf("\n=======================================================\n");
            printf("TRANSISTOR |  IDS    |    IDS          |     ERROR    |\n");
              printf("NUMBER     |  EQN    |    MACROMODEL   |    (perCent) |\n");
              printf("           | (mA)    |   (mA)          |              |\n");
            printf("=======================================================\n");
            for(int i=0;i<(testbenchNum/4);i++){
            printf("U%4d|       %f      %f       %0.2f\n",i+1,IdsEquationsArray[i],IdsMacromodelArray[i],errorPerCentArray[i]);
            }

            //Printing the mean value of all the errors
            printf("\n================================================================\n");
            printf("MEAN VALUE OF ALL THE TRANSISTOR's ERRORS IS: %0.3f (Per Cent)\n",(4*moError)/testbenchNum);
            printf("================================================================\n");

            //printing all the costs of equations and macromodel
            printf("\n===========================================\n");
            printf("OPERATIONS       |  EQN   |   MACROMODEL  |\n");
            printf("===========================================\n");
            printf("INTEGER/BOOLEAN     %d          %d \n",equIntegers,macroIntegers);
            printf("FP MUL              %d          %d \n",equFpMuls,macroFpMuls);
            printf("FP ADD              %d             %d\n",equFpAdds,macroFpAdds);
            printf("FP SUBS             %d          %d \n",equFpSubs,macroFpSubs);
            printf("FP DIVS             %d          %d \n",equFpDivs,macroFpDivs);


    return 0;
}
