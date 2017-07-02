
#include <iostream>
#include <fstream>
#include <iomanip>
#include <bitset>
#include "geradorMIF.h"

using namespace std;

void gerarMIF(int numInst, string argv){

    ofstream fout;
    ifstream arq;
    arq.open("../tst/saida.o");
    fout.open("../tst/saida.mif", ios::out); //cria um arquivo de saida
    //fout.open(argv, ios::out);
    fout<<"DEPTH = 256;\nWIDTH = 8;\nADDRESS_RADIX = HEX;\nDATA_RADIX = BIN;\nCONTENT\nBEGIN\n"<<endl;
    int i;
    string line;
    int j, aux=0;
    char hex[2];
    int k=0;

    //verifica numero de linhas do arquivo (eof não funcionou)
    if (arq.is_open()) {
        while (getline(arq, line)) {
            aux++;
        }
    }
    else{
        cout<<"erro"<<endl;
    }

    arq.close();
    arq.open("../tst/saida.o");
    string lineaux[aux*2];
    //divide a linha em duas (8 bits cada)
    for(i=0; i<aux; i++){
        getline(arq, line);

        for (j=0; j<8; j++){
            lineaux[k] += line.at(j);
            lineaux[k+1] += line.at(j+8);

        }

        k+=2;
    }
    arq.close();
    //imprime as linhas do arquivo com as instrucoes
    for(i=0; i<aux*2;i++){

        //std::bitset<8> b(lineaux[i]);
        fout<<std::hex<< std::uppercase<< std::setw(2) << std::setfill('0') <<i<<"  :  "<<lineaux[i]<<";"<<endl;
    }
    fout<<std::hex<< std::uppercase<<"["<<i<<"..FF]:  00000000"<<endl;


    fout<<"END;"<<endl;
    fout.close();
}




