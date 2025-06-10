# Star Wars HTTPS API Test 🚀

This is a university project aimed at creating and testing HTTPS requests to the Star Wars API (https://swapi.py4e.com). The objective is to fetch and work with data from the API (characters, starships, etc.) and run basic question-based tests on the received resources.

## 🧠 Project Structure

- **Main.java** (required): Located in `httpTestStarWars/src/es/uma/rys/app/Main` — runs the test and lets you add questions.
[📥 Download `Main.java` from here](https://raw.githubusercontent.com/Soviji13/University-Projects/refs/heads/main/httpTestStarWars/src/es/uma/rys/app/Main.java)

- **SWClient.java** (required): Located in `httpTestStarWars/src/es/uma/rys/app/SWClient` — handles HTTPS requests and syncs data with Main.
[📥 Download `SWClient.java` from here](https://raw.githubusercontent.com/Soviji13/University-Projects/refs/heads/main/httpTestStarWars/src/es/uma/rys/app/SWClient.java)

> ⚠️ Make sure to **download both `Main.java` and `SWClient.java`** from the correct path inside the `src` folder.

---

## 🔧 How to Compile & Run

You have **two options**:

### Option 1: With `extract-tls-secrets-4.0.0`
1. Download `extract-tls-secrets-4.0.0` (included in this repo).
2. Place it in the **same folder** as `Main.java` and `SWClient.java`.
3. Keep the current VM options (configured to use `jSSKeyLog.jar`).

### Option 2: Without extra tools
1. **Remove** the VM option related to `jSSKeyLog.jar`.
2. Compile normally using any Java compiler or IDE.

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
