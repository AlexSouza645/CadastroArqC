#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<locale.h>
#include<string.h>


//Struct para cadastro de login e senha do usu�rio
typedef struct CadastroU {
    char login[12];
    char senha[12];
}CadastroU;
CadastroU *NovoUser;

//Struct para o endere�o
typedef struct CadEnd{
    char Rua[50];
    int Numero;
    char Bairro[50];
    char Cidade[50];
    char Estado[2];
    char CEP[9];
}CadEnd;

//Struct para Data
typedef struct Data{
    int Dia;
    int Mes;
    int Ano;
}Data;

//Struct para cadastro dos pacientes
typedef struct CadastroP {
    char Nome[150];
    char CPF[11];
    char Contato[14];
    CadEnd Endereco;
    Data DataNasc;
    char Email[100];
    Data DataDiag;
    char Comorbidade[250];
    struct CadastroP *ProximoCadP;
}CadastroP;
CadastroP *primeiro, *atual, *NovoPaciente;

//Enum para facilitar a leitura do c�digo na fun��o login
enum boolean{
    true = 1,false = 0
};

//Definindo a variavel bool global do tipo enum boolean
typedef enum boolean bool;

//Cabe�alho autentica��o do usu�rio
void Cabecalho1(){
    system("cls");
    printf("-----------------------------------------------------------------\n\n\n");
    printf("AUTENTICA��O DE USU�RIO\n\n\n");
    printf("-----------------------------------------------------------------\n\n\n");
}

//Sistema para cadastro de novo usu�rio
bool CadUser(){
    FILE *arquser;
    bool ret;
    char temp[12];

    arquser = fopen("usuarios.bin","wb");

     //Iniciando estrutura de cadastro de usu�rio e alocando mem�ria

    NovoUser=(CadastroU *)malloc(sizeof(CadastroU));

    printf("Favor entrar com os dados de login e senha a seguir para realizar o cadastro...\n\n");
    //Solicitando cadastro do login
    fflush(stdin);
    printf("Login (M�x 12 caracteres): ");
    gets(NovoUser->login);

    //Solicitando cadastro da senha
    fflush(stdin);
    printf("Senha (M�x 12 caracteres): ");
    gets(NovoUser->senha);


    //Gravando os dados de login e senha no arquivo usuarios.txt
    fwrite(NovoUser,sizeof(struct CadastroU),1,arquser);
    fclose(arquser);

    //Mensagem de retorno da grava��o ao usu�rio
    printf("\nDados cadastrados com Sucesso!\n");
    system("pause");
    ret = true;//ap�s cadastro do usu�rio, segue para cadastro paciente
    return ret;

}

//Sistema de autentica��o do usu�rio
bool Login(){

    bool ret;  //vari�vel para retorno da fun��o (login/senha v�lido ou n�o)
    char logtemp[12];
    char sentemp[12];

    //Criando ponteiro para FILE para manipula��o de arquivos
    FILE *arquser;
    arquser = fopen("usuarios.bin","rb"); //arquivo de cadastro de login e senha do usu�rio

    if(arquser==NULL){
        printf("N�o h� cadastro de usu�rio e senha!\n");
        ret = CadUser();
        arquser = fopen("usuarios.bin","rb");
        if(arquser==NULL){
            printf("Imposs�vel abrir o registro de usu�rios!\n");
            ret = false;
            return ret;
        }
        else{
            fclose(arquser);
            return ret;
        }

    }
    else{
        //Leitura do registro de usu�rio para autenticar
        CadastroU *usuario = NULL;
        arquser = fopen("usuarios.bin", "rb");
        if(arquser==NULL){
            printf("Imposs�vel ler registro de usu�rio\n");
            ret = false;
            return ret;
        }
        else{
            usuario = (CadastroU*) malloc(sizeof(CadastroU));
            if(fread(usuario, sizeof(CadastroU), 1, arquser) != 1){
                printf("A base de usu�rios est� corrompida e sera necess�ria a cria��o de uma nova.\n");
                // Coloca a posi��o do ponteiro do arquivo para o in�cio do arquivo,
                //para sobrescrever dados corrompidos
                fseek(arquser, 0, SEEK_SET);
                ret = CadUser();
                return ret;
            }
            else{
                printf("Entre com seu Login e Senha a seguir...\n");
                fflush(stdin);
                printf("Login (M�x 12 caracteres): ");
                gets(logtemp);
                fflush(stdin);
                printf("Senha (M�x 12 caracteres): ");
                gets(sentemp);

                /* Testando se a captura do login e senha e leitura do arquivo est� correta
                //printf("Logins: %s;%s\n",usuario->login,logtemp);
                //printf("Senha: %s;%s\n",usuario->senha,sentemp);
                */

                if(strcmp(usuario->login, logtemp) != 0 || strcmp(usuario->senha, sentemp) != 0) {
                    ret = false;
                    return ret;
                }
                ret = true;
                fclose(arquser);
                return ret;
             }
        }

    }
}

//Cabe�alho cadastro do paciente
void Cabecalho2(){
    system("cls");
    printf("-----------------------------------------------------------------\n\n\n");
    printf("BEM VINDO(A) AO SISTEMA DE CADASTRO DE PACIENTES COM COVID-19\n\n\n");
    printf("-----------------------------------------------------------------\n\n\n");
}

//Valida se o paciente faz parte ou n�o do grupo de risco
int idade;
bool comorb;
bool ValidGR(comorb){
    bool gr;
    int anonasc,anodiag;
    FILE *arqgr;


    //Calcula idade do paciente
    idade = 0;
    anonasc = atual->DataNasc.Ano;
    anodiag = atual->DataDiag.Ano;
    idade = anodiag - anonasc;
    if((idade>=65) || (comorb==true)){
        gr = true;
    }
    else{
        gr = false;
    }

    if(gr==true){
        arqgr = fopen("grupoRisco.txt","a");
        char buffer[sizeof(atual->Endereco.CEP)+6];
        sprintf(buffer, "%s;%d\n", &atual->Endereco.CEP, idade);
        fprintf(arqgr,buffer);
        fclose(arqgr);
        if(arqgr==NULL){
            printf("Ocorreu um erro e o paciente n�o foi incluido no arquivo de grupo de risco\n");
        }
        else{
            printf("Dados do paciente para grupo de risco salvos com sucesso!\n");
        }
    }

    return gr;
}

//Sistema de cadastro do paciente com Covid-19
void inputPaciente(){
    char temp[30];
    char res;
    int dia;
    int mes;
    int ano;
    bool grupoRisco;

    //Cria��o de ponteiro apontando pro arquivo file
    FILE *arqpac;
    arqpac = fopen("pacientes.bin","a");//cria o arquivo na pasta onde est� o programa e d� sempre um append nele

    //Aloca��o din�mica de mem�ria
    NovoPaciente=(CadastroP *)malloc(sizeof(CadastroP));//Bloco de mem�ria do tipo da struct e retornando
    //um ponteiro que aponta para esse bloco

    if(primeiro==(CadastroP *)NULL){
        primeiro = atual = NovoPaciente;
    }
    else{
        atual = primeiro;
        while(atual->ProximoCadP !=(CadastroP *)NULL){
            atual = atual->ProximoCadP;
        }
        atual->ProximoCadP=NovoPaciente;
        atual=NovoPaciente;
    }
    //Entrada de dados para grava��o no arquivo de paciente
    fflush(stdin);
    printf("Digite o nome do paciente: ");
    gets(atual->Nome);

    fflush(stdin);
    printf("Digite o CPF do paciente (somente n�meros): ");
    gets(atual->CPF);

    fflush(stdin);
    printf("Digite o contato do paciente ((DDD)XXXXX-XXXX): ");
    gets(atual->Contato);

    printf("Digite o endere�o do paciente a seguir...\n");
    fflush(stdin);
    printf("Logradouro:");
    gets(atual->Endereco.Rua);
    fflush(stdin);
    printf("N�mero:");
    gets(temp);
    atual->Endereco.Numero=atoi(temp);
    fflush(stdin);
    printf("Bairro:");
    gets(atual->Endereco.Bairro);
    fflush(stdin);
    printf("Cidade:");
    gets(atual->Endereco.Cidade);
    fflush(stdin);
    printf("Estado(sigla):");
    gets(atual->Endereco.Estado);
    fflush(stdin);
    printf("CEP (XXXXX-XXX):");
    gets(atual->Endereco.CEP);


    fflush(stdin);
    printf("Digite a data de nascimento do paciente (formato: DD MM AAAA): ");
    scanf("%i %i %i",&dia,&mes,&ano);
    atual->DataNasc.Dia=dia;
    atual->DataNasc.Mes=mes;
    atual->DataNasc.Ano=ano;

    fflush(stdin);
    printf("Digite o email do paciente: ");
    gets(atual->Email);

    fflush(stdin);
    printf("Digite a data de diagn�stico do paciente (formato: DD MM AAAA): ");
    scanf("%i %i %i",&dia,&mes,&ano);
    atual->DataDiag.Dia=dia;
    atual->DataDiag.Mes=mes;
    atual->DataDiag.Ano=ano;

    fflush(stdin);
    printf("O paciente tem alguma comorbidade? (s/n): ");
    res = getch();
    if((res!='n') && (res!='N')){
        fflush(stdin);
        printf("\nDigite as comorbidades do paciente: ");
        gets(atual->Comorbidade);
        comorb = true;
    }
    else{
        comorb = false;
    }

    //Atribuindo o endere�o da pr�xima struct vazia de cadastro
    atual->ProximoCadP=(CadastroP *)NULL;

    //Gravando os dados do paciente no arquivo pacientes.bin
    fwrite(NovoPaciente,sizeof(struct CadastroP),1,arqpac);
    fclose(arqpac);

    printf("Dados do paciente salvos com sucesso!\n");

    //Chama fun��o ValidGR para verificar se o paciente faz parte do grupo de risco
    //sendo enviada a informa��o se ele tem comorbidade ou n�o
    grupoRisco = ValidGR(comorb);

    //Sa�da de dados quanto ao fato do paciente fazer parte do grupo de risco ou n�o
    if(grupoRisco==true){
        printf("Paciente faz parte do Grupo de Risco!\n");
    }
    else{
        printf("Paciente n�o faz parte do Grupo de Risco\n");
    }

}


int main(){
    setlocale(LC_ALL,"Portuguese");//lib para permitir o uso de caracteres especiais no terminal
    bool LoginValid;
    char cadNovo;

    //Iniciando a autentica��o do usu�rio
    Cabecalho1();
    LoginValid = Login();

    while(LoginValid==false){
        printf("\nLogin ou senha inv�lidos! Favor tentar novamente!\n\n");
        system("pause");
        Cabecalho1();
        LoginValid = Login();
    }

    //Iniciando sistema de cadastro de pacientes
    if(LoginValid==true){
        Cabecalho2();
        do{
           inputPaciente();
           printf("\nCadastrar outro Paciente (s/n)?");
           cadNovo = getch();
           printf("\n");
        }while((cadNovo!='n') && (cadNovo!='N'));
    }

    system("pause");
    return 0;
}
