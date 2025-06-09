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

public class SWClient {
    // TODO: Completa el nombre de la aplicación
    private final String app_name = "";
    private final int year = 2025;
    private final String url_api = "https://swapi.py4e.com/api/";

    // Métodos auxiliares

    // Obtiene la URL del recurso id del tipo resource
	public String generateEndpoint(String resource, Integer id){
	    return url_api + resource + "/" + id + "/";
	}
	
	// Dada una URL de un recurso obtiene su ID
	public Integer getIDFromURL(String url){
		String[] parts = url.split("/");

		return Integer.parseInt(parts[parts.length-1]);
	}
	
	// Consulta un recurso y devuelve cuántos elementos tiene
    public int getNumberOfResources(String resource) {
        String urlname = url_api + resource + "/";
        int number = 0;

        // TODO: Maneja de forma apropiada las posibles excepciones
        try {
            // TODO: Crea un objeto para manejar una conexión (HttpClient) de HTTP/1.1

            // TODO: Crea una petición HTTP (HttpRequest) con las siguientes características
            // - la URL está en urlname
            // - Añada las cabeceras indicadas en el enunciado
            // - Use el método GET

            // TODO: envíe la petición para obtener una respuesta (HttpResponse)

            // TODO Comprueba que el código de la respuesta es correcto (2xx)

            // Deserialice la respuesta
            Gson parser = new Gson();
            // TODO: Reemplace "" por el cuerpo de a respuesta
            ResourceCountResult c = parser.fromJson("", ResourceCountResult.class);
            number = c.count;
            
        } catch (Exception e) {
            System.err.println(e);
            return 0;
        }
        
        return number;
    }

    public Person getPerson(String urlname) {
        Person p = null;
        // TODO: Maneja de forma apropiada las posibles excepciones
        try {
            urlname = urlname.replaceAll("http:", "https:");

            // TODO: Crea un objeto para manejar una conexión (HttpClient) de HTTP/1.1

            // TODO: Crea una petición HTTP (HttpRequest) con las siguientes características
            // - la URL está en urlname
            // - Añada las cabeceras indicadas en el enunciado
            // - Use el método GET

            // TODO: envíe la petición para obtener una respuesta (HttpResponse)

            // TODO Comprueba que el código de la respuesta es correcto (2xx)

            // TODO: Deserialice la respuesta usando la clase Person
            Gson parser = new Gson();

            // TODO: Para las preguntas 2 y 3 (no lo necesita para la pregunta 1: tallest)
            // TODO: Usando la URL en el campo homeworld, obtenga los datos del planeta y almacénelos en el atributo homeplanet

        } catch (Exception e) {
            System.err.println(e);
            return null;
        }
        return p;
    }

    public World getWorld(String urlname) {
        World p = null;
        // TODO: Maneja de forma apropiada las posibles excepciones
        try {
            urlname = urlname.replaceAll("http:", "https:");
            // TODO: Crea un objeto para manejar una conexión (HttpClient) de HTTP/1.1

            // TODO: Crea una petición HTTP (HttpRequest) con las siguientes características
            // - la URL está en urlname
            // - Añada las cabeceras indicadas en el enunciado
            // - Use el método GET

            // TODO: envíe la petición para obtener una respuesta (HttpResponse)

            // TODO Comprueba que el código de la respuesta es correcto (2xx)

            // TODO: Deserialice la respuesta usando la clase World
            Gson parser = new Gson();

        } catch (Exception e) {
            System.err.println(e);
            return null;
        }
        return p;
    }

    public Person searchPersonByName (String name) {
        Person p = null;
        // TODO: Maneja de forma apropiada las posibles excepciones
        try {
            // TODO: Cree la URL a partir del nombre procesado tal como se indica en el enunciado
            String urlname = "";
            urlname = urlname.replaceAll("http:", "https:");
            // TODO: Crea un objeto para manejar una conexión (HttpClient) de HTTP/1.1

            // TODO: Crea una petición HTTP (HttpRequest) con las siguientes características
            // - la URL está en urlname
            // - Añada las cabeceras indicadas en el enunciado
            // - Use el método GET

            // TODO: envíe la petición para obtener una respuesta (HttpResponse)

            // TODO Comprueba que el código de la respuesta es correcto (2xx)

            // TODO: Deserialice la respuesta usando la clase QueryResponse
            Gson parser = new Gson();

            // TODO: Si la respuesta tiene al menos una persona, considere como respuesta esa primera persona

            // TODO: Para las preguntas 2 y 3 (no lo necesita para la pregunta 1: tallest)
            // TODO: Usando la URL en el campo homeworld, obtenga los datos del planeta y almacénelos en el atributo homeplanet

        } catch (Exception e) {
            System.err.println(e);
            return null;
        }
        return p;
    }
}
