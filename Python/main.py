import paho.mqtt.client as mqtt
import json
from class_roz import obrabotka
import psycopg2
from datetime import datetime

host = 'localhost'
name_user = 'postgres'
password = '123'
database = 'SmartSocket'
connect = '5432'

MQTT_TOPIC = 'kvant/R22/BV/reception'

obr = obrabotka()

last_hour = datetime.now().hour


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
    # client.publish("kvant/R22/BV/test", str(data['rms_A']))
    try:
        connection = psycopg2.connect(host=host, user=name_user, password=password, database=database)
        connection.autocommit = True
        with connection.cursor() as cursor:
            cursor.execute('insert into data_current(uuid, flash_current, rms_current) values (%s, %s, %s)',
                           (data['uuid'], str(data['amper']['data']), str(data['rms_A'])))
            cursor.execute('insert into data_voltage(uuid, flash_voltage, rms_voltage) values (%s, %s, %s)',
                           (data['uuid'], str(data['voltage']['data']), str(data['rms_V'])))
            cursor.execute('insert into calculation(uuid, P, I, V, cosfi) values (%s, %s, %s, %s, %s)', (
                data['uuid'], str(data['rms_A'] * data['rms_V']), str(data['rms_A']), str(data['rms_V']),
                str(obr.cosfi)))
    except Exception as e:
        print(f'[info]: Ошибка {e}')
    finally:
        if connection:
            connection.close()
            print('[info]: коннект закрыт')
    try:
        connection = psycopg2.connect(host=host, user=name_user, password=password, database=database)
        connection.autocommit = True
        with connection.cursor() as cursor:
            cursor.execute('SELECT sum(data_records) from data_power_records where date_records = CURRENT_DATE')
            #row = cursor.fetchone()
            P_kWh = cursor.fetchone()[0] or 0
            I_mg = data['rms_A']
            V_mg = data['rms_V']
            money = P_kWh * 5.77
            cosfi = obr.cosfi
            P = I_mg * V_mg
            reception = {"P": P, "I_mg": I_mg, "V_mg": V_mg, "money": money, "cosfi": cosfi}
            with open('reception.json', 'w', encoding='utf-8') as file:
                json.dump(reception, file, ensure_ascii=False, indent=4)
            client.publish(MQTT_TOPIC, json.dumps(reception))
    except Exception as e:
        print(f'[info]: Ошибка {e}')
    finally:
        if connection:
            connection.close()
            print('[info]: коннект закрыт')
    global last_hour
    now = datetime.now()
    if now.hour != last_hour:
        try:
            connection = psycopg2.connect(host=host, user=name_user, password=password, database=database)
            connection.autocommit = True
            with connection.cursor() as cursor:
                cursor.execute(
                    'SELECT uuid, SUM(I * V) / 720.0 / 1000.0 FROM (SELECT I, V, uuid FROM calculation ORDER BY num_measurements DESC LIMIT 720) AS last_hour GROUP BY uuid')
                data = cursor.fetchall()
                cursor.execute('insert into data_power_records(uuid, data_records) values(%s, %s)',
                               (data[0][0], str(data[0][1])))
            last_hour = now.hour
        except Exception as e:
            print(f'[info]: Ошибка {e}')
        finally:
            if connection:
                connection.close()


client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_message = on_message  # Обработчик сообщений
client.username_pw_set("Rasbery", "154321")  # Логин и пароль
client.connect("m5.wqtt.ru", 14182)  # Подключение
client.subscribe('kvant/R22/BV/update')  # Подписка на топик

client.loop_forever()  # Запуск цикла
