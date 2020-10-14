#include <stdio.h>      // libreria estandar
#include <stdlib.h>     // para usar exit y funciones de la libreria standard
#include <string.h>
#include <pthread.h>    // para usar threads
#include <semaphore.h>  // para usar semaforos
#include <unistd.h>


#define LIMITE 50

//mutex
pthread_mutex_t plancha;
pthread_mutex_t salero;
pthread_mutex_t horno;


//verificar ganador
int alguienGano=0;
int ganador;

//creo estructura de semaforos 
struct semaforos {
    sem_t sem_mezclar;
    sem_t sem_salar;
    sem_t sem_armarMedallones;    
    sem_t sem_cocinar;
    sem_t sem_panListo;
    sem_t sem_medallonesListos;
    sem_t sem_lechugaYtomate;
    
	//poner demas semaforos aqui
};

//creo los pasos con los ingredientes
struct paso {
   	char accion [LIMITE];
 	char ingredientes[5][LIMITE];
   
};

//creo los parametros de los hilos 
struct parametro {
 	int equipo_param;
 	FILE *competencia;
 	struct semaforos semaforos_param;
 	struct paso pasos_param[8];
};

//funcion para imprimir las acciones y los ingredientes de la accion
void* imprimirAccion(void *data, char *accionIn) {	
	struct parametro *mydata = data;
	FILE *competencia= mydata->competencia;
	//calculo la longitud del array de pasos 
	int sizeArray = (int)( sizeof(mydata->pasos_param) / sizeof(mydata->pasos_param[0]));
	//indice para recorrer array de pasos 
	int i;
	for(i = 0; i < sizeArray; i ++){
		//pregunto si la accion del array es igual a la pasada por parametro (si es igual la funcion strcmp devuelve cero)
		if(strcmp(mydata->pasos_param[i].accion, accionIn) == 0){
		printf("Equipo %d - %s " , mydata->equipo_param, mydata->pasos_param[i].accion);
		fprintf(competencia,"Equipo %d - %s " , mydata->equipo_param, mydata->pasos_param[i].accion);
		//calculo la longitud del array de ingredientes
		int sizeArrayIngredientes = (int)( sizeof(mydata->pasos_param[i].ingredientes) / sizeof(mydata->pasos_param[i].ingredientes[0]) );
		//indice para recorrer array de ingredientes
		int h;		 
			for(h = 0; h < sizeArrayIngredientes; h++) {
				//consulto si la posicion tiene valor porque no se cuantos ingredientes tengo por accion 
				if(strlen(mydata->pasos_param[i].ingredientes[h]) != 0) {
							printf("%s ",mydata->pasos_param[i].ingredientes[h]);
							fprintf(competencia,"%s ",mydata->pasos_param[i].ingredientes[h]);
				}
			}			
		}
	}
}
//funcion para tomar de ejemplo
void* cortar(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "cortar";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;	
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 2000000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    sem_post(&mydata->semaforos_param.sem_mezclar);
    pthread_exit(NULL);
}
void* mezclar(void *data) {
	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_mezclar);
	char *accion = "mezclar";	
	imprimirAccion(mydata,accion);
	usleep( 3000000 );
    sem_post(&mydata->semaforos_param.sem_salar);
    pthread_exit(NULL);
}
void* salar(void *data) {
	pthread_mutex_lock(&salero);
	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_salar);
	char *accion = "salar";	
	imprimirAccion(mydata,accion);
	usleep( 1000000 );
    sem_post(&mydata->semaforos_param.sem_armarMedallones);
    pthread_mutex_unlock(&salero);
    pthread_exit(NULL);
}
void* hornear(void *data) {
	pthread_mutex_lock(&horno);
	char *accion = "hornear";
	struct parametro *mydata = data;
	FILE *competencia= mydata->competencia;
	imprimirAccion(mydata,accion);	
	usleep( 10000000 );
	printf("Equipo %d - panes listos\n " , mydata->equipo_param);
	fprintf(competencia,"Equipo %d - panes listos\n " , mydata->equipo_param);
    	sem_post(&mydata->semaforos_param.sem_panListo);
    	pthread_mutex_unlock(&horno);
    	pthread_exit(NULL);
}
void* armar(void *data) {
	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_armarMedallones);
	char *accion = "armar";
	imprimirAccion(mydata,accion);
	usleep( 4000000 );
    	sem_post(&mydata->semaforos_param.sem_cocinar);
    	pthread_exit(NULL);
}

void* cocinar(void *data) {
	pthread_mutex_lock(&plancha);
	struct parametro *mydata = data;
	FILE *competencia= mydata->competencia;
	sem_wait(&mydata->semaforos_param.sem_cocinar);
	char *accion = "cocinar";
	imprimirAccion(mydata,accion);
	usleep( 5000000 );
	printf("Equipo %d - 1er medallon listo\n " , mydata->equipo_param);
	fprintf(competencia,"Equipo %d - 1er medallon listo\n " , mydata->equipo_param);
	imprimirAccion(mydata,accion);
	usleep( 5000000 );
	printf("Equipo %d - 2do medallon listo\n " , mydata->equipo_param);
	fprintf(competencia,"Equipo %d - 2do medallon listo\n " , mydata->equipo_param);		
    sem_post(&mydata->semaforos_param.sem_medallonesListos);
    pthread_mutex_unlock(&plancha);	
    pthread_exit(NULL);
}
void* preparar(void *data) {	
	char *accion = "preparar";
	struct parametro *mydata = data;
	imprimirAccion(mydata,accion);
	usleep( 5000000 );	
    sem_post(&mydata->semaforos_param.sem_lechugaYtomate);
    pthread_exit(NULL);
}

void* terminar(void *data) {
	struct parametro *mydata = data;
	FILE *competencia= mydata->competencia;
	sem_wait(&mydata->semaforos_param.sem_medallonesListos);
	sem_wait(&mydata->semaforos_param.sem_panListo);
	sem_wait(&mydata->semaforos_param.sem_lechugaYtomate);
	char *accion = "terminar";
	imprimirAccion(mydata,accion);
	usleep(3000000 );
	printf("Equipo %d - 1er hamburguesa lista\n " , mydata->equipo_param);
	fprintf(competencia,"Equipo %d - 1er hamburguesa lista\n " , mydata->equipo_param);
	imprimirAccion(mydata,accion);
	usleep(3000000 );
	printf("Equipo %d - 2da hamburguesa lista\n " , mydata->equipo_param);
	fprintf(competencia,"Equipo %d - 2da hamburguesa lista\n " , mydata->equipo_param);
	if (alguienGano!=1){
		ganador=mydata->equipo_param;
		}
	alguienGano=1;		
    	pthread_exit(NULL);
}

void* ejecutarReceta(void *i) {
	
	//variables semaforos
	sem_t sem_mezclar;
    	sem_t sem_salar;
    	sem_t sem_armarMedallones;
   	sem_t sem_cocinar;
   	sem_t sem_panListo;
    	sem_t sem_medallonesListos;
    	sem_t sem_lechugaYtomate;
	//crear variables semaforos aqui
	
	//variables hilos
	pthread_t p1;
	pthread_t p2;
	pthread_t p3;
	pthread_t p4;
	pthread_t p5;
	pthread_t p6;
	pthread_t p7;
	pthread_t p8; 
	//crear variables hilos aqui
	
	
	
	//reservo memoria para el struct
	struct parametro *pthread_data = i;

	//seteo los otros valores al struct
	
	
	//seteo semaforos
	
	pthread_data->semaforos_param.sem_mezclar = sem_mezclar;
    	pthread_data->semaforos_param.sem_salar = sem_salar;    
    	pthread_data->semaforos_param.sem_armarMedallones = sem_armarMedallones;
    	pthread_data->semaforos_param.sem_cocinar = sem_cocinar;
    	pthread_data->semaforos_param.sem_panListo = sem_panListo;
    	pthread_data->semaforos_param.sem_medallonesListos = sem_medallonesListos;
    	pthread_data->semaforos_param.sem_lechugaYtomate = sem_lechugaYtomate;
	//setear demas semaforos al struct aqui	

	//lee receta
	FILE *receta;	
	receta= fopen("receta.txt","r");
	char linea[LIMITE];
	//seteo las acciones y los ingredientes		
	int j=0;		
    	while(fgets(linea, LIMITE, (FILE*) receta)){
    	char delim[] = " ";    		
    	char *ptr = strtok(linea, delim);					
		strcpy(pthread_data->pasos_param[j].accion,ptr);		
		int i=0;
		while (ptr != NULL){
			if(strcmp(pthread_data->pasos_param[j].accion, ptr)!=0){
				strcpy(pthread_data->pasos_param[j].ingredientes[i], ptr);				
			}
			ptr = strtok(NULL, delim);
			i++;		
		}
		j++;		
	}   

	//inicializo los semaforos
	sem_init(&(pthread_data->semaforos_param.sem_mezclar),0,0);
    	sem_init(&(pthread_data->semaforos_param.sem_salar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_armarMedallones),0,0);
    	sem_init(&(pthread_data->semaforos_param.sem_cocinar),0,0); 
    	sem_init(&(pthread_data->semaforos_param.sem_panListo),0,0); 
    	sem_init(&(pthread_data->semaforos_param.sem_medallonesListos),0,0);
    	sem_init(&(pthread_data->semaforos_param.sem_lechugaYtomate ),0,0);


	//creo los hilos a todos les paso el struct creado (el mismo a todos los hilos) ya que todos comparten los semaforos 
    int rc;
    rc = pthread_create(&p1,                           //identificador unico
                            NULL,                          //atributos del thread
                                cortar,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia	
	rc = pthread_create(&p2,                           
                            NULL,                         
                                mezclar,            
                                pthread_data);                                
	rc = pthread_create(&p3,                           
                            NULL,                         
                                hornear,            
                                pthread_data);
	rc = pthread_create(&p4,                           
                            NULL,                         
                                salar,            
                                pthread_data);                                
	rc = pthread_create(&p5,                           
                            NULL,                         
                                preparar,            
                                pthread_data);
	rc = pthread_create(&p6,                           
                            NULL,                         
                                cocinar,            
                                pthread_data);
                                
	rc = pthread_create(&p7,                           
                            NULL,                         
                                armar,            
                                pthread_data);
	rc = pthread_create(&p8,                           
                            NULL,                         
                                terminar,            
                                pthread_data);  
	                                                                                                                                        
	//join de todos los hilos
	pthread_join (p1,NULL);
	pthread_join (p2,NULL);
	pthread_join (p3,NULL);
	pthread_join (p4,NULL);
	pthread_join (p5,NULL);
	pthread_join (p6,NULL);
	pthread_join (p7,NULL);
	pthread_join (p8,NULL);
	//crear join de demas hilos	

	//valido que el hilo se alla creado bien 
    if (rc){
       printf("Error:unable to create thread, %d \n", rc);
       exit(-1);
     }
	 
	//destruccion de los semaforos 
	sem_destroy(&sem_mezclar);
	sem_destroy(&sem_salar);   
    sem_destroy(&sem_armarMedallones);
    sem_destroy(&sem_cocinar);
    sem_destroy(&sem_panListo);
    sem_destroy(&sem_medallonesListos);
    sem_destroy(&sem_lechugaYtomate);
	//destruir demas semaforos 
	
	
	
	//salida del hilo
	pthread_exit(NULL);
}


int main (){
		
	//creo los nombres de los equipos 
	int rc;
	FILE *competencia;	
	competencia = fopen("competencia.txt","w+");
	
	//seteo  el FILE de salida y el numero de equipo a cada hilo
	struct parametro *equipo1Param =malloc(sizeof(struct parametro));
	struct parametro *equipo2Param =malloc(sizeof(struct parametro));
	struct parametro *equipo3Param =malloc(sizeof(struct parametro));	
	equipo1Param->competencia = competencia;
	equipo2Param->competencia = competencia;
	equipo3Param->competencia = competencia;
	equipo1Param->equipo_param=1;
	equipo2Param->equipo_param=2;
	equipo3Param->equipo_param=3;
		
	printf("----------------------¡¡COMIENZA LA COMPETENCIA!!---------------------- \n ");
	fprintf(competencia,"----------------------¡¡COMIENZA LA COMPETENCIA!!---------------------- \n ");
	
	
	pthread_mutex_init(&salero,NULL);
	pthread_mutex_init(&horno,NULL);
	pthread_mutex_init(&plancha,NULL);

	//creo las variables los hilos de los equipos
	pthread_t equipo1; 
	pthread_t equipo2;
	pthread_t equipo3;
 
	//inicializo los hilos de los equipos
    rc = pthread_create(&equipo1,                           //identificador unico
                            NULL                          //atributos del thread
                                ,ejecutarReceta,             //funcion a ejecutar
                                equipo1Param); 

    rc = pthread_create(&equipo2,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipo2Param);

    rc = pthread_create(&equipo3,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipo3Param);

   if (rc){
       printf("Error:unable to create thread, %d \n", rc);
       exit(-1);
     } 

	//join de todos los hilos
	pthread_join (equipo1,NULL);
	pthread_join (equipo2,NULL);
	pthread_join (equipo3,NULL);

	//imprime quien fue el ganador
	printf("----------------------¡¡HA GANADO EL EQUIPO %d!!---------------------- \n", ganador);
	fprintf(competencia,"----------------------¡¡HA GANADO EL EQUIPO %d!!---------------------- \n", ganador);	
	
	fclose(competencia);
	
	pthread_mutex_destroy(&salero);
	pthread_mutex_destroy(&horno);
	pthread_mutex_destroy(&plancha);

    pthread_exit(NULL);
}
