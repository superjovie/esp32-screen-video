
import cv2
from PIL import ImageGrab,Image
import requests
from struct import *
import socket
import threading
from io import BytesIO  
import binascii
import os 

def file_name(file_dir):
        for root, dirs, files in os.walk(file_dir):
            print(root) #当前目录路径
            print(files) #
            return files

class TcpClient:

    def __init__(self):
        self.ip = "192.168.0.113"
        self.port = 8081
        self.tcp_client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.tcp_client.connect((self.ip, self.port))
        

    def getMessage(self):
        data, addr = self.tcp_client.recvfrom(1024)

        if 'ok' in data.decode():
            self.sendMessages('JPEG',85)
        if 'exit' in data.decode():
            print(data.decode())
            self.closeClient()
            return

             

    def sendMessage(self, message):
        self.tcp_client.send(message)

    def sendMessages(self,format='JPEG',quality=None):
        message = self.getImage(format,quality)
        print(len(message))
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

    def splitFrames(self,videoFileName,format='JPEG',quality=None)-> None:
        cap = cv2.VideoCapture(videoFileName) # 打开视频文件
        i = 0
        while True:
            i = cap.get(cv2.CAP_PROP_POS_MSEC)
            # success 表示是否成功，data是当前帧的图像数据；.read读取一帧图像，移动到下一帧
            success, dat = cap.read()
            if not success:
                break
            im = Image.fromarray(cv2.cvtColor(dat, cv2.COLOR_BGR2RGB))
            #im = Image.fromarray(dat) # 重建图像
            image = im.resize((240, 135))
            image.tobytes()
            img = BytesIO()
            image.save(img, format=format, quality=quality)
            data = img.getvalue()
            aa = b'\xaa\xbb\xcc'
            x= len(data) +3
            s = pack('<H', x)
            data = s+data+aa 
            datas, addr = self.tcp_client.recvfrom(1024)
            #
            if 'ok' in datas.decode():
                #print(len(data))
                #print(i)
                self.sendMessage(data)
            if 'go' in datas.decode():
                i = i + 30000
                cap.set(cv2.CAP_PROP_POS_MSEC, i)
            if "exit" in datas.decode():
                print(datas.decode())
                i=0
                break
        
        cap.release()

if __name__ == '__main__':
    T = TcpClient()
#    files = file_name('./')
#    for file in files:
#        if "mp4" in file:
#            try:
#                T.splitFrames(file,'JPEG',85)
#            except IOError as e:
#                T.closeClient()
    try:
        #T.splitFrames("http://39.134.66.110/PLTV/88888888/224/3221225677/index.m3u8","JPEG",95) #中央10台
        #T.splitFrames("http://219.151.31.38/liveplay-kk.rtxapp.com/live/program/live/cctv6hd/4000000/mnf.m3u8","JPEG",95) #中央6台
        T.splitFrames("http://ivi.bupt.edu.cn/hls/cctv6hd.m3u8","JPEG",85)#中央6台
    except IOError as e:
        T.closeClient()
               

        #T.getMessage()
        