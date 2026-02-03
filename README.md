# MQTT Client Example with TLS 1.3 Encryption on NUCLEO-F429ZI with Temperature Sensor

This example demonstrates how to establish a secure connection to an MQTT broker using **TLS v1.3 encryption** on a NUCLEO-F429ZI board.

The application periodically reads a temperature value from an **analog temperature sensor** using the STM32 **ADC (Analog-to-Digital Converter)** and publishes the measured data to the MQTT broker on a dedicated topic.

---

## Features 🧰

- Secure connection to MQTT broker using TLS v1.3 🔐
- Periodic MQTT message publishing ⏱️
- Analog temperature sensing via STM32 ADC 🌡️
- Utilizes NUCLEO-F429ZI board for implementation 💻

---

## Requirements ✅

- **NUCLEO-F429ZI board:**  
  This example is specifically tailored for the NUCLEO-F429ZI development board.

- **IDE:**  
  STM32CubeIDE 1.16.0 📥

- **MQTT broker:**  
  An MQTT broker that supports TLS v1.3 connections 🧩

- **Temperature Sensor:**  
  Any **analog temperature sensor** compatible with the STM32 ADC input range.

---

## Temperature Sensor Overview 🌡️

The temperature measurement is performed using an **analog sensor** whose output voltage varies as a function of temperature.

The sensor is **not digital** and does **not** use communication protocols such as One-Wire, I²C, or SPI.

The measurement flow is:

```
Analog sensor → ADC raw value → Voltage → Temperature → MQTT publish
```

---

## How to Connect the Temperature Sensor 🛠️

1. **Signal Pin:**  
   Connect the analog output of the temperature sensor to an **ADC-capable pin** of the NUCLEO-F429ZI board.

2. **Power Supply:**  
   Power the sensor according to its specifications (typically 3.3 V).

3. **Ground:**  
   Connect the sensor ground to one of the board GND pins.

4. **Important Note:**  
   The ADC input **must not be left floating**.  
   If the sensor is disconnected, ADC readings are undefined due to noise and the internal ADC sampling capacitor.

---

## How to Run 🚀

1. **Compile:**  
   Compile the firmware using STM32CubeIDE with the configured MQTT broker and TLS settings 🧑‍💻

2. **Execute:**  
   Flash the compiled firmware onto the NUCLEO-F429ZI board and run the application 🏃‍♀️

---

## Important Notes ⚠️

- **Network Connectivity:**  
  Ensure that the board has network connectivity to reach the MQTT broker 🌐

- **Broker Requirements:**  
  The broker may require specific authentication credentials or topic permissions 📖

- **Error Handling:**  
  For production use, additional error handling and logging should be implemented 🐛

---

## Disclaimer 📢

This is a basic example provided for demonstration purposes.  
Adapt and extend it according to your specific use case and security requirements 🛠️

---

## License 📄

This code is provided under the [MIT License](LICENSE).  
Feel free to use, modify, and distribute it 👍

---

## Contributing 🤝

Contributions are welcome!  
Please open an issue or submit a pull request if you find bugs or have suggestions 🙏

---

## Todo 🚧

1. Add post-quantum secure element integration 🛡️
