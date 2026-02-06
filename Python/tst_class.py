from class_roz import obrabotka

obr = obrabotka()

a = [0.0, 0.43, 0.83, 1.14, 1.34, 1.41, 1.34, 1.14, 0.83, 0.43, 0.0, -0.43, -0.83, -1.14, -1.34, -1.41, -1.34,
     -1.14, -0.83, -0.43]
v = [0, 96, 183, 252, 296, 311, 296, 252, 183, 96, 0, -96, -183, -252, -296, -311, -296, -252, -183, -96]

obr.update_data(amp_new=a, volt_new=v)

obr.raschet()
print(obr.cosfi)
print(obr.Q)
print(obr.P)
print(obr.S)
print(obr.P)
print(obr.P_max)
print(obr.P_min)
print(obr.coleb)
