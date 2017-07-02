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
#include "geradorMIF.h"

using namespace std;

typedef struct{ //struct que receberá os campos de cada instrução
    string label="\0"; //Label da instrução. Valerá \0 caso não exista
    string opcode="\0"; //opcode da instrução
    string op1="\0"; //Operador 1 da instrução. Valerá \0 caso não exista
    string op2="\0"; //Operador 2 da instrução. Valerá \0 caso não exista
    string op3="\0"; //Operador 3 da instrução. Valerá \0 caso não exista
} instrucoes;

typedef struct{ //Tabela que será utilizada no tratamento das pseudoinstruções .extern. Elas são semelhantes a calls, mas usando labels de outros módulos
    string label="\0"; //recerá o label chamado pelo extern
    string nomearquivochamada="\0"; //receberá o nome do arquivo que contém a chamada .extern
    int larquivochamada=-1; //receberá a linha do arquivo que contém a chamada .extern
    string nomearquivofuncao="\0"; //receberá o nome do arquivo que contém o label chamado pela função .extern
    int larquivofuncao=-1; //receberá a linha do arquivo que contém tal label
    int lret=-1; //receberpa a linha do arquivo do label que contém a instrução ret
}tabeladeextern;

typedef struct{ //struct responsável por implementar a tabela de símbolos. Um novo campo foi adicionado com relação à tabela de símbolos do montador, o inteiro novopc
    int pc=-1; //contém o pc da montagem
    int novopc=-1; //receberá o novopc correspondente às instrução chamada na pós-ligação
    string inst="\0"; //contém o opcode da instrução
    string label="\0"; //contém o label
    string arquivoorigem="\0"; //contém o arquivo do qual o símbolo de origina
}tabeladesimbolos;

//Função que faz uma busca na tabela de símbolos, tentando encontrar um pc igual ao valor passado no parâmetro k
// desde que o arquivo de origem também seja igual e
// //retorna o novopc correpondente ou -1 em caso de não encontrá-lo
int buscapcantigo (tabeladesimbolos* ts, int tam, int l,string arquivo){
    for(int i=0;i<tam;i++){ //procura po um pc igual a l
        if(ts[i].pc==l&&ts[i].arquivoorigem==arquivo){
            return (ts[i].novopc); //se acha retorna novopc correspondente
        }
    }
    return -1;
}

//Função que busca no vetor de instruções um determinado label. Retorna a linha do vetor em que o label foi encontrado
//ou -1 em caso de erro
int buscalabelextern(instrucoes** ins, int nlinhas, int modulo, string labelprocurado){ //Função que procura um dado label em um vetor de instruções. Auxiliará no tratamento da pseudoinstrução .extern
    for(int i=0;i<nlinhas;i++){
        if(ins[i][modulo].label==labelprocurado){ //se acha o label procurado no vetor de instrucoes, retorna a linha da tabela
            return i;
        }
    }
    return -1; //senão, retorna -1
}

//Função que busca na tabela de externs uma determinada linha no campo larquivochamada
//Retorna a linha da tabela de externs correspondente se encontrar e -1 caso contrário
int buscaposlarquivochamada(tabeladeextern* te, int tam, int linha){
    for(int i=0;i<tam;i++){
        if(te[i].larquivochamada==linha){ //procura linha no campo larquivochamada de te
            return i; //se acha, retorna a linha de te correspondente
        }
    }
    return -1; //senão, retorna -1
}


//Função que busca um determinado label na tabela de externs te
//Retorna true se encontra e false caso contrário
bool buscalabel(tabeladeextern* te, int tam, string labelprocurado){
    for(int i=0;i<tam;i++){
        if(te[i].label==labelprocurado){ //se acha o label, retorna true
            return true;
        }
    }
    return false; //senão, retorna false
}


//Função que busca um determinado label na tabela de símbolos ts
//Caso encontre o label, retorna a linha de ts em que ele foi encontrado. Caso contrário, retorna -1
int buscalabelts(tabeladesimbolos* ts, int tam, string label){
    for(int i=0;i<tam;i++){
        if(ts[i].label==label){ //se encontra o label, retorna a linha de ts correpondente
            return i;
        }
    }
    return -1;
}


//Funçao que busca um determinado label na tabela de símbolos dos modulos
//Caso encontre, retorna a linha correspondente
//Caso contrário, retorna -1
int buscalabeltsmodulos(tabeladesimbolos** ts, int nlinhas, int modulo, string label){
    for(int i=0;i<nlinhas;i++){
        if(ts[i][modulo].label==label){ //se encontra, retorna a linha correspondente
            return i;
        }
    }
    return -1;//senão, retorna -1
}

//Função que procura se um dado label já existe na tabela de externs. Se sim, retorna a linha de te correspondente. Senão, retorna -1
int buscaposlabel(tabeladeextern* te, int tam, string labelprocurado){
    for(int i=0;i<tam;i++){
        if(te[i].label==labelprocurado){ //se acha o label, retorna a linha correpondente
            return i;
        }
    }
    return -1; //senão, retorna -1
}

//Função que procura uma dada linha entre as linhas de ret da tabela de externs te. Se encontra, retorna true. Retorna false caso contrário
bool buscaret (tabeladeextern* te, int tam, string nome, int linha){
    for(int i=0;i<tam;i++){
        if(te[i].lret==linha && te[i].nomearquivofuncao==nome){ //procura linha em lret de te, desde que o arquivo do ret seja o mesmo
            return true;//retorna true caso encontre
        }
    }
    return false;//retorna false caso contrário
}

//Função que procura uma dada linha entre as linhas de ret da tabela de externs te. Se encontra, retorna a linha. Retorna -1 caso contrário
int buscaposret (tabeladeextern* te, int tam, string nome, int linha){
    for(int i=0;i<tam;i++){
        if(te[i].lret==linha && te[i].nomearquivofuncao==nome){//procura linha em lret de te, desde que o arquivo do ret seja o mesmo
            return i; //retorna a linha caso encontre
        }
    }
    return -1;//retorna -1 caso contrário
}

//Função que retorna o maior elemento de um vetor
int maior(int* vet, int tam){
    int n=-1;
    for(int i=0;i<tam;i++){
        if(vet[i]>n){
            n=vet[i];
        }
    }
    return n;
}

//Função usada pra buscar o arquivo do qual uma dada posição do arquivo final é proveniente com base nas variáveis fimp e fimm que
// armazenam a última posição proveniente do arquivo principal e de cada um dos módulos, respectivamente
//A função retorna -1 se a linha for proveniente do arquivo principal e o número do módulo correspondente se for
//proveniente de um módulo
int buscaarquivoproveniente(int i,int fimp,int *fimm, int nummodulos,string nomeprincipal, string*nomesmodulos){
    if(i<=fimp){
        return -1;
    }
    for(int j=0;j<nummodulos;j++){
        if(i<=fimm[j]){
            return j;
        }
    }
}

//Função que converte um registrador de assembly para binario
string convertereg(string registrador){
    if(registrador=="R0"){
        return "000";
    }
    if(registrador=="R1"){
        return "001";
    }
    if(registrador=="R2"){
        return "010";
    }
    if(registrador=="R3"){
        return "011";
    }
    if(registrador=="R4"){
        return "100";
    }
    if(registrador=="R5"){
        return "101";
    }
    if(registrador=="R6"){
        return "110";
    }
    if(registrador=="R7"){
        return "111";
    }
}

//Função que converte uma posição de memoria de assembly para binario
string convertemem (string memoria){
    int num=atoi(memoria.c_str());
    int div[8];
    for(int i=7;i>=0;i--){
        div[i]=num/pow(2,i);
        if(div[i]==1){
            num=num-pow(2,i);
        }
    }
    string conversao="\0";
    for(int i=7;i>=0;i--){
        conversao=conversao+to_string(div[i]);
    }
    return conversao;
}

//Função que converte uma constante de assembly para binario com complemento de 2
string converteconst(string constante){
    int numsinal=atoi(constante.c_str());
    int num=abs(numsinal);
    int div[8];
    for(int i=7;i>=0;i--){
        div[i]=num/pow(2,i);
        if(div[i]==1){
            num=num-pow(2,i);
        }
    }
    if(numsinal<0) { //se for menor que zero faz complemento de 2
        for (int i = 0; i < 8; i++) {
            if (div[i] == 0) {
                div[i] = 1;
            } else {
                div[i] = 0;
            }
        }
        for (int i = 0; i < 8; i++) {
            if (div[i] == 0) {
                div[i] = 1;
                break;
            } else {
                div[i] = 0;
            }
        }
    }
    string conversao="\0";
    for(int i=7;i>=0;i--){
        conversao=conversao+to_string(div[i]);
    }
    return conversao;
}

//Uso de argc e argv estudado via: https://study.cs50.net/argv
int main (int argc, char* argv[]) { //argc será o número de parâmetros e argv um vetor com os nomes dos arquivos.
    //argv[0] é o nome do programa na chamada
    string nomeprincipal = argv[2]; //nome do módulo principal é o primeiro parâmetro da chamada do ligador
    int numerodemodulos=argc-3; //o número de módulos (excluindo o principal) é argc-3 (1 é a saida.mif)
    string* nomesmodulos=new string[numerodemodulos]; //vetor que receberá os nomes dos demais módulos
    for(int i=0;i<numerodemodulos;i++){ //preenchimento do vetor
        nomesmodulos[i]=  argv[i+3];

    }

    string caminhoarquivos = "../tst"; //Pasta em que o montador salva os arquivos por ele gerados. Não precisa ser mudada, a menos que as especificações sejam outras
    ifstream arq; //Leitura dos arquivos
    string linha; //Variável auxiliar para a leitura das linhas
    int nlinhasp=0; //Número de linhas do arquivo principal
    arq.open(caminhoarquivos + "/" + nomeprincipal); //Abre arquivo principal
    //Contagem do número de linhas do arquivo principal
    if (arq.is_open()) { //testa  abertura do arquivo
        while (!arq.eof()) { //loop que percorre o arquivo
            getline(arq, linha); //lê linha por linha
            if (linha != " " && linha != "" && linha != "\t" && linha!= "\n" &&linha != "\0") { //conta as linhas não-vazias
                nlinhasp++; //conta linhas
            }
        }
    } else {
        cout << "Unable to open file";
        return 0;
    }
    arq.close(); //fecha arquivo com o principal
    int* nlinhasm=new int[numerodemodulos]; //Vetor com o número de linhas de cada um dos módulos
    for(int i=0;i<numerodemodulos;i++){ //zera o vetor para depois realizar a contagem
        nlinhasm[i]=0;
    }
    //Contagem do número de linhas de cada um dos módulos
    for(int i=0;i<numerodemodulos;i++) { //loop que percorre cada um dos módulos
        arq.open(caminhoarquivos + "/" + nomesmodulos[i]); //abre módulo
        if (arq.is_open()) { //testa  abertura do arquivo
            while (!arq.eof()) { //loop que percorre o arquivo
                getline(arq, linha); //lê linha por linha
                if (linha != " " && linha != "" && linha != "\t" && linha != "\n" &&
                    linha != "\0") { //conta as linhas não-vazias
                    nlinhasm[i]++; //conta linhas
                }
            }
        } else {
            cout << "Unable to open file";
            return 0;
        }
        arq.close(); //fecha arquivo do módulo
    }
    instrucoes* insp=new instrucoes[nlinhasp]; //vetor struct de instrucoes que receberá as intruções(linhas) do módulo principal
    int maiornlinhasm=maior(nlinhasm, numerodemodulos); //recebe o maior número de linhas dentre o número de linhas dos módulos
    instrucoes **insm= new instrucoes *[maiornlinhasm]; //Alocação dinâmica da matriz que receberá as instruções de cada módulo
    for(int i=0;i<maiornlinhasm;i++){
        insm[i]=new instrucoes[numerodemodulos];
    }
    arq.open(caminhoarquivos+"/"+nomeprincipal); //arquivo principal reaberto
    int n=0; //variável auxiliar para a leitura
    //Leitura do arquivo principal
    while (!arq.eof()) { //lê o arquivo principal novamente
        getline(arq, linha); //lê linha por linha
        if (linha != " " && linha != "" && linha != "\t" && linha != "\n" &&
            linha != "\0") { //se a linha não for vazia, armazena a linha
            int a = 0; //usado ṕara ler caracter por caracter da linha de forma a extrair cada um dos campos
            //Os campos são delimitados pelo montador por meio do caracter de separação -
            int tam = linha.size(); //tam recebe o número de caracteres da linha sendo lida
            while (linha[a] != '-') { //leitura do campo do label
                insp[n].label = insp[n].label + linha[a];
                a++;
            }
            a++; //Pula -
            while (linha[a] != '-') { //Leitura do campo do opcode
                insp[n].opcode = insp[n].opcode + linha[a];
                a++;
            }
            a++; //Pula -
            while (linha[a] != '-') { //Leitura do campo do operador 1
                insp[n].op1 = insp[n].op1 + linha[a];
                a++;
            }
            a++; //Pula -
            while (linha[a] != '-') { //Leitura do campo do operador 2
                insp[n].op2 = insp[n].op2 + linha[a];
                a++;
            }
            a++; //Pula -
            while (a < tam) { //Leitura do campo do operador 3
                insp[n].op3 = insp[n].op3 + linha[a];
                a++;
            }
            n++; //Incrementa contador
        }
    }
    arq.close();//fecha arquivo principal
    //Leitura dos módulos
    for(int i=0;i<numerodemodulos;i++) {
        n = 0; //zera auxiliar para leitura do módulo
        arq.open(caminhoarquivos + "/" + nomesmodulos[i]); //arquivo do módulo reaberto
        while (!arq.eof()) { //lê o arquivo módulo novamente
            getline(arq, linha); //lê linha por linha
            if (linha != " " && linha != "" && linha != "\t" && linha != "\n" && linha != "\0") { //Caso a linha não seja vazia, a armazena
                int a = 0; //usado ṕara ler caracter por caracter da linha de forma a extrair cada um dos campos
                //Os campos são delimitados pelo montador por meio do caracter de separação -
                int tam = linha.size(); //tam recebe o número de caracteres da linha sendo lida
                while (linha[a] != '-') { //Leitura do campo do label
                    insm[n ][i].label = insm[n][i].label + linha[a];
                    a++;
                }
                a++; //Pula -
                while (linha[a] != '-') { //Leitura do campo do opcode
                    insm[n][i].opcode = insm[n][i].opcode + linha[a];
                    a++;
                }
                a++; //Pula -
                while (linha[a] != '-') { //Leitura do campo do operador 1
                    insm[n][i].op1 = insm[n][i].op1 + linha[a];
                    a++;
                }
                a++;  //Pula -
                while (linha[a] != '-') { //Leitura do campo do operador 2
                    insm[n][i].op2 = insm[n][i].op2 + linha[a];
                    a++;
                }
                a++; //Pula -
                while (a < tam) { //Leitura do campo do operador 3
                    insm[n][i].op3 = insm[n][i].op3 + linha[a];
                    a++;
                }
                n++; //incrementa contador
            }
        }
        arq.close(); //fecha arquivo módulo
    }
    tabeladesimbolos* tsprincipal= new tabeladesimbolos[nlinhasp]; //vetor de structs que receberá a tabela de símbolos do arquivo principal
    tabeladesimbolos** tsmodulos=new tabeladesimbolos*[maiornlinhasm]; //alocação dinâmica da matriz de structs que receberá a tabela de símbolos dos módulos
    for(int i=0;i<maiornlinhasm;i++){
        tsmodulos[i]=new tabeladesimbolos[numerodemodulos];
    }
    arq.open(caminhoarquivos+"/tabsim"+nomeprincipal); //arquivo da tabela de símbolos principal aberto
    int ntsp=0; //variável que contará o tamanho da tabela de símbolos principal
    //Leitura da tabela de símbolos do arquivo principal
    while (!arq.eof()) { //lê o arquivo tabsim principal
        getline(arq, linha); //lê linha por linha
        if (linha != " " && linha != "" && linha != "\t" && linha != "\n" &&linha != "\0") { //se a linha não for vazia, armazena a linha;
            int a = 0; //usado ṕara ler caracter por caracter da linha de forma a extrair cada um dos campos
            //Os campos são delimitados pelo montador por meio do caracter de separação -
            int tam = linha.size(); //tam recebe o número de caracteres da linha sendo lida
            while (linha[a] != '-') { //leitura do campo do label
                tsprincipal[ntsp].label = tsprincipal[ntsp].label + linha[a];
                a++;
            }
            a++; //Pula -
            while (linha[a] != '-') { //Leitura do campo do opcode
                tsprincipal[ntsp].inst = tsprincipal[ntsp].inst + linha[a];
                a++;
            }
            a++; //Pula -
            string aux="\0";
            while (a<tam) { //Leitura do campo do operador 1
                aux =aux + linha[a];
                a++;
            }
            tsprincipal[ntsp].pc=atoi(aux.c_str());
            tsprincipal[ntsp].arquivoorigem=nomeprincipal; //recebe o arquivo de origem
            a++; //Pula -
            aux="\0"; //Instruções .data já tratatadas -> demais informações são desnecessárias
            ntsp++; //Incrementa contador
        }
    }
    arq.close();//fecha arquivo tabsim principal
    int* ntsm=new int[numerodemodulos]; //variável que contará o tamanho da tabela de símbolos de cada módulo
    //Leitura da tabela de símbolos dos módulos
    for(int i=0;i<numerodemodulos;i++) {
        arq.open(caminhoarquivos + "/tabsim" + nomesmodulos[i]); //arquivo da tabela de símbolos do módulo aberto
        ntsm[i] = 0; //variável auxiliar para a leitura
        while (!arq.eof()) { //lê o arquivo tabsim do módulo novamente
            getline(arq, linha); //lê linha por linha
            if (linha != " " && linha != "" && linha != "\t" && linha != "\n" &&linha != "\0") { //se a linha não for vazia, armazena a linha;
                int a = 0; //usado ṕara ler caracter por caracter da linha de forma a extrair cada um dos campos
                //Os campos são delimitados pelo montador por meio do caracter de separação -
                int tam = linha.size(); //tam recebe o número de caracteres da linha sendo lida
                while (linha[a] != '-') { //leitura do campo do label
                    tsmodulos[ntsm[i]][i].label = tsmodulos[ntsm[i]][i].label + linha[a];
                    a++;
                }
                a++; //Pula -
                while (linha[a] != '-') { //Leitura do campo do opcode
                    tsmodulos[ntsm[i]][i].inst =
                            tsmodulos[ntsm[i]][i].inst + linha[a];
                    a++;
                }
                a++; //Pula -
                string aux = "\0";
                while (a < tam) { //Leitura do campo do operador 1
                    aux = aux + linha[a];
                    a++;
                }
                tsmodulos[ntsm[i]][i].pc = atoi(aux.c_str());
                tsmodulos[ntsm[i]][i].arquivoorigem=nomesmodulos[i]; //recebe o arquivo de origem
                a++; //Pula -
                aux = "\0";
                //Instruções .data já tratatadas -> demais informações são desnecessárias
                ntsm[i]++; //Incrementa contador
            }
        }
        arq.close();//fecha arquivo tabsim do módulo
    }
    //Etapa de criação de uma tabela das instruções extern de modo a facilitar seu tratamento
    //Iremos considerar, para fins de simplificação, que só o arquivo principal poderá ter pseudo-instruções .extern
    int nexternp=0; //contará o número de pseudoinstrucoes .extern do módulo principal
    tabeladeextern* exprincipal= new tabeladeextern[nlinhasp]; //criação da tabela de externs do módulo principal
    //Prenchimento da tabela do módulo principal
    int externsrepetidos=0; //conta chamadas repetidas a um mesmo extern para tratar vezes em que um mesmo label é chamado mais de uma vez via .extern
    //Será útil no cálculo do tamanho do vetor de instruções total, dado que a cada função externa uma linha extra será acrescentada para empilhamento da posição de retorno
    for (int i=0;i<nlinhasp;i++){ //Percorre o vetor principal de instruções em busca de .extern
        if(insp[i].opcode==".extern"){ //Se acha uma pseudoinstrução .extern, a armazena na tabela de externs
            exprincipal[nexternp].label=insp[i].op1; //Preenche o label
            exprincipal[nexternp].nomearquivochamada=nomeprincipal; //O arquivo que o chamou é o arquivo principal
            exprincipal[nexternp].larquivochamada=i; //A linha do extern é a linha em que se encontra a leitura do vetor
            //Agora, precisamos encontrar em qual dos módulos se encontra a label chamada via .extern
            int aux=-1; //receberá o número do módulo em que a label se encontra
            int k=-1; //receberá a linha do módulo em que a label se encontra
            for(int j=0;j<numerodemodulos;j++){ //percorre os módulos
                k=buscalabelextern(insm,nlinhasm[j],j,exprincipal[nexternp].label); //função que busca a label em um dado módulo
                if(k!=-1){ //se a label é encontrada, salva qual o módulo e interrompe a busca
                    aux=j;
                    break;
                }
            }
            exprincipal[nexternp].nomearquivofuncao=nomesmodulos[aux]; //salva o nome do módulo em que se encontra o label
            exprincipal[nexternp].larquivofuncao=k; //salva a linha do móculo em que se encontra o label
            //Agora, resta achar a linha do módulo em que se encontra o ret. Ela estará após a linha do labe
            for(int j=k+1;j<nlinhasm[aux];j++){ //acha linha de retorno procurando a partir da linha depois do label
                if(insm[j][aux].opcode=="ret"){
                    exprincipal[nexternp].lret=j;
                    break;
                }
            }
            //Contagem dos externs repetidos:
            if(buscalabel(exprincipal, nexternp, exprincipal[nexternp].label)){ //Se o label já estava cadastrado na tabela de externs, incrementa contador de externs repetidos
                externsrepetidos++;
            }
            nexternp++; //incrementa contador
        }
    }
    //tamtot é o número total de linhas de instruções que o arquivo final terá
    int linhasm=0; //variável que receberá o número total de linhas do módulo, para posteriormente calcularmos o número de linhas do arquivo final
    for(int i=0;i<numerodemodulos;i++){
        linhasm=linhasm+nlinhasm[i];
    }
    //O tamanho total do arquivo final será o tamanho total do arquivo principal+o tamanho total dos módulos+ o número de externs do
    //arquivo principal - o numero de externs repeitos -o número de módulos
    //O -numerodemodulos se deve ao fato de que não queremos repetir diversas vezez o clear R0 usado no tratamento de
    // //pseudo-instruções .data
    int tamtot=nlinhasp+linhasm+nexternp-externsrepetidos-numerodemodulos;
    instrucoes* ins=new instrucoes[tamtot]; //cria o vetor final de instruções
    //Armazenando todas as linhas:
    int cont=0; //contador que auxiliará no preenchimento do vetor
    int aux=0; //variável que contará o número de linhas até o primeiro clear R0 do módulo principal (delimita o tratamento das instruções .data que devem ficar no início do arquivo)
    while(insp[aux].opcode!="clear"){ //armazena até o primeiro clear do módulo principal
        ins[cont]=insp[aux];
        cont++;
        aux++;
    }
    int *aux2=new int[numerodemodulos];//variável que contará o número de linhas até o primeiro clear R0 de cada módulo (delimita o tratamento das instruções .data que devem ficar no início do arquivo)
    for(int i=0;i<numerodemodulos;i++) {
        aux2[i] = 0;
        while (insm[aux2[i]][i].opcode != "clear") { //armazena até o primeiro clear de cada módulo
            ins[cont] = insm[aux2[i]][ i];
            cont++;
            aux2[i]++;
        }
    }

    ins[cont]=insp[aux];//recebe apenas um clear R0
    cont++;
    //Armazenando demais linhas do módulo principal:
    for (int i=aux+1;i<nlinhasp;i++){ //percorre arquivo principal
        int aux = buscalabelts(tsprincipal, ntsp, insp[i].label); //procura se o label da instrução está sendo usado por alguma instrução do arquivo
        if (insp[i].label != "\0" && aux != -1) { //se sim, armazena a posição em que o label estará no novo arquivo para ser usado na ligação final
            tsprincipal[aux].novopc = cont * 2;
        }
        if (insp[i].opcode == ".extern") { //testa se é uma chamada .extern
            int l = buscaposlabel(exprincipal, nexternp, insp[i].op1); // se sim, busca a linha da tabela de externs correspondente e atualiza a linha do arquivo de chamada
            exprincipal[l].larquivochamada = cont;
            ins[cont] = insp[i];
            cont++;
        } else { //se for alguma outra instrução comum, apenas a armazena
            ins[cont] = insp[i];
            cont++;
        }

    }
    int fimp=cont-1;
    int* fimm=new int[numerodemodulos];
    //Armazenando demais linhas dos módulos
    for(int ii=0;ii<numerodemodulos;ii++){//percorre os módulos
        for (int i=aux2[ii]+1;i<nlinhasm[ii];i++){ //percorre as linhas restantes de cada módulo
            int k=buscaposlabel(exprincipal, nexternp, insm[i][ii].label); //busca se o label da minha está na tabela de externs
            if(k!=-1&&insm[i][ii].label!="\0"){ //se sim, é uma linha a ser chamada por um extern e precisamos empilhar o valor de retorno
                exprincipal[k].larquivofuncao=cont; //atualiza linha do arquivo em que há a função
                //Instrução de empilhamento do valor de retorno:
                ins[cont].label=insm[i][ii].label;
                ins[cont].opcode="storeRa"; //armazena aa em sp
                ins[cont].op1="0";
                cont++;
                //Instrução inicial da função
                ins[cont]=insm[i][ii];
                ins[cont].label="\0";
                cont++;
            }else {
                int aux = buscalabeltsmodulos(tsmodulos, ntsm[ii], ii, insm[i][ii].label); //busca label na tabela de labels do módulo em questão
                if (insm[i][ii].label != "\0" && aux != -1) { //se o label está sendo usado por alguma instrução, atualiza o novo pc
                    tsmodulos[aux][ii].novopc = cont*2;
                }
                if (insm[i][ii].opcode == "ret" && buscaret(exprincipal, nexternp, nomesmodulos[ii], i)) {//se for uma instrução ret, atualiza a linha de retorno na tabela de externs
                    int j = buscaposret(exprincipal, nexternp, nomesmodulos[ii], i);
                    exprincipal[j].lret = cont;
                    ins[cont] = insm[i][ii];
                    cont++;
                } else { //se for uma instrução comum, armazena a intrução normalmente
                    ins[cont] = insm[i][ii];
                    cont++;
                }
            }
        }
        fimm[ii]=cont-1;
    }
    //Para fins de simplificação, vamos criar uma tabela de símbolos geral com as tabelas de símbolos do arquivo principal e dos modulos sem as instruções .data
    int tammodulos=0; //variaével que receberá a soma do tamanho da tabela de símbolos dos módulos
    for (int i=0;i<numerodemodulos;i++){ //fazendo a soma
        tammodulos=tammodulos+ntsm[i];
    }
    tabeladesimbolos* ts= new tabeladesimbolos[ntsp+tammodulos];//Tabela de símbolos "geral"
    int cont2=0; //contador que auxiliará no preenchimento de ts
    //Preenchimento de ts com a tabela de símbolos principal:
    for(int i=0;i<ntsp;i++){
        if(tsprincipal[i].inst!=".data") {
            ts[cont2] = tsprincipal[i];
            cont2++;
        }
    }
    //Preenchimento de ts com as tabelas de símbolos dos módulos:
    for(int ii=0;ii<numerodemodulos;ii++) {
        for (int i = 0; i < ntsm[ii]; i++) {
            if(tsmodulos[i][ii].inst!=".data") {
                ts[cont2] = tsmodulos[i][ii];
                cont2++;
            }
        }
    }
    //Mudança dos pcs de instruções de jumps, calls, etc que são afetadas pela ligação e tratamento da instrução .extern
    for(int i=0;i<tamtot;i++){
        if(ins[i].opcode==".extern"){ //se for uma instrução .extern, será transformada em um call para a linha da função chamada
            ins[i].opcode="call";
            int j=buscaposlabel(exprincipal,nexternp,ins[i].op1);
            ins[i].op1=to_string(exprincipal[j].larquivofuncao*2);
        }else{
            if(ins[i].opcode=="jump"||ins[i].opcode=="call"){ //se for um jump ou um call, o operador 1 deverá ser alterado pelo novo pc da linha
                int b=buscaarquivoproveniente(i,fimp,fimm,numerodemodulos,nomeprincipal,nomesmodulos);
                string a; //a reberá o nome do arquivo de origem a ser usado na função buscapcantigo
                if(b==-1){
                    a=nomeprincipal;
                }else{
                    a=nomesmodulos[b];
                }
                int j=buscapcantigo (ts,cont2, atoi(ins[i].op1.c_str()),a); //instrução que busca um pc antigo e retorna o pc novo
                ins[i].op1 = to_string(j);
            }else{
                if (ins[i].opcode == "jmpz"||ins[i].opcode == "jmpn"||ins[i].opcode == "jmpp"){ //se for um jmpz, jmpn ou jmpp, o que deve ser substituido é o operador 2
                    int b=buscaarquivoproveniente(i,fimp,fimm,numerodemodulos,nomeprincipal,nomesmodulos);
                    string a; //a reberá o nome do arquivo de origem a ser usado na função buscapcantigo
                    if(b==-1){
                        a=nomeprincipal;
                    }else{
                        a=nomesmodulos[b];
                    }
                    int j=buscapcantigo (ts, cont2, atoi(ins[i].op2.c_str()),a);//instrução que busca um pc antigo e retorna o pc novo
                    ins[i].op2 = to_string(j);
                }
            }
        }
    }
    //Impressão do arquivo ligado final fins para fins de visualização
    cout<<"Versão final:"<<endl;
    cout<<"------------------"<<endl;
    for(int i=0;i<tamtot;i++){
        cout<<ins[i].label<< " - "<<ins[i].opcode<<" - "<<ins[i].op1<<" - "<<ins[i].op2<<" - "<<ins[i].op3<<endl;
    }
    cout<<"------------------"<<endl;
    ofstream saida;
    string caminhoarquivosaida="../tst";
    string nomearquivosaida="saida.o"; //arquivo ligado final será escrito em saida.o
    saida.open(caminhoarquivosaida+"/"+nomearquivosaida); //abertura do arquivo de saída
    //Conversão de assembly para binário segundo os padrões do anexo 1 do TP1 e escrita no arquivo de saída
    for(int i=0;i<tamtot;i++){
        if(ins[i].opcode=="exit"){
            saida<<"0000000000000000\n";
        }else{
            if(ins[i].opcode=="loadi"){
                saida<<"00001";
                saida<<convertereg(ins[i].op1);
                saida<<convertemem(ins[i].op2)<<endl;
            }else{
                if(ins[i].opcode=="storei"){
                    saida<<"00010";
                    saida<<convertereg(ins[i].op1);
                    saida<<convertemem(ins[i].op2)<<endl;
                }else{
                    if(ins[i].opcode=="add"){
                        saida<<"00011";
                        saida<<convertereg(ins[i].op1);
                        saida<<convertereg(ins[i].op2)<<"00000"<<endl;
                    }else{
                        if(ins[i].opcode=="subtract"){
                            saida<<"00100";
                            saida<<convertereg(ins[i].op1);
                            saida<<convertereg(ins[i].op2)<<"00000"<<endl;
                        }else{
                            if(ins[i].opcode=="multiply"){
                                saida<<"00101";
                                saida<<convertereg(ins[i].op1);
                                saida<<convertereg(ins[i].op2)<<"00000"<<endl;
                            }else{
                                if(ins[i].opcode=="divide"){
                                    saida<<"00110";
                                    saida<<convertereg(ins[i].op1);
                                    saida<<convertereg(ins[i].op2)<<"00000"<<endl;
                                }else{
                                    if(ins[i].opcode=="jump"){
                                        saida<<"00111";
                                        saida<<"000"<<convertemem(ins[i].op1)<<endl;
                                    }else{
                                        if(ins[i].opcode=="jmpz"){
                                            saida<<"01000";
                                            saida<<convertereg(ins[i].op1);
                                            saida<<convertemem(ins[i].op2)<<endl;
                                        }else{
                                            if(ins[i].opcode=="jmpn"){
                                                saida<<"01001";
                                                saida<<convertereg(ins[i].op1);
                                                saida<<convertemem(ins[i].op2)<<endl;
                                            }else{
                                                if(ins[i].opcode=="move"){
                                                    saida<<"01010";
                                                    saida<<convertereg(ins[i].op1);
                                                    saida<<convertereg(ins[i].op2)<<"00000"<<endl;
                                                }else{
                                                    if(ins[i].opcode=="load"){
                                                        saida<<"01011";
                                                        saida<<convertereg(ins[i].op1);
                                                        saida<<convertereg(ins[i].op2)<<"00000"<<endl;
                                                    }else{
                                                        if(ins[i].opcode=="store"){
                                                            saida<<"01100";
                                                            saida<<convertereg(ins[i].op1);
                                                            saida<<convertereg(ins[i].op2)<<"00000"<<endl;
                                                        }else{
                                                            if(ins[i].opcode=="loadc"){
                                                                saida<<"01101";
                                                                saida<<convertereg(ins[i].op1);
                                                                saida<<converteconst(ins[i].op2)<<endl;
                                                            }
                                                            else{
                                                                if(ins[i].opcode=="clear"){
                                                                    saida<<"01110";
                                                                    saida<<convertereg(ins[i].op1)<<"00000000"<<endl;
                                                                }else{
                                                                    if(ins[i].opcode=="moveSp"){
                                                                        saida<<"01111";
                                                                        saida<<"000"<<converteconst(ins[i].op1)<<endl;
                                                                    }else{
                                                                        if(ins[i].opcode=="slt"){
                                                                            saida<<"10000";
                                                                            saida<<convertereg(ins[i].op1);
                                                                            saida<<convertereg(ins[i].op2);
                                                                            saida<<convertereg(ins[i].op3)<<"00"<<endl;
                                                                        }else{
                                                                            if(ins[i].opcode=="call"){
                                                                                saida<<"10001";
                                                                                saida<<"000"<<convertemem(ins[i].op1)<<endl;
                                                                            }else{
                                                                                if(ins[i].opcode=="loadSp"){
                                                                                    saida<<"10010";
                                                                                    saida<<convertereg(ins[i].op1);
                                                                                    saida<<converteconst(ins[i].op2)<<endl;
                                                                                }else{
                                                                                    if(ins[i].opcode=="storeSp"){
                                                                                        saida<<"10011";
                                                                                        saida<<convertereg(ins[i].op1);
                                                                                        saida<<converteconst(ins[i].op2)<<endl;
                                                                                    }else{
                                                                                        if(ins[i].opcode=="ret"){
                                                                                            saida<<"1010000000000000"<<endl;
                                                                                        }else{
                                                                                            if(ins[i].opcode=="loadRa"){
                                                                                                saida<<"10101";
                                                                                                saida<<"000"<<converteconst(ins[i].op1)<<endl;
                                                                                            }else{
                                                                                                if(ins[i].opcode=="storeRa"){
                                                                                                    saida<<"10110";
                                                                                                    saida<<"000"<<converteconst(ins[i].op1)<<endl;
                                                                                                }else{
                                                                                                    if(ins[i].opcode=="addi"){
                                                                                                        saida<<"10111";
                                                                                                        saida<<convertereg(ins[i].op1);
                                                                                                        saida<<converteconst(ins[i].op2)<<endl;
                                                                                                    }else{
                                                                                                        if(ins[i].opcode=="sgt"){
                                                                                                            saida<<"11000";
                                                                                                            saida<<convertereg(ins[i].op1);
                                                                                                            saida<<convertereg(ins[i].op2);
                                                                                                            saida<<convertereg(ins[i].op3)<<"00"<<endl;
                                                                                                        }else{
                                                                                                            if(ins[i].opcode=="seq"){
                                                                                                                saida<<"11001";
                                                                                                                saida<<convertereg(ins[i].op1);
                                                                                                                saida<<convertereg(ins[i].op2);
                                                                                                                saida<<convertereg(ins[i].op3)<<"00"<<endl;
                                                                                                            }else{
                                                                                                                if(ins[i].opcode=="jmpp"){
                                                                                                                    saida<<"11010";
                                                                                                                    saida<<convertereg(ins[i].op1);
                                                                                                                    saida<<convertemem(ins[i].op2)<<endl;
                                                                                                                }
                                                                                                            }
                                                                                                        }
                                                                                                    }
                                                                                                }
                                                                                            }
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    saida.close(); //Fecha o arquivo de saída
    //Liberando a memória
    delete[] ts;
    delete[] tsmodulos;
    delete[] tsprincipal;
    delete[] exprincipal;
    delete [] insp;
    delete[] insm;
    delete[] ins;
    delete[] nlinhasm;
    delete[] ntsm;
    gerarMIF(tamtot, argv[1]); //chama o gerador MIF
    return 0;
}