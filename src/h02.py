from datetime import datetime
import unittest

class H02Serializer:
    @staticmethod
    def serialize_location(device_id, latitude, longitude, speed, direction, date_time, status):
        time_str = date_time.strftime("%H%M%S")
        date_str = date_time.strftime("%d%m%y")
        # *HQ,123456789012345,V1,222134,A,2263.1291,N,11486.8730,E,0.00,0.00,010112,ffffffff,000000,000000,000000,000000#
        # *HQ,123456789012345,V1,121300,A,6000.0000,N,13000.0000,E,0.00,0.00,010112,ffffffff,000000,000000,000000,000000#
        return f"*HQ,{device_id},V1,{time_str},A,{latitude * 100:.4f},N,{longitude * 100:.4f},E,{speed:.2f},{direction:.2f},{date_str},{status},000000,000000,000000,000000#"

    @staticmethod
    def serialize_heartbeat(device_id):
        return f"*HQ,{device_id},XT#"

    @staticmethod
    def serialize_server_settings(device_id, index, ip, port, domain, time):
        return f"*HQ,{device_id},IP,{index},{ip},{port},{domain},{time}#"


    @staticmethod
    def deserialize_location(message):
        parts = message.strip('#').split(',')
        if len(parts) != 17 or not parts[0].startswith("*HQ"):
            raise ValueError("Invalid H02 location packet format")
        
        device_id = parts[1]
        time = parts[3]
        date = parts[11]
        validity = parts[4]
        lat = float(parts[5]) / 100
        lat_dir = parts[6]
        lon = float(parts[7]) / 100
        lon_dir = parts[8]
        speed = float(parts[9])
        direction = float(parts[10])
        status = parts[12]
        
        dt = datetime.strptime(time + date, "%H%M%S%d%m%y")
        
        latitude = lat if lat_dir == 'N' else -lat
        longitude = lon if lon_dir == 'E' else -lon
        
        return {
            "device_id": device_id,
            "datetime": dt,
            "validity": validity,
            "latitude": latitude,
            "longitude": longitude,
            "speed": speed,
            "direction": direction,
            "status": status
        }

    @staticmethod
    def deserialize_heartbeat(message):
        parts = message.strip('#').split(',')
        if len(parts) < 3 or not parts[0].startswith("*HQ"):
            raise ValueError("Invalid H02 heartbeat packet format")
        return {"device_id": parts[1]}
    
    @staticmethod
    def deserialize_server_settings(message):
        parts = message.strip('#').split(',')
        if len(parts) < 7 or not parts[0].startswith("*HQ"):
            raise ValueError("Invalid H02 server settings packet format")
        
        return {
            "device_id": parts[1],
            "index": int(parts[3]),
            "ip": parts[4],
            "port": parts[5],
            "domain": parts[6],
            "time": parts[7]
        }

class TestH02Serializer(unittest.TestCase):
    def test_location_serialization(self):
        serializer = H02Serializer()
        dt = datetime(2024, 2, 25, 12, 30, 45)
        packet = serializer.serialize_location("1234567890", 22.394210, 114.008825, 0.00, 348, dt, "FFFFFFFF")
        self.assertIn("1234567890", packet)
        self.assertIn("2239.4210", packet)
        self.assertIn("11400.8825", packet)

    def test_location_deserialization(self):
        serializer = H02Serializer()
        message = "*HQ,123456789012345,V1,222134,A,2263.1291,N,11486.8730,E,0.00,0.00,010112,ffffffff,000000,000000,000000,000000#"
        data = serializer.deserialize_location(message)
        self.assertEqual(data["device_id"], "123456789012345")
        self.assertEqual(data["latitude"], 22.631291)
        self.assertEqual(data["longitude"], 114.86873)
    
    def test_heartbeat_serialization(self):
        serializer = H02Serializer()
        packet = serializer.serialize_heartbeat("1234567890")
        self.assertEqual(packet, "*HQ,1234567890,XT#")
    
    def test_heartbeat_deserialization(self):
        serializer = H02Serializer()
        message = "*HQ,1234567890,XT#"
        data = serializer.deserialize_heartbeat(message)
        self.assertEqual(data["device_id"], "1234567890")
    
    def test_server_settings_serialization(self):
        serializer = H02Serializer()
        packet = serializer.serialize_server_settings("1234567890", 1, "192.168.1.1", "8080", "gps.server.com", "130305")
        self.assertEqual(packet, "*HQ,1234567890,IP,1,192.168.1.1,8080,gps.server.com,130305#")
    
    def test_server_settings_deserialization(self):
        serializer = H02Serializer()
        message = "*HQ,1234567890,IP,1,192.168.1.1,8080,gps.server.com,130305#"
        data = serializer.deserialize_server_settings(message)
        self.assertEqual(data["device_id"], "1234567890")
        self.assertEqual(data["index"], 1)
        self.assertEqual(data["ip"], "192.168.1.1")
        self.assertEqual(data["port"], "8080")
        self.assertEqual(data["domain"], "gps.server.com")
        self.assertEqual(data["time"], "130305")

if __name__ == "__main__":
    unittest.main()
