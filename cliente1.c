//nombres: Alain Moraga Vargas


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>


#define FIFONAME "myfifo"
#define FIFONAME3 "myfifo3"

void ingresar_barcos(char *cadena, int mapa[5][5]);
void mostrar_mapa(int mapa[5][5]);

int main (void)
{
    int fd;
    char buf[1024]; 
    int readbytes;  
    int barcos_restantes=5; 
    char mensaje_acierto[]="HAS ACERTADO EL ATAQUE !"; //mensaje que se utilizará para comparar.
    int mapa[5][5]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int mapa_ataques[5][5]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    char buf_coordenadas[1024];
    char buf_mensaje[1024];
    int x,y; 

    printf("***************************************************************\n");
    printf("***************  BIENVENIDO CLIENTE 1!  ***********************\n");
    printf("***************************************************************\n");
    printf("\n\n");
    printf("\t\t#\n\t\t#|\n\t\t#|#\n\t\t#|##\n\t\t#|###\n\t\t#|####");
    printf("\n\t\t#|#####\n\t\t#|######\n\t#########################\n\t _______________________");
    printf("\n\t  ####/)###############\n\t   ###(/##############\n\t    #################\n\t     ###############");

    printf("\n\n");

    fd= open(FIFONAME3, O_RDONLY);  //se reciben coordenadas.
    readbytes= read(fd, buf_coordenadas, sizeof(buf_coordenadas)); 

    ingresar_barcos(buf_coordenadas,mapa);

    printf("SE TE ASIGNARON ESTOS BARCOS, A GANAR !\n");
    printf("\n");
    mostrar_mapa(mapa);

    do
    {
    printf("|| INGRESA LAS COORDENADA DE ATAQUE. ||\n");
    printf("COORDENADA X, Y: ");
    fgets(buf,sizeof(buf),stdin);

    mkfifo(FIFONAME ,0777);
    fd = open(FIFONAME , O_WRONLY);
    write(fd, buf, sizeof(buf));

    printf("\n");
    fd= open(FIFONAME3, O_RDONLY);  
    readbytes= read(fd, buf_mensaje, sizeof(buf_mensaje)); 
    printf("%s\n",buf_mensaje);

    if (strcmp(mensaje_acierto,buf_mensaje) == 0) 
    {
        barcos_restantes= barcos_restantes-1;
        printf("Bien, te faltan %d barcos por derribar!. \n",barcos_restantes);
    }
    else
    {
       printf("Intentalo de nuevo, te faltan %d barcos por derribar. \n",barcos_restantes);   
    }
    printf("\n");
    } while(barcos_restantes!=0);

    if(barcos_restantes==0)
    {
      printf("***************************************************************\n");
      printf("**********  FELICIDADES, HAS GANADO EL JUEGO !  ***************\n");
      printf("***************************************************************\n");
      close(fd);
      return 0; 
    }
    else
    {
      printf("|| HAS PERDIDO EL JUEGO, PARA LA PRÓXIMA SERÁ.||\n");
      close(fd);
      return 0; 
    }
}

void ingresar_barcos(char *cadena, int mapa[5][5])
{
    int x_1= cadena[0]-'0'; 
    int y_1= cadena[1]-'0';

    int x_2= cadena[2]-'0';
    int y_2= cadena[3]-'0';

    int x_3= cadena[4]-'0';
    int y_3= cadena[5]-'0';

    int x_4= cadena[6]-'0';
    int y_4= cadena[7]-'0';

    int x_5= cadena[8]-'0';
    int y_5= cadena[9]-'0';
    for(int i=1; i<6; i++)
    {
        for(int j=1; j<6; j++)
        {
            if(i==x_1 && j==y_1)
            {
                mapa[i][j]=1;   
            }

            if(i==x_2 && j==y_2)
            {
                mapa[i][j]=1;
            }

            if(i==x_3 && j==y_3)
            {
                mapa[i][j]=1;
            }

            if(i==x_4 && j==y_4)
            {
                mapa[i][j]=1;
            }

            if(i==x_5 && j==y_5)
            {
                mapa[i][j]=1;
            }
        }
    }

}

void mostrar_mapa(int mapa[5][5])
{
  for (int i=1; i<6; i++)
  {
    for(int j=1; j<6; j++)
  {
    if(mapa[i][j]==1)
    {
      printf(" [ B ] ");    
    }
    else
    {
      printf(" [ O ] ");
    }
  }
      printf ("\n\n");
}
}

