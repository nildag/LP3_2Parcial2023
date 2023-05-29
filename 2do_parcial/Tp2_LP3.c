#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <ctype.h>
#define lon 50
#define dormir 2
#define lineas 8
#define gotoxy(x,y) printf("\033[%d;%dH", (x), (y))
#define clear() printf("\033[H\033[J")
char * colores[7]= {"\033[0;31m","\033[0;32m","\033[0;33m","\033[0;34m","\033[0;35m","\033[0;36m","\033[0;37m"};
	// "\033[0,31m"		// rojo
	// "\033[0,32m"		// verde
	// "\033[0,33m"		// amarillo
	// "\033[0,34m"		// azul
	// "\033[0,35m"		// magenta
	// "\033[0,36m"		// celeste
	// "\033[0,37m"		// blanco


/*
	Compilar programa
	gcc -pthread parcialv3.c

	Comandos
		Agregar auto a la derecha (cola izquierda): 
		car nombre_auto dcha

		Agregar auto que va a la izquierda (cola derecha): 
		car nombre_auto izda

		Estado de las colas
		status

		Iniciar simulacion 
		start

		Salir del programa
		exit
*/

char * nombreAuto[100];
int q=0;
int primerAuto=0;

//MUTEX
pthread_mutex_t msg_mutex = PTHREAD_MUTEX_INITIALIZER;

void* iniciar(void *str); //inicia la simulacion del puente
void * leer (void *str); //lee los comandos

#define max 30

//tipo de dato
typedef struct{
	char nombre[6];
	int direccion; //0 = llega del lado derecho; 1 = llega del lado izdo
	int color;
}car;

typedef car tipoDato;


//Estructura del puente
typedef struct{
	car primero; //para indicar que no hay auto, en direccion poner -1
	car segundo;
	car tercero;
	int cantidad;//cantidad de autos en el puente
	int cruce; //cantidad de autos que cruzaron
	int salida; //0 o 1 para saber si salio o no un auto
}Puente;

//Estructura de cola
typedef struct{
	tipoDato listaCola[max];
	int head, tail, cantidad;
}Cola;


//operaciones sobre la cola
int sigte(int n);
void crearCola(Cola *c);
void encolar(Cola* c, tipoDato nuevo);
tipoDato decolar(Cola* c);
int getHead(Cola c);
int isEmpty(Cola c);
int isFull(Cola c);
int getCantidad(Cola c);
void listar(Cola c);


//Operaciones sobre puente
void crearPuente(Puente *p);
void ingresarI(Puente *p); // --->
void ingresarD(Puente *p); // <---
void avanzaI(Puente* p); //  <---
void avanzaD(Puente* p); //  --->
void verPuente(Puente p);
void verPuente2(Puente p);
int getCruce(Puente p);
void aumentarCruce(Puente *p);
void resetCruce(Puente *p);
int comprobarAuto(char nombre[]);

char *autoI = "  -->  ";
char *autoD = "  <--  ";
char *autoN = "       ";


//Variables globales
int quit = 0; //para saber cuando salir del programa
int start = 0; //para saber si ya se inicio la simulacion
int mensaje = 0;


char *argumentos[10]; //para los parametros del comando

car nulo;

Puente p;

Cola colaI;

Cola colaD;

int funcion = 0;

void (*ptr_avanza[])(Puente*) = {ingresarI, ingresarD};

////////////////////////////////////////////////////////////////////////////////////////////////////Inicio Main////
int main(int argc, char const *argv[]){

	system("clear");
	printf("\n\n\n\n\n\n\n\n");
	int h;
	for(h=0; h<10; h++)
    	argumentos[h] = malloc(sizeof(char)*15); ///////////////////////////////Liberar despues

    //Para auto nulo
    strcpy(nulo.nombre, "      ");
	nulo.direccion = -1;

	//Para cola
	crearCola(&colaI);
	crearCola(&colaD);


	//Para puente
	crearPuente(&p);
	

    /* Hilos */
    pthread_t h_leer;
    pthread_t h_iniciar; 

    pthread_create(&h_leer, NULL, leer, NULL);
    pthread_create(&h_iniciar, NULL, iniciar, NULL);

    pthread_join(h_leer, NULL);

    for(h=0; h<10; h++)
    	free(argumentos[h]);
    for(h=0; h<=q; h++)
    	free(nombreAuto[h]);

    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////Fin Main//////


/////////Para el inicio /////////////////////////////
void* iniciar (void *str) {

	while(start == 0){
		sleep(2);
	}

	while(quit == 0){

		sleep(1);
	    (*ptr_avanza[funcion])(&p);
	}
        
    pthread_exit(0);
}
////////Para leer //////////////////////////////////
void * leer (void *str){
	srand(time((NULL)));

	while(quit == 0){

		//Para tokens
		char cat[lon];
		char cat2[lon];
		int i, j = 0, val;
	    const char s[2] = " ";
	    char *token;
	    int check=1;

	    car aux; 

	    //Lectura de comandos

	    gotoxy(16,1);
	    printf(">> ");
	    gotoxy(16,3);
	    fflush(stdin);    
	    scanf(" %[^\n]s", cat);   
	    fflush(stdin);
	    strcpy(cat2,cat);


	    token = strtok(cat, s);
	    /* walk through other tokens */
	    i = 0;
	    while(token != NULL) {
	        j = 0;
	        while(token[j] != '\0') {
	            token[j] = toupper(token[j]);
	            j++;
	        }

	        strcpy(argumentos[i],token);
	        token = strtok(NULL, s);
	        i++;
	    }

	    if(strcmp(argumentos[0], "EXIT") == 0){
	    	pthread_mutex_lock(&msg_mutex);
	        quit = 1;
	        pthread_mutex_unlock(&msg_mutex);
	    }
	         
	    while(quit == 0){

	        if(strcmp(argumentos[0], "CAR") == 0){

	        	if( strlen(argumentos[1])==6 && (strcmp(argumentos[2],"IZQ")==0 || strcmp(argumentos[2],"DER")==0) && i==3 && (check=comprobarAuto(cat2)==1)){
		            
		            if(primerAuto==0){
		            	if(strcmp(argumentos[2],"IZQ")==0)
		            		funcion=1;
		            	else
		            		funcion=0;
		            	primerAuto=1;
		            }
		            strcpy(aux.nombre, argumentos[1]);

		            pthread_mutex_lock(&msg_mutex); //se bloquea el mutex, inicio de la seccion critica

		            if(strcmp(argumentos[2], "IZQ") == 0){

						aux.direccion = 0;
						aux.color = rand() % 7;
						encolar(&colaD, aux);
		            }else{

						aux.direccion = 1;
						aux.color = rand() % 7;
						encolar(&colaI, aux);
					}
					printf("Auto agregado\033[K");
					
					pthread_mutex_unlock(&msg_mutex); //se desbloquea el mutex, fin de la seccion criticas
				}else if(strlen(argumentos[1])!=6){
					printf("El nombre del auto debe ser de 6 caracteres\033[K");
				}else if(check!=1){
					check=1;
				}else if( strcmp(argumentos[2],"IZQ")!=0 && strcmp(argumentos[2],"DER")!=0){
					printf("Introduzca una direccion correcta\033[K");
				}else if(i!=3){
					printf("Cantidad incorrecta de argumentos\033[K");
				}
	        }else if(strcmp(argumentos[0], "START") == 0){

	            if(start == 0){

					printf("Iniciando.\033[K");
					fflush(stdout);
					usleep(500000);
					printf(".");
					fflush(stdout);
					usleep(500000);
					printf(".");
					fflush(stdout);
					usleep(500000);					
					printf(".");
					fflush(stdout);
					start = 1;
					usleep(500000);
					printf(".");
					fflush(stdout);						
				}else{
					printf("La simulacion ya ha comenzado!!!\033[K");
				}
	            
				
	        }else if(strcmp(argumentos[0], "STATUS") == 0){
	            
	            if(start == 1){
	            	printf("Ya se pueden visualizar los autos en espera\033[K");
	            }else{
	            	gotoxy(10,1);
	            	printf("ListaI: ");
					listar(colaI);
					printf("\033[K\nListaD: ");
					listar(colaD);
					printf("\033[K\n");
	            }
	        }else {
	        	printf("Introduzca una opcion valida!!!\033[K");
	        	fflush(stdout);
	        }

	        if(quit == 1)
	        	break;

	        fflush(stdout);
			sleep(1);

	        //borrar mensajes si es que hay
	        printf("\033[1C\033[2K\033[1A");
	        fflush(stdout);

	        
		    gotoxy(16,1);
		    printf("\33[2K");
		    printf(">> ");
		    gotoxy(16,3);
	        fflush(stdin);
	        scanf(" %[^\n]s", cat); 
	        fflush(stdin);
	        strcpy(cat2,cat);

	        token = strtok(cat, s);  
	        /* recorre los tokens */
	        i = 0;
	        while(token != NULL) {     
	            j = 0;
	            while(token[j] != '\0') {
	                token[j] = toupper(token[j]);
	                j++;
	            }
	            strcpy(argumentos[i],token);
	            token = strtok(NULL, s);
	            i++;
	        }
	        
	        if(strcmp(argumentos[0], "EXIT") == 0){
	        	pthread_mutex_lock(&msg_mutex);
	            quit = 1;
	            pthread_mutex_unlock(&msg_mutex);
	        }
	    }///fin lectura comandos
	}

    pthread_exit(0);
}


//Cola//

void crearCola(Cola *c){
	c->head = 0;
	c->tail = max-1;
	c->cantidad = 0;
}

void encolar(Cola *c, tipoDato nuevo){
	if(isFull(*c)){
		printf("Cola llena!!!\n");
		exit(1);
	}else{
		c->tail = sigte(c->tail);
		c->listaCola[c->tail] = nuevo;

		(c->cantidad)++;
	}
}

tipoDato decolar(Cola *c){
	tipoDato temp;

	if(isEmpty(*c)){
		printf("Cola vacia!!!\n");
		exit(1);
	}else{
		temp = c->listaCola[c->head];
		c->head = sigte(c->head);

		(c->cantidad)--;

		return temp;
	}
}

int isEmpty(Cola c){
	return (c.head == sigte(c.tail));
}

int isFull(Cola c){
	return (c.head == sigte(sigte(c.tail)));
}

int getHead(Cola c){
	return c.head;
}

int getCantidad(Cola c){
	return c.cantidad;
}

int sigte(int n){
	return ((n+1) % max);
}

void listar(Cola c){

	if(isEmpty(c) == 1)
		printf("Vacio\033[K\n");
	else{
		int z;
		for(z = c.head; z <= c.tail; z++ ){
			printf("%s%s", c.listaCola[z].nombre, (z == c.tail)?"\033[K\n":" ");
		}
	}
}

//Puente//

void crearPuente(Puente *p){
	p->cantidad = p->cruce = 0;
	p->primero = p->segundo = p->tercero = nulo;
}

void ingresarI(Puente *p){ // --->
	if(getCantidad(colaI) == 0 && p->cantidad == 0){
		funcion = 1; 
		verPuente2(*p);
		return;
	}


	avanzaD(p); //hacer circular los autos de adelante hacia la dcha
	if(getCantidad(colaI) == 0 && getCantidad(colaD) == 0)
		verPuente2(*p);

	//decolar de colaI para avanzar hacia la dcha
	if(getCantidad(colaD) == 0){ //ningun auto espera del otro lado

		if(!isEmpty(colaI)){
			pthread_mutex_lock(&msg_mutex); //se bloquea el mutex, inicio de la seccion critica

			p->primero = decolar(&colaI);
			(p->cantidad)++;
			verPuente2(*p);

			pthread_mutex_unlock(&msg_mutex); //se desbloquea el mutex, fin de la seccion criticas
		}	
	}else{ //hay autos esperando

		int autos = getCantidad(colaI) + p->cantidad; //catidad total de autos en el puente y en la colaI

		if(autos >= 4 && quit == 0){

			int i = 0;
			while(i + (p->cantidad) < 4){ //hace que pasen 5 autos
				avanzaD(p);

				if(i < 3){
					if(!isEmpty(colaI)){
						pthread_mutex_lock(&msg_mutex); //se bloquea el mutex, inicio de la seccion critica

						p->primero = decolar(&colaI);
						(p->cantidad)++;

						pthread_mutex_unlock(&msg_mutex); //se desbloquea el mutex, fin de la seccion criticas
					}
				}
				i += (p->salida);
				verPuente2(*p); //////////////////////////////////////////////////////////////////////////////
			}

			while(p->cantidad > 0){ //hace que el puente se vacie para habilitar el otro sentido
				avanzaD(p);
				verPuente2(*p);
			}
		}else{

			int i = 0;
			while(i+(p->cantidad) < autos  && quit == 0){ //hace que pasen los autos que haya
				avanzaD(p);

				if(!isEmpty(colaI)){
					pthread_mutex_lock(&msg_mutex); //se bloquea el mutex, inicio de la seccion critica

					p->primero = decolar(&colaI);
					(p->cantidad)++;

					pthread_mutex_unlock(&msg_mutex); //se desbloquea el mutex, fin de la seccion criticas
				}
				
				i += (p->salida);
				verPuente2(*p);/////////////////////////////////////////////////////////////////////////////
			}

			while(p->cantidad > 0){ //hace que el puente se vacie para habilitar el otro sentido
				avanzaD(p);
				verPuente2(*p);
			}
		}

		resetCruce(p);
		funcion = 1; //cambio de sentido
	}
}

void avanzaD(Puente* p){ // --->

	p->salida = 0;
	if((p->tercero).direccion != -1){
		(p->cantidad)--; //un auto menos en el puente
		p->salida = 1; //indica si salio o no un auto
	}

	p->tercero = nulo;
	
	p->tercero = p->segundo;

	p->segundo = p->primero;

	p->primero = nulo;
}

void ingresarD(Puente *p){ // <---
	if(getCantidad(colaD) == 0 && p->cantidad == 0){
		funcion = 0; 
		verPuente2(*p);
		return;
	}

	avanzaI(p); //hacer circular los autos de adelante
	if(getCantidad(colaI) == 0 && getCantidad(colaD) == 0)
		verPuente2(*p);

	//decolar de colaD para ir hacia la izda
	if(getCantidad(colaI) == 0){ //ningun auto espera del otro lado

		if(!isEmpty(colaD)){
			pthread_mutex_lock(&msg_mutex); //se bloquea el mutex, inicio de la seccion critica

			p->tercero = decolar(&colaD);
			(p->cantidad)++;
			verPuente2(*p);

			pthread_mutex_unlock(&msg_mutex); //se desbloquea el mutex, fin de la seccion criticas
		}
		
	}else{ //hay autos esperando

		int autos = getCantidad(colaD) + p->cantidad; //catidad total de autos en el puente y en la cola

		if(autos >= 4){

			int i = 0;
			while(i+(p->cantidad) < 4 && quit == 0){ //hace que pasen 5 autos
				avanzaI(p);

				if(i < 3){
					if(!isEmpty(colaD)){
						pthread_mutex_lock(&msg_mutex); //se bloquea el mutex, inicio de la seccion critica

						p->tercero = decolar(&colaD);
						(p->cantidad)++;

						pthread_mutex_unlock(&msg_mutex); //se desbloquea el mutex, fin de la seccion criticas
					}
				}
				i += (p->salida);
				verPuente2(*p);///////////////////////////////////////////////////////////////////////////////////////////
			}

			while(p->cantidad > 0){ //hace que el puente se vacie para habilitar el otro sentido
				avanzaI(p);
				verPuente2(*p);
			}
		}else{

			int i = 0;
			while(i+(p->cantidad) < autos  && quit == 0){ //hace que pasen los autos que haya
				avanzaI(p);

				if(!isEmpty(colaD)){
					pthread_mutex_lock(&msg_mutex); //se bloquea el mutex, inicio de la seccion critica

					p->tercero = decolar(&colaD);
					(p->cantidad)++;

					pthread_mutex_unlock(&msg_mutex); //se desbloquea el mutex, fin de la seccion criticas
				}
				
				i += (p->salida);
				verPuente2(*p);/////////////////////////////////////////////////////////////////////////////////////////////
			}

			while(p->cantidad > 0){ //hace que el puente se vacie para habilitar el otro sentido
				avanzaI(p);
				verPuente2(*p);
			}
		}

		resetCruce(p);
		funcion = 0; //cambio de sentido
	}
	
}

void avanzaI(Puente* p){ // <---

	p->salida = 0;
	if((p->primero).direccion != -1){
		(p->cantidad)--;
		p->salida = 1; //indica si salio o no un auto
	}
	
	p->primero = nulo;

	p->primero = p->segundo;
	
	p->segundo = p->tercero;

	p->tercero = nulo;
}

int getCruce(Puente p){
	return p.cruce;
}

void aumentarCruce(Puente *p){
	(p->cruce)++;
}

void resetCruce(Puente *p){
	p->cruce = 0;
}

void verPuente(Puente p){

	printf("\033[s"); //guardar posision del cursor
	gotoxy(1,1);
	printf("\33[2K");
	//cambiar desde aca
	printf("%7s -- %7s -- %7s    Puente: %2d  \n", p.primero.nombre, p.segundo.nombre, p.tercero.nombre, p.cantidad);

	if(funcion == 0){ //Sentido hacia la dcha

		//cambiar los printf

		if(p.primero.direccion == -1 )
			printf("%7s -- ", autoN );
		else
			printf("%7s -- ", autoI);

		if(p.segundo.direccion == -1 )
			printf("%7s -- ", autoN);
		else
			printf("%7s -- ", autoI);

		if(p.tercero.direccion == -1 )
			printf("%7s\n", autoN);
		else
			printf("%7s\n", autoI);
	}else{ //sentido hacia la izda

		//cambiar los printf

		if(p.primero.direccion == -1 )
			printf("%7s -- ", autoN);
		else
			printf("%7s -- ", autoD);

		if(p.segundo.direccion == -1 )
			printf("%7s -- ", autoN);
		else
			printf("%7s -- ", autoD);

		if(p.tercero.direccion == -1 )
			printf("%7s\n", autoN);
		else
			printf("%7s\n", autoD);
	}

	printf("\nCola Izda: ");
	listar(colaI);
	printf("\nCola Dcha: " );
	listar(colaD);
	printf("\n");

	printf("\033[u"); //restaurar posision cursor

	fflush(stdout);

	sleep(dormir);
}

void verPuente2(Puente p){
	printf("\033[s"); //guardar posision del cursor
	gotoxy(1,1);

	char * autoNulo[4]={"                       ","                       ", "                       ", "                       "};
	char * autoIzq[3] ={"     <<<<<<<<<<     ","     <<<auto<<<     ", "     <<<<<<<<<<     "};
	char * autoDer[3] ={"     >>>>>>>>>>     ","     >>>auto>>>     ", "     >>>>>>>>>>     "};
	char * finColor = "\033[0m";
	int colorAuto[3];
	int j, i;

	for(i=0; i<15; i++){
		if(i!=17 && i!=16)
			printf("\33[K\n");

	}

	gotoxy(1,1);
	printf("\33[2K");


	printf("           %6s                     %6s                     %6s          Puente: %2d  \n", p.primero.nombre, p.segundo.nombre, p.tercero.nombre, p.cantidad);

	if(funcion == 0){ //Sentido hacia la dcha
		for(i=0; i<3; i++){
			j=0;
			if(p.primero.direccion == -1)
				printf("    %s%6s%s", colores[p.primero.color], autoNulo[i], finColor);
			else
				printf("    %s%6s%s", colores[p.primero.color], autoDer[i], finColor);
			if(p.segundo.direccion == -1 )
				printf("    %s%6s%s", colores[p.segundo.color], autoNulo[i], finColor);
			else
				printf("    %s%6s%s", colores[p.segundo.color], autoDer[i], finColor);
			if(p.tercero.direccion == -1 )
				printf("    %s%6s%s", colores[p.tercero.color], autoNulo[i], finColor);
			else
				printf("    %s%6s%s", colores[p.tercero.color], autoDer[i], finColor);
			if(i!=3)
				printf("\n");
			else
				printf("\n\033[1;100m%-100s\n%-100s%s", " ", " ", finColor);
		}
	}else{ //sentido hacia la izda

		for(i=0; i<3; i++){
			j=0;
			if(p.primero.direccion == -1)
				printf("    %s%6s%s", colores[p.primero.color], autoNulo[i], finColor);
			else
				printf("    %s%6s%s", colores[p.primero.color], autoIzq[i], finColor);
			if(p.segundo.direccion == -1)
				printf("    %s%6s%s", colores[p.segundo.color], autoNulo[i], finColor);
			else
				printf("    %s%6s%s", colores[p.segundo.color], autoIzq[i], finColor);
			if(p.tercero.direccion == -1 )
				printf("    %s%6s%s", colores[p.tercero.color], autoNulo[i], finColor);
			else
				printf("    %s%6s%s", colores[p.tercero.color], autoIzq[i], finColor);
			if(i!=3)
				printf("\n");
			else
				printf("\n\033[1;100m%-100s\n%-100s%s", " ", " ", finColor);

		}
		
	}
	printf("\nCola Izda: ");
	listar(colaI);
	printf("\033[K\nCola Dcha: " );
	listar(colaD);
	printf("\n");
	gotoxy(16,1);


	printf(">>");
	printf("\033[u"); //restaurar posision cursor

	fflush(stdout);

	sleep(dormir);
}

int comprobarAuto(char nombre[]){
	int exit=0;
	int i=0;
    char *token;
    const char s[2]=" ";
    token=strtok(nombre,s);	
	token=strtok(NULL,s);
	 	while (nombreAuto[i])
	 	{
	 		if(strcmp(token,nombreAuto[i])==0)
	 		{
              printf("El auto ya ha sido incluido\033[K");
              exit=1;
              return 0;
	 		}
	 		i++;
	 	}
        if(exit==0)
        {
        	nombreAuto[q]=malloc(sizeof(char)*15);
        	strcpy(nombreAuto[q],token);
        	q++;
        	return 1;
        }

}