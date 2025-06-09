package es.uma.rys.app;

import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URLEncoder;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.net.http.HttpResponse.BodyHandlers;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;

import com.google.gson.Gson;

import es.uma.rys.entities.ResourceCountResult ;
import es.uma.rys.entities.Movie;
import es.uma.rys.entities.QueryResponse;
import es.uma.rys.entities.Specie;
import es.uma.rys.entities.SpaceShip;
import es.uma.rys.entities.Vehicle;
import es.uma.rys.entities.World;
import es.uma.rys.entities.Person;

public class SWClient
{
    // TODO: Completa el nombre de la aplicación
    private final String app_name = "Star Wars Test";
    private final int year = 2025;
    private final String url_api = "https://swapi.py4e.com/api/";

    // Métodos auxiliares

    // Obtiene la URL del recurso id del tipo resource
	public String generateEndpoint(String resource, Integer id)
    {
	    return url_api + resource + "/" + id + "/";
	}
	
	// Dada una URL de un recurso obtiene su ID
	public Integer getIDFromURL(String url)
    {
		String[] parts = url.split("/");

		return Integer.parseInt(parts[parts.length-1]);
	}

    /************************************************************************************
     * Métodos de consulta a la API de Star Wars
     ************************************************************************************/
	
	// Consulta un recurso y devuelve cuántos elementos tiene
    public int getNumberOfResources (String resource)
    {
        String urlname = url_api + resource + "/";
        int number = 0;

        // TODO: Maneja de forma apropiada las posibles excepciones
        try
        {
            // TODO: Crea un objeto para manejar una conexión (HttpClient) de HTTP/1.1
            URI servicio = new URI(urlname);

            // TODO: Crea una petición HTTP (HttpRequest) con las siguientes características
            // - la URL está en urlname
            // - Añada las cabeceras indicadas en el enunciado
            // - Use el métod o GET

            /* Primero abrimos una conexión HTTP asociada a la URL */
            HttpClient cliente = HttpClient.newBuilder()
                    .version(HttpClient.Version.HTTP_1_1)
                    .build();

            /* Creamos la petición HTTP */
            HttpRequest request = HttpRequest.newBuilder()
                    .uri(servicio)
                    .header("User-Agent", app_name + "-" + year)    // Cabecera User-Agent
                    .header("Accept", "application/json")           // Cabecera Accept
                    .GET()                                                       // Tipo de petición
                    .build();

            // TODO: envíe la petición para obtener una respuesta (HttpResponse)
            HttpResponse<String> response = cliente.send(request, BodyHandlers.ofString());

            // TODO Comprueba que el código de la respuesta es correcto (2xx)
            if(response.statusCode() < 200 || response.statusCode() >= 300)
            {
                return 0;       // Si no es correcto, devolvemos 0
            }

            // Deserialice la respuesta
            Gson parser = new Gson();
            // TODO: Reemplace "" por el cuerpo de la respuesta
            ResourceCountResult c = parser.fromJson(response.body(), ResourceCountResult.class);
            number = c.count;
            
        }
        catch (Exception e)
        {
            System.err.println(e);
            return 0;
        }
        
        return number;      // Cantidad de recursos
    }

    // Obtenemos los datos básicos del personaje
    public Person getPerson (String urlname)
    {
        Person p = null;
        // TODO: Maneja de forma apropiada las posibles excepciones
        try
        {
            urlname = urlname.replaceAll("http:", "https:");

            // TODO: Crea un objeto para manejar una conexión (HttpClient) de HTTP/1.1
            URI servicio = new URI(urlname);

            // TODO: Crea una petición HTTP (HttpRequest) con las siguientes características
            // - la URL está en urlname
            // - Añada las cabeceras indicadas en el enunciado
            // - Use el método GET

            /* Primero abrimos una conexión HTTP asociada a la URL */
            HttpClient cliente = HttpClient.newBuilder()
                    .version(HttpClient.Version.HTTP_1_1)
                    .build();

            /* Creamos la petición HTTP */
            HttpRequest request = HttpRequest.newBuilder()
                    .uri(servicio)
                    .header("User-Agent", app_name + "-" + year)    // Cabecera User-Agent
                    .header("Accept", "application/json")           // Cabecera Accept
                    .GET()                                                       // Tipo de petición
                    .build();

            // TODO: envíe la petición para obtener una respuesta (HttpResponse)
            HttpResponse<String> response = cliente.send(request, BodyHandlers.ofString());

            // TODO Comprueba que el código de la respuesta es correcto (2xx)
            if(response.statusCode() < 200 || response.statusCode() >= 300)
            {
                return null;       // Si no es correcto, devolvemos null
            }

            // TODO: Deserialice la respuesta usando la clase Person
            Gson parser = new Gson();
            p = parser.fromJson(response.body(), Person.class);

            // TODO: Para las preguntas 2 y 3 (no lo necesita para la pregunta 1: tallest)

            // TODO: Usando la URL en el campo homeworld, obtenga los datos del planeta y almacénelos
            //  en el atributo homeplanet

            if (p.homeworld != null && !p.homeworld.isEmpty()) // Si tiene un homeworld - planeta natal
            {
                World homeworld = getWorld(p.homeworld);
                p.homeplanet = homeworld;                       // Asignamos el planeta al atributo homeplanet
            }
            else
            {
                p.homeplanet = null; // Si no tiene homeworld, asignamos null
            }

        }
        catch (Exception e)
        {
            System.err.println(e);
            return null;
        }
        return p;
    }

    // Obtenemos los datos básicos del planeta
    public World getWorld(String urlname) {
        World p = null;
        // TODO: Maneja de forma apropiada las posibles excepciones
        try {
            urlname = urlname.replaceAll("http:", "https:");

            // TODO: Crea un objeto para manejar una conexión (HttpClient) de HTTP/1.1
            URI servicio = new URI(urlname);

            // TODO: Crea una petición HTTP (HttpRequest) con las siguientes características
            // - la URL está en urlname
            // - Añada las cabeceras indicadas en el enunciado
            // - Use el método GET

            /* Primero abrimos una conexión HTTP asociada a la URL */
            HttpClient cliente = HttpClient.newBuilder()
                    .version(HttpClient.Version.HTTP_1_1)
                    .build();

            /* Creamos la petición HTTP */
            HttpRequest request = HttpRequest.newBuilder()
                    .uri(servicio)
                    .header("User-Agent", app_name + "-" + year)    // Cabecera User-Agent
                    .header("Accept", "application/json")           // Cabecera Accept
                    .GET()                                                       // Tipo de petición
                    .build();

            // TODO: envíe la petición para obtener una respuesta (HttpResponse)
            HttpResponse<String> response = cliente.send(request, BodyHandlers.ofString());

            // TODO Comprueba que el código de la respuesta es correcto (2xx)
            if(response.statusCode() < 200 || response.statusCode() >= 300)
            {
                return null;       // Si no es correcto, devolvemos null
            }

            // TODO: Deserialice la respuesta usando la clase World
            Gson parser = new Gson();
            p = parser.fromJson(response.body(), World.class);

        } catch (Exception e) {
            System.err.println(e);
            return null;
        }
        return p;
    }

    // Busca un personaje por su nombre
    public Person searchPersonByName (String name) {
        Person p = null;
        // TODO: Maneja de forma apropiada las posibles excepciones
        try {
            // TODO: Cree la URL a partir del nombre procesado tal como se indica en el enunciado

            /* Debemos codificar el nombre de forma que sea válido en una URL */
            name = URLEncoder.encode(name, StandardCharsets.UTF_8);

            // Accedemos a la API de Star Wars para buscar el personaje
            String urlname = "https://swapi.py4e.com/api/people/?search=" + name;

            urlname = urlname.replaceAll("http:", "https:");

            // TODO: Crea un objeto para manejar una conexión (HttpClient) de HTTP/1.1
            URI servicio = new URI(urlname);

            // TODO: Crea una petición HTTP (HttpRequest) con las siguientes características
            // - la URL está en urlname
            // - Añada las cabeceras indicadas en el enunciado
            // - Use el método GET

            /* Primero abrimos una conexión HTTP asociada a la URL */
            HttpClient cliente = HttpClient.newBuilder()
                    .version(HttpClient.Version.HTTP_1_1)
                    .build();

            /* Creamos la petición HTTP */
            HttpRequest request = HttpRequest.newBuilder()
                    .uri(servicio)
                    .header("User-Agent", app_name + "-" + year)    // Cabecera User-Agent
                    .header("Accept", "application/json")           // Cabecera Accept
                    .GET()                                                       // Tipo de petición
                    .build();

            // TODO: envíe la petición para obtener una respuesta (HttpResponse)
            HttpResponse<String> response = cliente.send(request, BodyHandlers.ofString());

            // TODO Comprueba que el código de la respuesta es correcto (2xx)
            if (response.statusCode() < 200 || response.statusCode() >= 300)
            {
                return null;       // Si no es correcto, devolvemos null
            }

            // TODO: Deserialice la respuesta usando la clase QueryResponse
            Gson parser = new Gson();
            QueryResponse q = parser.fromJson(response.body(), QueryResponse.class);

            // TODO: Si la respuesta tiene al menos una persona, considere como respuesta esa primera persona
            if (q.results != null && q.results.length > 0)
            {
                p = q.results[0];   // Tomamos la primera persona del resultado
            }
            else
            {
                return null;        // Si no hay resultados, devolvemos null
            }

            // TODO: Para las preguntas 2 y 3 (no lo necesita para la pregunta 1: tallest)
            // TODO: Usando la URL en el campo homeworld, obtenga los datos del planeta y almacénelos
            //  en el atributo homeplanet

            if (p.homeworld != null && !p.homeworld.isEmpty()) // Si tiene un homeworld - planeta natal
            {
                World homeworld = getWorld(p.homeworld);
                p.homeplanet = homeworld;                       // Asignamos el planeta al atributo homeplanet
            }
            else
            {
                p.homeplanet = null; // Si no tiene homeworld, asignamos null
            }
        }
        catch (Exception e)
        {
            System.err.println(e);
            return null;
        }
        return p;
    }

    // Obtenemos los datos básicos de la nave espacial
    public SpaceShip getStarShip (String urlname)
    {
        SpaceShip p = null;
        // TODO: Maneja de forma apropiada las posibles excepciones
        try
        {
            urlname = urlname.replaceAll("http:", "https:");

            // TODO: Crea un objeto para manejar una conexión (HttpClient) de HTTP/1.1
            URI servicio = new URI(urlname);

            // TODO: Crea una petición HTTP (HttpRequest) con las siguientes características
            // - la URL está en urlname
            // - Añada las cabeceras indicadas en el enunciado
            // - Use el método GET

            /* Primero abrimos una conexión HTTP asociada a la URL */
            HttpClient cliente = HttpClient.newBuilder()
                    .version(HttpClient.Version.HTTP_1_1)
                    .build();

            /* Creamos la petición HTTP */
            HttpRequest request = HttpRequest.newBuilder()
                    .uri(servicio)
                    .header("User-Agent", app_name + "-" + year)    // Cabecera User-Agent
                    .header("Accept", "application/json")           // Cabecera Accept
                    .GET()                                                       // Tipo de petición
                    .build();

            // TODO: envíe la petición para obtener una respuesta (HttpResponse)
            HttpResponse<String> response = cliente.send(request, BodyHandlers.ofString());

            // TODO Comprueba que el código de la respuesta es correcto (2xx)
            if(response.statusCode() < 200 || response.statusCode() >= 300)
            {
                return null;       // Si no es correcto, devolvemos null
            }

            // TODO: Deserialice la respuesta usando la clase SpaceShip
            Gson parser = new Gson();
            p = parser.fromJson(response.body(), SpaceShip.class);
        }
        catch (Exception e)
        {
            System.err.println(e);
            return null;
        }
        return p;
    }
}
