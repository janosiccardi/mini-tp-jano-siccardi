# Sistemas Operativos y Redes  
## Trabajo Práctico de hilos y semáforos  

En este trabajo se debia realizar un programa, utilizando hilos y semaforos, que simule una competencia donde tres equipos deben realizar dos hamburguesas y el primero en hacerlo gana.  Los equipos debian leer la receta desde un archivo y guardar la salida en otro. Para esto se creo un struct paso, un struct semaforos , un struct parametro y tres hilos donde cada uno simula un equipo y se le asigno su struct parametro los semaforos, el numero de equipo, los pasos a realizar y el archivo donde se registra la salida. 
 
###### Funciones:   

**main():**  En esta funcion, por un lado se crean y se inician  los mutex e hilos y se le asigna a cada hilo su struct con un puntero del archivo donde se debe registrar la competencia y el numero de equipo correspondiente. Por otro lado aqui tambien se imprime al inicio un cartel que informa que la competencia comenzo y al finalizar  imprime quien fue el ganador. 
 
**imprimirAccion():** Esta funcion recibe un puntero que se le asigna a un  “struct parametro *my data” con la informacion de un hilo y un puntero con la accion que se debe imprimir. Esta se encarga de verificar que esa accion exista dentro de arreglo de pasos que tiene ese hilo, busca cuales son los ingredientes e imprime.  

**ejecutarReceta():** Esta funcion recibe el puntero al struct parametro de el equipo(hilo) que la esta ejecutando, lee la receta y agrega las acciones e ingredientes al arreglo de pasos de cada equipo. Luego crea e inicializa los hilos que van a ejecutar las tareas y sus semaforos correspondientes. Y llama a cada una de las funciones de tareas. 

###### Funciones de tareas:

Estas funciones reciben el puntero con la informacion del equipo, crean un char *accion con la palabra correspondiente, luego verifican si su semaforo correspondiente esta habilitado, simulan la tarea que debe realizar el equipo con “usleep” y  llaman a la funcion imprimirAccion() con dicha accion y la informacion del equipo. 

**cortar():**  Simula la accion de cortar la cebolla, el ajo y el perejil y al finalizar hace un signal al semaforo que habilita la accion mezclar. 

**preparar():** Simula la accion de cortar el tomate y preparar la lechuga y al finalizar hace un signal al semaforo “lechugaYtomate” que indica que estan listos para colocarse. 

**hornear():** Comprueba que el horno este libre (mutex), simula la accion de hornear los panes y hace un signal al  semaforo “panListo” que indica que los panes estan listos. 

**mezclar():** Verifica el semaforo mezclar, simula la accion de mezclar lo previamente cortado junto con la carne picada y habilita con un signal el semaforo salar. 

**salar():** Verifica su semaforo y si el salero (mutex) esta en uso, simula la accion de salar la mezcla y por ultimo libera el salero(mutex) y habilita el semaforo armarMedallones con un signal. 

**armar():** Comprueba que este habilitado su semaforo, simula la accion de armar los medallones y por ultimo hace un signal al semaforo que habilita la accion cocinar. 

**cocinar():** Bloquea el mutex de plancha si esta estaba libre, verifica si el semaforo cocinar esta habilitado, simula la accion de cocinar los dos medallones y al finalizar hace un signal al semaforo que indica que los medallones estan listos. 

**terminar():** Verifica que los medallones, los panes, la lechuga y el tomate esten listos, comprobando que esten habilitados sus dichos semaforos, simula la accion de armar las hamburguesas, y verifica si este equipo es el primero en terminar para guardar su numero de equipo en la variable “ganador”. 


###### Sincronizacion de tareas y Prueba de escritorio: 

 cortar()              preparar()               mutex(horno) 
 V(s_mezclar)     V(s_lechugaYtomate)             hornear()        
                                                V(s_panListo)
 p(s_mezclar)                                    mutex(horno)
   mezclar() 
  V(s_salar) 
 
 
 p(s_salar) 
mutex(salero) 
  salar() 
V(s_armarMedallones) 
 mutex(salero) 
 
 
p(s_armarMedallones)   
    armar() 
  V(s_cocinar) 
 
 
 p(s_cocinar) 
mutex(plancha) 
   cocinar() 
V(s_medallonesListos)  
 mutex(plancha) 
 
 
p(s_medallonesListos) 
  p(s_panListo) 
p(s_lechugaYtomate) 
    terminar()  
 
 
 
-------------------------------------------------------------------------------------------------------------------- 
                  |preparar()|   horenar()   |cortar()|mezclar()|    salar()    |armar()|    cocinar()  |terminar()| 
------------------|----------|-------|-------|--------|---------|-------|-------|-------|-------|-------|----------|
s_lechugaYtomate  |     0    |   1   |   1   |    1   |    1    |   1   |   1   |   1   |   1   |   1   |     1    |
s_panListo        |     0    |   0   |   1   |    1   |    1    |   1   |   1   |   1   |    1  |   1   |     1    |
s_mezclar         |     0    |   0   |   0   |    1   |    0    |   0   |   0   |   0   |    0  |   0   |     0    |
s_salar           |     0    |   0   |   0   |    0   |    1    |   1   |   0   |   0   |    0  |   0   |     0    | 
s_armarMedallones |     0    |   0   |   0   |    0   |    0    |   0   |   1   |   0   |    0  |   0   |     0    |
s_cocinar         |     0    |   0   |   0   |    0   |    0    |   0   |   0   |   1   |    1  |   0   |     0    |
s_medallonesListos|     0    |   0   |   0   |    0   |    0    |   0   |   0   |   0   |    0  |   1   |     1    |
------------------|----------|-------|-------|--------|---------|-------|-------|-------|-------|-------|----------|
mutex_horno       |     1    |   0   |   1   |    1   |    1    |   1   |   1   |   1   |    1  |   1   |     1    |
mutex_salero      |     1    |   1   |   1   |    1   |    1    |   0   |   1   |   1   |    1  |   1   |     1    |
mutex_plancha     |     1    |   1   |   1   |    1   |    1    |   1   |   1   |   1   |    0  |   1   |     1    |
