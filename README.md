# Clipboard Hijacking Trojan

---

## Project Summary

The Clipboard Hijacking Trojan project focuses on implementing a Trojan that utilizes anti-analysis techniques for clipboard hijacking. This Trojan monitors clipboard activities, replacing specific content with malicious payloads. It employs anti-sandbox, anti-virtualization, and anti-debugging techniques to enhance concealment. Additionally, the Trojan includes persistence mechanisms to maintain a long-term threat presence.

The Clipboard Hijacking Trojan demonstrates the following capabilities:

1. **Clipboard Monitoring:**
   - Effectively replaces specific clipboard content, ensuring high concealment.

2. **Anti-Analysis Techniques:**
   - Implements robust anti-sandbox, anti-virtualization, and anti-debugging techniques, posing a challenge to reverse engineering.

3. **Persistence Mechanisms:**
   - Utilizes various persistence technologies, requiring behavioral analysis for effective removal.

---

## Project Structure

### 1. Clipboard Monitoring Payload

- **Features:**
  - Monitors clipboard content.
  - Replaces specific content with a predefined address if it matches the cryptocurrency wallet address pattern.

- **Files:**
  - `payload.cpp`: Contains the binary constant of the compiled payload program.

- **Implementation:**
  - Uses C++ and WinAPI to create a hidden window for clipboard monitoring.
  - Periodically checks clipboard content, replacing it when conditions are met.
  - Ensures concealment and persistence.

### 2. Trojan Main Body

- **Features:**
  - Detects sandbox, virtualization, and debugging environments.
  - Installs the payload as a service for persistence.
  - Sets payload file attributes to hide it.

- **Files:**
  - `main.cpp`: Controls the operation flow.
  - `anti.cpp`: Implements anti-analysis techniques.
  - `payload.cpp`: Declares the binary constant.
  - `dropPayload.cpp`: Writes the payload to the user's startup directory.
  - `createService.cpp`: Installs the service.

- **Implementation:**
  - Uses various anti-analysis functions to detect the analysis environment.
  - Installs the payload as a service for persistence.
  - Hides the payload file.

---

## Build and Run Instructions

1. **Requirements:**
   - C/C++ compiler (e.g., Clion).
   - Windows operating system.

2. **Build:**
   - Open the project in a C++ IDE.
   - Compile and build the entire project.

3. **Run:**
   - Execute the compiled main executable file.

---

## Disclaimer

This tool is for educational and research purposes only. Its use for illegal activities is strictly prohibited. The developers are not responsible for any legal consequences resulting from misuse.
