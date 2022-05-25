from PIL import ImageGrab,Image
import requests
from struct import *
import socket
import threading
from io import BytesIO	
import binascii
import time
class TcpClient:

    def __init__(self):
        self.ip = "192.168.0.113"
        self.port = 8081
        self.tcp_client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.tcp_client.connect((self.ip, self.port))
        

    def getMessage(self):
        
        data, addr = self.tcp_client.recvfrom(1024)
    
        #print(data.decode())
        if 'ok' in data.decode():
            self.sendMessages('JPEG',95)
        if 'exit' in data.decode():
            self.closeClient()
            return
                    

    def sendMessage(self, message):
        self.tcp_client.send(message)

    def sendMessages(self,format='JPEG',quality=None):
        message = self.getImage(format,quality)
        #print(len(message))
        self.sendMessage(message)
        if message == 'exit':
            self.closeClient()
                
    def closeClient(self):
        self.tcp_client.close()
        print("客户端已关闭")

    def getImage(self,format='JPEG',quality=None)-> None:
        if format == 'PNG' or format == 'png':
            self.format = 'PNG'
        if quality:
            self.quality = quality
        image = ImageGrab.grab()
        image = image.resize((240, 135))
        image.tobytes()
        img = BytesIO()
        image.save(img, format=format, quality=quality)
        data = img.getvalue()
        aa = b'\xaa\xbb\xcc'
        x= len(data) +3
        s = pack('<H', x)
        data = s+data+aa 
        return data
    
if __name__ == '__main__':
    while True:
        try:
            T = TcpClient()
            print("server connected")
            while True:
                try:
                    T.getMessage()
                except IOError as e:
                    print(e)
                    break
            raise IOError("Invalid level!")
        except IOError as e:
            print("waiting server")
            time.sleep(10)
            continue