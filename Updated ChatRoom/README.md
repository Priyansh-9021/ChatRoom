# 🚀 *Multi-User Chatroom Application*  

A *cross-platform* 💻✨ chatroom with *real-time messaging* 📨, *sound notifications* 🔊, and *timestamps* 🕒—built for *fun* and *performance*!  

---

## 🔥 *Features*  

✅ *Cross-Platform* – Works on *Windows, Linux, and macOS*  
✅ *Real-Time Messaging* – Instant delivery to all clients ⚡  
✅ *Username Support* – Show off your identity 🎭  
✅ *Sound Alerts* – Beep! (Windows) or terminal bell (Linux/macOS)  
✅ *Timestamps* – Know exactly when messages were sent 🕒  
✅ *Multi-Threaded* – Handles *dozens of clients* effortlessly 🚀  

---

## 🛠 *Components*  

### *1. Socket Library* (socket.cpp, socket.h)  
📦 *Cross-platform wrapper* for sockets  
⚙ *Auto-cleanup* (no memory leaks!)  
🔄 *Supports both Windows & POSIX*  

### *2. Server* (server.cpp)  
🌐 *Broadcasts messages* to all clients  
🧵 *Thread-per-client* for smooth performance  
🔌 *Graceful shutdown* (Ctrl+C friendly)  

### *3. Client* (client.cpp)  
💬 *Clean UI* with usernames & timestamps  
🔔 *Sound notifications* for new messages  
🚪 **Type exit to leave** gracefully  

---

## 🚀 *Quick Start*  

### *Compile & Run*  

#### *Linux/macOS*  
bash
g++ -std=c++17 server.cpp -o server -lpthread
g++ -std=c++17 client.cpp -o client -lpthread


#### *Windows*  
bash
g++ -std=c++17 server.cpp -o server.exe -lws2_32 -lpthread
g++ -std=c++17 client.cpp -o client.exe -lws2_32 -lpthread


### *Run the Server*  
bash
./server 8080  # Replace 8080 with your desired port


### *Connect Clients*  
bash
./client 127.0.0.1 8080  # Replace with server IP if remote


---

## 🎮 *How to Use*  

### *Server*  
- Just run it! Clients connect automatically.  
- Press *Ctrl+C* to shut down gracefully.  

### *Client*  
1️⃣ *Enter your name* when prompted.  
2️⃣ *Type messages* and hit *Enter* to send.  
3️⃣ *See real-time updates* from others.  
4️⃣ **Type exit** to leave.  

---

## 🎯 *Why This Chatroom Rocks*  

💡 *Simple but powerful* – No bloat, just fast messaging.  
⚡ *Low-latency* – Messages arrive instantly.  
🔒 *Stable* – Proper error handling & cleanup.  
🎨 *Fun UX* – Sound alerts & timestamps.  

---

## 📜 *License*  
MIT License – *Free to use, modify, and share!*  

---

### *Ready to chat?* 🚀 *Fire it up and start messaging!* 💬🔥