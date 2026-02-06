import paho.mqtt.client as mqtt
import json
from class_roz import obrabotka

# def parse_turple(st):
#     a, b = st[1:-1].split(',')
#     return (a,b)

# topic_lists = ['kvant/R22/BV/amper', 'kvant/R22/BV/voltage']
# amper_list = []
# voltage_list = []


# def device_classifare(amp, volt):
#     amp = np.array(amp)
#     volt = np.array(volt)
#     I_rms = np.sqrt(np.mean(amp**2))
#     U_rms = np.sqrt(np.mean(volt**2))
#     S = I_rms * U_rms
#     P = np.mean(amp * volt)
#     cosfi = P / S
#     print(f'''I_rms:{I_rms}\nU_rms:{U_rms}\nS:{S}\nP:{P}\ncosfi:{cosfi}''')

obr = obrabotka()

def on_message(client, userdata, msg):
    print(f"Топик: {msg.topic}, Сообщение: {msg.payload.decode()}, Тип данных {type(msg.payload.decode())}")
    data = json.loads(msg.payload.decode())
    print(type(data))
    print(data['timestamp'])
    data_string = json.dumps(data, indent=4, ensure_ascii=False)
    with open('data.json', 'w', encoding='utf-8') as file:
        file.write(data_string)
    obr.update_data(amp_new=data['amper']['data'], volt_new=data['voltage']['data'])
    obr.raschet()
    print(obr.cosfi)
    print(obr.Q)
    print(obr.S)
    print(obr.P)

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_message = on_message  # Обработчик сообщений
client.username_pw_set("Rasbery", "154321")  # Логин и пароль
client.connect("m5.wqtt.ru", 14182)  # Подключение
client.subscribe('kvant/R22/BV/update')  # Подписка на ваш топик
client.loop_forever()  # Запуск цикла
