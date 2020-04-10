import matplotlib.pyplot as plt

def simulate1(max_s, a):
    speed = []
    s = 0.0
    dT = 0.0001
    while (s < max_s):
        speed.append(s)
        speed_percentage = s / max_s
        if (speed_percentage < 0.45):
            acceleration = a / 2.0
        elif (speed_percentage < 0.95):
            acceleration = a * (1.0 - speed_percentage)
        else:
            acceleration = (0.05 * max_s) / 4.0
        s += acceleration * dT
    
    for _ in range(800):
        speed.append(max_s)

    return speed

def simulate2(max_s, a):
    speed = []
    s = 0.0
    dT = 0.0001
    while (s < max_s):
        speed.append(s)
        speed_percentage = s / max_s
        if (speed_percentage < 0.25):
            acceleration = a / 2.0
        elif (speed_percentage < 0.45):
            acceleration = a * (speed_percentage + 0.075)
        elif (speed_percentage < 0.95):
            acceleration = a / 2.0
        else:
            acceleration = (0.05 * max_s) / 4.0
        s += acceleration * dT
    
    for _ in range(800):
        speed.append(max_s)

    return speed

# plt.plot(simulate1(0.099 * 2.0, 0.10 * 2.0), color='tab:red', label='BALANCED')
# plt.plot(simulate1(0.100 * 2.0, 0.15 * 2.0), color='tab:green', label='ACCELERATION')
# plt.plot(simulate2(0.104 * 2.0, 0.07 * 2.0), color='tab:blue', label='HEAVY')
# plt.plot(simulate1(0.103 * 2.0, 0.12 * 2.0), color='tab:orange', label='HANDLING')

plt.plot(simulate1(0.103 * 2.0, 0.11 * 2.0), color='tab:red', label='BALANCED')
plt.plot(simulate1(0.102 * 2.0, 0.15 * 2.0), color='tab:green', label='ACCELERATION')
plt.plot(simulate2(0.104 * 2.0, 0.07 * 2.0), color='tab:blue', label='HEAVY')
plt.plot(simulate1(0.100 * 2.0, 0.12 * 2.0), color='tab:orange', label='HANDLING')

plt.legend()
plt.show()