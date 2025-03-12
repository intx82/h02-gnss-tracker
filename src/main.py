import socket
import time
import random
from datetime import datetime
from h02 import H02Serializer

class GNSSClient:
    SERVER_DOMAIN = "tracar.gps-monitoring.wezom.agency"
    SERVER_PORT = 5013

    def __init__(self):
        self.server_ip = self.resolve_domain()
        if not self.server_ip:
            print("Could not resolve server domain, aborting connection.")
            return
        
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((self.server_ip, self.SERVER_PORT))
        print("Connected to server.")

    @staticmethod
    def resolve_domain():
        try:
            return socket.gethostbyname(GNSSClient.SERVER_DOMAIN)
        except socket.gaierror as e:
            print(f"DNS resolution failed for {GNSSClient.SERVER_DOMAIN}: {e}")
            return None

    def send_location(self, device_id, latitude, longitude, speed, direction, date_time, status):
        packet = H02Serializer.serialize_location(device_id, latitude, longitude, speed, direction, date_time, status)
        self.socket.sendall(packet.encode('utf-8'))
        print(f"Sent: {packet}")
    
    def send_heartbeat(self, device_id):
        packet = H02Serializer.serialize_heartbeat(device_id)
        self.socket.sendall(packet.encode('utf-8'))
        print(f"Sent: {packet}")

if __name__ == "__main__":
    client = GNSSClient()
    if not client.server_ip:
        exit(1)

    device_id = "123456789012345"
    while True:
        client.send_heartbeat(device_id)
        time.sleep(30)
        
        lat = round(random.uniform(22.0, 23.0), 6)
        lon = round(random.uniform(114.0, 115.0), 6)
        speed = 0
        direction = 0
        client.send_location(device_id, lat, lon, speed, direction, datetime.now(), "ffffffff")
        time.sleep(30)
