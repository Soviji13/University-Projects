package es.uma.rys.entities;

// Clase obtenida al consultar la información de un personaje

public class Person {
	public String name;
	public String birth_year;
	public String eye_color;
	public String gender;
	public String hair_color;
	public String height;
	public String mass;
	public String skin_color;
	public String homeworld;
	public String[] films;
	public String[] species;
	public String[] starships;
	public String[] vehicles;

	// Todos los atributos anteriores se obtienen directamente del objeto devuelto por la consulta
	// Los siguientes atributos deben rellenarse, si es necesario, consultando las URLs devueltas
	// en los campos anteriores

	public World homeplanet = null;
	public Movie[] movies = null;
	public Specie[] speciesDetails = null;
	public SpaceShip[] spaceships = null;
	public Vehicle[] vehiclesDetails = null;

	public String toString() {
		String text = name + " (" + gender + ") was born in the year " + birth_year;
		if (homeplanet != null) text += " on " + homeplanet;
		text += "\nWeight: " + mass + " Kg and height: " + height + " cm\n";
		if (movies != null) {
			text += "Appears in:\n";
			for (Movie f : movies) {
				text += "- " + f + "\n";
			}
		}
		return text;
	}
}
