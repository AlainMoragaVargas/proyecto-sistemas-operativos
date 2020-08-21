//nombres: Alain Moraga Vargas


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/sem.h> 
#include <sys/ipc.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#define FIFONAME "myfifo" //comunicacion de cliente 1.
#define FIFONAME2 "myfifo2" //comunicacion de cliente 2.
#define FIFONAME3 "myfifo3" //permite el envio de mensajes de servidor a clientes.

void error(char* errorInfo);
void doSignal(int semid, int numSem);
void doWait(int semid, int numSem);
void initSem(int semid, int numSem, int valor);
void inicializar_mapa(int mapa[5][5]);
void mostrar_mapa(int mapa[5][5]);
void removerCaracteres(char *cadena); 
int retornar_x(char *cadena);
int retornar_y(char *cadena);
void generar_barcos(int mapa[5][5]);
void procesar_coordenadas(char *cadena, int mapa[5][5]);

int main() 
{
	srand(time(NULL));
    int cliente_1, cliente_2; //representación de los clientes 1 y 2.
    int readbytes_1=0, readbytes_2=0; //lectores de bytes de los buffers.
    char buffer_1[1024], buffer_2[1024]; //buffer para cada cliente.
    int proceso; //representacion de fork para clientes.
    int semaforo; //para utilización de semaforos.
    int mapa_cliente1[5][5],mapa_cliente2[5][5]; //mapas de los clientes.
    int estado_ataque=0; //variable de control para resultado del ataque.
    char buffer_ataque[1024]; //buffer que recibe los ataques de los clientes.
    char buffer_coordenadas_1[1024], buffer_coordenadas_2[1024];
    int contador_barcos1=5, contador_barcos2=5; //para termino del servidor en caso de ganador.

    printf("***************************************************************\n");
    printf("******************  MODO SERVIDOR  ****************************\n");
    printf("***************************************************************\n");
	
    mkfifo(FIFONAME,0777);
    mkfifo(FIFONAME2,0777);

    //creación de semaforos con sus permisos.
    if((semaforo=semget(IPC_PRIVATE,1,IPC_CREAT | 0700))<0) 
    {
        perror(NULL);
        error("Semaforo: semget");

    }

    initSem(semaforo,0,1); //inicialización del semaforo.

    printf("\n");
    inicializar_mapa(mapa_cliente1); //se inicializan los mapas en 0.
    inicializar_mapa(mapa_cliente2);
    printf("...... Creación de mapas para clientes: OK! \n\n");
    sleep(1);
    printf("...... Creación de barcos para los clientes: OK! \n\n");

    generar_barcos(mapa_cliente1); //se generan barcos aleatorios en los mapas.
    generar_barcos(mapa_cliente2);


    printf("Mapa del cliente 1:\n");
    printf("-------------------\n");
    mostrar_mapa(mapa_cliente1); //se muestran los mapas preparados para el juego.
    printf("\n");
    printf("Mapa del cliente 2:\n");
    printf("-------------------\n");
    mostrar_mapa(mapa_cliente2);


    procesar_coordenadas(buffer_coordenadas_1,mapa_cliente1);

    mkfifo(FIFONAME3,0777);
    cliente_1 = open(FIFONAME3 , O_WRONLY);
    write(cliente_1, buffer_coordenadas_1, sizeof(buffer_coordenadas_1));
    unlink(FIFONAME3);
    close(cliente_1);

    procesar_coordenadas(buffer_coordenadas_2,mapa_cliente2);

    mkfifo(FIFONAME3,0777);
    cliente_2= open(FIFONAME3, O_WRONLY);
    write(cliente_2, buffer_coordenadas_2, sizeof(buffer_coordenadas_2));
    close(cliente_2);

    printf("...... Esperando ataque de los jugadores\n\n");


    proceso=fork(); //procesos para representar los clientes.

//-------------- [ JUGABILIDAD ] ---------------------------------

    if(proceso==0)
    {
        do
        {
        doWait(semaforo,0); //se protege la sección critica del juego.
        int x,y; 
        cliente_1= open(FIFONAME, O_RDONLY);  
        readbytes_1 = read(cliente_1, buffer_ataque, sizeof(buffer_ataque)); 
        printf("EL CLIENTE 1 ATACARÁ EN POSICIÓN: %s",buffer_ataque);

        removerCaracteres(buffer_ataque); //se remueven caracteres de la cadena.
        x= retornar_x(buffer_ataque);
        y= retornar_y(buffer_ataque);
        printf("El cliente 1 atacó en posición: [%d,%d] \n",x,y);

            for(int i=1; i<6; i++)
            {
                for(int j=1; j<=6; j++)
                {
                    int coordenada_x=i;
                    int coordenada_y=j; 
                    if(mapa_cliente2[i][j]==1 && coordenada_x==x && coordenada_y==y)
                    {
                        estado_ataque=1;
                    } 
                }
            }
            if(estado_ataque==1)
            {
                printf("estado ataque cliente 1: ACERTADO! \n");
                char mensaje[]="HAS ACERTADO EL ATAQUE !"; 
                cliente_1= open(FIFONAME3, O_WRONLY);
                write(cliente_1, mensaje, sizeof(mensaje));  //se comunica al cliente estado de su ataque.    
                contador_barcos1--;   
            }
            if(estado_ataque==0)
            {
                printf("estado ataque cliente 1: FALLÓ. \n");
                char mensaje[]="HAS FALLADO EL ATAQUE.";
                cliente_1= open(FIFONAME3, O_WRONLY);
                write(cliente_1, mensaje, sizeof(mensaje));     
            }
            estado_ataque=0; 
        doSignal(semaforo,0); //se cierra la protección de la sección critica.
        printf("\n");

        if(contador_barcos1==0)
        {
            close(cliente_1);
            close(cliente_2);
            return 0; 
        }

        }while(1);
    }
    else
    {
        do
        {
        int x,y; 
        doWait(semaforo,0);
        cliente_2 = open(FIFONAME2, O_RDONLY);  
        readbytes_2 = read(cliente_2, buffer_ataque, sizeof(buffer_ataque));
        printf("EL CLIENTE 2 ATACARÁ EN POSICIÓN: %s",buffer_ataque);

        removerCaracteres(buffer_ataque);
        x= retornar_x(buffer_ataque);
        y= retornar_y(buffer_ataque);
        printf("El cliente 2 atacó en posición: [%d,%d] \n",x,y);

            for(int i=1; i<6; i++)
            {
                for(int j=1; j<6; j++)
                {
                    int coordenada_x=i;
                    int coordenada_y=j; 
                    if(mapa_cliente1[i][j]==1 && coordenada_x==x && coordenada_y==y)
                    {
                        estado_ataque=1;
                    } 
                }
            }
            if(estado_ataque==1)
            {
                printf("estado ataque cliente 2: ACERTADO! \n");
                char mensaje[]="HAS ACERTADO EL ATAQUE !";
                cliente_2= open(FIFONAME3, O_WRONLY);
                write(cliente_2, mensaje, sizeof(mensaje));   
                contador_barcos2--;
            }
            if(estado_ataque==0)
            {
                printf("estado ataque cliente 2: FALLÓ. \n");
                char mensaje[]="HAS FALLADO EL ATAQUE.";
                cliente_2= open(FIFONAME3, O_WRONLY);
                write(cliente_2, mensaje, sizeof(mensaje));   
            }
            estado_ataque=0;
        doSignal(semaforo,0);
            printf("\n");

        if(contador_barcos2==0)
        {
            close(cliente_1);
            close(cliente_2);
            return 0; 
        }

        }while(1); 

    }


}

void error(char* errorInfo) 
{
    fprintf(stderr,"%s",errorInfo);
    exit(1);
}

void doSignal(int semid, int numSem) 
{
    struct sembuf sops; //Signal
    sops.sem_num = numSem;
    sops.sem_op = 1;
    sops.sem_flg = 0;

    if (semop(semid, &sops, 1) == -1) 
    {
        perror(NULL);
        error("Error al hacer Signal");
    }
}

void doWait(int semid, int numSem) 
{
    struct sembuf sops;
    sops.sem_num = numSem;
    sops.sem_op = -1; 
    sops.sem_flg = 0;

    if (semop(semid, &sops, 1) == -1) 
    {
        perror(NULL);
        error("Error al hacer el Wait");
    }
}

void initSem(int semid, int numSem, int valor) 
{   
    if (semctl(semid, numSem, SETVAL, valor) < 0) 
    {        
        perror(NULL);
        error("Error iniciando semaforo");
    }
}

void inicializar_mapa(int mapa[5][5])
{
    int i, j;

    for(i=1; i<6; i++)
    {
        for(j=1; j<6; j++)
        {
            mapa[i][j]=0;
        }
    }
}

void mostrar_mapa(int mapa[5][5])
{
    for(int s=1; s<6; s++)
    {
        for (int k=1; k<6; k++)
            {
                printf("[ %d ]",mapa[s][k]);
            }
        printf("\n\n");
    }
}

void removerCaracteres(char *cadena) 
{
  int indiceCadena = 0, indiceCadenaLimpia = 0;
  int deberiaAgregarCaracter = 1;
  char caracteres[] = ". -,";
  while (cadena[indiceCadena]) {
    deberiaAgregarCaracter = 1;
    int indiceCaracteres = 0;
    while (caracteres[indiceCaracteres]) {
      if (cadena[indiceCadena] == caracteres[indiceCaracteres]) {
        deberiaAgregarCaracter = 0;
      }
      indiceCaracteres++;
    }
    if (deberiaAgregarCaracter) {
      cadena[indiceCadenaLimpia] = cadena[indiceCadena];
      indiceCadenaLimpia++;
    }
    indiceCadena++;
  }
  cadena[indiceCadenaLimpia] = 0;
}

int retornar_x(char *cadena)
{
    int x= cadena[0]-'0'; 
    return x; 
}

int retornar_y(char *cadena)
{
    int y= cadena[1]-'0';
    return y; 
}

void generar_barcos(int mapa[5][5])
{

    int c1_x;
    int c1_y;
    int cantidad_barcos=0;
do
{
    do
    {
        c1_x= rand()%6;
        c1_y= rand()%6;
    }while(c1_x==0 || c1_y==0);

    for(int i=1; i<6; i++)
    {
        for(int j=1; j<6; j++)
        {
            int coordenada_x=i;
            int coordenada_y=j;
            if(coordenada_x==c1_x && coordenada_y==c1_y && mapa[i][j]==0)
            {
                mapa[i][j]=1;
                cantidad_barcos++;
            }
        }
    }
}while(cantidad_barcos!=5);

}

void procesar_coordenadas(char *cadena, int mapa[5][5])
{
    int contador=0;
    int arreglo_aux[10];
    for(int i=1; i<6; i++)
    {
        for (int j=1; j<6; j++)
        {
            if(mapa[i][j]==1)
            {
                arreglo_aux[contador]=i;
                contador++;
                arreglo_aux[contador]=j;
                contador++;
            }
        }
    }

    sprintf(cadena, "%d%d%d%d%d%d%d%d%d%d",arreglo_aux[0],arreglo_aux[1],arreglo_aux[2],arreglo_aux[3],arreglo_aux[4],arreglo_aux[5],arreglo_aux[6],arreglo_aux[7],arreglo_aux[8],arreglo_aux[9]);
    
}
