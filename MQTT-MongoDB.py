import paho.mqtt.client as mqtt
import datetime
from pymongo import MongoClient

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("/sensor/#")

def on_message(client, userdata, msg):
    receiveTime=datetime.datetime.now()
    message=msg.payload.decode("utf-8")
    isfloatValue=False
    try:
        #convert the string to a float so that it is stored as a number and not a string in the database
        val = float(message)
        isfloatValue=True
    except:
        isfloatValue=False

    if isfloatValue:
        print(str(receiveTime)+": "+msg.topic+" "+str(val))
        post={"time":receiveTime,"topic":msg.topic,"value": val}
    else:
        print(str(receiveTime)+": "+msg.topic+" "+message)
        post={"time":receiveTime,"topic":msg.topic,"value": message}
    collection.insert_one(post)

#set up client for MongoDB   

mongoClient=MongoClient('mongodb://tschang:<password>@plants-shard-00-00-l7uuf.mongdb.net:27017,plants-shard-00-01-l7uuf.mongodb.net:27017,plants-shard-00-02-l7uuf.mongodb.net:27017/basil?ssl=true&replicaSet=plants-shard-0&authSource=admin&retryWrites=true')
db=mongoClient.basil
collection=db.july2020

#initialize the client that should connect to the Mosquitto broker
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("192.168.1.150", 1883, 60)
#blocking loop to the Mosquitto broker
client.loop_forever()



