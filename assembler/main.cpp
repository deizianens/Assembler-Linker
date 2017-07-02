/*
 -------------------------------------------------
 UNIVERSIDADE FEDERAL DE MINAS GERAIS
 DEPARTAMENTO DE CIÊNCIA DA COMPUTAÇÃO
 DISCIPLINA DE SOFTWARE BÁSICO
 TRABALHO PRÁTICO 2
 ALUNAS: DEIZIANE SILVA E LORENA BARRETO SIMEDO
 */

#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <cstring>



using namespace std;

int posmem=140; //primeira palavra vazia da seção de memória reservada a dados

typedef struct{ //struct que receberá os campos de cada instrução
    string label="\0"; //Label da instrução. Valerá \0 caso não exista
    string opcode="\0"; //opcode da instrução
    string op1="\0"; //Operador 1 da instrução. Valerá \0 caso não exista
    string op2="\0"; //Operador 2 da instrução. Valerá \0 caso não exista
    string op3="\0";
} instrucoes;

int tamts=0; //tamanho da tabela de símbolos

typedef struct{
    int pc=0;
    string inst="\0";
    string label="\0";
    int datatam=0; //campos usados apenas para operações .data
    int datapos=0; //campos usados apenas para operações .data
}tabeladesimbolos;

int buscatabela(tabeladesimbolos* tabela, string labelprocurado){ //busca um determinado label na tabela de símbolos. Retorna -1 se não achar
    for(int i=0;i<tamts;i++){
        if (tabela[i].label==labelprocurado){
            return i;
        }
    }
    return (-1);
}

int convert2par (int x){ //converte um número de bytes para um número correspondente a um número inteiro de palavras arredondando para cima
    if(x%2==0){
        return x;
    }
    return x+1;
}

int contndatas(instrucoes* inst,int tam){ //conta numero de pseudo-intrucoes .data
    int cont=0;
    for(int i=0;i<tam;i++){
        if(inst[i].opcode==".data"){
            cont++;
        }
    }
    return cont;
}

int contret(instrucoes* inst,int tam){ //conta numero de pseudo-intrucoes .data
    int cont=0;
    for(int i=0;i<tam;i++){
        if(inst[i].opcode=="ret"){
            cont++;
        }
    }
    return cont;
}

int contchamadasdefuncoes(instrucoes*inst, int tam){ //conta numero de chamadas de funcoes
    int cont=0;
    for(int i=0;i<tam;i++){
        if(inst[i].opcode=="call"){
            cont++;
        }
    }
    return cont;
}

int nprocedimentos=0; //variavel que armazena o numero de procedimentos do programa

int buscaprocedimentos(string* proc, string buscar){ //encontra um dado procedimento em uma string
    for(int i=0;i<nprocedimentos;i++){
        if(proc[i]==buscar){
            return i;
        }
    }
    return -1;
}

int main () {
    string caminhoarquivo="../tst";
    string nomearquivo="main"; //alterar nome do arquivo para montar diferentes arquivos (sem extensão)
    string linhacom,linha; //linhacom recebe cada linha do arquivo e linha recebe as linhas com os comentários removidos
    int nlinhas=0; //conta o número de linhas do arquivo
    ifstream arq; //arquivo a ser manipulado
    ofstream saida;
    int aux=0,cont=0; //variável auxiliar e contador
    char caracter; //leitura dos caracteres
    arq.open(caminhoarquivo+"/"+nomearquivo+".a"); //abre arquivo a ser montado
    if (arq.is_open()){ //testa  abertura do arquivo
        cout<<"Arquivo:"<<endl<<"----------"<<endl; //impresão do arquivo para fins de teste
        while (!arq.eof() ){ //loop que percorre o arquivo
            getline (arq,linhacom); //lê linha por linha
            int a=0;
            int tam=linhacom.size();
            while((linhacom[a]==' '||linhacom[a]=='\t')&&a<tam){
                a++;
            }
            if(linhacom!=" "&&linhacom!=""&&linhacom!="\t"&&linhacom[a]!='\n'&&linhacom!="\0"&&linhacom[a]!=';'){ //conta as linhas não-vazias
                cout<<linhacom<< endl;
                nlinhas++;
            }
        }
        cout<<"----------"<<endl;
        cout<<"Numero de linhas: "<<nlinhas<<endl; //imprime número de linhas
    } else{
        cout << "Unable to open file";
        return 0;
    }
    arq.close();
    arq.open(caminhoarquivo+"/"+nomearquivo+".a"); //arquivo reaberto
    instrucoes* ins=new instrucoes[nlinhas];  //Linguagem de montagem -> 1 instrução por linha
    tabeladesimbolos* tabela=new tabeladesimbolos[nlinhas];//tabela de simbolos
    string* procedimentos=new string[nlinhas]; //recebe os procedimentos presentes no programa. Seu tamanho é controlado pela variável global nprocedimentos
    for(int i=0;i<nlinhas;i++){
        procedimentos[i]="/0";
    }
    while (!arq.eof()){ //lê o arquivo novamente
        getline (arq,linhacom); //lê linha por linha
        int a=0;
        int tam=linhacom.size();
        while((linhacom[a]==' '||linhacom[a]=='\t')&&a<tam){ //remove identação inicial
            a++;
        }
        if(linhacom!=" "&&linhacom!=""&&linhacom!="\t"&&linhacom[a]!='\n'&&linhacom!="\0"&&linhacom[a]!=';'){ //conta as linhas não-vazias
            aux=0;
            //Remoção dos comentários
            for(int ii=0;ii<tam;ii++){
                if(linhacom[ii]==';'){ //Os comentários ocorrem depois do ;
                    tam=ii; //localiza ; caso exista
                }
            }
            linha="\0"; //variável que receberá as linhas sem comentários
            for(int j=0;j<tam;j++){
                linha=linha+linhacom[j];
            }
            caracter=linha[0];
            if(caracter=='_'){ //testa se há ou não o label
                aux=0; //conta o tamanho do label
                caracter=linha[0];
                while(caracter!=':'){ //o label termina em :
                    ins[cont].label=ins[cont].label+caracter; //o '+' em c++ concatena as strings. Vamos adicionando os caracteres do label
                    aux++;
                    caracter=linha[aux];
                }
                aux=aux+2;//pular o : e o espaço
            }else{ //testa se há ou não variáveis
                int b=0;
                caracter=linha[b];
                while(b<tam){
                    if(caracter==':'){ //se há um :, há variáveis
                        b=-1;
                        break;
                    }else{
                        b++;
                        caracter=linha[b];
                    }
                }
                if(b==-1){ //pega label da variável
                    aux=0;
                    caracter=linha[aux];
                    while(caracter!=':'){
                        ins[cont].label=ins[cont].label+caracter; //o '+' em c++ concatena as strings. Vamos adicionando os caracteres do label
                        aux++;
                        caracter=linha[aux];
                    }
                    aux=aux+2;//pular o : e o espaço
                }
            }
            caracter=linha[aux]; //pega o primeiro caracter caso não haja label e o caracter depois do : e do espaço caso haja
            while(aux<tam&&(caracter==' '||caracter=='\t')){ //Lógica para dar suporte ao caso em que há mais deum epaço seguidos, conforme pedido nas intruções do TP
                aux++;
                caracter=linha[aux];
            }
            while(caracter!=' '&&caracter!='\n'&&aux<tam&&caracter!='\t'){ //pega o opcode que vai ate antes do proximo espaço ou fim da linha
                ins[cont].opcode=ins[cont].opcode+caracter;
                aux++;
                caracter=linha[aux];
            }
            aux++;//pula o espaço
            caracter=linha[aux];
            while(aux<tam&&(caracter==' '||caracter=='\t')){ //remove todos os espaçamentos intermediarios
                aux++;
                caracter=linha[aux];
            }
            while(caracter!=' '&&aux<tam&&caracter!='\t'){ //pega o primeiro operando, caso ele exista caso contrario, teremos aux>= tam e nao entrara no while
                ins[cont].op1=ins[cont].op1+caracter;
                aux++;
                caracter=linha[aux];
            }
            if(ins[cont].op1=="IO"){//substitui o símbolo IO pela posição de memória correspondente à entrada e saída
                ins[cont].op1="254";
            }
            aux++; //pula o espaco
            caracter=linha[aux];
            while(aux<tam&&(caracter==' '||caracter=='\t')){//remove todos os espaçamentos intermediarios
                aux++;
                caracter=linha[aux];
            }
            while(caracter!=' '&&aux<tam&&caracter!='\t'){//pega o segundo operando caso ele exista
                ins[cont].op2=ins[cont].op2+caracter;
                aux++;
                caracter=linha[aux];
            }
            if(ins[cont].op2=="IO"){//substitui o símbolo IO pela posição de memória correspondente à entrada e saída
                ins[cont].op2="254";
            }
            aux++;//pula o espaço
            caracter=linha[aux];
            while(aux<tam&&(caracter==' '||caracter=='\t')){ //remove todos os espaçamentos intermediarios
                aux++;
                caracter=linha[aux];
            }
            while(caracter!=' '&&aux<tam&&caracter!='\t'){ //pega o primeiro operando, caso ele exista caso contrario, teremos aux>= tam e nao entrara no while
                ins[cont].op3=ins[cont].op3+caracter;
                aux++;
                caracter=linha[aux];
            }
            if(ins[cont].op3=="IO"){//substitui o símbolo IO pela posição de memória correspondente à entrada e saída
                ins[cont].op3="254";
            }
            if(ins[cont].label!="\0"&&ins[cont].opcode==".data"){ //se há label, coloca-o na tabela de símbolos
                tabela[tamts].label=ins[cont].label;
                tabela[tamts].inst=ins[cont].opcode;
                tabela[tamts].pc=cont*2;
                int auxiliar = tabela[tamts].pc;
                tabela[tamts].datatam = convert2par(atoi(ins[auxiliar / 2].op1.c_str()));
                tabela[tamts].datapos = posmem;
                posmem = posmem + tabela[tamts].datatam;
                tamts++; //incrementa tamanho da tabela de símbolos
            }
            if(ins[cont].opcode=="call"){ //calls chamam procedimentos
                int b=buscaprocedimentos(procedimentos, ins[cont].op1); //procura se tal procedimento já foi buscado previamente em outra instução
                if(b==-1){ //senão, armazena seu nome
                    procedimentos[nprocedimentos]=ins[cont].op1;
                    nprocedimentos++;
                }
            }
            cont++; //incrementa o contador dos vetores de strings
        }
    }
    arq.close();
    int ndatas=contndatas(ins,nlinhas); //número de pseudo-intrucoes .data
    int nfuncoes= contchamadasdefuncoes(ins,nlinhas); //número de funções
    int nretornos=contret(ins,nlinhas);
    int nlinhas2=nlinhas+nfuncoes+nretornos+ndatas+1; //tamanho do novo "arquivo" de intruções
    instrucoes* inst2=new instrucoes[nlinhas2]; //novo "arquivo" de instruções
    //Agora precisamos substituir as pseudo-instruções .data por gravações na memória do valor inicial desejado.
    //Colocaremos estas gravações no inicio do arquivo, de forma a não prejudicar nenhum futuro acesso às variáveis.
    //Como não é possível gravar diretamente um imediato na memória, vamos usar o registrador 0 como auxiliar nesta esta operação.
    //Assim, tal operação necessitará de duas intruções.
    //Uma vez que as gravações serão no início do código, nenhum dado que possa estar no registrador 1 ser´a sobrescrito.
    //Ele será limpo após a operação.
    int auxiliar=0;
    int j=0;
    for(int i=0;i<nlinhas;i++){
        if(ins[i].opcode==".data"){ //se for uma pseudo-instrução.data
            j=buscatabela(tabela,ins[i].label); //encontra a posição na tabela de simbolos da mesma
            //armazenando constante desejada em R0:
            inst2[auxiliar].opcode="loadc";
            inst2[auxiliar].op1="R0";
            inst2[auxiliar].op2=ins[i].op2;
            auxiliar++;
            //transferindo valor de R0 para a memória
            inst2[auxiliar].opcode="storei";
            inst2[auxiliar].op1="R0";
            inst2[auxiliar].op2=to_string(tabela[j].datapos);
            auxiliar++;
        }
    }
    //limpando registrador R0
    inst2[auxiliar].opcode="clear";
    inst2[auxiliar].op1="R0";
    auxiliar++;
    //Agora, precisamos tratar as chamadas de função.
    //Como as chamadas de função não possuem argumentos, tudo o que precisamos é empilhar a posição de retorno na pilha
    // antes de desviar para a mesma e no final voltar para a posição de retorno
    for(int i=0;i<nlinhas;i++){
        if(ins[i].opcode!=".data"){//as pseudosinstruções .data ja foram tratadas
            if(buscaprocedimentos(procedimentos,ins[i].label)!=-1){ //trata as chamadas de procedimento
                inst2[auxiliar].label=ins[i].label;
                inst2[auxiliar].opcode="storeRa"; //armazena aa em sp
                inst2[auxiliar].op1="0";
                auxiliar++;
                //inst2[auxiliar].opcode="moveSp"; //aumenta pilha (dimunuindo o sp)
                //inst2[auxiliar].op1="-2";
                //auxiliar++;
                inst2[auxiliar]=ins[i];
                inst2[auxiliar].label="\0";
                auxiliar++;
            }else{
                if(ins[i].opcode=="ret"){ //trata retorno de procedimentos
                    inst2[auxiliar].opcode="loadRa"; //carrega sp em ra
                    inst2[auxiliar].op1="0";
                    auxiliar++;
                    //inst2[auxiliar].opcode="moveSp"; //diminui pilha(aumentando sp)
                    //inst2[auxiliar].op1="2";
                    //auxiliar++;
                    inst2[auxiliar]=ins[i];
                    auxiliar++;
                }else{ //trata demais instruções
                    inst2[auxiliar]=ins[i];
                    auxiliar++;
                }
            }

        }
    }

    for(int i=0;i<nlinhas2;i++){ //atualiza tabela de símbolos
        if(inst2[i].label!="\0"){ //se há label, coloca-o na tabela de símbolos
            tabela[tamts].label=inst2[i].label;
            tabela[tamts].inst=inst2[i].opcode;
            tabela[tamts].pc=i*2;
            tamts++; //incrementa tamanho da tabela de símbolos
        }
    }

    for(int i=0;i<nlinhas2;i++){ //faz ligações referentes aos labels
        int aux1=buscatabela(tabela,inst2[i].op1); //procura se o operando 1 é um label
        if(aux1!=-1){ //se for um label
            if(tabela[aux1].inst==".data"){ //se a instrução for do tipo data, substitui pela posição de memória do dado
                string auxiliar=to_string(tabela[aux1].datapos);
                inst2[i].op1=auxiliar;
            }else { //se não, substitui pelo opcode da instrução
                if (inst2[i].opcode == "jump"||inst2[i].opcode == "call") {
                    inst2[i].op1 = to_string(tabela[aux1].pc);
                }
            }
        }

        int aux2=buscatabela(tabela,inst2[i].op2);//procura se o operando 2 é um label
        if(aux2!=-1){ //se for label
            if(tabela[aux2].inst==".data"){ //se a instrução for do tipo data, substitui pela posição de memória do dado
                string auxiliar=to_string(tabela[aux2].datapos);
                inst2[i].op2=auxiliar;
            }else {//se não, substitui pelo opcode da instrução
                if (inst2[i].opcode == "jmpz"||inst2[i].opcode == "jmpn"||inst2[i].opcode == "jmpp") {
                    inst2[i].op2 = to_string(tabela[aux2].pc);
                }
            }
        }
    }
    /*O montador terá dois arquivos de saída. Um com as instruções já montadas que terá o mesmo nome que o arquivo original.
     O segundo terá a tabela de símbolos gerada e seu nome será tabsim+nome do arquivo original.
     Ambos os arquivos gerados serão salvos na pasta ArquivosMontador dentro da pasta do montador.*/
    string caminhoarquivosaida="../tst"; //Pasta em que serão salvos os arquivos
    string nomearquivosaida=nomearquivo;
    saida.open(caminhoarquivosaida+"/"+nomearquivosaida+".o"); //Arquivo em que serão salvos as instruções montadas
    for(int i=0;i<nlinhas2;i++){ //Criação do arquivo com o separador - entre os campos
        saida<<inst2[i].label<< "-"<<inst2[i].opcode<<"-"<<inst2[i].op1<<"-"<<inst2[i].op2<<"-"<<inst2[i].op3<<endl;
    }
    saida.close(); //fechamento do arquivo
    saida.open(caminhoarquivosaida+"/tabsim"+nomearquivosaida+".o"); //abertura do aquivo em que será salva a tabela de símbolos
    for(int i=0;i<tamts;i++){ //Criação do arquivo com o separador - entre os campos
        saida<< tabela[i].label<<"-"<<tabela[i].inst<<"-"<<tabela[i].pc<<endl;
    }
    saida.close(); //fechamento do arquivo
    //Imprimindo os dados para fins de visulização e teste
    cout<<"----------"<<endl;//imprime os labels para fins de visualização e teste
    cout<<"Tabela de símbolos: "<<endl;
    for(int i=0;i<tamts;i++){
        cout<< tabela[i].label<<" - "<<tabela[i].inst<<" - "<<tabela[i].pc<<" - "<<tabela[i].datapos<<" - "<<tabela[i].datatam<<endl;
    }
    cout<<"----------------"<<endl;
    cout<<"Versão final assembly"<<endl;
    for(int i=0;i<nlinhas2;i++){
        cout<<inst2[i].label<< " - "<<inst2[i].opcode<<" - "<<inst2[i].op1<<" - "<<inst2[i].op2<<" - "<<inst2[i].op3<<endl;
    }
    cout<<"----------------"<<endl;
    cout<<"Procedimentos:"<<endl;
    for(int i=0;i<nprocedimentos;i++){
        cout<<procedimentos[i]<<endl;
    }
    //Liberando a memória
    delete[] ins;
    delete[] inst2;
    delete[] procedimentos;
    return 0;
}