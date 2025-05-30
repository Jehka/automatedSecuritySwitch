
# 🔌 Automatic Security Switch (Capstone Project)

An **IoT-enabled power redundancy system** designed to automate load transfer between a main power line and two backup generators—keeping mission-critical facilities like hospitals always online.

---

## 📜 Project Summary

The Automatic Security Switch (ASS) is a smart, fail-safe solution for managing power sources in environments where uptime is non-negotiable. Developed with hospital operation wings in mind, this system continuously monitors power availability and autonomously switches between:

- **Main city power line**
- **Backup Generator 1**
- **Backup Generator 2**

Should the primary source fail, the system ensures uninterrupted power by activating the appropriate backup source. It also includes a **manual override via a secure web interface**, giving facility operators direct control when needed.

---

## 💡 Features

- ✅ **Automated Load Shedding**: Reduces downtime by up to 40% by smartly shifting loads between sources.
- 📶 **IoT Connectivity**: Built with an ESP32 microcontroller; utilizes MQTT/CoAP protocols for real-time updates.
- 🖥️ **Web-Based Control**: HTML-powered interface allows manual override from any browser.
- 🔁 **Redundant Failover Logic**: Prioritized switching logic ensures seamless fallback to Generator 2 if Generator 1 fails.
- 🔧 **Built-in Manual Control**: Integrated relays offer physical override capabilities in case of system-level failure.
- 📟 **Serial Monitor Debugging**: Real-time diagnostics via the Arduino IDE.

---

## ⚙️ Hardware Specifications

| Component                          | Description                      |
|-----------------------------------|----------------------------------|
| **Microcontroller**               | ESP32                            |
| **Power Inputs**                  | Main supply, 2 backup generators |
| **Relays/Contactors**             | For switching and control        |
| **Connectivity**                  | Wi-Fi, Wired PCB traces          |
| **Status Monitoring**             | LEDs, Serial Monitor             |
| **Build Enclosure**               | Plastic Electric Box             |

---

## 🧠 Software Stack

- **Arduino IDE** – Code development
- **HTML** – Web interface for manual override
- **OrCAD & PADS** – Schematic and PCB design
- **MS Visio** – Mechanical layout & planning

---

## 🌐 IoT Functionality

The project integrates basic IoT capabilities using:

- **Wi-Fi-enabled ESP32**
- **Protocols**: MQTT / CoAP (configurable per deployment)
- **Secure Web UI**: Toggle switch states and receive live feedback

---

## 🧪 Testing & Use Case

The system was designed for and tested in high-dependency environments like hospital operation wings. All failure cases and override pathways were simulated and validated pre-deployment.

---

## 💰 Budget Overview

> 💸 Estimated Total Cost: **~$350**

This includes MCU, contactors, power modules, PCB, casing, and peripheral hardware.

---

## 🚀 Future Improvements

- 🔐 Add secure login for remote control interface
- 📊 Dashboard for live telemetry and alert logging
- ☁️ Cloud-based backup using AWS or Firebase
- 🧠 AI prediction module for generator health checks

---

## 👨‍💻 Team A05

| Role              | Name             | Student ID   |
|-------------------|------------------|--------------|
| Project Manager 1 | Arbri Goxhaj     | 155717218    |
| Project Manager 2 | Jehkaran Singh   | 161922216    |

---

## 📄 License

This project is released under the [MIT License](LICENSE), allowing open-source use and modification.
