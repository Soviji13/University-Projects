/*--------------------------------------------------------
UNIX Shell Project
job_control module

Sistemas Operativos
Grados I. Informatica, Computadores & Software
Dept. Arquitectura de Computadores - UMA

Some code adapted from "Fundamentos de Sistemas Operativos", Silberschatz et al.
--------------------------------------------------------*/

// @author: Sofía Si Villalba Jiménez

#include "job_control.h"
#ifndef __APPLE__
#include <malloc.h>
#endif
#include <string.h>


/**-------- GET_COMMAND() ---------------------------------------------------------------
 * 
 * get_command() recibe la línea de comando que el usuario escribe, separándola en 
 * distintos argumentos, usando espacios en blanco como delimitadores
 */

void get_command(char inputBuffer[], int size, char *args[],int *background)
{
	/* Variables importantes */
	int length, /* número de caracteres en la línea de comandos */
		i,      /* índice del bucle para acceder al inputBuffer (almacena comando introducido) */
		start,  /* -1 si no estamos en una palabra, i si estamos dentro de una palabra que empieza en i */
		ct;     /* contador de argumentos que hemos extraído */
	
	ct = 0;					// No hemos extraído ningún argumento aún
	*background=0;			// Iniciamos en fg

	/* lee lo que el usuario introduce en la línea de comandos y lo mete en inputbuffer */
	length = read(STDIN_FILENO, inputBuffer, size);

	start = -1;
	if (length == 0)		// ^D a bajo nivel es end of file entonces es el único que hace que length sea 0
	{
		printf("\nBye\n");
		exit(0);            /* ^d was entered, end of user command stream */
	}
	if (length < 0){
		perror("error reading the command");
		exit(-1);           /* terminate with error code of -1 */
	}

	/* examinamos cada caracter del inputbuffer */
	for (i = 0; i < length; i++)
	{
		switch (inputBuffer[i])
		{
		/* ---- Espacio o tab ---- */
		case ' ':
		case '\t': 
			// Si lo último fue un carácter
			if(start != -1)			
			{
				args[ct] = &inputBuffer[start];    	// args[ct] apunta al inicio del argumento ct 
				ct++;								// Nos enontraremos ahora en un próximo argumento
			}
			inputBuffer[i] = '\0'; 					// añadimos carácter nulo para cerrar el string 
			start = -1;								// Ahora lo último no fue un carácter
			break;
		/* ---- Salto de línea o enter ---- */
		case '\n':
			if (start != -1)
			{
				args[ct] = &inputBuffer[start];
				ct++;
			}
			inputBuffer[i] = '\0';
			args[ct] = NULL; 						// Ya no habrán más argumentos
			break;
		/* ---- Cualquier otro carácter --- */
		default :
			/* Si nos encontramos un indicador de bg */
			if (inputBuffer[i] == '&')				
			{
				*background  = 1;					// activamos el bg para el comando

				if (start != -1)					// si lo último fue un carácter, lo de siempre
				{
					args[ct] = &inputBuffer[start];
					ct++;
				}
				inputBuffer[i] = '\0';				// cerramos el string
				args[ct] = NULL; 					// Ya no habrán más argumentos
				i=length; 							// Forzamos a salir del bucle

			}
			
			// Si lo último fue un espacio, entonces start apunta al inicio del nuevo argumento
			else if (start == -1) start = i;  

			// Si lo último fue un carácter seguimos iterando normalmente
			// Si fue enter, 100% es el último carácter así que terminamos

		}  	// end switch
	
	}  // end for, iteramos al siguiente carácter
	
	args[ct] = NULL; /* Asegura que el array de argumentos termine con NULL, como espera execvp */
}


// -------- NEW_JOB() -----------------------------------------------------------------
/**
 * Descripción: 
 * 
 * Crea un nodo individual, un job suelto sin meterlo en ninguna lista
 * 
 * Devuelve un puntero a un nodo (trabajo) con sus valores incializados
 * y NULL si no pudo realizarse la reserva de memoria
 * 
 */

/**
 * Parámetros:
 * 
 * pid: id del proceso al que asignarle la tarea
 * command: tarea que asignar al proceso con pid
 * state: bg o fg o stopped
 * 
 */

 /**
  * Struct job (ya viene definida):
  * {
  *		pid_t pgid;            Si eres el líder, tienes el número de procesos que hay en la lista
  *							   si eres otro, tienes tu pid real
  *		char * command;        comando que está ejecutando = nombre
  *		enum job_state state;  bg o fg
  *		struct job_ *next;     puntero al siguiente trabajo en la lista
  * }
  */

job * new_job(pid_t pid, const char * command, enum job_state state)
{
	job * aux;
	aux=(job *) malloc(sizeof(job));
	aux->pgid=pid;
	aux->state=state;
	aux->command=strdup(command);
	aux->next=NULL;
	return aux;
}

// -------- ADD_JOB() --------------------------------------------------------------

/**
 * Anotación importante:
 * 
 * La cabeza de la lista no es un job real (dummy node), es un job vacío que
 * usamos para:
 * 
 * 		-> Insertar siempre al principio
 * 		-> Evitar casos especiales cuando la lista está vacía
 * 		-> Llevar un contador de trabajos en su campo pgid
 * 
 * Es decir que list es un job, pero solo sirve para agrupar en lista a otros
 * 
 * list -> job que sirve como cabecera de la lista
 * item -> job que vamos a introducir como el "primero" de la lista
 * 
 */

 /**
  * Ejemplo de uso:
  * 
  * 	Antes: [list] → [job1] → [job2] → NULL
  *
  * 	Después: [list] → [item] → [job1] → [job2] → NULL
  * 
  */

void add_job (job * list, job * item)
{
	job * aux=list->next;		// Guarda el primer job real (NULL si lista vacía)
	list->next=item;			// Insertamos el nuevo job después del dummy node
	item->next=aux;				// Nuevo job apunta al primero antes (si había)
	list->pgid++;				// Aumenta el contador de trabajos

}

// -------- DELETE_JOB() ---------------------------------------------------------------
/**
 * Elimina un job de la lista específico
 * 
 * Devuelve 1 si tuvo éxito y 0 si no se encontró
 * 
 */
int delete_job(job * list, job * item)
{
	job * aux=list;				// Empezamos desde el dummy node

	/* Recorremos hasta encontrar item*/
	while(aux->next!= NULL && aux->next!= item) aux=aux->next;

	/* Si se encontró, lo eliminamos y restamos uno al contador */
	if(aux->next)
	{
		aux->next=item->next;
		free(item->command);
		free(item);
		list->pgid--;
		return 1;
	}
	else
		return 0;					

}
// --------- GET_ITEM_BYPID () --------------------------------------------------------------

/**
 * Descripción:
 * 
 * Busca en list el job cuyo id sea pid y devuelve un puntero a ese trabajo
 * 
 * Si no encuentra nada, debuelve NULL
 * 
 */
job * get_item_bypid  (job * list, pid_t pid)
{
	job * aux=list;
	while(aux->next!= NULL && aux->next->pgid != pid) aux=aux->next;
	return aux->next;
}

// --------- GET_ITEM_BYPOS () --------------------------------------------------------------

/**
 * Descripción:
 * 
 * Busca en list el job que está en la posición n (ignorando el nodo dummy)
 * y devuelve un puntero a ese trabajo
 * 
 * Si no existe esa posición o no es válida, debuelve NULL
 * 
 */
job * get_item_bypos( job * list, int n)
{
	job * aux=list;
	if(n<1 || n>list->pgid) return NULL;
	n--;
	while(aux->next!= NULL && n) { aux=aux->next; n--;}
	return aux->next;
}

// ---------- PRINT_ITEM () --------------------------------------------------------------------------------
/*imprime una linea en el terminal con los datos del trabajo: pid, nombre ... */
void print_item(job * item)
{

	printf("pid: %d, command: %s, state: %s\n", item->pgid, item->command, state_strings[item->state]);
}

// ----------- PRINT_LIST() ----------------------------------------------------------------------------
/*recorre la lista y le aplica la funcion pintar a cada elemento */
/**
 * Descripción:
 * 
 * 	Previamente debemos tener hecho un procedimiento nuestro propio para 
 *  mostrar por pantalla algo de un trabajo (podemos usar la que ya está
 *  definida print_item)
 * 
 * 	El procedimiento debe ser: void nombre(job * nombreJob)
 * 
 */
void print_list(job * list, void (*print)(job *))
{
	int n=1;
	job * aux=list;
	// printf("Contents of %s:\n",list->command);
	while(aux->next!= NULL)
	{
		printf(" [%d] ",n);
		print(aux->next);
		n++;
		aux=aux->next;
	}
}

// ------------ TERMINAL_SIGNALS() ----------------------------------------------------------
// cambia la accion de las señales relacionadas con el terminal
void terminal_signals(void (*func) (int))
{
	signal (SIGINT,  func); // crtl+c interrupt tecleado en el terminal
	signal (SIGQUIT, func); // ctrl+\ quit tecleado en el terminal
	signal (SIGTSTP, func); // crtl+z Stop tecleado en el terminal
	signal (SIGTTIN, func); // proceso en segundo plano quiere leer del terminal
	signal (SIGTTOU, func); // proceso en segundo plano quiere escribir en el terminal
}

// ------------- BLOCK_SIGNAL() -------------------------------------------------------------
void block_signal(int signal, int block)
{
	/* declara e inicializa máscara */
	sigset_t block_sigchld;
	sigemptyset(&block_sigchld );
	sigaddset(&block_sigchld,signal);
	if(block)
	{
		/* bloquea señal */
		sigprocmask(SIG_BLOCK, &block_sigchld, NULL);
	}
	else
	{
		/* desbloquea señal */
		sigprocmask(SIG_UNBLOCK, &block_sigchld, NULL);
	}
}

// --------------- PARSE_REDIRECTIONS() --------------------------------------------------------
// Parse redirections operators '<' '>' once args structure has been built
// Call immediately after get_commad()
//    get_command(..);
//    char *file_in, *file_out;
//    parse_redirections(args, &file_in, &file_out);
//
// For a valid redirection, a blank space is required before and after
// redirection operators '<' or '>'
// ----------------------------------------------------------------------------------------
/**
 * Descripción:
 * 
 * 	Busca operadores de redirección (< y >) dentro del array de argumentos y guarda:
 * 
 * 	-> El nombre del fichero de entrada si hay un <
 * 	-> El nombre del fichero de salida si hay un >
 * 
 */

 /**
  * Ejemplo de uso:
  * 
  * 	Escribo por la terminal "cat archivo.txt > salida.txt" - Debe haber espacios 
  * 	args = {"cat", "archivo.txt", ">", "salida.txt", NULL}; - Datos que tiene args
  * 	
  * 	Ahora en el código tengo:
  * 
  * 	char *file_in, *file_out;
  * 	parse_redirections(args, &file_in, &file_out);
  * 
  * 	Resultado:
  * 
  * 	file_out = "salida.txt"
  * 	file_in = NULL
  * 	args = {"cat", "archivo.txt", NULL};
  * 	
  * 	NOTA: GUARDAMOS SIEMPRE EL ARGUMENTO DE LA DERECHA SEA OUT O IN
  */


  /**
   * Diferencia entre < y > 
   * 
   * 	> : Mete la salida de la izquierda a la derecha
   * 	comando > fichero: la salida del comando la metemos en el fichero
   * 
   * 	< : Mete como parámetro la derecha en la izquierda
   * 	comando < fichero: aplicamos el comando al fichero
   */
int parse_redirections(char **args, char **file_in, char **file_out)
{

	int comprobador = 0;	// Si es < o > devuelve 0, si es >> devuelve 1

	// Inicializamos a null, por si no hay redirección
	*file_in = NULL;
	*file_out = NULL;

	// Guardamos el array original para poder modificarlo si hay error de sintaxis
	char **args_start = args;

	// Recorremos args hasta llegar al final (NULL)
	while (*args)
	{
		// Comprobamos si el argumento actual es un operador de redirección
		int is_in = !strcmp(*args, "<");	// 1 si arg = "<"
		int is_out = !strcmp(*args, ">");	// 1 si arg = ">"
		int is_append = !strcmp(*args, ">>");

		/* Si hemos detectado un operador */
		if (is_in || is_out || is_append)
		{
			args++;							// Pasamos al siguiente argumento - Nombre del archivo
			if (*args)						// Si existe el siguiente argumento
			{
				/* Si es de entrada (<) */
				if (is_in)					
					*file_in = *args;		// Guardamos el nombre de archivo de entrada
				/* Si es de salida (>) */	
				if (is_out)
				{
					*file_out = *args;	// Guardamos el nombre del archivo de salida
				}
				if (is_append)
				{
					comprobador = 1;
					*file_out = *args;
				}
				/* Limpiamos: quitamos el operador y el nombre del archivo */
				char **aux = args + 1;
				while (*aux)
				{
					*(aux - 2) = *aux;
					aux++;
				}
				*(aux - 2) = NULL;			// Terminamos el array con NULL
				args--;						// Volvemos atrás para seguir el bucle sin saltar nada
			}
			else							// Si no existe el siguiente argumento
			{
				/* Syntax error */
				fprintf(stderr, "syntax error in redirection\n");
				args_start[0] = NULL; // Invalidamos los argumentos para no ejecutar nada
			}
		}
		/* Si no hay ac*/
		else
		{
			args++;							// Seguimos iterando
		}
	}

	return comprobador;
}
