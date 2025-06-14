package es.uma.rys.app;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.Scanner;

import es.uma.rys.entities.Person;
import es.uma.rys.entities.SpaceShip;
import es.uma.rys.entities.World;

public class Main {
	/*************************************************************************************************
	 ATRIBUTOS / VARIABLES
	 *************************************************************************************************/

	private static Random rand; // para números aleatorios
	private static Scanner sc; // para leer de teclado
	private final static String proxy = "proxy.lcc.uma.es";
	private final static String proxy_port = "3128";

	private static String[] acierto = {"Ese es el camino",
			"Eres uno con la Fuerza. La Fuerza esté contigo",
			"Que la fuerza te acompañe",
			"Nada ocurre por accidente",
			"Sin duda, maravillosa tu mente es",
			"Cuando te fuiste, no era más que el aprendiz. Ahora eres el maestro",
			"La Fuerza te está llamando, déjala entrar",
			"Tu cantidad de midiclorianos debe ser muy alta",
			"Una lección aprendida es una lección ganada",
			"Debes creer en ti mismo o nadie lo hará",
			"El camino a la sabiduria es sencillo para aquellos que no se dejan cegar por el ego"};
	private static String[] fracaso = {"El mejor profesor, el fracaso es.",
			"El miedo es el camino hacia el Lado Oscuro. El miedo lleva a la ira, la ira lleva al odio, el odio lleva al sufrimiento. Percibo mucho miedo en ti",
			"Tu carencia de fe resulta molesta",
			"La capacidad de hablar no te hace inteligente",
			"Concéntrate en el momento. Siente, no pienses, usa tu instinto",
			"No lo intentes. Hazlo, o no lo hagas, pero no lo intentes",
			"Paciencia, utiliza la Fuerza. Piensa",
			"Siento una perturbación en la fuerza",
			"El lado oscurso se intensifica en ti",
			"El primer paso para corregir un error es la paciencia",
			"El exceso de confianza es el mas peligroso de los descuidos",
			"El camino de la ignorancia es guiado por el miedo"};

	// Colores ANSI
	private static final String ANSI_RESET = "\u001B[0m";
	private static final String ANSI_RED = "\u001B[31m";
	private static final String ANSI_GREEN = "\u001B[32m";
	private static final String ANSI_YELLOW = "\u001B[33m";
	private static final String ANSI_BLUE = "\u001B[34m";
	private static final String ANSI_PURPLE = "\u001B[35m";
	private static final String ANSI_CYAN = "\u001B[36m";

	/*************************************************************************************************
	 * MAIN
	 *************************************************************************************************/
	public static void main(String[] args) {

		// Descomente las siguiente líneas si lo está probando en el laboratorio y accede a Internet usando el proxy
		// System.setProperty("https.proxyHost",proxy);
		// System.setProperty("https.proxyPort",proxy_port);

		SWClient sw = new SWClient();
		String respuesta = "s";
		rand = new Random();
		sc = new Scanner(System.in);

		// Preguntas que se pueden hacer (comentamos las que no se quieren probar)
		do {
			if (respuesta.equals("s"))
			{
				tallest(sw);
				whoBornIn1(sw);
				whoBornIn2(sw);
				mostExpensive(sw);
				whoPilots(sw);
			}

			System.out.println("Desea otra ronda (s/n)?");
			respuesta = sc.nextLine();

			if (respuesta.equals("n"))
			{
				System.out.println(ANSI_CYAN + "Hasta pronto!" + ANSI_RESET);
			}
			else if (!respuesta.equals("s"))
			{
				System.out.println(ANSI_RED + "Respuesta no válida, introduzca una correcta: " + ANSI_RESET);
			}
		}
		while (respuesta.equals("s") || !respuesta.equals("n"));
		sc.close();

	}

	/*************************************************************************************************
	 * MÉTODOS AUXILIARES
	 *************************************************************************************************/

	// Sirve para obtener recursos aleatorios sin repetir de algo en específico (personajes, planetas, etc.)
	public static Integer getRandomResource(int max, List<Integer> l) {
		// En l se almacenan los recursos que ya se han usado
		// En max se indica el número máximo de recursos que se pueden usar (los que hemos podido obtener)

		if (max == l.size()) return null;    // Si ya se han usado todos, no hay más que buscar

		Integer p = rand.nextInt(max);        // Generamos un número aleatorio entre 0 y max-1

		while (l.contains(p))                // Si ya se ha usado, buscamos otro
		{
			p = (p + 1) % max;
		}
		return p;
	}

	/*************************************************************************************************
	 * PREGUNTAS
	 * ***********************************************************************************************/

	/*-------- TALLEST -------------------------------------------------------------------------------*/
	// Pregunta que obtiene dos recursos iguales (personajes en este caso) y los compara
	public static void tallest(SWClient sw)
	{
		// Obtenemos la cantidad de personas que podemos usar como máximo
		int max_people = sw.getNumberOfResources("people");

		// Si no hay personas, no podemos hacer la pregunta
		if (max_people == 0) {
			System.out.println("No se encontraron personas.");
			return;
		}

		System.out.println("Generando nueva pregunta...");

		// Almacenamos las personas usadas y las que vamos a usar
		List<Integer> used = new ArrayList<Integer>();
		List<Person> people = new ArrayList<Person>();

		int contador = 0;

		// Buscamos dos personas que tengan altura definida
		while (contador < 2) {
			Integer p = getRandomResource(max_people, used);                            // Obtenemos un ID aleatorio
			Person person = sw.getPerson(sw.generateEndpoint("people", p));    // Obtenemos la persona con ese ID

			// Si no se ha podido obtener, mostramos un mensaje de error
			if (person == null) {
				System.out.println("Hubo un error al encontrar el recurso " + p);
			} else {
				try {
					Double test = Double.parseDouble(person.height);    // Comprobamos que la altura es un número válido
					people.add(person);                                    // La añadimos las personas a usar
					contador++;                                            // Aumentamos el contador de personas
				} catch (
						Exception e)                                        // Si hubo fallo al hacer parser, no la usamos
				{
					continue;
				}
			}
			used.add(p);
		}    // Salimos del while


		// Escribiendo la pregunta y leyendo la opción
		Integer n = null;
		do {
			System.out.println(ANSI_YELLOW + "¿Quién es más alto?\n" + ANSI_RESET  +
					"[0] " + people.get(0).name +
					"\n[1] " + people.get(1).name);

			System.out.print(ANSI_PURPLE + "Respuesta: " + ANSI_RESET);
			// Intentamos leer la respuesta
			try {
				n = Integer.parseInt(sc.nextLine());
			}
			// Si hubo fallo, asignamos un valor inválido
			catch (NumberFormatException ex) {
				n = -1;
			}
		}
		while (n != 0 && n != 1);    // Preguntamos hasta tener una respuesta válida (0 o 1)

		System.out.println();

		// Mostrando la información de los personajes elegidos
		for (Person p : people) {
			System.out.println("* " + p.name + " mide " + p.height);
		}

		// Si el que se eligió es más alto que el otro, se muestra un mensaje de acierto
		if (Double.parseDouble(people.get(n).height) >= Double.parseDouble(people.get((n + 1) % 2).height)) {
			System.out.println(ANSI_GREEN + "Enhorabuena!!! " + acierto[rand.nextInt(acierto.length)] + ANSI_RESET);
		}
		// Si no, se muestra un mensaje de fracaso
		else {
			System.out.println(ANSI_RED + "Fallaste :( " + fracaso[rand.nextInt(fracaso.length)] + ANSI_RESET);
		}
	}

	/*-------- WHO BORN IN 1 --------------------------------------------------------------------------*/
	// Pregunta que relaciona varios recursos:
	// - Elige un recurso (planeta en este caso)
	// - Pregunta sobre algún recurso relacionado (persona que nació o fue crear ahí)
	// - Busca ese recurso y comprueba si está relacionado con el primero (si la persona buscada tiene como planeta el original)

	public static void whoBornIn1(SWClient sw)
	{
		// Obteniendo la cantidad de planetas
		int max_planet = sw.getNumberOfResources("planets");
		if (max_planet == 0) {
			System.out.println("No se encontraron planetas.");
			return;
		}

		System.out.println("Generando nueva pregunta...");

		// Obteniendo el planeta (que tenga personas)
		List<Integer> used = new ArrayList<Integer>();
		World planet = null;
		do {
			Integer p = getRandomResource(max_planet, used);
			planet = sw.getWorld(sw.generateEndpoint("planets", p));
			if (planet == null) {
				System.out.println("Hubo un error al encontrar el recurso " + p);
			}
			used.add(p);
		}

		while
		(
				planet == null ||
						planet.residents == null ||
						planet.residents.length < 1 ||
						planet.name.equals("unknown")
		);

		// Planteamos la pregunta
		String s = null;
		System.out.println(ANSI_YELLOW + "¿Quién nació o fue creado en " + planet.name + "?" + ANSI_RESET);
		System.out.print(ANSI_PURPLE + "Respuesta: " + ANSI_RESET);
		s = sc.nextLine();

		// Buscamos la persona indicada
		Person p = sw.searchPersonByName(s);

		// Validamos la respuesta y mostramos sus datos
		if (p == null) {
			System.out.println("No hay nadie con ese nombre");
		} else {
			System.out.println();
			System.out.println("* " + p.name + " nació en " + p.homeplanet.name);
		}

		// Resultados
		if (p != null && p.homeplanet.name.equals(planet.name)) {
			System.out.println(ANSI_GREEN + "Enhorabuena!!! " + acierto[rand.nextInt(acierto.length)] + ANSI_RESET);
		} else {
			System.out.println(ANSI_RED + "Fallaste :( " + fracaso[rand.nextInt(fracaso.length)] + ANSI_RESET);
		}
	}

	/*-------- WHO BORN IN 2 --------------------------------------------------------------------------*/
	// Similar a la previa pero en vez de solicitar que escriba se le ofrecen alternativa para ello:
	// - Se elige una al azar de las disponibles en el recurso, persona del planeta (la correcta)
	// - Se buscar aleatoriamente otras 3 que no estén relacionados con el recurso (las incorrectas)
	// - Se inserta la correcta de forma aleatoria

	public static void whoBornIn2(SWClient sw) {

		// Obteniendo la cantidad de planetas y personas
		int max_people = sw.getNumberOfResources("people");
		int max_planet = sw.getNumberOfResources("planets");

		if (max_people == 0 || max_planet == 0) {
			System.out.println("No se encontraron personas o planetas.");
			return;
		}

		System.out.println("Generando nueva pregunta...");

		List<Integer> used = new ArrayList<Integer>();

		// Obteniendo el planeta (que tenga personas)
		World planet = null;
		do {
			Integer p = getRandomResource(max_planet, used);
			planet = sw.getWorld(sw.generateEndpoint("planets", p));
			if
			(planet == null) {
				System.out.println("Hubo un error al encontrar el recurso " + p);
			}

			used.add(p);
		}
		while (
				planet == null ||
						planet.residents == null ||
						planet.residents.length < 1 ||
						planet.name.equals("unknown")
		);

		used.clear();

		// Cogemos uno al azar como acierto
		String[] residents = planet.residents;
		Person correcta = sw.getPerson(residents[rand.nextInt(residents.length)]);

		// Metemos todas las personas del planeta como usados para que no se seleccionen
		for (String s : residents) {
			used.add(sw.getIDFromURL(s));
		}

		// Buscamos 3 personas más
		List<Person> people = new ArrayList<Person>();
		int contador = 0;
		while (contador < 3) {
			Integer p = getRandomResource(max_people, used);
			Person person = sw.getPerson(sw.generateEndpoint("people", p));

			if (person == null) {
				System.out.println("Hubo un error al encontrar el recurso " + p);
			} else {
				people.add(person);
				contador++;
			}
			used.add(p);
		}
		// Metemos el correcto de forma aleatoria
		int pos_acierto = rand.nextInt(4);
		people.add(pos_acierto, correcta);

		// Leemos la opción
		Integer n = null;
		do {
			System.out.println(ANSI_YELLOW + "¿Quién nació o fue fabricado en " + planet.name + "?" + ANSI_RESET);

			for (int i = 0; i < 4; i++) {
				System.out.println(" [" + i + "] " + people.get(i).name);
			}
			System.out.print(ANSI_PURPLE + "Respuesta: " + ANSI_RESET);

			try
			{
				n = Integer.parseInt(sc.nextLine());    // Intentamos leer la respuesta
			}
			catch (NumberFormatException ex)
			{
				n = -1;
			}
		}
		while (n < 0 || n > 3);    // Mientras no sea un número entre 0 y 3, seguimos preguntando

		System.out.println();

		// Se muestran los resultados
		for (Person p : people) {
			System.out.println("* " + p.name + " nació en " + p.homeplanet.name);
		}

		// Resultados
		if (n == pos_acierto) {
			System.out.println(ANSI_GREEN + "Enhorabuena!!! " + acierto[rand.nextInt(acierto.length)] + ANSI_RESET);
		} else {
			System.out.println(ANSI_RED + "Fallaste :( " + fracaso[rand.nextInt(fracaso.length)] + ANSI_RESET);
		}
	}

	/*-------- MOST EXPENSIVE --------------------------------------------------------------------------*/
	// Pregunta que obtiene dos naves y ve cuál es más cara (Similar a tallest pero con naves)

	public static void mostExpensive(SWClient sw) {

		// Obtenemos la cantidad de naves que podemos usar como máximo
		int max_ships = sw.getNumberOfResources("starships");

		// Si no hay naves, no podemos hacer la pregunta
		if (max_ships == 0) {
			System.out.println("No se encontraron naves.");
			return;
		}

		System.out.println("Generando nueva pregunta...");

		// Almacenamos las naves usadas y las que vamos a usar
		List<Integer> used = new ArrayList<Integer>();
		List<SpaceShip> ships = new ArrayList<SpaceShip>();

		int contador = 0;

		// Buscamos dos naves que tengan coste definido
		while (contador < 2) {
			Integer p = getRandomResource(max_ships, used);                                    // Obtenemos un ID aleatorio
			SpaceShip ship = sw.getStarShip(sw.generateEndpoint("starships", p));    // Obtenemos la nave con ese ID
			used.add(p);                                                                    // Añadimos el ID de la nave a los usados

			// Si hemos podido obtener la nave, comprobamos que tenga coste definido
			if (ship != null) {
				try {
					Double test = Double.parseDouble(ship.cost_in_credits);    // Comprobamos que el precio es un número válido
					ships.add(ship);                                        // La añadimos las naves a usar
					contador++;                                                // Aumentamos el contador de personas
				} catch (
						Exception e)                                            // Si hubo fallo al hacer parser, no la usamos
				{
					continue;
				}
			}
		}    // Salimos del while

		/*
		En este caso, no he añadido un mensaje de error en caso de que no se haya podido
		obtener ninguna nave, ya que faltan naves en la API y es muy probable que no
		se pueda obtener una nave válida.
		 */

		// Escribiendo la pregunta y leyendo la opción
		Integer n = null;
		do {
			System.out.println(ANSI_YELLOW + "¿Qué nave es más cara?\n" + ANSI_RESET  +
					"[0] " + ships.get(0).name +
					"\n[1] " + ships.get(1).name);

			System.out.print(ANSI_PURPLE + "Respuesta: " + ANSI_RESET);
			// Intentamos leer la respuesta
			try {
				n = Integer.parseInt(sc.nextLine());
			}
			// Si hubo fallo, asignamos un valor inválido
			catch (NumberFormatException ex) {
				n = -1;
			}
		}
		while (n != 0 && n != 1);    // Preguntamos hasta tener una respuesta válida (0 o 1)

		System.out.println();

		// Mostrando la información de los personajes elegidos
		for (SpaceShip p : ships) {
			System.out.println("* " + p.name + " cuesta " + p.cost_in_credits + " créditos");
		}

		// Si el que se eligió es más alto que el otro, se muestra un mensaje de acierto
		if (Double.parseDouble(ships.get(n).cost_in_credits) >= Double.parseDouble(ships.get((n + 1) % 2).cost_in_credits)) {
			System.out.println(ANSI_GREEN + "Enhorabuena!!! " + acierto[rand.nextInt(acierto.length)] + ANSI_RESET);
		}
		// Si no, se muestra un mensaje de fracaso
		else {
			System.out.println(ANSI_RED + "Fallaste :( " + fracaso[rand.nextInt(fracaso.length)] + ANSI_RESET);
		}
	}

	/*--------- WHO PILOTS --------------------------------------------------------------------------*/
	// Pregunta que obtiene una nave y pregunta quién la pilota

	public static void whoPilots(SWClient sw) {
		// Obtenemos la cantidad de naves y personas que podemos usar como máximo
		int max_ships = sw.getNumberOfResources("starships");
		int max_people = sw.getNumberOfResources("people");

		if (max_people == 0 || max_ships == 0) {
			System.out.println("No se encontraron naves o personas.");
			return;
		}

		System.out.println("Generando nueva pregunta...");

		List<Integer> used = new ArrayList<Integer>();

		// Obteniendo una nave al azar (que tenga pilotos)
		SpaceShip ship = null;
		do {
			Integer p = getRandomResource(max_ships, used);
			used.add(p);
			ship = sw.getStarShip(sw.generateEndpoint("starships", p));
		}
		while
		(
				ship == null ||
						ship.pilots == null ||
						ship.pilots.length < 1 ||
						ship.name.equals("unknown")
		);

		used.clear();

		// Cogemos un piloto al azar como acierto
		String[] pilots = ship.pilots;
		Person correcta = sw.getPerson(pilots[rand.nextInt(pilots.length)]);

		// Metemos todas las personas de la nave como usados para que no se seleccionen
		for (String s : pilots) {
			used.add(sw.getIDFromURL(s));
		}

		// Buscamos 3 personas más
		List<Person> people = new ArrayList<Person>();
		int contador = 0;
		while (contador < 3) {
			Integer p = getRandomResource(max_people, used);
			Person person = sw.getPerson(sw.generateEndpoint("people", p));

			if (person == null) {
				System.out.println("Hubo un error al encontrar el recurso " + p);
			} else {
				people.add(person);
				contador++;
			}
			used.add(p);
		}

		// Metemos el correcto de forma aleatoria
		int pos_acierto = rand.nextInt(4);
		people.add(pos_acierto, correcta);

		// Leemos la opción
		Integer n = null;
		do {
			System.out.println(ANSI_YELLOW + "¿Quién pilota la nave " + ship.name + "?" + ANSI_RESET);

			for (int i = 0; i < 4; i++) {
				System.out.print(" [" + i + "] " + people.get(i).name);
				System.out.println();
			}
			System.out.print(ANSI_PURPLE + "Respuesta: " + ANSI_RESET);

			try
			{
				n = Integer.parseInt(sc.nextLine());    // Intentamos leer la respuesta
			}
			catch (NumberFormatException ex)
			{
				n = -1;
			}
		}
		while (n < 0 || n > 3);    // Mientras no sea un número entre 0 y 3, seguimos preguntando

		System.out.println(); // Añadimos una línea en blanco para mejorar la legibilidad

		// Se muestran los resultados
		for (Person p : people) {
			// Si la persona pilota una nave, se muestra un mensaje
			if (p.spaceships != null) {
				System.out.print("* " + p.name + " pilota la(s) nave(s): ");

				// Se muestran las naves que pilota
				for (SpaceShip s : p.spaceships)
				{
					System.out.print(s.name + " ");
				}
				System.out.println();
			}
			// Si la persona no pilota ninguna nave, se muestra un mensaje
			else {
				System.out.println("*"+ p.name + " no pilota ninguna nave.");
			}
		}

		// Resultados
		if (n == pos_acierto) {
			System.out.println(ANSI_GREEN + "Enhorabuena!!! " + acierto[rand.nextInt(acierto.length)] + ANSI_RESET);
		} else {
			System.out.println(ANSI_RED + "Fallaste :( " + fracaso[rand.nextInt(fracaso.length)] + ANSI_RESET);
		}
	}
}
