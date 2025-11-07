# 🎵 Advanced Audio Player

A **modern, modular, and professional audio player** built with JUCE, designed for smooth playback, advanced controls, and a beautiful, user-friendly interface.

---

## 🚀 Features

### 🔹 Core Playback
- **Load, Restart, Stop** – essential controls kept intact.  
- **Modular Design:** `Player.h` / `Player.cpp` (or split into `PlayerGUI` & `PlayerAudio`).

### 🔹 Playback Controls
- **Play / Pause:** `►` / `||`  
- **Go to Start / End:** `|◄` / `►|`  
- **Mute / Unmute** – restores previous volume  
- **Loop Playback** – continuously repeats audio  
- **Jump Forward / Backward 10s** – convenient for podcasts or long tracks  

### 🔹 Audio Information
- Displays **metadata** (Title, Artist, Duration) or filename if metadata is missing  
- **Playback speed slider** – play audio faster or slower  
- **Position slider** – seek to specific time, shows current playback  

### 🔹 Advanced Features
- **Playlist Support** – load and navigate multiple tracks  
- **Progress Bar or Waveform Display** – visualize playback with a moving pointer  
- **Segment Looping (A-B)** – loop a portion of a track  
- **Track Markers & Navigation** – add and jump to markers  
- **Session Persistence** – remembers last played file and position  

### 🎨 UI Design
- Professional, intuitive, and friendly layout  
- Customizable colors, icons, and buttons  
- Simple yet elegant for real-world usage  

---

## 🖼 Screenshots / GIFs

![Player UI](./resources/screenshot1.png)  
*Example of playback UI with waveform and controls.*

![Playlist View](./resources/screenshot2.png)  
*Playlist support with multiple tracks.*

![AB Loop](./resources/screenshot3.png)  
*Segment looping and markers.*

---

## ⚡ Quick Start

```bash
# Clone the repository
git clone <repository-url>

# Open the project in JUCE or your preferred IDE
# Build & run the project
# Load audio files and enjoy the intuitive controls and advanced features
