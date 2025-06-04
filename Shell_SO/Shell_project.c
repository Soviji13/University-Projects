/**
UNIX Shell Project

To compile and run the program:
   $ gcc Shell_project.c job_control.c -o Shell
   $ ./Shell          
	(then type ^D to exit program)

**/

// @author: Sofía Si Villalba Jiménez

#include <dirent.h>			// Para zjobs

#include "job_control.h"   // Acordarnos de compilarlo con el módulo job_control.c 
#define MAX_LINE 256 /* 256 carateres por línea, por comando, deberían ser suficientes */

// ------------------------------------------------------------------------------------------------------------------
//							VARIABLES GLOBALES
// ------------------------------------------------------------------------------------------------------------------

job *listaBg = NULL;		// Lista de trabajos en bg o suspendidos

// ------------------------------------------------------------------------------------------------------------------
//							MANEJADORES
// ------------------------------------------------------------------------------------------------------------------

/* Señal SIGCHLD ---------------------------------------------------------------------------------------------------- */

// La usaremos para avisar al padre que un proceso hijo ha terminado o se ha detenido en bg
// y evitar que se quede zombie (por ahora)

// SOLO SE APLICA A LOS PROCESOS EN la lista BG (POR AHORA)
// SOLO SE APLICA A LOS PROCESOS EN la lista BG (POR AHORA)
void manejadorSIGCHLD(int signal)
{
    int status;
    pid_t wpid;
    job *parado;

    // Verificación inicial de la lista
    if (listaBg == NULL) return;

    // Recorremos la lista bg
    for (int i = 1; i <= list_size(listaBg); i++)
    {
        parado = get_item_bypos(listaBg, i);
        if (parado == NULL) continue;

        // Verificamos el estado del proceso
        wpid = waitpid(parado->pgid, &status, WUNTRACED | WNOHANG | WCONTINUED);
        
        // Si no hay cambios o error
        if (wpid <= 0) continue;

        // Proceso terminado (normal o por señal)
        if (WIFEXITED(status) || WIFSIGNALED(status))
        {
            if (WIFEXITED(status))
            {
                printf("Background process %s (%d) Exited, info: %d\n", 
                       parado->command, wpid, WEXITSTATUS(status));
            }
            else if (WIFSIGNALED(status))
            {
                printf("Background process %s (%d) Exited, info: %d\n", 
                       parado->command, wpid, WTERMSIG(status));
            }

            // Corregimos la lógica del delete_job
            if (delete_job(listaBg, parado) == 1)  // delete_job retorna 1 en éxito
            {
                i--;  // Ajustamos el índice ya que la lista se ha reducido
            }
            else
            {
                fprintf(stderr, "Error al intentar eliminar proceso de la lista\n");
            }
        }
        // Proceso detenido
        else if (WIFSTOPPED(status) && parado->state == BACKGROUND)
        {
            printf("Background pid: %d, command: %s, state: Suspended, info: %d\n", 
                   wpid, parado->command, WSTOPSIG(status));
            parado->state = STOPPED;
        }
        // Proceso continuado
        else if (WIFCONTINUED(status))
        {
            if (parado->state == STOPPED)
            {
                printf("Background process %s (%d) continued\n", 
                       parado->command, wpid);
                parado->state = BACKGROUND;
            }
        }
    }
}

/* Señal SIGHUP ----------------------------------------------------------------------------------------------------- */

// La usaremos para añadir al fichero "hup.txt" el mensaje "SIGHUP recibido.\n", cada vez que el shell reciba esa señal

void manejadorSIGHUP (int signal)
{
	// Intentamos abrir el fichero hup.txt
	FILE *fp; 
	fp = fopen("hup.txt", "a");	// Lo abre en modo append

	/* Si todo fue bien */
	if(fp)
	{
		fprintf(fp, "SIGHUP recibido.\n");	// Escribe en el fichero
		// Probamos a cerrarlo correctamente
		if(fclose(fp) != 0)
		{
			fprintf(stderr, "Error al cerrar hup.txt\n");
		}
	}
	/* Si algo hubo mal */
	else
	{
		fprintf(stderr, "Error al abrir hup.txt\n");
	}
}


// ------------------------------------------------------------------------------------------------------------------
//							COMANDOS INTERNOS
// ------------------------------------------------------------------------------------------------------------------

/* cd ---------------------------------------------------------------------------------------------------------------*/
int cd_interno (char **args)
{
	char *ruta = args[1];				// Esto será con lo que nos moveremos
	char cwd[1024];						// Buffer para el directorio actual

	if (getcwd(cwd, sizeof(cwd)) == NULL)	// a la ruta cwd le asignaremos ahora la ruta actual
	{
		perror("getcwd");					// Manejamos posibles errores
		return (1);
	}

	/* Si se pasa sin argumentos */
	if (ruta == NULL)
	{
		ruta = getenv ("HOME");		// guardamos la ruta absoluta del usuario

		if (ruta == NULL)					// manejamos posibles errores
		{
			fprintf(stderr, "cd: No se encontró la variable HOME\n");
			return (1);
		}
	}
	/* Caso especial: volver al directorio anterior con -- (el .. lo detecta solo) */
	else if (strcmp(ruta, "-") == 0)
	{
		ruta = getenv("OLDPWD");		// guardamos la ruta anterior del usuario

		if (ruta == NULL)
		{
			fprintf(stderr, "cd: No se puede acceder a la ruta anterior\n");
			return (1);
		}
	}

	/* Cambiamos de directorio habiéndo visto ya los casos especiales */
	if(chdir(ruta) != 0)
	{
		perror("cd");					// Manejamos posibles errores
		return (1);
	}
	else							
	{
		// Si todo ha salido bien, debemos actualizar tanto OLDPWD como PWD
		setenv("OLDPWD", cwd, 1);					// a OLDPWD (ruta anteior) le asignaremos la ruta actual
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
            setenv("PWD", cwd, 1);
        }
	}

	return (0);
}

/* jobs ------------------------------------------------------------------------------------------------------------ */

/* Tarea 4: Imprime lista de tareas en segundo plano y suspendidas o un mensaje si no 
	hay ninguna 
*/

int jobs_interno (void)
{
	printf("Contents of Lista de trabajos:\n");	// Sale siempre

	/* Si hay algo en la lista */
	if (listaBg != NULL)
	{
		print_job_list (listaBg);
	}

	return 0;
}

/* fg ---------------------------------------------------------------------------------------------------------------- */

// Ponemos en fg una tarea que estaba en bg o Stopped (suspendidas) 

/*
	Primer parámetro: identificador del lugar que ocupa esa tarea en la lista
	Si no se pone nada, pone en fg el primer comando
*/
int fg_interno (char *arg)
{
    if (listaBg == NULL) return 1;				// Si la lista está vacía, nos vamos

    int n = list_size(listaBg);			
    int pos;									// Trabajo que pondremos en fg y actualizaremos
	
	/* Si no hay argumentos */ 
    if (arg == NULL) 
	{
        pos = 1; 								// Actualizaremos el primer trabajo de todos
    } 
	/* Si hay argumentos */
	else 
	{
        pos = atoi(arg);							// Actualizaremos el trabajo que nos pasen por argumento
        if (pos <= 0 || pos > n)					// Si la posición no es válida
		{
			fprintf(stderr, "Posición pasada por argumento no válida\n");
			return (1);
		}	
    }

    job *trabajo = get_item_bypos(listaBg, pos);		// Cogemos el trabajo correspondiente

    if (trabajo == NULL)								// Si no encontamos el trabajo
	{
		fprintf(stderr, "Error, trabajo seleccionado no se encuentra en la lista de bg o stopped\n");
		return (1);
	} 	


	/* Obtenemos id del trabajo para poder reanudar este */
    pid_t pid = trabajo->pgid;
    kill(-pid, SIGCONT); 							// Reanudar el grupo de procesos ('-' sirve para eso)

    tcsetpgrp(STDIN_FILENO, pid); 					// Le damos el control de terminal

    int status;				
    waitpid(pid, &status, WUNTRACED);				// Esperamos a que termine el proceso (hijo) y recogemos su info

    tcsetpgrp(STDIN_FILENO, getpid()); 				// Recuperamos el control de la terminal

	/* Manejamos las formas en las que ha terminado el hijo */

	/* Si ha sido bloqueado - suspendido */
    if (WIFSTOPPED(status)) 
	{
        printf("Foreground pid: %d, command: %s, Suspended, info: %d\n", pid, trabajo->command, WSTOPSIG(status));
        trabajo->state = STOPPED;
    }
	/* Si ha terminado por una señal */ 
	else if (WIFSIGNALED(status)) 
	{
		// Mostramos su información
		printf("Foreground pid: %d, command: %s, Signaled, info: %d\n", pid, trabajo->command, WTERMSIG(status));

		// Eliminamos el trabajo de la lista bg
        block_SIGCHLD();
        delete_job(listaBg, trabajo);
        unblock_SIGCHLD();

    }
	/* Si ha terminado normalmente*/
	else if (WIFEXITED(status))
	{
		// Mostramos su información
		printf("Foreground pid: %d, command: %s, Exited, info: %d\n", pid, trabajo->command, WEXITSTATUS(status));

		// Eliminamos el trabajo de la lista bg
        block_SIGCHLD();
        delete_job(listaBg, trabajo);
        unblock_SIGCHLD();
	}
	
    return 0;
}

/* bg ---------------------------------------------------------------------------------------------------------------- */

// Ponemos en bg una tarea que estaba en Stopped (suspendidas) 

/*
	Primer parámetro: identificador del lugar que ocupa esa tarea en la lista
	Si no se pone nada, pone en bg el primer comando detenido
*/
int bg_interno (char *arg)
{
	/* Si no hay nada en la lista bg */
    if (listaBg == NULL)
	{
		fprintf(stderr, "La lista de procesos en bg está vacía\n");
		return (1);
	}	

	/* Si la lista bg no está vacía */
    int n = list_size(listaBg);						
    int pos = 0;									// Trabajo que será actualizado

	/* Si no se pasan argumentos, buscamos el primer trabajo STOPPED */
    if (arg == NULL) 
	{
        // Iteramos hasta encontrar primer trabajo stopped
        for (int i = 1; i <= n; i++) 
		{
            job *j = get_item_bypos(listaBg, i);

			/* Si el trabajo está en estado STOPPED y no da fallos */
            if (j && j->state == STOPPED) 
			{
                pos = i;							// Nos quedamos con este trabajo
            }
        }
		/* Si "pos" no se ha actualizado, no hay trabajos STOPPED */
        if (pos == 0)
		{
			fprintf(stderr, "No hay trabajos en STOPPED\n");
			return (1);
		}
    }
	/* Si se pasan argumentos, nos quedamos con el trabajo en la posición arg */ 
	else 
	{
        pos = atoi(arg);						// Conviert string a int

		/* Si la posición no es válida */
        if (pos <= 0 || pos > n) 
		{
			fprintf(stderr, "Posición pasada por argumento no válida\n");
			return (1);
		}
    }

	/* Obtenemos el trabajo y su pid para poder ejecutarlo en segundo plano */

    job *trabajo = get_item_bypos(listaBg, pos);

	// Si hubo fallo al obtener el trabajo o el trabajo no estaba suspendido
    if (trabajo == NULL || trabajo->state != STOPPED)
	{
		if(trabajo == NULL)
		{
			fprintf(stderr, "Fallo al obtener el proceso requerido\n");
			return (1);
		}
		else
		{
			fprintf(stderr, "Error, trabajo seleccionado no se encuentra en STOPPED\n");
			return (1);
		}
	}

	// Reanudamos trabajo obteniendo su pid
    pid_t pid = trabajo->pgid;
    kill(-pid, SIGCONT); 

    trabajo->state = BACKGROUND;		// Cambiamos estado del trabajo

    printf("Background process %s (%d) continued\n", trabajo->command, pid);

    return 0;
}

/* currjob --------------------------------------------------------------------------------------------------------- */

// Imprime el último trabajo de listabg

int currjob_interno (void)
{
	// Primero nos aseguramos de que bg no esté vacía
	if (listaBg == NULL)
	{
		fprintf(stderr, "Error, la lista de trabajos en bg o stopped está vacía\n");
		return 1;
	}
	/* Si la lista no está vacía, puede que igualmente solo esté el dummy node, entonces debemos
	   seguir comprobando
	*/
	else
	{
		// Ahora obtenemos el primer trabajo (el último en añadirse)
		job *trabajo = get_item_bypos(listaBg, 1);

		/* Si no había */
		if(trabajo == NULL)		
		{
			fprintf(stderr, "Error, la lista de trabajos en bg o stopped está vacía\n");
			return 1;
		}
		/* Si sí había */
		else
		{
			printf("Trabajo actual: PID=%d command=%s\n", trabajo->pgid, trabajo->command);
		}
	}

	return 0;
}

/* deljob ----------------------------------------------------------------------------------------------------------- */

// Borra el últomp trabajo SI NO ESTÁ SUSPENDIDO de la lista listaBg
// Aunque lo quitemos de la lista, este seguirá su ejecución normalmente

int deljob_interno (void)
{
	/* Si la lista está vacía */
	if (listaBg == NULL)
	{
		fprintf (stderr, "No hay trabajo actual\n");
		return (1);
	}
	/* Si la lista no está vacía */
	// Comprobamos que haya al menos un trabajo ahí
	else
	{
		job *trabajo = get_item_bypos(listaBg, 1);

		/* Si no hay trabajos en la lista */
		if(trabajo == NULL)
		{
			fprintf (stderr, "No hay trabajo actual \n");
			return (1);
		}
		/* Si hemos podido acceder correctamente al último trabajo */
		else
		{
			/* Si el trabajo está suspendido */
			if (trabajo->state == STOPPED)
			{
				fprintf(stderr, "No se permiten borrar trabajos en segundo plano suspendidos\n");
				return (1);
			}
			/* Si el trabajo está en BG, lo eliminamos */
			else
			{
				printf ("Borrando trabajo actual de la lista de jobs: PID=%d command=%s\n", trabajo->pgid, trabajo->command);

				/* Comprobamos algún posible fallo por si acaso */
				if (delete_job(listaBg, trabajo) == 0)
				{
					fprintf (stderr, "Error al eliminar trabajo de la lista\n");
					return (1);
				}
			}
		}
	}
	return (0);
}

/* zjobs ------------------------------------------------------------------------------------------------------------ */

// Muestra pid de los procesos zombie que se han quedado

void zjobs_interno (void)
{
	/* 
		Recorremos /proc y buscamos los pid cuyo padre sea nuestro shell y estén en 
	   estado zombie. Tendremos que acceder a /proc/<pid>/stat para cada proceso cuyo
	   id es <pid>
	*/

	DIR *d;						// Directorio al que accederemos
	struct dirent *dir;			// Representa la entrada en un directorio	
	char buff[2048];
	d = opendir("/proc");		// En d guardamos el directorio /proc

	if(d)
	{
		// Mientras al leer d, eso siga siendo un directorio
		while ((dir = readdir(d)) != NULL)
		{
			// En buff guardamos la cadena de texto formateada (segundo parámetro)
			sprintf (buff, "/proc/%s/stat", dir->d_name);	// dir->d_name es lo que se pone en %s

			// Abrimos esa ruta como fichero en modo lectura
			FILE *fd = fopen(buff, "r");

			// Si no da fallo
			if (fd)
			{
				long pid;		// Cojo el pid
				long ppid;		// Cojo el ppid
				char state;		// Estado: R (runnable), S (sleeping), T (stopped), Z (Zombie)

				// Aquí leeremos pid, state y ppid de /proc/<pid>/stat
				fscanf (fd, "%ld %s %c %ld", &pid, buff, &state, &ppid);

				// Convertimos los datos
				//pid_t pid_hijo = (pid_t) pid;
				//pid_t pid_padre = (pid_t) ppid;

				/* Comprobamos si su estado es zombie y si su padre es la shell */
				if (ppid == getpid() && state == 'Z')
				{
					printf("%ld\n", pid);
				}

				fclose (fd);	// Cerramos
			}
		}
		closedir(d);
	}
}


/* bgteam ----------------------------------------------------------------------------------------------------------- */

/**
 * Primer parámetro: 	Número de veces a lanzar comando
 * Segundo parámetro: 	comando a lanzar en segundo plano
 * 
 * Si no pasas nada: 	Error
 */
void bgteam_interno (int nVeces, char **comando)
{
	pid_t pid;								// Iremos creando hijos a partir del padre

	for (int i = 0; i < nVeces; i++)
	{
		// Creamos un hijo
		pid = fork();

		/* Proceso hijo */
		if (pid == 0)
		{
			if(listaBg != NULL)
			{
				/* Como en el segundo parámetro metemos args[1], comando[0] = args[1] */
				execv(comando[0], comando);	
				/* Si hubo fallo */				
				fprintf(stderr, "Error, command not found: %s\n", comando[0]);		// Si el comando no es válido, ERROR								// Eliminamos trabajo
				exit (255);															// Código estándar para command not found
			}												
		}
		/* Error */
		else if (pid < 0)
		{
				fprintf(stderr, "Error en fork\n");
		}
		/* Padre */
		else
		{
			// Nos aseguramos de que no hay fallos en la lista
			if (listaBg != NULL)
			{
				/* Metemos este proceso en la lista de trabajos */
				job *trabajo = new_job(pid, comando[0], BACKGROUND);
				add_job(listaBg, trabajo);
			}
		}
													
	}
}

// ------------------------------------------------------------------------------------------------------------------
//                            MAIN          
// ------------------------------------------------------------------------------------------------------------------

int main(void)
{
	/* VARIABLES NECESARIAS ----------------------------------------------------------------------------------------- */

	char inputBuffer[MAX_LINE]; 	// buffer para almacenar el comando introducido 
	int background;             	// vale 1 si un comando es seguido por '&' 		
	char *args[MAX_LINE/2];     	// línea de comandos como máximo 128 argumentos, guarda los argumentos que se pasan

	/* VARIABLES PROBABLEMENTE ÚTILES ------------------------------------------------------------------------------- */

	int pid_fork, pid_wait; 		// pid para el proceso creado y el proceso en espera 
	int status;             		// status devuelto por wait 
	char *file_in, *file_out; 		// nombres de archivos para la redirección 

	/* Tarea 2 ---------- IGNORAMOS SEÑALES DE LA TERMINAL ----------------------------------------------------------- */
	/* Así, si usamos ^C o cualquier así, la shell no se cierra */

	ignore_terminal_signals ();

	/* Tarea 3 ---------- INICIALIZAMOS LISTA DE TRABAJOS EN BG/STOPPED ---------------------------------------------------- */

	listaBg = new_list("bgYstopped");		// dummy node, guarda tanto procesos en bg como stopped

	/* Tarea 3 ---------- ASIGNAMOS MANEJADORES PROPIOS -------------------------------------------------------------- */
	
	signal(SIGCHLD, manejadorSIGCHLD);
	signal(SIGHUP, manejadorSIGHUP);

	/* PROGRAMA ------------------------------------------------------------------------------------------------------ */
	/* Termina normalmente dentro de get_command() después de que ^D se teclee */

	while (1)
	{  
		// Imprimimos siempre 

		printf("COMMAND->");
		fflush(stdout);	

		// --- Leemos comando ---	

		get_command(inputBuffer, MAX_LINE, args, &background);  // Lee el comando introducido
		
		if(args[0]==NULL) continue;   // si es un comando vacío, volvemos a preguntar (inicio del while con CONTINUE)

		/* TAREA 5 - Ver si hay redirecciones ---------------------------------------------------------------------------*/

		int redireccion_normal = parse_redirections(args, &file_in, &file_out);	// Función en job_control explicada

		/* TAREA 2 - Diferenciar entre comandos internos y los que no ...................................................*/
		// COMANDOS INTERNOS ----------------------------------------------------------------------------------------------

		/* ------ Comando cd ------ */
		if (strcmp(args[0], "cd") == 0)		
		{
			if (cd_interno(args) != 0)
			{
				fprintf (stderr, "Error al ejecutar cd\n");
			}
			continue;				// Volvemos al inicio del while, no pasamos por el resto				
		}
		
		/* ----- Comando jobs ----- */
		if (strcmp(args[0], "jobs") == 0)		
		{
			if (jobs_interno() != 0)
			{
				fprintf (stderr, "Error al ejecutar jobs\n");
			}
			continue;				// Volvemos al inicio del while, no pasamos por el resto				
		}

		/* ----- Comando fg ----- */
		if (strcmp(args[0], "fg") == 0)		
		{
			if (fg_interno(args[1]) != 0)
			{
				fprintf(stderr, "Error al ejecutar fg\n");
			}
			continue;				// Volvemos al inicio del while, no pasamos por el resto				
		}

		/* ----- Comando bg ----- */
		if (strcmp(args[0], "bg") == 0)		
		{
			if (bg_interno(args[1]) != 0)
			{
				fprintf(stderr, "Error al ejecutar bg\n");
			}
			continue;				// Volvemos al inicio del while, no pasamos por el resto				
		}

		/* ---- Comando currjob ---- */
		if (strcmp(args[0], "currjob") == 0)
		{
			if (currjob_interno() != 0)
			{
				fprintf(stderr, "Error al ejecutar currjob\n");
			}
			continue;
		}

		/* ---- Comando deljob ---- */
		if (strcmp(args[0], "deljob") == 0)
		{
			if (deljob_interno() != 0)
			{
				fprintf (stderr, "Error al ejecutar deljob\n");
			}
			continue;
		}

		/* ---- Comando zjobs ---- */
		if (strcmp(args[0], "zjobs") == 0)
		{
			zjobs_interno();	// No hay gestión de errores, así que no hay que revisar nada
			continue;
		}

		/* ---- Comando bgteam ---- */
		if (strcmp(args[0], "bgteam") == 0)
		{
			// Primero paso los parámetros correctamente
			if (args[1] != NULL && args[2] != NULL)
			{
				int n = atoi(args[1]);		// Pasamos a int
				if (n>=1)					// Evitamos posibles fallos
				{
					// Debemos pasarle la dirección de memoria para poder apuntar a este
					bgteam_interno(n, &args[2]);
				}
				else
				{
					fprintf(stderr, "Argumento inválido para n veces\n");
				}
			}
			/* Si no hay suficientes argumentos */
			else
			{
				fprintf(stderr, "El comando bgteam requiere dos argumentos\n");
			}
			continue;
		}

		
		// COMANDOS EXTERNOS ----------------------------------------------------------------------------------------------
		/* TAREA 1 - ejecutar comandos y analizarlos .....................................................................*/

		pid_fork = fork();		// Creamos proceso hijo que ejecutará el comando

		/* --- Proceso hijo ----------------- */
		if (pid_fork == 0)
		{
			/* Tarea 2 - creamos nuevo grupo de procesos para hijo */

			setpgid (0, 0);			// Hijo crea un nuevo grupo y se convierte en líder
			
			/* Tarea 2 - dar control del shell al hijo si se ejecutó en fg */
			
			if (!background)	// Si se ejecutó en fg, damos control
			{
				tcsetpgrp(STDIN_FILENO, getpid());	// Asignamos control de la terminal al hijo
			}

			/* TAREA 2, activamos las señales por defecto para poder enviar señales a la ejecución del hijo */
			// Esto no le influirá a nuestra shell (padre), ya que el control de la terminal la tiene sólo el hijo

			restore_terminal_signals();

			// Aplicamos las redirecciones -----------------------------------------------------------------------------------
			/* --- Si es < ---  ENTRADA */
			if (file_in != NULL)
			{
				FILE *fd_in = fopen(file_in, "r");	// Abrimos archivo en modo lectura
				/* Si da fallo */
				if (fd_in == NULL)
				{
					perror("Error al abrir archivo de entrada");
					exit(1);
				}
				/* Redirigimos la entrada estándar al archivo - Lo que debería entrar por teclado, entra por el archivo */
				if (dup2(fileno(fd_in), STDIN_FILENO) < 0)
				{
					// Manejamos posibles fallos
					perror ("Error al redirigir entrada estándar");
					exit (1);
				}

				fclose(fd_in);		// Cerramos 
			}
			/* --- Si es > --- SALIDA */
			if (file_out != NULL)
			{
				/* Adicional, vemos si es con anexado >> */
				if (redireccion_normal == 1)
				{
					FILE *fd_out = fopen (file_out, "a");	// Abrimos el archivo en modo append
					/* Si da fallo */
					if(fd_out == NULL)
					{
						perror("Error al abrir archivo de salida");
						exit(1);
					}
					/* Redirigimos la salida estándar al archivo - Lo que debería salir por terminal, se lo pasamos al archivo */
					if (dup2(fileno(fd_out), STDOUT_FILENO) < 0)
					{
						perror ("Error al redirigir la salida estándar");
						exit(1);
					}

					fclose(fd_out);		// Cerramos
				}
				/* Si es normal > */
				else
				{
					FILE *fd_out = fopen (file_out, "w");	// Abrimos el archivo en modo escritura
					/* Si da fallo */
					if(fd_out == NULL)
					{
						perror("Error al abrir archivo de salida");
						exit(1);
					}
					/* Redirigimos la salida estándar al archivo - Lo que debería salir por terminal, se lo pasamos al archivo */
					if (dup2(fileno(fd_out), STDOUT_FILENO) < 0)
					{
						perror ("Error al redirigir la salida estándar");
						exit(1);
					}

					fclose(fd_out);		// Cerramos
				}

			}
			// Terminan redirecciones ----------------------------------------------------------------------

			// Independientemente de si fue en bg o fg, ejecutamos comando

			execvp (args[0], args);	// Ejecutamos el comando junto sus argumentos 
									// (args[1] apunta al primer argumento extra, por ende podemos ir accediendo al resto)
			
			fprintf(stderr, "Error, command not found: %s\n", args[0]);		// Si el comando no es válido, ERROR
			exit (255);				// Código estándar para command not found
		}
		/* --- Proceso padre -------------------------------------------------------- */
		if (pid_fork > 0)
		{
			/* Si hijo se ejecutó en bg */
			if (background)
			{
				/* Añadimos a este proceso a la lista bg - tarea 3 */
				job *hijobg = new_job(pid_fork, args[0], BACKGROUND);
				add_job(listaBg, hijobg);

				// Mostramos que su hijo va a ejecutar el comando en bg
				printf ("Background job running, pid: %d, command: %s\n", pid_fork, args[0]);
			}
			/* Si hijo se ejecutó en fg */
			else
			{
				waitpid(pid_fork, &status, WUNTRACED);	// Esperamos a que termine su ejecución
				
				/* WUNTRACED (tarea 2)
					Retorna información tanto de los procesos hijos que terminaron, como de 
					los que fueron pausados, permitiéndolo poder mover a segundo plano
				*/

				// Retomamos control del shell - Tarea 2
				tcsetpgrp (STDIN_FILENO, getpid());

				// ----------- INFORMACIÓN DE CÓMO TERMINARON LOS HIJOS - mod Tarea 2 --------------------
				/* Si fue detenido - stopped */
				if (WIFSTOPPED(status))
				{
					/* Tarea 3 - añadimos hijo a la lista de bg */
					job *hijoStopped = new_job(pid_fork, args[0], STOPPED);
					add_job(listaBg, hijoStopped);

					printf ("Foreground pid: %d, command: %s, Suspended, info: %d\n", pid_fork, args[0], WSTOPSIG(status));
				}
				/* Si terminó por una señal */
				else if (WIFSIGNALED(status))
				{
					printf ("Foreground pid: %d, command: %s, Signaled, info: %d\n", pid_fork, args[0], WTERMSIG(status));
				}
				/* Si terminó de forma normal */
				else if (WIFEXITED(status))
				{
					printf ("Foreground pid: %d, command: %s, Exited, info: %d\n", pid_fork, args[0], WEXITSTATUS(status));
				}
			}
		}
		/* --- Si dió fallo --- */
		else if (pid_fork == -1)
		{
			fprintf (stderr, "Error, no pudo crearse correctamente un hijo\n");
		}
	} // end while
}