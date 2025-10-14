import sensor, image, lcd
from machine import UART
from fpioa_manager import fm

# 配置串口（P7=TX, P6=RX）
fm.register(7, fm.fpioa.UART1_TX, force=True)
fm.register(6, fm.fpioa.UART1_RX, force=True)
uart_A = UART(UART.UART1, 115200, 8, 0, 1, timeout=1000)

# 配置参数
black_threshold = ((0, 3))  # 黑色阈值
roi = [0, 100, 320, 16]      # 检测区域（中间偏下）
center_line = 160            # 屏幕中线位置
LEFT_THRESHOLD = -30   # 左偏阈值
RIGHT_THRESHOLD = 30   # 右偏阈值

# 初始化
lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QVGA)  # 320x240
sensor.skip_frames(time=3000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)

def send_direction(direction):
    ascii_char = chr(direction + 48)
    uart_A.write(ascii_char)
    # 打印发送的ASCII码信息
    print("发送ASCII字符: '" + ascii_char + "' (ASCII码值: " + str(ord(ascii_char)) + ")")
    return direction

while True:
    img = sensor.snapshot()
    blobs = img.find_blobs([black_threshold], roi=roi, area_threshold=200, merge=True)

    direction = 0  # 0=居中, 1=左偏, 2=右偏
    if blobs:
        b = blobs[0]
        img.draw_rectangle(b[0:4], color=255)
        img.draw_cross(b[5], b[6], color=255)

        line_center = b[5]
        deviation = line_center - center_line

        if deviation < LEFT_THRESHOLD:
            direction = 1
        elif deviation > RIGHT_THRESHOLD:
            direction = 2
        else:
            direction = 0

    # 屏幕显示方向（修复为字符串拼接）
    img.draw_string(10, 10, "Dir: " + str(direction), color=255)
    lcd.display(img)

    # 发送ASCII码值
    send_direction(direction)
    print("方向: " + str(direction))
