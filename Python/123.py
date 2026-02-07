import paho.mqtt.client as mqtt
import json
from class_roz import obrabotka
import psycopg2

host = 'localhost'
name_user = 'postgres'
password = '123'
database = 'SmartSocket'
connect = '5432'

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
    print(obr.I_rms)
    print(obr.U_rms)
    print(obr.P_rms)
    #client.publish("kvant/R22/BV/test", str(data['rms_A']))
    try:
        connection = psycopg2.connect(host=host, user=name_user, password=password, database=database)
        connection.autocommit = True
        with connection.cursor() as cursor:
            cursor.execute('insert into data_current(uuid, flash_current, rms_current) values (%s, %s, %s)', (data['uuid'], str(data['amper']['data']), str(data['rms_A'])))
            cursor.execute('insert into data_voltage(uuid, flash_voltage, rms_voltage) values (%s, %s, %s)', (data['uuid'], str(data['voltage']['data']), str(data['rms_V'])))
    except Exception as e:
        print(f'[info]: Ошибка {e}')
    finally:
        if connection:
            connection.close()
            print('[info]: коннект закрыт')


client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_message = on_message  # Обработчик сообщений
client.username_pw_set("Rasbery", "154321")  # Логин и пароль
client.connect("m5.wqtt.ru", 14182)  # Подключение
client.subscribe('kvant/R22/BV/update')  # Подписка на топик

client.loop_forever()  # Запуск цикла