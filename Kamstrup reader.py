from Cryptodome.Cipher import AES # Library: PyCryptodome
import serial
import paho.mqtt.client as mqtt
encryption_key = bytes.fromhex('5AD84121D9D20B364B7A11F3C1B5827F')
authentication_key = bytes.fromhex('AFB3F93E3E7204EDB3C27F96DBD51AE0')
client =mqtt.Client("KamstrupReader")
client.connect("mqttserver.local")

with serial.Serial('/dev/ttyUSB0', 2400, timeout=11) as ser:
    while True:
        s = b''
        while len(s) != 491:
            try:
                ser.flushInput()
                s = ser.read(491)
            except:
                print('øv')
            print('.' + s.hex())

        try:
            #print(s.hex())
            #print(len(s))
            cipher_text = s[11:len(s)-3]
            #print(cipher_text.hex())
            system_title = cipher_text[2:2+8]
            initialization_vector = system_title + cipher_text[14:14+4]
            additional_authenticated_data = cipher_text[13:13+1] + authentication_key
            authentication_tag = cipher_text[len(cipher_text)-12:len(cipher_text)]
            cipher = AES.new(encryption_key, AES.MODE_GCM, nonce=initialization_vector,
            mac_len=len(authentication_tag)) #len(authentication_tag) 4..14
            cipher.update(additional_authenticated_data)
            plaintext = cipher.decrypt_and_verify(cipher_text[18:len(cipher_text)-12],authentication_tag)
        except:
            continue
        find_bytes = bytes.fromhex('09060101010800ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Active energy A14: " + str(val/1000) + "KWh")
        client.publish("Kamstrup/A14",str(val/1000))

        find_bytes = bytes.fromhex('09060101020800ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Active energy A23: " + str(val/1000) + "KWh")
        client.publish("Kamstrup/A23",str(val/1000))

        find_bytes = bytes.fromhex('09060101030800ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Reactive energy R12: " + str(val) + "varh")
        client.publish("Kamstrup/R12",str(val))

        find_bytes = bytes.fromhex('09060101040800ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Reactive energy R34: " + str(val) + "varh")
        client.publish("Kamstrup/R34",str(val))

        find_bytes = bytes.fromhex('09060101010700ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Actual power P14: " + str(val) + "W")
        client.publish("Kamstrup/P14",str(val))

        find_bytes = bytes.fromhex('09060101020700ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Actual power P23: " + str(val) + "W")
        client.publish("Kamstrup/P23",str(val))

        find_bytes = bytes.fromhex('09060101030700ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Actual power Q12: " + str(val) + "var")
        client.publish("Kamstrup/Q12",str(val))

        find_bytes = bytes.fromhex('09060101040700ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Actual power Q34: " + str(val) + "var")
        client.publish("Kamstrup/Q34",str(val))

        find_bytes = bytes.fromhex('09060101200700ff12')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+2],"big")
        print("RMS voltage L1: " + str(val) + "V")
        client.publish("Kamstrup/RMSVL1",str(val))

        find_bytes = bytes.fromhex('09060101340700ff12')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+2],"big")
        print("RMS voltage L2: " + str(val) + "V")
        client.publish("Kamstrup/RMSVL2",str(val))

        find_bytes = bytes.fromhex('09060101480700ff12')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+2],"big")
        print("RMS voltage L3: " + str(val) + "V")
        client.publish("Kamstrup/RMSVL3",str(val))

        find_bytes = bytes.fromhex('090601011f0700ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("RMS current L1: " + str(val/100) + "A")
        client.publish("Kamstrup/RMSCL1",str(val/100))

        find_bytes = bytes.fromhex('09060101330700ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("RMS current L2: " + str(val/100) + "A")
        client.publish("Kamstrup/RMSCL2",str(val/100))

        find_bytes = bytes.fromhex('09060101470700ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("RMS current L3: " + str(val/100) + "A")
        client.publish("Kamstrup/RMSCL3",str(val/100))

        find_bytes = bytes.fromhex('09060101150700ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Actual power (P14) L1: " + str(val) + "W")
        client.publish("Kamstrup/P14L1",str(val))

        find_bytes = bytes.fromhex('09060101290700ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Actual power (P14) L2: " + str(val) + "W")
        client.publish("Kamstrup/P14L2",str(val))

        find_bytes = bytes.fromhex('090601013d0700ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Actual power (P14) L3: " + str(val) + "W")
        client.publish("Kamstrup/P14L3",str(val))

        find_bytes = bytes.fromhex('09060101210700ff12')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+2],"big")
        print("Power factor L1: " + str(val))
        client.publish("Kamstrup/PFL1",str(val))

        find_bytes = bytes.fromhex('09060101350700ff12')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+2],"big")
        print("Power factor L2: " + str(val))
        client.publish("Kamstrup/PFL2",str(val))

        find_bytes = bytes.fromhex('09060101490700ff12')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+2],"big")
        print("Power factor L3: " + str(val))
        client.publish("Kamstrup/PFL3",str(val))

        find_bytes = bytes.fromhex('090601010d0700ff12')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+2],"big")
        print("Power factor Total: " + str(val))
        client.publish("Kamstrup/PF",str(val))

        find_bytes = bytes.fromhex('09060101160700ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Active power (P23) L1: " + str(val) + "W")
        client.publish("Kamstrup/P23L1",str(val))

        find_bytes = bytes.fromhex('090601012a0700ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Active power (P23) L2: " + str(val) + "W")
        client.publish("Kamstrup/P23L2",str(val))

        find_bytes = bytes.fromhex('090601013e0700ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Active power (P23) L3: " + str(val) + "W")
        client.publish("Kamstrup/P23L3",str(val))

        find_bytes = bytes.fromhex('09060101160800ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Active energy (A23) L1: " + str(val/1000) + "KWh")
        client.publish("Kamstrup/A23L1",str(val/1000))

        find_bytes = bytes.fromhex('090601012a0800ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Active energy (A23) L2: " + str(val/1000) + "KWh")
        client.publish("Kamstrup/A23L2",str(val/1000))

        find_bytes = bytes.fromhex('090601013e0800ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Active energy (A23) L3: " + str(val/1000) + "KWh")
        client.publish("Kamstrup/A23L3",str(val/1000))

        find_bytes = bytes.fromhex('09060101150800ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Active energy (A14) L1: " + str(val/1000) + "KWh")
        client.publish("Kamstrup/A14L1",str(val/1000))

        find_bytes = bytes.fromhex('09060101290800ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Active energy (A14) L2: " + str(val/1000) + "KWh")
        client.publish("Kamstrup/A14L2",str(val/1000))

        find_bytes = bytes.fromhex('090601013d0800ff06')
        found_pos = plaintext.find(find_bytes, 0) +len(find_bytes)
        val = int.from_bytes(plaintext[found_pos:found_pos+4],"big")
        print("Active energy (A14) L3: " + str(val/1000) + "KWh")
        client.publish("Kamstrup/A14L3",str(val/1000))