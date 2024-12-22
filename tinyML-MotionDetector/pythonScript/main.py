import serial
import paho.mqtt.client as mqtt
import time

# Replace '/dev/cu.usbmodem101' with your actual serial port
port = '/dev/cu.usbmodem101'
baud_rate = 115200

# MQTT broker details
mqtt_broker = "broker.hivemq.com"
mqtt_port = 1883
mqtt_topic_right = "amal/right"
mqtt_topic_top = "athira/tinyml"

# Initialize MQTT client
client = mqtt.Client()

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT broker.")
    else:
        print(f"Failed to connect to MQTT broker. Return code: {rc}")

client.on_connect = on_connect

try:
    # Connect to MQTT broker
    client.connect(mqtt_broker, mqtt_port, 60)
    client.loop_start()
except Exception as e:
    print(f"Failed to connect to MQTT broker: {e}")
    exit()

try:
    # Open the serial port
    ser = serial.Serial(port, baud_rate, timeout=1)
    print(f"Connected to {port} at {baud_rate} baud.")
except serial.SerialException as e:
    print(f"Failed to open serial port {port}: {e}")
    client.loop_stop()
    exit()

while True:
    try:
        if ser.in_waiting > 0:
            # Read a line from the serial port
            line = ser.readline().decode('utf-8').strip()
            print(f"Received: {line}")

            # Trigger actions based on the received command
            if line.lower() == 'right':
                print("Action: Move Right")
                client.publish(mqtt_topic_right, "Turn On Light")
            elif line.lower() == 'top':
                print("Action: Move Top")
                client.publish(mqtt_topic_top, "Turn on ")
            else:
                print("Unknown command.")
    except serial.SerialException as e:
        print(f"Serial error: {e}")
        break
    except Exception as e:
        print(f"Unexpected error: {e}")
        break

    time.sleep(0.1)  # Prevent high CPU usage
