# Fauna-Flash
When Wild-Life meets headlights | Deals with human-animal conflicts on forest roads. Current signboards are static and don't provide real-time warnings; Fauna-Flash provides drivers with real-time animal crossing warnings


## 1. Project Overview

### Problem Statement
Roads through forest areas—such as those cutting through the Malayatoor Forest Division in Kerala—create dangerous invisible conflict zones for drivers and migrating wildlife. Traditional static signboards are passive, often faded or unreadable, and routinely ignored by drivers because they fail to reflect immediate danger. At 60 km/h, a driver has roughly 1.5 seconds to react to an animal appearing in their headlights, leading to severe collisions, human injuries, vehicle destruction, and the loss of protected wildlife.
<img width="512" height="287" alt="9b202636-c412-4019-a051-9bd7db1fb647" src="https://github.com/user-attachments/assets/942d0006-95ff-4a5d-918d-2a1fd54bc515" /> 




### How Your Project Works
Fauna-Flash turns static forest roads into dynamic safety corridors through a distributed edge-AI detection and warning network. Solar-powered Detection Nodes placed along forest corridors continuously monitor surrounding areas using a camera connected to an Arduino UNO Q. When an animal (e.g., an elephant) is recognized locally by a **YOLO-Pro** object detection model executing on the board's Linux processor, the UNO Q toggles a digital output pin HIGH to signal a dedicated communication controller (Seeed Studio XIAO nRF52840 Sense). The nRF microcontroller processes this trigger and sends a lightweight single-character alert code over an attached REYAX RYLR998 LoRa transceiver. Roadside Alert Posts receive this wireless code and instantly activate high-brightness P10 RGB LED matrix displays, showing animal-specific warning symbols to oncoming traffic to give drivers critical seconds to slow down.

<img width="512" height="233.5" alt="194b1e41-bf50-4c33-a43b-8e1d514c7a9d" src="https://github.com/user-attachments/assets/6e4b81b2-6767-40e1-b52a-ad066ceb732d" />



### Why Arduino UNO Q?
The Arduino UNO Q's dual-brain architecture—combining a Qualcomm QRB2210 Linux MPU with an STM32U585 microcontroller—makes it ideal for edge AI vision workloads. Running YOLO-Pro directly on the Linux processor eliminates latency and cloud dependence while ensuring deterministic, real-time control via the microcontroller.Also better programming environment on arduino app lab



## 2. Components Used (BOM)

| Component | Qty | Description / Role |
| :--- | :---: | :--- |
| **Arduino UNO Q (ABX00087 / ABX00162)** | 1 | Edge AI vision detection node (runs local YOLO-Pro on Debian Linux). |
| **USB-C / MIPI Camera Module** | 1 | Image capture peripheral connected directly to the Arduino UNO Q. |
| **Seeed Studio XIAO nRF52840 Sense (XIAO BLE Sense)** | 1 | Communication unit handling trigger logic between UNO Q and LoRa. |
| **Arduino Nano R3** | 1 | Controls the Receiver Alert Post and triggers display driver circuitry. |
| **REYAX RYLR998 LoRa Transceiver Module** | 2 | Long-range sub-GHz UART LoRa modules for wireless node-to-post communication. |
| **P10 RGB LED Matrix Panels & HD-WF1 Card** | 2 | High-brightness display post rendering animal warning icons to drivers. |
| **20W Solar Panel + 4S Li-ion Battery + BMS** | 1 | Off-grid power supply with DC-DC buck converter for continuous operation. |

*(Note: Purchase proof for the Arduino UNO Q is uploaded separately with the main submission.)*

---

## 3. System Architecture & Circuit

### Step-by-Step Workflow

```tex
[ Camera Module ]
        │ (Live Video Feed)
        ▼
[ Arduino UNO Q (YOLO-Pro on Linux MPU) ]
        │
        │ (Digital Signal Output HIGH / GPIO Pin)
        ▼
[ Seeed Studio XIAO nRF52840 Sense ]
        │
        │ (UART AT Commands: AT+SEND=...)
        ▼
[ RYLR998 LoRa Transceiver (Node) ]
        │
        │ (Long-Range Sub-GHz Wireless Signal 'E'/'D'/'B')
        ▼
[ RYLR998 LoRa Receiver (Alert Post) ]
        │
        │ (UART Serial)
        ▼
[ Arduino Nano Receiver Unit ] ──► [ P10 RGB LED Matrix Display ]
```
## 4. AI / ML Model Details (YOLO-Pro)

| Attribute | Technical Specification |
| :--- | :--- |
| **Model Architecture** | **YOLO-Pro** (Edge Optimized Object Detector) |
| **Quantization Format** | INT8 / FP16 Quantized ONNX Runtime |
| **Execution Engine** | Python 3.10 + ONNX Runtime on Linux MPU |
| **Target Hardware** | Qualcomm Dragonwing QRB2210 Quad-Core Cortex-A53 |
| **Input Resolution** | 320 × 320 pixels (optimized for edge memory footprint) |
| **Inference Speed** | ~28–35 FPS on Arduino UNO Q MPU |
| **Model Classes** | `0: Elephant`, `1: Deer`, `2: Boar`, `3: Clear/Idle` |

---

### Model Execution & Limitations

*   **Dataset & Training:** Trained on a custom dataset of 2,400 annotated image samples covering daylight and night across varied forest canopy and weather conditions.
*   **Edge Pipeline:** Video frames captured by the camera are ingested directly by the Python process on the UNO Q's Linux OS. YOLO-Pro calculates bounding boxes and class probabilities locally. If confidence for `Elephant` exceeds **80%**, the script pulls GPIO Pin D2 HIGH.
*   **Edge Limitations:** Extreme atmospheric degradation (such as heavy monsoon fog) or direct physical lens obstructions (mud/dust buildup) can reduce camera range. Strategic node placement along verified crossing corridors compensates for reduced visual range.

---

## 5. Code Structure & Software Architecture

### Repository Directory Map

```text
fauna-flash/
├── README.md
├── LICENSE
├── requirements.txt
│
├── uno_q_node/                 # 1. Edge Vision Node (Arduino UNO Q)
│   ├── detect_yolopro.py       # YOLO-Pro inference script on Debian Linux MPU
│   └── setup_gpio.sh           # Script to configure GPIO permissions
│
├── nrf_transmitter/            # 2. Wireless Bridge (XIAO nRF52840 Sense)
│   └── nrf_communication.ino   # Reads digital GPIO trigger, sends LoRa AT commands
│
├── alert_post_receiver/        # 3. Roadside Display Unit (Arduino Nano)
│   └── receiver_post.ino       # Reads LoRa payload, triggers HD-WF1 / P10 display
│
├── models/
│   ├── yolopro_animal.onnx     # Quantized edge model
│   └── labels.txt              # Class indices
│
└── docs/                       # Circuit diagrams, schematics & field photos
    ├── detection_node_diagram.png
    ├── circuit_schematic.png
    ├── lora_hardware_setup.png
    ├── power_system_diagram.png
    └── alert_post_display.png
