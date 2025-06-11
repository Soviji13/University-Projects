# Star Wars HTTPS API Test 🚀

This is a university project aimed at creating and testing HTTPS requests to the Star Wars API (https://swapi.py4e.com). The objective is to fetch and work with data from the API (characters, starships, etc.) and run basic question-based tests on the received resources.


---

## 🧠 Project Structure - Downloadable Files

- **Main.java** (required): Located in `httpTestStarWars/src/es/uma/rys/app/Main` — runs the test and lets you add questions.   
📥 Download [`Main.java` from here](https://raw.githubusercontent.com/Soviji13/University-Projects/refs/heads/main/httpTestStarWars/src/es/uma/rys/app/Main.java)  


- **SWClient.java** (required): Located in `httpTestStarWars/src/es/uma/rys/app/SWClient` — handles HTTPS requests and syncs data with Main.  
  📥 Download [`SWClient.java` from here](https://raw.githubusercontent.com/Soviji13/University-Projects/refs/heads/main/httpTestStarWars/src/es/uma/rys/app/SWClient.java)  
  

- **extract-tls-secrets-4.0.0.jar** (optional): Located in `httpTestStarWars/lib` — a Java agent for extracting TLS secrets, useful for Wireshark analysis.  
  📥 Download [`extract-tls-secrets-4.0.0.jar` from here](https://raw.githubusercontent.com/Soviji13/University-Projects/refs/heads/main/httpTestStarWars/lib/extract-tls-secrets-4.0.0.jar)

> ⚠️ To download the .java files, please right-click the link and choose "Save Link As..." (or your browser’s equivalent) because clicking the link directly will open the file as plain text in your browser instead of downloading it automatically.

---

## 🔧 How to Compile & Run

You have **two options**:

### Option 1: With basics 🤠 (Recommended if you only want to run the test)

Make sure you have both **Main.java** and **SWClient.java** in the same folder (or properly structured in your project).

### Option 2: With `extract-tls-secrets-4.0.0` 🦈 (Wireshark Analysis)

If you want to generate a key log file for analyzing HTTPS traffic in **Wireshark**, download `extract-tls-secrets-4.0.0.jar` and add the following to your **VM options**:
``` -javaagent:/<path-to-extract>/extract-tls-secrets-4.0.0.jar=sslkey-app.log ```  

When you compile and run the program, a file named `sslkey-app.log` will be generated.  

To use it in **Wireshark**, go to: ```Preferences → Protocols → TLS → (Key Log File Name)```

Then filter the traffic with: ```http.host == "swapi.py4e.com"```.
You should now see the decrypted **GET** request packets.





---

## 🧹 Project Cleanup Info
This project contains some development/config files that are **not needed** to run the code, and they are excluded via `.gitignore`. These include:

- `.idea/`, `.classpath`, `.project`, `.iml` files (IDE-specific config)
- `bin/`, `out/`, `lib/` (compiled classes or dependencies)
- `jSSKeyLog.jar`, `sslkey-app.log` (temporary or log files)

If you're cloning this repo just to run the test: **stick to the `src/` folder**.

---

## 📜 License
This is a student project and has no license. Feel free to learn from it or adapt it with proper attribution.

Enjoy exploring the galaxy with Java! 💫


---

## 📧 Contact

If you have any questions, feel free to contact me at: **sofiasivillalbajimenez@gmail.com**