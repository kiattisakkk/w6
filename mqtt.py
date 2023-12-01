import json
import requests
import paho.mqtt.client as mqtt

# MQTT Configuration
mqtt_broker = "192.168.16.74"
mqtt_topic = "DHT11"

# JSON Server Configuration
json_server_url = "http://192.168.16.74:3000/sensors"

def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT Broker with result code "+str(rc))
    client.subscribe(mqtt_topic)

def on_message(client, userdata, msg):
    payload = msg.payload.decode("utf-8")
    print("Received message on topic {}: {}".format(msg.topic, payload))

    # Assuming the payload is in the format "Humidity: xx%, Temperature: xx°C"
    # You may need to adapt this based on your actual payload format
    data = {"topic": msg.topic, "payload": payload}

    # Send data to the JSON server
    send_to_json_server(data)

def send_to_json_server(data):
    headers = {"Content-Type": "application/json"}
    response = requests.post(json_server_url, data=json.dumps(data), headers=headers)

    if response.status_code == 200:
        print("Data sent to JSON server successfully.")
    else:
        print("Failed to send data to JSON server. Status code:", response.status_code)

def main():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(mqtt_broker, 1883, 60)

    # Run the MQTT client loop in a non-blocking way
    client.loop_start()

    try:
        while True:
            pass  # Keep the script running
    except KeyboardInterrupt:
        print("Exiting...")
        client.disconnect()
        client.loop_stop()

if __name__ == "__main__":
    main()